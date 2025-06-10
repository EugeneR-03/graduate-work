#pragma once

#include <string>
#include <vector>
#include "../tokens/Tokens.hpp"

class TransliterationBlock {
public:
    TransliterationBlock();
    SimpleToken transliterate_symbol(char symbol, int index);
    std::vector<SimpleToken> transliterate_string(const std::string& str);
};