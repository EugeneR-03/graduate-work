#include "MainBlock.hpp"

MainBlock::MainBlock()
{
    this->working_mode = SyntaxBlockWorkingMode::UntilFirstError;
    this->lexical_block = LexicalBlock();
    this->syntax_block = SyntaxBlock();
}

MainBlock::MainBlock(SyntaxBlockWorkingMode working_mode)
{
    this->working_mode = working_mode;
    this->lexical_block = LexicalBlock();
    this->syntax_block = SyntaxBlock(working_mode);
}

MainBlock::~MainBlock() {}

bool MainBlock::check_string(const std::string& str)
{
    std::vector<std::variant<SimpleToken, ComplexToken>> combined_tokens = this->lexical_block.transliterate_string(str);
    this->syntax_block.load_token_vector(combined_tokens);
    
    return this->syntax_block.check_token_vector(combined_tokens, working_mode);
}

bool MainBlock::check_string(int string_index, const std::string& str)
{
    this->syntax_block.set_string_index(string_index);
    return this->check_string(str);
}