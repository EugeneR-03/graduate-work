#include "TransliterationBlock.hpp"
#include "LexicalBlock.hpp"
#include "SyntaxBlock.hpp"
#include "./../utils/DataUtils.hpp"
#include "./../utils/Debugger.hpp"

class MainBlock {
public:
    MainBlock();
    MainBlock(SyntaxBlockWorkingMode working_mode = SyntaxBlockWorkingMode::UntilFirstError);
    ~MainBlock();

    bool check_string(const std::string& str);
    bool check_string(int string_index, const std::string& str);

private:
    SyntaxBlockWorkingMode working_mode;
    LexicalBlock lexical_block;
    SyntaxBlock syntax_block;
};