#include "Debugger.hpp"

const char* Debugger::token_types[] = {
    "Letter", 
    "Digit", 
    "OpeningCurlyBrace", 
    "ClosingCurlyBrace", 
    "OpeningParenthesis", 
    "ClosingParenthesis", 
    "Comma", 
    "Colon", 
    "Semicolon", 
    "QuotationMark", 
    "EqualSign", 
    "ComparisonSign", 
    "ExclamationMark", 
    "ArithmeticSign", 
    "LogicalSign", 
    "SpaceSign", 
    "Other",
};

const char* Debugger::complex_token_types[] = {
    "Identifier",             // идентификатор
    "Integer",                // целое число
};

const char* Debugger::special_identifier_types[] = {
    "Operation",
    "OperandOfUnaryOperation",
    "FirstOperandOfBinaryOperation",
    "SecondOperandOfBinaryOperation",
    "V",
    "No",
    "Real",
    "Solution",
    "Modulus",
    "SquareOfNumber",
    "SquareRootOfNumber",
};

Debugger::Debugger()
{
    omp_init_lock(&lock);
}

void Debugger::add_message_to_vector(const Message& message, std::vector<std::set<Message>>& messages)
{
    omp_set_lock(&lock);
    messages[message.string_index].insert(message);
    omp_unset_lock(&lock);
}

void Debugger::print_message(std::ostringstream& oss, const Message& message)
{
    auto current_message_pool = SharedRepository::get_instance().get_message_storage().get_current_pool();
    auto message_description = current_message_pool["other"]["position"];
    auto message_text = current_message_pool[message.message_pool][message.message_pool_identifier];
    oss << message_description << ": " << message.token_index << "\t" << message.token_value << "\t" << message_text << std::endl;
}

void Debugger::print_message(std::ofstream& file, const Message& message)
{
    auto current_message_pool = SharedRepository::get_instance().get_message_storage().get_current_pool();
    auto message_description = current_message_pool["other"]["position"];
    auto message_text = current_message_pool[message.message_pool][message.message_pool_identifier];
    file << message_description << ": " << message.token_index << "\t" << message.token_value << "\t" << message_text << std::endl;
}

void Debugger::print_message(const std::string& language_pool_name, std::string& validity, const Message& message)
{
    auto message_storage = SharedRepository::get_instance().get_message_storage();
    auto message_description = message_storage[language_pool_name]["other"]["position"];
    auto message_text = message_storage[language_pool_name][message.message_pool][message.message_pool_identifier];
    validity += message_description + ": " + std::to_string(message.token_index) + "\t" + message.token_value + "\t" + message_text + "\n";
}

std::vector<std::string> Debugger::get_message_and_results(const std::vector<std::set<Message>>& messages, const std::vector<bool>& results)
{
    std::vector<std::string> strings;
    std::ostringstream oss;
    print_message_and_results(oss, messages, results);
    auto buf = oss.str()
        | std::ranges::views::split('\n')
        | std::ranges::views::transform([](auto&& word) {
              return std::string(word.begin(), word.end());
          })
        | std::ranges::to<std::vector<std::string>>();
    return buf;
}

void Debugger::print_message_and_results(std::ostringstream& oss, const std::vector<std::set<Message>>& messages, const std::vector<bool>& results)
{
    auto current_message_pool = SharedRepository::get_instance().get_message_storage().get_current_pool();
    for (int i = 0; i < (int)messages.size(); i++) {
        oss << current_message_pool["other"]["string"] << ": " << i+1 << std::endl;
        for (auto it = messages[i].begin(); it != messages[i].end(); it++) {
            oss << "\t";
            print_message(oss, *it);
        }
        if (results[i]) {
            oss << "\t" << current_message_pool["other"]["result"] << ": " << current_message_pool["other"]["success"] << std::endl;
        }
        else {
            oss << "\t" << current_message_pool["other"]["result"] << ": " << current_message_pool["other"]["failure"] << std::endl;
        }
        if (i != int(messages.size() - 1)) {
            oss << std::endl;
        }
    }
}

void Debugger::print_message_and_results(std::ofstream& file, const std::vector<std::set<Message>>& messages, const std::vector<bool>& results)
{
    auto current_message_pool = SharedRepository::get_instance().get_message_storage().get_current_pool();
    for (int i = 0; i < (int)messages.size(); i++) {
        file << current_message_pool["other"]["string"] << ": " << i+1 << std::endl;
        for (auto it = messages[i].begin(); it != messages[i].end(); it++) {
            file << "\t";
            print_message(file, *it);
        }
        if (results[i]) {
            file << "\t" << current_message_pool["other"]["result"] << ": " << current_message_pool["other"]["success"] << std::endl;
        }
        else {
            file << "\t" << current_message_pool["other"]["result"] << ": " << current_message_pool["other"]["failure"] << std::endl;
        }
        if (i != int(messages.size() - 1)) {
            file << std::endl;
        }
    }
}

void Debugger::print_message_and_results(const std::vector<std::string>& message_pools_names, AlgorithmRepresentation& algorithm_representation, const std::vector<std::set<Message>>& messages, const std::vector<bool>& results)
{
    auto message_storage = SharedRepository::get_instance().get_message_storage();

    if (messages.empty()) {
        return;
    }

    std::vector<json> validity_vector(messages.size());
    for (auto& message_pool_name : message_pools_names) {
        for (int i = 0; i < (int)messages.size(); i++) {
            std::string validity_temp_str;
            for (auto it = messages[i].begin(); it != messages[i].end(); it++) {
                print_message(message_pool_name, validity_temp_str, *it);
            }
            if (results[i]) {
                validity_temp_str += message_storage[message_pool_name]["other"]["result"] + ": " + message_storage[message_pool_name]["other"]["success"] + "\n";
            }
            else {
                validity_temp_str += message_storage[message_pool_name]["other"]["result"] + ": " + message_storage[message_pool_name]["other"]["failure"] + "\n";
            }
            validity_vector[i][message_pool_name] = validity_temp_str;
        }
    }

    for (int i = 0; i < (int)messages.size(); i++) {
        algorithm_representation.representation_strings[i].validity = validity_vector[i];
    }
}

void Debugger::print_tokens(std::ofstream& file, const std::string& target_string, const std::vector<VariableToken>& combined_tokens)
{
    for (int i = 0; i < (int)combined_tokens.size(); i++) {
        std::variant<SimpleToken, ComplexToken> combined_token = combined_tokens[i];
        if (std::holds_alternative<SimpleToken>(combined_token)) {
            SimpleToken token = std::get<SimpleToken>(combined_token);
            char symbol = target_string[token.index];
            file << symbol << '\t' << token.index << "\t" << token_types[static_cast<std::underlying_type<SimpleTokenType>::type>(token.type)] << std::endl;
        }
        else {
            ComplexToken token = std::get<ComplexToken>(combined_token);
            std::string buf_str = target_string.substr(token.index, token.value.length());
            if (std::holds_alternative<ComplexTokenType>(token.type)) {
                file << buf_str << '\t' << token.index << "\t" << complex_token_types[static_cast<std::underlying_type<ComplexTokenType>::type>(std::get<ComplexTokenType>(token.type))] << std::endl;
            }
            else if (std::holds_alternative<SpecialIdentifierType>(token.type)) {
                file << buf_str << '\t' << token.index << "\t" << special_identifier_types[static_cast<std::underlying_type<SpecialIdentifierType>::type>(std::get<SpecialIdentifierType>(token.type))] << std::endl;
            }
        }
    }
}