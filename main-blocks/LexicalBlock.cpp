#include "LexicalBlock.hpp"

LexicalBlock::LexicalBlock()
{
    this->state = LexicalBlockState::Space;
}

void LexicalBlock::switch_state(LexicalBlockState new_state)
{
    this->state = new_state;
}

void LexicalBlock::process(const VariableToken& token, std::optional<char> symbol = std::nullopt)
{
    if (combined_token_vector.size() > 1) {
        // проверка предпоследней лексемы
        if (std::holds_alternative<ComplexToken>(combined_token_vector[combined_token_vector.size() - 2])) {
            SpecialIdentifierBlock special_identifier_block = SpecialIdentifierBlock();
            special_identifier_block.try_change_identifier_type_to_special(std::get<ComplexToken>(combined_token_vector[combined_token_vector.size() - 2]));
        }
    }
    if (!symbol.has_value())
        return;

    if (std::holds_alternative<ComplexToken>(combined_token_vector.back())) {
        ComplexToken &complex_token = std::get<ComplexToken>(combined_token_vector.back());
        complex_token.value += symbol.value();
    }
    
    return;
}

void LexicalBlock::start_identifier(const SimpleToken& token, char symbol)
{
    if (token.type != SimpleTokenType::Letter)
        throw std::invalid_argument("Invalid identifier");

    std::string value = "";
    this->combined_token_vector.push_back(ComplexToken(ComplexTokenType::Identifier, value, token.index));
    this->process(token, symbol);
}

void LexicalBlock::start_integer(const SimpleToken& token, char symbol)
{
    if (token.type != SimpleTokenType::Digit)
        throw std::invalid_argument("Invalid integer");

    std::string value = "";
    this->combined_token_vector.push_back(ComplexToken(ComplexTokenType::Integer, value, token.index));
    this->process(token, symbol);
}

void LexicalBlock::start_arithmetic_sign(const SimpleToken& token)
{
    if (token.type != SimpleTokenType::ArithmeticSign)
        throw std::invalid_argument("Invalid arithmetic sign");
    
    this->combined_token_vector.push_back(token);
    this->process(token);
}

void LexicalBlock::start_comparison_sign(const SimpleToken& token)
{
    if (token.type != SimpleTokenType::ComparisonSign)
        throw std::invalid_argument("Invalid comparison sign");
    
    this->combined_token_vector.push_back(token);
    this->process(token);
}

void LexicalBlock::start_logical_sign(const SimpleToken& token)
{
    if (token.type != SimpleTokenType::LogicalSign)
        throw std::invalid_argument("Invalid logical sign");
    
    this->combined_token_vector.push_back(token);
    this->process(token);
}

void LexicalBlock::start_equal_sign(const SimpleToken& token)
{
    if (token.type != SimpleTokenType::EqualSign)
        throw std::invalid_argument("Invalid equal sign");
    
    this->combined_token_vector.push_back(token);
    this->process(token);
}

void LexicalBlock::start_exclamation_mark(const SimpleToken& token)
{
    if (token.type != SimpleTokenType::ExclamationMark)
        throw std::invalid_argument("Invalid exclamation mark");
    
    this->combined_token_vector.push_back(token);
    this->process(token);
}

void LexicalBlock::start_opening_curly_brace(const SimpleToken& token)
{
    if (token.type != SimpleTokenType::OpeningCurlyBrace)
        throw std::invalid_argument("Invalid opening curly brace");

    this->combined_token_vector.push_back(token);
    this->process(token);
}

void LexicalBlock::start_closing_curly_brace(const SimpleToken& token)
{
    if (token.type != SimpleTokenType::ClosingCurlyBrace)
        throw std::invalid_argument("Invalid closing curly brace");

    this->combined_token_vector.push_back(token);
    this->process(token);
}

void LexicalBlock::start_opening_parenthesis(const SimpleToken& token)
{
    if (token.type != SimpleTokenType::OpeningParenthesis)
        throw std::invalid_argument("Invalid opening parenthesis");

    this->combined_token_vector.push_back(token);
    this->process(token);
}

void LexicalBlock::start_closing_parenthesis(const SimpleToken& token)
{
    if (token.type != SimpleTokenType::ClosingParenthesis)
        throw std::invalid_argument("Invalid closing parenthesis");
    
    this->combined_token_vector.push_back(token);
    this->process(token);
}

void LexicalBlock::start_comma(const SimpleToken& token)
{
    if (token.type != SimpleTokenType::Comma)
        throw std::invalid_argument("Invalid comma");
        
    this->combined_token_vector.push_back(token);
    this->process(token);
}

void LexicalBlock::start_colon(const SimpleToken& token)
{
    if (token.type != SimpleTokenType::Colon)
        throw std::invalid_argument("Invalid colon");
    
    this->combined_token_vector.push_back(token);
    this->process(token);
}

void LexicalBlock::start_semicolon(const SimpleToken& token)
{
    if (token.type != SimpleTokenType::Semicolon)
        throw std::invalid_argument("Invalid semicolon");

    this->combined_token_vector.push_back(token);
    this->process(token);
}

void LexicalBlock::start_quotation_mark(const SimpleToken& token)
{
    if (token.type != SimpleTokenType::QuotationMark)
        throw std::invalid_argument("Invalid quotation mark");

    this->combined_token_vector.push_back(token);
    this->process(token);
}

void LexicalBlock::process_symbol_in_state_SPACE(const SimpleToken& token, std::optional<char> symbol = std::nullopt)
{
    switch (token.type) {
        case SimpleTokenType::Letter:
            if (!symbol.has_value())
                throw std::invalid_argument("Invalid letter");
            this->start_identifier(token, symbol.value());
            this->switch_state(LexicalBlockState::IdentifierBegin);
            return;
        case SimpleTokenType::Digit:
            if (!symbol.has_value())
                throw std::invalid_argument("Invalid digit");
            this->start_integer(token, symbol.value());
            this->switch_state(LexicalBlockState::Integer);
            return;
        case SimpleTokenType::OpeningCurlyBrace:
            this->start_opening_curly_brace(token);
            return;
        case SimpleTokenType::ClosingCurlyBrace:
            this->start_closing_curly_brace(token);
            return;
        case SimpleTokenType::OpeningParenthesis:
            this->start_opening_parenthesis(token);
            return;
        case SimpleTokenType::ClosingParenthesis:
            this->start_closing_parenthesis(token);
            return;
        case SimpleTokenType::Comma:
            this->start_comma(token);
            return;
        case SimpleTokenType::Colon:
            this->start_colon(token);
            return;
        case SimpleTokenType::Semicolon:
            this->start_semicolon(token);
            return;
        case SimpleTokenType::QuotationMark:
            this->start_quotation_mark(token);
            return;
        case SimpleTokenType::EqualSign:
            this->start_equal_sign(token);
            return;
        case SimpleTokenType::ExclamationMark:
            this->start_exclamation_mark(token);
            return;
        case SimpleTokenType::ComparisonSign:
            this->start_comparison_sign(token);
            return;
        case SimpleTokenType::ArithmeticSign:
            this->start_arithmetic_sign(token);
            return;
        case SimpleTokenType::LogicalSign:
            this->start_logical_sign(token);
            return;
        case SimpleTokenType::SpaceSign:
            return;
        case SimpleTokenType::Other:
            throw std::invalid_argument("Invalid symbol: '" + std::string(1, symbol.value()) + "', index: " + std::to_string(token.index));
    }
}

void LexicalBlock::process_symbol_in_state_IDENTIFIER_BEGIN(const SimpleToken& token, std::optional<char> symbol = std::nullopt)
{
    switch (token.type) {
        case SimpleTokenType::Letter:
            if (!symbol.has_value())
                throw std::invalid_argument("Invalid letter");
            this->process(token, symbol.value());
            this->switch_state(LexicalBlockState::IdentifierBegin);
            return;
        case SimpleTokenType::Digit:
            if (!symbol.has_value())
                throw std::invalid_argument("Invalid digit");
            this->process(token, symbol.value());
            this->switch_state(LexicalBlockState::IdentifierEnd);
            return;
        case SimpleTokenType::OpeningCurlyBrace:
            this->start_opening_curly_brace(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::ClosingCurlyBrace:
            this->start_closing_curly_brace(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::OpeningParenthesis:
            this->start_opening_parenthesis(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::ClosingParenthesis:
            this->start_closing_parenthesis(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::Comma:
            this->start_comma(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::Colon:
            this->start_colon(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::Semicolon:
            this->start_semicolon(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::QuotationMark:
            this->start_quotation_mark(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::EqualSign:
            this->start_equal_sign(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::ExclamationMark:
            this->start_exclamation_mark(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::ComparisonSign:
            this->start_comparison_sign(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::ArithmeticSign:
            this->start_arithmetic_sign(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::LogicalSign:
            this->start_logical_sign(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::SpaceSign:
        case SimpleTokenType::Other:
            this->switch_state(LexicalBlockState::Space);
            return;
    }
}

void LexicalBlock::process_symbol_in_state_IDENTIFIER_END(const SimpleToken& token, std::optional<char> symbol = std::nullopt)
{
    switch (token.type) {
        case SimpleTokenType::Letter:
            if (!symbol.has_value())
                throw std::invalid_argument("Invalid letter");
            this->start_identifier(token, symbol.value());
            this->switch_state(LexicalBlockState::IdentifierBegin);
            return;
        case SimpleTokenType::Digit:
            if (!symbol.has_value())
                throw std::invalid_argument("Invalid digit");
            this->process(token, symbol.value());
            this->switch_state(LexicalBlockState::IdentifierEnd);
            return;
        case SimpleTokenType::OpeningCurlyBrace:
            this->start_opening_curly_brace(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::ClosingCurlyBrace:
            this->start_closing_curly_brace(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::OpeningParenthesis:
            this->start_opening_parenthesis(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::ClosingParenthesis:
            this->start_closing_parenthesis(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::Comma:
            this->start_comma(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::Colon:
            this->start_colon(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::Semicolon:
            this->start_semicolon(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::QuotationMark:
            this->start_quotation_mark(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::EqualSign:
            this->start_equal_sign(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::ExclamationMark:
            this->start_exclamation_mark(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::ComparisonSign:
            this->start_comparison_sign(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::ArithmeticSign:
            this->start_arithmetic_sign(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::LogicalSign:
            this->start_logical_sign(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::SpaceSign:
        case SimpleTokenType::Other:
            this->switch_state(LexicalBlockState::Space);
            return;
    }
}

void LexicalBlock::process_symbol_in_state_INTEGER(const SimpleToken& token, std::optional<char> symbol = std::nullopt)
{
    switch (token.type) {
        case SimpleTokenType::Letter:
            if (!symbol.has_value())
                throw std::invalid_argument("Invalid letter");
            this->start_identifier(token, symbol.value());
            this->switch_state(LexicalBlockState::IdentifierBegin);
            return;
        case SimpleTokenType::Digit:
            if (!symbol.has_value())
                throw std::invalid_argument("Invalid digit");
            this->process(token, symbol.value());
            this->switch_state(LexicalBlockState::Integer);
            return;
        case SimpleTokenType::OpeningCurlyBrace:
            this->start_opening_curly_brace(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::ClosingCurlyBrace:
            this->start_closing_curly_brace(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::OpeningParenthesis:
            this->start_opening_parenthesis(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::ClosingParenthesis:
            this->start_closing_parenthesis(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::Comma:
            this->start_comma(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::Colon:
            this->start_colon(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::Semicolon:
            this->start_semicolon(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::QuotationMark:
            this->start_quotation_mark(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::EqualSign:
            this->start_equal_sign(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::ExclamationMark:
            this->start_exclamation_mark(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::ComparisonSign:
            this->start_comparison_sign(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::ArithmeticSign:
            this->start_arithmetic_sign(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::LogicalSign:
            this->start_logical_sign(token);
            this->switch_state(LexicalBlockState::Space);
            return;
        case SimpleTokenType::SpaceSign:
        case SimpleTokenType::Other:
            this->switch_state(LexicalBlockState::Space);
            return;
    }
}

void LexicalBlock::process_symbol_depending_on_state(const SimpleToken& token, std::optional<char> symbol = std::nullopt)
{
    switch (this->state) {
        case LexicalBlockState::Space:
            this->process_symbol_in_state_SPACE(token, symbol);
            break;
        case LexicalBlockState::IdentifierBegin:
            this->process_symbol_in_state_IDENTIFIER_BEGIN(token, symbol);
            break;
        case LexicalBlockState::IdentifierEnd:
            this->process_symbol_in_state_IDENTIFIER_END(token, symbol);
            break;
        case LexicalBlockState::Integer:
            this->process_symbol_in_state_INTEGER(token, symbol);
            break;
    }
}

std::vector<VariableToken> LexicalBlock::transliterate_string(const std::string& str)
{
    TransliterationBlock transliteration_block = TransliterationBlock();
    for (int i = 0; i < (int)str.size(); i++) {
        char symbol = str[i];
        int index = i;
        SimpleToken token = transliteration_block.transliterate_symbol(symbol, index);
        this->process_symbol_depending_on_state(token, symbol);
    }
    
    std::vector<VariableToken> result = this->combined_token_vector;
    this->combined_token_vector.clear();
    return result;
}