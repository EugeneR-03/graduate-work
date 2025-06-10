#include "TransliterationBlock.hpp"

TransliterationBlock::TransliterationBlock() {}

SimpleToken TransliterationBlock::transliterate_symbol(char symbol, int index = 0)
{
    // проверка, является ли символ буквой
    if (std::isalpha(symbol)) {
        return SimpleToken(SimpleTokenType::Letter, index);
    }

    // проверка, является ли символ цифрой
    else if (std::isdigit(symbol)) {
        return SimpleToken(SimpleTokenType::Digit, index);
    }

    // проверка на уникальные символы (не буква, не цифра)
    switch (symbol) {
        case '{':
            return SimpleToken(SimpleTokenType::OpeningCurlyBrace, index);
        case '}':
            return SimpleToken(SimpleTokenType::ClosingCurlyBrace, index);
        case '(':
            return SimpleToken(SimpleTokenType::OpeningParenthesis, index);
        case ')':
            return SimpleToken(SimpleTokenType::ClosingParenthesis, index);
        case ',':
            return SimpleToken(SimpleTokenType::Comma, index);
        case ':':
            return SimpleToken(SimpleTokenType::Colon, index);
        case ';':
            return SimpleToken(SimpleTokenType::Semicolon, index);
        case '\"':
            return SimpleToken(SimpleTokenType::QuotationMark, index);
        case '=':
            return SimpleToken(SimpleTokenType::EqualSign, index);
        case '>':
        case '<':
            return SimpleToken(SimpleTokenType::ComparisonSign, index);
        case '!':
            return SimpleToken(SimpleTokenType::ExclamationMark, index);
        case '+':
        case '-':
        case '*':
        case '/':
            return SimpleToken(SimpleTokenType::ArithmeticSign, index);
        case '&':
        case '|':
            return SimpleToken(SimpleTokenType::LogicalSign, index);
        case ' ':
            return SimpleToken(SimpleTokenType::SpaceSign, index);
        default:
            return SimpleToken(SimpleTokenType::Other, index);
    }
}

std::vector<SimpleToken> TransliterationBlock::transliterate_string(const std::string& str)
{
    std::vector<SimpleToken> tokens;
    for (int i = 0; i < (int)str.length(); i++) {
        tokens.push_back(transliterate_symbol(str[i], i));
    }
    return tokens;
}