#include "VerificationSystem.hpp"

VerificationSystem::VerificationSystem(SyntaxBlockWorkingMode working_mode)
{
    this->working_mode = working_mode;
}

VerificationSystem::~VerificationSystem() {}

void VerificationSystem::check_strings(const std::vector<std::string> strings, const std::string& output_file_path)
{
    int strings_count = strings.size();

    std::vector<std::set<Message>> messages = std::vector<std::set<Message>>(strings_count);
    std::vector<bool> results = std::vector<bool>(strings_count);

    Debugger debugger = Debugger();
    reset_events();
    connect_events(debugger, messages);

    #pragma omp parallel for
    for (int string_index = 0; string_index < strings_count; string_index++) {
        MainBlock main_block = MainBlock(working_mode);
        bool result = main_block.check_string(string_index, strings[string_index]);
        results[string_index] = result;
    }

    std::ofstream output_file;
    output_file.open(output_file_path);
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output file" << std::endl;
        return;
    }

    debugger.print_message_and_results(output_file, messages, results);

    output_file.close();
}

void VerificationSystem::check_file(const std::string& input_file_path, const std::string& output_file_path)
{
    std::vector<std::string> strings = DataUtils::FileUtils::read_strings_from_file(input_file_path);
    check_strings(strings, output_file_path);
}

void VerificationSystem::check_db_representation(AlgorithmRepresentation& representation)
{
    std::vector<std::set<Message>> messages = std::vector<std::set<Message>>(representation.representation_strings.size());
    std::vector<bool> results = std::vector<bool>(representation.representation_strings.size());

    Debugger debugger = Debugger();
    reset_events();
    connect_events(debugger, messages);

    #pragma omp parallel for
    for (int string_index = 0; string_index < (int)representation.representation_strings.size(); string_index++) {
        MainBlock main_block = MainBlock(working_mode);
        auto representation_string = representation.representation_strings[string_index];
        bool result = main_block.check_string(string_index, representation_string.content);
        results[string_index] = result;
    }

    debugger.print_message_and_results({"en", "ru"}, representation, messages, results);
    
    SharedRepository::get_instance().get_algorithm_representation_repository().update(representation);
}

void VerificationSystem::reset_events()
{
    SyntaxBlock::CFG::Symbol::OpeningCurlyBrace::on_error_occurs.clear();
    SyntaxBlock::CFG::Symbol::ClosingCurlyBrace::on_error_occurs.clear();
    SyntaxBlock::CFG::Symbol::OpeningParenthesis::on_error_occurs.clear();
    SyntaxBlock::CFG::Symbol::ClosingParenthesis::on_error_occurs.clear();
    SyntaxBlock::CFG::Symbol::Comma::on_error_occurs.clear();
    SyntaxBlock::CFG::Symbol::Colon::on_error_occurs.clear();
    SyntaxBlock::CFG::Symbol::Semicolon::on_error_occurs.clear();
    SyntaxBlock::CFG::Symbol::QuotationMark::on_error_occurs.clear();
    SyntaxBlock::CFG::Symbol::EqualSign::on_error_occurs.clear();
    SyntaxBlock::CFG::Symbol::ComparisonSign::on_error_occurs.clear();
    SyntaxBlock::CFG::Symbol::ExclamationMark::on_error_occurs.clear();
    SyntaxBlock::CFG::Symbol::ArithmeticSign::on_error_occurs.clear();
    SyntaxBlock::CFG::Symbol::LogicalSign::on_error_occurs.clear();
    
    SyntaxBlock::CFG::SpecialIdentifier::Operation::on_error_occurs.clear();
    SyntaxBlock::CFG::SpecialIdentifier::OperandOfUnaryOperation::on_error_occurs.clear();
    SyntaxBlock::CFG::SpecialIdentifier::FirstOperandOfBinaryOperation::on_error_occurs.clear();
    SyntaxBlock::CFG::SpecialIdentifier::SecondOperandOfBinaryOperation::on_error_occurs.clear();
    SyntaxBlock::CFG::SpecialIdentifier::Modulus::on_error_occurs.clear();
    SyntaxBlock::CFG::SpecialIdentifier::SquareOfNumber::on_error_occurs.clear();
    SyntaxBlock::CFG::SpecialIdentifier::SquareRootOfNumber::on_error_occurs.clear();

    SyntaxBlock::CFG::Variable::Edge::on_error_occurs.clear();
    SyntaxBlock::CFG::Variable::Identifier::on_error_occurs.clear();
    SyntaxBlock::CFG::Variable::Integer::on_error_occurs.clear();
    
    SyntaxBlock::CFG::Operation::Logical::on_error_occurs.clear();
    SyntaxBlock::CFG::Operation::Arithmetic::on_error_occurs.clear();

    SyntaxBlock::CFG::String::on_error_occurs.clear();
    SyntaxBlock::CFG::String::Beginning::on_error_occurs.clear();
    SyntaxBlock::CFG::String::Logical::on_error_occurs.clear();
    SyntaxBlock::CFG::String::Arithmetic::on_error_occurs.clear();
    SyntaxBlock::CFG::String::Inner::Operation::on_error_occurs.clear();
    SyntaxBlock::CFG::String::Inner::Operand::Variable::on_error_occurs.clear();
    SyntaxBlock::CFG::String::Inner::Operand::on_error_occurs.clear();
}

void VerificationSystem::connect_events(int strings_count)
{
    std::vector<std::set<Message>> messages = std::vector<std::set<Message>>(strings_count);
    Debugger debugger = Debugger();
    connect_events(debugger, messages);
}

void VerificationSystem::connect_events(Debugger& debugger, std::vector<std::set<Message>>& messages)
{
    SyntaxBlock::CFG::Symbol::OpeningCurlyBrace::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::Symbol::ClosingCurlyBrace::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::Symbol::OpeningParenthesis::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::Symbol::ClosingParenthesis::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::Symbol::Comma::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::Symbol::Colon::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::Symbol::Semicolon::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::Symbol::QuotationMark::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::Symbol::EqualSign::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::Symbol::ComparisonSign::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::Symbol::ExclamationMark::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::Symbol::ArithmeticSign::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::Symbol::LogicalSign::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    
    SyntaxBlock::CFG::SpecialIdentifier::Operation::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::SpecialIdentifier::OperandOfUnaryOperation::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::SpecialIdentifier::FirstOperandOfBinaryOperation::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::SpecialIdentifier::SecondOperandOfBinaryOperation::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::SpecialIdentifier::Modulus::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::SpecialIdentifier::SquareOfNumber::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::SpecialIdentifier::SquareRootOfNumber::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };

    SyntaxBlock::CFG::Variable::Edge::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::Variable::Identifier::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::Variable::Integer::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    
    SyntaxBlock::CFG::Operation::Logical::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::Operation::Arithmetic::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };

    SyntaxBlock::CFG::String::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::String::Beginning::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::String::Logical::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::String::Arithmetic::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::String::Inner::Operation::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::String::Inner::Operand::Variable::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
    SyntaxBlock::CFG::String::Inner::Operand::on_error_occurs += [&debugger, &messages](const Message& message) { debugger.add_message_to_vector(message, messages); };
}