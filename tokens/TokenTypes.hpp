#pragma once

enum class SimpleTokenType {
    Letter,                 // буква ('A'..'Z', 'a'..'z')
    Digit,                  // цифра ('0'..'9')
    OpeningCurlyBrace,      // открывающая фигурная скобка ('{')
    ClosingCurlyBrace,      // закрывающая фигурная скобка ('}')
    OpeningParenthesis,     // открывающая круглая скобка ('(')
    ClosingParenthesis,     // закрывающая круглая скобка (')')
    Comma,                  // запятая (',')
    Colon,                  // двоеточие (':')
    Semicolon,              // точка с запятой (';')
    QuotationMark,          // кавычка ('"')
    EqualSign,              // равно ('=')
    ComparisonSign,         // знак сравнения ('>', '<') 
    ExclamationMark,        // восклицательная знак ('!')
    ArithmeticSign,         // знак арифметическое операции ('+', '-', '*', '/')
    LogicalSign,            // знак логической операции ('&' или '|')
    SpaceSign,              // пробел (' ')
    Other,                  // другие символы
};

enum class ComplexTokenType {
    Identifier,             // идентификатор
    Integer,                // целое число
};

enum class SpecialIdentifierType {
    Operation,
    OperandOfUnaryOperation,
    FirstOperandOfBinaryOperation,
    SecondOperandOfBinaryOperation,
    V,
    No,
    Real,
    Solution,
    Modulus,
    SquareOfNumber,
    SquareRootOfNumber,
};