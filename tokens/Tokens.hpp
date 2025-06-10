#pragma once

#include <string>
#include <variant>
#include "TokenTypes.hpp"

template <typename TokenType>
struct Token {
    TokenType type;
    int index;

    Token(TokenType token_type, int index) : type(token_type), index(index) {}

    bool operator==(const Token &token) const
    {
        return type == token.type && index == token.index;
    }
};

struct SimpleToken : Token<SimpleTokenType> {
    SimpleToken(SimpleTokenType token_type, int index = 0) : Token(token_type, index) {}

    bool operator==(const SimpleToken& token) const
    {
        return type == token.type && index == token.index;
    }
};

struct ComplexToken : Token<std::variant<ComplexTokenType, SpecialIdentifierType>> {
    std::string value = "";

    ComplexToken(ComplexTokenType token_type, const std::string& value, int index = -1) : Token(token_type, index), value(value) {}
    ComplexToken(SpecialIdentifierType token_type, const std::string& value, int index = -1) : Token(token_type, index), value(value) {}
    ComplexToken(const std::variant<ComplexTokenType, SpecialIdentifierType>& token_type, const std::string& value, int index = -1) : Token(token_type, index), value(value) {}
    
    bool operator==(const ComplexToken& token) const
    {
        return type == token.type && index == token.index && value == token.value;
    }
};