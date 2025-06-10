#pragma once

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include "../tokens/Tokens.hpp"
#include "TransliterationBlock.hpp"
#include "SpecialIdentifierBlock.hpp"

using VariableToken = std::variant<SimpleToken, ComplexToken>;

enum class LexicalBlockState {
    Space,
    IdentifierBegin,
    IdentifierEnd,
    Integer,
};

// класс реализует алгоритм работы ДКА
class LexicalBlock {
public:
    LexicalBlock();
    std::vector<VariableToken> transliterate_simple_token_vector(std::vector<VariableToken>& tokens);
    std::vector<VariableToken> transliterate_string(const std::string& str);

private:
    std::vector<VariableToken> combined_token_vector;      // итоговый список лексем

    LexicalBlockState state;     // состояние автомата
    void switch_state(LexicalBlockState new_state);

    void process(const VariableToken& token, std::optional<char> symbol);
    // сложные (составные) лексемы
    void start_identifier(const SimpleToken& token, char symbol);
    void start_integer(const SimpleToken& token, char symbol);
    // простые (односимвольные) лексемы
    void start_arithmetic_sign(const SimpleToken& token);
    void start_comparison_sign(const SimpleToken& token);
    void start_logical_sign(const SimpleToken& token);
    void start_exclamation_mark(const SimpleToken& token);
    void start_equal_sign(const SimpleToken& token);
    void start_opening_curly_brace(const SimpleToken& token);
    void start_closing_curly_brace(const SimpleToken& token);
    void start_opening_parenthesis(const SimpleToken& token);
    void start_closing_parenthesis(const SimpleToken& token);
    void start_comma(const SimpleToken& token);
    void start_colon(const SimpleToken& token);
    void start_semicolon(const SimpleToken& token);
    void start_quotation_mark(const SimpleToken& token);
    
    void process_symbol_in_state_SPACE(const SimpleToken& token, std::optional<char> symbol);
    void process_symbol_in_state_IDENTIFIER_BEGIN(const SimpleToken& token, std::optional<char> symbol);
    void process_symbol_in_state_IDENTIFIER_END(const SimpleToken& token, std::optional<char> symbol);
    void process_symbol_in_state_INTEGER(const SimpleToken& token, std::optional<char> symbol);
    void process_symbol_depending_on_state(const SimpleToken& token, std::optional<char> symbol);
};