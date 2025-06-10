#include "SyntaxBlock.hpp"

using VariableToken = std::variant<SimpleToken, ComplexToken>;

CFGTemplate::CFGTemplate(SyntaxBlock* syntax_block) : syntax_block(syntax_block) {}

CFGTemplateInner::CFGTemplateInner(SyntaxBlock* syntax_block) : syntax_block(syntax_block) {}

SyntaxBlock::SyntaxBlock(SyntaxBlockWorkingMode working_mode)
{
    this->working_mode = working_mode;
    this->string_index = 0;
    this->current_token_index_in_vector = 0;
}

SyntaxBlock::~SyntaxBlock()
{
    this->token_vector.clear();
}

SyntaxBlockWorkingMode SyntaxBlock::get_working_mode()
{
    return this->working_mode;
}

void SyntaxBlock::set_working_mode(SyntaxBlockWorkingMode working_mode)
{
    this->working_mode = working_mode;
}

int SyntaxBlock::get_string_index()
{
    return this->string_index;
}

void SyntaxBlock::set_string_index(int index)
{
    this->string_index = index;
}

void SyntaxBlock::load_token_vector(const std::vector<VariableToken>& token_vector)
{
    this->token_vector = token_vector;
    this->current_token_index_in_vector = 0;
}

void SyntaxBlock::load_token()
{
    this->current_token_index_in_vector++;
}

VariableToken SyntaxBlock::get_current_token()
{
    return this->token_vector[this->current_token_index_in_vector];
}

int SyntaxBlock::get_current_token_index()
{
    auto token = this->get_current_token();
    if (std::holds_alternative<SimpleToken>(token)) {
        return std::get<SimpleToken>(token).index;
    }
    else {
        return std::get<ComplexToken>(token).index;
    }
}

void SyntaxBlock::cancel_load_token()
{
    this->current_token_index_in_vector--;
}

Message SyntaxBlock::make_message(const VariableToken& token, const std::string& message_pool, const std::string& message_pool_identifier)
{
    Message full_message;
    if (std::holds_alternative<SimpleToken>(token)) {
        full_message = Message(this->get_string_index(), std::get<SimpleToken>(token).index, message_pool, message_pool_identifier);
    }
    else {
        full_message = Message(this->get_string_index(), std::get<ComplexToken>(token).index, std::get<ComplexToken>(token).value, message_pool, message_pool_identifier);
    }
    return full_message;
}

bool SyntaxBlock::check_token_vector(const std::vector<std::variant<SimpleToken, ComplexToken>>& combined_tokens, SyntaxBlockWorkingMode working_mode)
{
    this->load_token_vector(combined_tokens);
    
    if (this->get_working_mode() != working_mode) {
        this->set_working_mode(working_mode);
    }

    if (working_mode == SyntaxBlockWorkingMode::AllErrors) {
        SyntaxBlock::CFG::String(this).check_all_inner_parts(true, false);
    }
    else if (working_mode == SyntaxBlockWorkingMode::AllErrorsInDetail) {
        SyntaxBlock::CFG::String(this).check_all_inner_parts(true, true);
    }

    if (working_mode == SyntaxBlockWorkingMode::UntilFirstError) {
        return SyntaxBlock::CFG::String(this).check(true);
    }
    else {
        return SyntaxBlock::CFG::String(this).check(false);
    }
    return false;
}


// Symbol
void SyntaxBlock::CFG::Symbol::OpeningCurlyBrace::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "symbol", "opening_curly_brace"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Symbol::OpeningCurlyBrace::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<SimpleToken>(token)) {
        SimpleToken token_opening_curly_brace = std::get<SimpleToken>(token);
        if (token_opening_curly_brace.type == SimpleTokenType::OpeningCurlyBrace) {
            syntax_block->load_token();
            return true;
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Symbol::ClosingCurlyBrace::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "symbol", "closing_curly_brace"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Symbol::ClosingCurlyBrace::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<SimpleToken>(token)) {
        SimpleToken token_closing_curly_brace = std::get<SimpleToken>(token);
        if (token_closing_curly_brace.type == SimpleTokenType::ClosingCurlyBrace) {
            syntax_block->load_token();
            return true;
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Symbol::OpeningParenthesis::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "symbol", "opening_parenthesis"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Symbol::OpeningParenthesis::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<SimpleToken>(token)) {
        SimpleToken token_opening_parenthesis = std::get<SimpleToken>(token);
        if (token_opening_parenthesis.type == SimpleTokenType::OpeningParenthesis) {
            syntax_block->load_token();
            return true;
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Symbol::ClosingParenthesis::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "symbol", "closing_parenthesis"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Symbol::ClosingParenthesis::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<SimpleToken>(token)) {
        SimpleToken token_closing_parenthesis = std::get<SimpleToken>(token);
        if (token_closing_parenthesis.type == SimpleTokenType::ClosingParenthesis) {
            syntax_block->load_token();
            return true;
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Symbol::Comma::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "symbol", "comma"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Symbol::Comma::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<SimpleToken>(token)) {
        SimpleToken token_comma = std::get<SimpleToken>(token);
        if (token_comma.type == SimpleTokenType::Comma) {
            syntax_block->load_token();
            return true;
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Symbol::Colon::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "symbol", "colon"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Symbol::Colon::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<SimpleToken>(token)) {
        SimpleToken token_colon = std::get<SimpleToken>(token);
        if (token_colon.type == SimpleTokenType::Colon) {
            syntax_block->load_token();
            return true;
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Symbol::Semicolon::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "symbol", "semicolon"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Symbol::Semicolon::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<SimpleToken>(token)) {
        SimpleToken token_semicolon = std::get<SimpleToken>(token);
        if (token_semicolon.type == SimpleTokenType::Semicolon) {
            syntax_block->load_token();
            return true;
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Symbol::QuotationMark::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "symbol", "quotation_mark"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Symbol::QuotationMark::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<SimpleToken>(token)) {
        SimpleToken token_quotation_mark = std::get<SimpleToken>(token);
        if (token_quotation_mark.type == SimpleTokenType::QuotationMark) {
            syntax_block->load_token();
            return true;
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Symbol::EqualSign::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "symbol", "equal_sign"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Symbol::EqualSign::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<SimpleToken>(token)) {
        SimpleToken token_equal_sign = std::get<SimpleToken>(token);
        if (token_equal_sign.type == SimpleTokenType::EqualSign) {
            syntax_block->load_token();
            return true;
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Symbol::ComparisonSign::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "symbol", "comparison_sign"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Symbol::ComparisonSign::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<SimpleToken>(token)) {
        SimpleToken token_comparison_sign = std::get<SimpleToken>(token);
        if (token_comparison_sign.type == SimpleTokenType::ComparisonSign) {
            syntax_block->load_token();
            return true;
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Symbol::ExclamationMark::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "symbol", "exclamation_mark"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Symbol::ExclamationMark::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<SimpleToken>(token)) {
        SimpleToken token_exclamation_mark = std::get<SimpleToken>(token);
        if (token_exclamation_mark.type == SimpleTokenType::ExclamationMark) {
            syntax_block->load_token();
            return true;
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Symbol::ArithmeticSign::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "symbol", "arithmetic_sign"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Symbol::ArithmeticSign::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<SimpleToken>(token)) {
        SimpleToken token_arithmetic_sign = std::get<SimpleToken>(token);
        if (token_arithmetic_sign.type == SimpleTokenType::ArithmeticSign) {
            syntax_block->load_token();
            return true;
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Symbol::LogicalSign::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "symbol", "logical_sign"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Symbol::LogicalSign::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<SimpleToken>(token)) {
        SimpleToken token_logical_sign = std::get<SimpleToken>(token);
        if (token_logical_sign.type == SimpleTokenType::LogicalSign) {
            syntax_block->load_token();
            return true;
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::SpecialIdentifier::Operation::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "special_identifier", "operation"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::SpecialIdentifier::Operation::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<ComplexToken>(token)) {
        ComplexToken token_operation_special_identifier = std::get<ComplexToken>(token);
        if (std::holds_alternative<SpecialIdentifierType>(token_operation_special_identifier.type)) {
            SpecialIdentifierType token_operation_special_identifier_type = std::get<SpecialIdentifierType>(token_operation_special_identifier.type);
            if (token_operation_special_identifier_type == SpecialIdentifierType::Operation) {
                syntax_block->load_token();
                return true;
            }
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::SpecialIdentifier::OperandOfUnaryOperation::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "special_identifier", "operand_of_unary_operation"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::SpecialIdentifier::OperandOfUnaryOperation::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<ComplexToken>(token)) {
        ComplexToken token_operand_of_unary_operation_special_identifier = std::get<ComplexToken>(token);
        if (std::holds_alternative<SpecialIdentifierType>(token_operand_of_unary_operation_special_identifier.type)) {
            SpecialIdentifierType token_operand_of_unary_operation_special_identifier_type = std::get<SpecialIdentifierType>(token_operand_of_unary_operation_special_identifier.type);
            if (token_operand_of_unary_operation_special_identifier_type == SpecialIdentifierType::OperandOfUnaryOperation) {
                syntax_block->load_token();
                return true;
            }
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::SpecialIdentifier::FirstOperandOfBinaryOperation::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "special_identifier", "first_operand_of_binary_operation"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::SpecialIdentifier::FirstOperandOfBinaryOperation::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<ComplexToken>(token)) {
        ComplexToken token_first_operand_of_binary_operation_special_identifier = std::get<ComplexToken>(token);
        if (std::holds_alternative<SpecialIdentifierType>(token_first_operand_of_binary_operation_special_identifier.type)) {
            SpecialIdentifierType token_first_operand_of_binary_operation_special_identifier_type = std::get<SpecialIdentifierType>(token_first_operand_of_binary_operation_special_identifier.type);
            if (token_first_operand_of_binary_operation_special_identifier_type == SpecialIdentifierType::FirstOperandOfBinaryOperation) {
                syntax_block->load_token();
                return true;
            }
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::SpecialIdentifier::SecondOperandOfBinaryOperation::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "special_identifier", "second_operand_of_binary_operation"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::SpecialIdentifier::SecondOperandOfBinaryOperation::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<ComplexToken>(token)) {
        ComplexToken token_second_operand_of_binary_operation_special_identifier = std::get<ComplexToken>(token);
        if (std::holds_alternative<SpecialIdentifierType>(token_second_operand_of_binary_operation_special_identifier.type)) {
            SpecialIdentifierType token_second_operand_of_binary_operation_special_identifier_type = std::get<SpecialIdentifierType>(token_second_operand_of_binary_operation_special_identifier.type);
            if (token_second_operand_of_binary_operation_special_identifier_type == SpecialIdentifierType::SecondOperandOfBinaryOperation) {
                syntax_block->load_token();
                return true;
            }
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::SpecialIdentifier::V::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "special_identifier", "v"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::SpecialIdentifier::V::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<ComplexToken>(token)) {
        ComplexToken token_v_special_identifier = std::get<ComplexToken>(token);
        if (std::holds_alternative<SpecialIdentifierType>(token_v_special_identifier.type)) {
            SpecialIdentifierType token_v_special_identifier_type = std::get<SpecialIdentifierType>(token_v_special_identifier.type);
            if (token_v_special_identifier_type == SpecialIdentifierType::V) {
                syntax_block->load_token();
                return true;
            }
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::SpecialIdentifier::No::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "special_identifier", "no"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::SpecialIdentifier::No::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<ComplexToken>(token)) {
        ComplexToken token_no_special_identifier = std::get<ComplexToken>(token);
        if (std::holds_alternative<SpecialIdentifierType>(token_no_special_identifier.type)) {
            SpecialIdentifierType token_no_special_identifier_type = std::get<SpecialIdentifierType>(token_no_special_identifier.type);
            if (token_no_special_identifier_type == SpecialIdentifierType::No) {
                syntax_block->load_token();
                return true;
            }
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::SpecialIdentifier::Real::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "special_identifier", "real"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::SpecialIdentifier::Real::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<ComplexToken>(token)) {
        ComplexToken token_real_special_identifier = std::get<ComplexToken>(token);
        if (std::holds_alternative<SpecialIdentifierType>(token_real_special_identifier.type)) {
            SpecialIdentifierType token_real_special_identifier_type = std::get<SpecialIdentifierType>(token_real_special_identifier.type);
            if (token_real_special_identifier_type == SpecialIdentifierType::Real) {
                syntax_block->load_token();
                return true;
            }
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::SpecialIdentifier::Solution::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "special_identifier", "solution"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::SpecialIdentifier::Solution::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<ComplexToken>(token)) {
        ComplexToken token_solution_special_identifier = std::get<ComplexToken>(token);
        if (std::holds_alternative<SpecialIdentifierType>(token_solution_special_identifier.type)) {
            SpecialIdentifierType token_solution_special_identifier_type = std::get<SpecialIdentifierType>(token_solution_special_identifier.type);
            if (token_solution_special_identifier_type == SpecialIdentifierType::Solution) {
                syntax_block->load_token();
                return true;
            }
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::SpecialIdentifier::Modulus::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "special_identifier", "modulus"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::SpecialIdentifier::Modulus::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<ComplexToken>(token)) {
        ComplexToken token_modulus_special_identifier = std::get<ComplexToken>(token);
        if (std::holds_alternative<SpecialIdentifierType>(token_modulus_special_identifier.type)) {
            SpecialIdentifierType token_modulus_special_identifier_type = std::get<SpecialIdentifierType>(token_modulus_special_identifier.type);
            if (token_modulus_special_identifier_type == SpecialIdentifierType::Modulus) {
                syntax_block->load_token();
                return true;
            }
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::SpecialIdentifier::SquareOfNumber::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "special_identifier", "square_of_number"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::SpecialIdentifier::SquareOfNumber::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<ComplexToken>(token)) {
        ComplexToken token_square_of_number_special_identifier = std::get<ComplexToken>(token);
        if (std::holds_alternative<SpecialIdentifierType>(token_square_of_number_special_identifier.type)) {
            SpecialIdentifierType token_square_of_number_special_identifier_type = std::get<SpecialIdentifierType>(token_square_of_number_special_identifier.type);
            if (token_square_of_number_special_identifier_type == SpecialIdentifierType::SquareOfNumber) {
                syntax_block->load_token();
                return true;
            }
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::SpecialIdentifier::SquareRootOfNumber::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "special_identifier", "square_root_of_number"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::SpecialIdentifier::SquareRootOfNumber::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<ComplexToken>(token)) {
        ComplexToken token_square_root_of_number_special_identifier = std::get<ComplexToken>(token);
        if (std::holds_alternative<SpecialIdentifierType>(token_square_root_of_number_special_identifier.type)) {
            SpecialIdentifierType token_square_root_of_number_special_identifier_type = std::get<SpecialIdentifierType>(token_square_root_of_number_special_identifier.type);
            if (token_square_root_of_number_special_identifier_type == SpecialIdentifierType::SquareRootOfNumber) {
                syntax_block->load_token();
                return true;
            }
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Variable::Identifier::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "variable", "identifier"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Variable::Identifier::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;
    
    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<ComplexToken>(token)) {
        ComplexToken token_identifier = std::get<ComplexToken>(token);
        if (std::holds_alternative<ComplexTokenType>(token_identifier.type)) {
            ComplexTokenType token_identifier_type = std::get<ComplexTokenType>(token_identifier.type);
            if (token_identifier_type != ComplexTokenType::Identifier) {
                cancel(is_sending_signal);
                return false;
            }
        }

        syntax_block->load_token();
        return true;
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Variable::Integer::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "variable", "integer"));
    }
    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Variable::Integer::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    VariableToken token = syntax_block->get_current_token();
    if (std::holds_alternative<ComplexToken>(token)) {
        ComplexToken token_integer = std::get<ComplexToken>(token);
        if (std::holds_alternative<ComplexTokenType>(token_integer.type)) {
            ComplexTokenType token_integer_type = std::get<ComplexTokenType>(token_integer.type);
            if (token_integer_type != ComplexTokenType::Integer) {
                cancel(is_sending_signal);
                return false;
            }

            syntax_block->load_token();
            return true;
        }
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Variable::Edge::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "variable", "edge"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Variable::Edge::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (!SyntaxBlock::CFG::Variable::Identifier(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }

    if (!SyntaxBlock::CFG::Symbol::OpeningParenthesis(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }

    if (!SyntaxBlock::CFG::Variable::Integer(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }

    if (!SyntaxBlock::CFG::Symbol::Comma(syntax_block).check(false)) {
        if (!SyntaxBlock::CFG::Symbol::ClosingParenthesis(syntax_block).check(is_sending_signal)) {
            cancel(is_sending_signal);
            return false;
        }

        return true;
    }
    else {
        if (!SyntaxBlock::CFG::Variable::Integer(syntax_block).check(is_sending_signal)) {
            cancel(is_sending_signal);
            return false;
        }

        if (!SyntaxBlock::CFG::Symbol::ClosingParenthesis(syntax_block).check(is_sending_signal)) {
            cancel(is_sending_signal);
            return false;
        }

        return true;
    }
}

void SyntaxBlock::CFG::Variable::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "variable", "variable"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Variable::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (SyntaxBlock::CFG::Variable::Edge(syntax_block).check(false)) {
        return true;
    }
    else if (SyntaxBlock::CFG::Variable::Identifier(syntax_block).check(false)) {
        return true;
    }
    else if (SyntaxBlock::CFG::Variable::Integer(syntax_block).check(false)) {
        return true;
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Operation::Logical::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "operation", "logical"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Operation::Logical::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (SyntaxBlock::CFG::Symbol::ExclamationMark(syntax_block).check(false)) {
        if (SyntaxBlock::CFG::Symbol::EqualSign(syntax_block).check(false)) {
            return true;
        }

        cancel(is_sending_signal);
        return false;
    }
    else if (SyntaxBlock::CFG::Symbol::EqualSign(syntax_block).check(false)) {
        if (SyntaxBlock::CFG::Symbol::EqualSign(syntax_block).check(false)) {
            return true;
        }

        cancel(is_sending_signal);
        return false;
    }
    else if (SyntaxBlock::CFG::Symbol::ComparisonSign(syntax_block).check(false)) {
        if (SyntaxBlock::CFG::Symbol::EqualSign(syntax_block).check(false)) {
            return true;
        }

        return true;
    }
    else if (SyntaxBlock::CFG::Symbol::LogicalSign(syntax_block).check(false)) {
        return true;
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Operation::Arithmetic::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "operation", "arithmetic"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Operation::Arithmetic::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (SyntaxBlock::CFG::Symbol::ArithmeticSign(syntax_block).check(false)) {
        return true;
    }
    if (SyntaxBlock::CFG::SpecialIdentifier::Modulus(syntax_block).check(false)) {
        return true;
    }
    if (SyntaxBlock::CFG::SpecialIdentifier::SquareOfNumber(syntax_block).check(false)) {
        return true;
    }
    if (SyntaxBlock::CFG::SpecialIdentifier::SquareRootOfNumber(syntax_block).check(false)) {
        return true;
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::Operation::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "operation", "operation"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::Operation::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (SyntaxBlock::CFG::Operation::Logical(syntax_block).check(false)) {
        return true;
    }
    if (SyntaxBlock::CFG::Operation::Arithmetic(syntax_block).check(false)) {
        return true;
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::String::Beginning::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "string", "beginning"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::String::Beginning::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (SyntaxBlock::CFG::Variable::Edge(syntax_block).check(false)) {
        return true;
    }
    if (SyntaxBlock::CFG::Variable::Identifier(syntax_block).check(false)) {
        return true;
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::String::Inner::Operation::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "string_inner", "operation"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::String::Inner::Operation::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::SpecialIdentifier::Operation(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::Colon(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Operation(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    return true;
}

void SyntaxBlock::CFG::String::Inner::Operand::Unary::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "string_inner", "operand_unary"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::String::Inner::Operand::Unary::check(bool is_sending_signal, bool isCheckingInner)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::SpecialIdentifier::OperandOfUnaryOperation(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::Colon(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }

    auto opening_curly_brace = SyntaxBlock::CFG::Symbol::OpeningCurlyBrace(syntax_block);
    if (opening_curly_brace.check(false)) {
        opening_curly_brace.cancel(false);
        if (!isCheckingInner) {
            return true;
        }
        if (!SyntaxBlock::CFG::String::Inner(syntax_block).check(is_sending_signal)) {
            cancel(is_sending_signal);
            return false;
        }
        return true;
    }
    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Variable(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    return true;
}

void SyntaxBlock::CFG::String::Inner::Operand::Binary::First::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "string_inner", "first_operand_of_binary_operation"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::String::Inner::Operand::Binary::First::check(bool is_sending_signal, bool isCheckingInner)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::SpecialIdentifier::FirstOperandOfBinaryOperation(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::Colon(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }

    auto opening_curly_brace = SyntaxBlock::CFG::Symbol::OpeningCurlyBrace(syntax_block);
    if (opening_curly_brace.check(false)) {
        opening_curly_brace.cancel(false);
        if (!isCheckingInner) {
            return true;
        }
        if (!SyntaxBlock::CFG::String::Inner(syntax_block).check(is_sending_signal)) {
            cancel(is_sending_signal);
            return false;
        }
        return true;
    }
    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Variable(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    return true;
}

void SyntaxBlock::CFG::String::Inner::Operand::Binary::Second::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "string_inner", "second_operand_of_binary_operation"));
    }
    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::String::Inner::Operand::Binary::Second::check(bool is_sending_signal, bool isCheckingInner)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::SpecialIdentifier::SecondOperandOfBinaryOperation(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::Colon(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }

    auto opening_curly_brace = SyntaxBlock::CFG::Symbol::OpeningCurlyBrace(syntax_block);
    if (opening_curly_brace.check(false)) {
        opening_curly_brace.cancel(false);
        if (!isCheckingInner) {
            return true;
        }
        if (!SyntaxBlock::CFG::String::Inner(syntax_block).check(is_sending_signal)) {
            cancel(is_sending_signal);
            return false;
        }
        return true;
    }
    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Variable(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    return true;
}

void SyntaxBlock::CFG::String::Inner::Operand::Binary::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "string_inner", "operand_binary"));
    }
    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::String::Inner::Operand::Binary::check(bool is_sending_signal, bool isCheckingInner)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (!SyntaxBlock::CFG::String::Inner::Operand::Binary::First(syntax_block).check(is_sending_signal, isCheckingInner)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::Comma(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::String::Inner::Operand::Binary::Second(syntax_block).check(is_sending_signal, isCheckingInner)) {
        cancel(is_sending_signal);
        return false;
    }
    return true;
}

void SyntaxBlock::CFG::String::Inner::Operand::Variable::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "string_inner", "operand_variable"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::String::Inner::Operand::Variable::check(bool is_sending_signal, bool isCheckingInner)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (SyntaxBlock::CFG::String::Inner::Operand::Unary(syntax_block).check(false, isCheckingInner)) {
        return true;
    }
    if (SyntaxBlock::CFG::String::Inner::Operand::Binary::First(syntax_block).check(false, isCheckingInner)) {
        return true;
    }
    if (SyntaxBlock::CFG::String::Inner::Operand::Binary::Second(syntax_block).check(false, isCheckingInner)) {
        return true;
    }
    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::String::Inner::Operand::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "string_inner", "operand"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::String::Inner::Operand::check(bool is_sending_signal, bool isCheckingInner)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (SyntaxBlock::CFG::String::Inner::Operand::Unary(syntax_block).check(false, isCheckingInner)) {
        return true;
    }
    if (SyntaxBlock::CFG::String::Inner::Operand::Binary(syntax_block).check(false, isCheckingInner)) {
        return true;
    }
    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::String::Inner::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "string_inner", "inner"));
    }
    
    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::String::Inner::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (!SyntaxBlock::CFG::Symbol::OpeningCurlyBrace(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::String::Inner::Operation(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::Comma(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::String::Inner::Operand(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::ClosingCurlyBrace(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    return true;
}

void SyntaxBlock::CFG::String::Logical::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "string", "logical"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::String::Logical::check(bool is_sending_signal, bool isCheckingInner)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    auto opening_curly_brace = SyntaxBlock::CFG::Symbol::OpeningCurlyBrace(syntax_block);
    if (opening_curly_brace.check(false)) {
        opening_curly_brace.cancel(false);
        if (!isCheckingInner) {
            return true;
        }
        if (!SyntaxBlock::CFG::String::Inner(syntax_block).check(is_sending_signal)) {
            cancel(is_sending_signal);
            return false;
        }
        return true;
    }

    // если нет логической части (то есть вид `[...]=;[...]`), проверяем, есть ли впереди знак `;` и возвращаемся обратно
    if (SyntaxBlock::CFG::Symbol::Semicolon(syntax_block).check(false)) {
        cancel(false);
        return true;
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::String::Arithmetic::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "string", "arithmetic"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::String::Arithmetic::check(bool is_sending_signal, bool isCheckingInner)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (SyntaxBlock::CFG::String::Arithmetic::NoRealSolution(syntax_block).check(false)) {
        return true;
    }
    
    auto opening_curly_brace = SyntaxBlock::CFG::Symbol::OpeningCurlyBrace(syntax_block);
    if (opening_curly_brace.check(false)) {
        opening_curly_brace.cancel(false);
        if (!isCheckingInner) {
            return true;
        }
        if (!SyntaxBlock::CFG::String::Inner(syntax_block).check(is_sending_signal)) {
            cancel(is_sending_signal);
            return false;
        }
        return true;
    }
    if (SyntaxBlock::CFG::Variable::Integer(syntax_block).check(false)) {
        return true;
    }
    if (SyntaxBlock::CFG::Variable::Edge(syntax_block).check(false)) {
        return true;
    }

    cancel(is_sending_signal);
    return false;
}

void SyntaxBlock::CFG::String::Arithmetic::NoRealSolution::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "string", "arithmetic_no_real_solution"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::String::Arithmetic::NoRealSolution::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (!SyntaxBlock::CFG::Symbol::OpeningCurlyBrace(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::SpecialIdentifier::V(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::Colon(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::SpecialIdentifier::No(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::SpecialIdentifier::Real(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::SpecialIdentifier::Solution(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::ClosingCurlyBrace(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    
    return true;
}

void SyntaxBlock::CFG::String::cancel(bool is_sending_signal)
{
    if (is_sending_signal) {
        on_error_occurs(syntax_block->make_message(syntax_block->get_current_token(), "string", "string"));
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}

bool SyntaxBlock::CFG::String::check(bool is_sending_signal)
{
    beginning_index = syntax_block->current_token_index_in_vector;

    if (!SyntaxBlock::CFG::String::Beginning(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::EqualSign(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::String::Logical(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::Symbol::Semicolon(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (!SyntaxBlock::CFG::String::Arithmetic(syntax_block).check(is_sending_signal)) {
        cancel(is_sending_signal);
        return false;
    }
    if (syntax_block->current_token_index_in_vector != (int)syntax_block->token_vector.size()) {
        cancel(is_sending_signal);
        return false;
    }

    return true;
}

void SyntaxBlock::CFG::String::check_all_inner_parts(bool is_sending_signal, bool isCheckingInner)
{
    beginning_index = syntax_block->current_token_index_in_vector;
    for (int current_index = beginning_index; current_index < (int)syntax_block->token_vector.size(); current_index++) {
        syntax_block->current_token_index_in_vector = current_index;
        auto identifier = SyntaxBlock::CFG::Variable::Identifier(syntax_block);
        if (SyntaxBlock::CFG::Symbol::EqualSign(syntax_block).check(false)) {
            auto opening_curly_brace = SyntaxBlock::CFG::Symbol::OpeningCurlyBrace(syntax_block);
            if (opening_curly_brace.check(false)) {
                opening_curly_brace.cancel(false);
                if (SyntaxBlock::CFG::String::Logical(syntax_block).check(is_sending_signal, isCheckingInner)) {
                    current_index = syntax_block->current_token_index_in_vector-1;
                }
            }
        }
        else if (SyntaxBlock::CFG::Symbol::Semicolon(syntax_block).check(false)) {
            if (SyntaxBlock::CFG::String::Arithmetic(syntax_block).check(is_sending_signal, isCheckingInner)) {
                current_index = syntax_block->current_token_index_in_vector-1;
            }
        }
        // проверка граней (Edge)
        else if (identifier.check(false)) {
            auto opening_parenthesis = SyntaxBlock::CFG::Symbol::OpeningParenthesis(syntax_block);
            if (opening_parenthesis.check(false)) {
                opening_parenthesis.cancel(false);
                identifier.cancel(false);
                
                if (SyntaxBlock::CFG::Variable::Edge(syntax_block).check(is_sending_signal)) {
                    current_index = syntax_block->current_token_index_in_vector-1;
                }
            }
        }
        // проверка операций (Operation)
        else if (SyntaxBlock::CFG::Symbol::OpeningCurlyBrace(syntax_block).check(false)) {
            if (SyntaxBlock::CFG::String::Inner::Operation(syntax_block).check(is_sending_signal)) {
                current_index = syntax_block->current_token_index_in_vector-1;
            }
        }
        // проверка операндов (Operand)
        else if (SyntaxBlock::CFG::Symbol::Comma(syntax_block).check(false)) {
            auto quotation_mark = SyntaxBlock::CFG::Symbol::QuotationMark(syntax_block);
            // проверяем на кавычки, т. к. запятые могут встречаться в гранях (Edge)
            if (quotation_mark.check(false)) {
                quotation_mark.cancel(false);

                if (SyntaxBlock::CFG::String::Inner::Operand::Variable(syntax_block).check(is_sending_signal, isCheckingInner)) {
                    current_index = syntax_block->current_token_index_in_vector-1;
                }
            }
        }
    }

    syntax_block->current_token_index_in_vector = beginning_index;
}