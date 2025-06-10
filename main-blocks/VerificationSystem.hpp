#include "TransliterationBlock.hpp"
#include "LexicalBlock.hpp"
#include "SyntaxBlock.hpp"
#include "MainBlock.hpp"
#include "./../utils/DataUtils.hpp"
#include "./../utils/Debugger.hpp"

class VerificationSystem {
public:
    VerificationSystem(SyntaxBlockWorkingMode working_mode = SyntaxBlockWorkingMode::UntilFirstError);
    ~VerificationSystem();

    void check_strings(const std::vector<std::string> strings, const std::string& output_file_path);
    void check_file(const std::string& input_file_path, const std::string& output_file_path);
    void check_db_representation(AlgorithmRepresentation& representation);

private:
    SyntaxBlockWorkingMode working_mode;

    void reset_events();
    void connect_events(int strings_count);
    void connect_events(Debugger& debugger, std::vector<std::set<Message>>& messages);
};