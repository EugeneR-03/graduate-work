#include "SpecialIdentifierBlock.hpp"

tsl::hopscotch_map<std::string, SpecialIdentifierType> SpecialIdentifierBlock::special_identifiers = {
    {"op", SpecialIdentifierType::Operation},                                 // Operation
    {"od", SpecialIdentifierType::OperandOfUnaryOperation},                   // OperandOfUnaryOperation
    {"fO", SpecialIdentifierType::FirstOperandOfBinaryOperation},             // FirstOperandOfBinaryOperation
    {"sO", SpecialIdentifierType::SecondOperandOfBinaryOperation},            // SecondOperandOfBinaryOperation
    {"v", SpecialIdentifierType::V},                                          // v: no real solution
    {"no", SpecialIdentifierType::No},                                        // no: no real solution
    {"real", SpecialIdentifierType::Real},                                    // real: no real solution
    {"solution", SpecialIdentifierType::Solution},                                 // solution: no real solution
    {"abs", SpecialIdentifierType::Modulus},                                  // Modulus
    {"sqr", SpecialIdentifierType::SquareOfNumber},                           // SquareOfNumber
    {"sqrt", SpecialIdentifierType::SquareRootOfNumber},                      // SquareRootOfNumber
};

SpecialIdentifierBlock::SpecialIdentifierBlock() {}

SpecialIdentifierBlock::~SpecialIdentifierBlock() {}

void SpecialIdentifierBlock::try_change_identifier_type_to_special(ComplexToken& token)
{
    std::string value = token.value;
    std::optional<SpecialIdentifierType> identifier = check_string(value);
    if (identifier.has_value())
        token.type = identifier.value();
}

bool SpecialIdentifierBlock::is_special_identifier(const ComplexToken& token)
{
    if (std::get<ComplexTokenType>(token.type) != ComplexTokenType::Identifier)
        return false;

    std::optional<SpecialIdentifierType> identifier = check_string(token.value);
    if (identifier.has_value())
        return true;
    return false;
}

std::optional<SpecialIdentifierType> SpecialIdentifierBlock::check_string(const std::string& str)
{
    if (special_identifiers.contains(str))
        return special_identifiers.at(str);
    
    return std::nullopt;
}