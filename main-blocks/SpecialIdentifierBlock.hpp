#pragma once

#include <string>
#include <tsl/hopscotch_map.h>
#include "../tokens/Tokens.hpp"
#include "LexicalBlock.hpp"

class SpecialIdentifierBlock {
public:
    SpecialIdentifierBlock();
    ~SpecialIdentifierBlock();
    void try_change_identifier_type_to_special(ComplexToken& token);
    bool is_special_identifier(const ComplexToken& token);

private:
    static tsl::hopscotch_map<std::string, SpecialIdentifierType> special_identifiers;
    std::optional<SpecialIdentifierType> check_string(const std::string& str);
};