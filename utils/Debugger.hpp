#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include "../tokens/Tokens.hpp"
#include "../messages/Messages.hpp"
#include "../dto/AlgorithmRepresentation.hpp"
#include "../repositories/SharedRepository.hpp"

#include <omp.h>

using VariableToken = std::variant<SimpleToken, ComplexToken>;

struct CompareStruc {
    bool operator()(const Message& lhs, const Message& rhs) const
    {
        return lhs.token_index < rhs.token_index;
    }
};

enum class DebuggerWorkingMode : short {
    None,
    Normal,
    Verbose
};

inline std::string debug_mode_to_string(DebuggerWorkingMode debug_mode)
{
    switch (debug_mode) {
    case DebuggerWorkingMode::None:
        return "None";
    case DebuggerWorkingMode::Normal:
        return "Normal";
    case DebuggerWorkingMode::Verbose:
        return "Verbose";
    default:
        return "Unknown";
    }
}

inline DebuggerWorkingMode debug_mode_from_string(const std::string& debug_mode)
{
    if (debug_mode == "None")
        return DebuggerWorkingMode::None;
    else if (debug_mode == "Normal")
        return DebuggerWorkingMode::Normal;
    else if (debug_mode == "Verbose")
        return DebuggerWorkingMode::Verbose;
    else
        return DebuggerWorkingMode::None;
}

class Debugger {
public:
    Debugger();
    void add_message_to_vector(const Message& message, std::vector<std::set<Message>>& messages);
    void print_message(std::ostringstream& oss, const Message& message);
    void print_message(std::ofstream& file, const Message& message);
    void print_message(const std::string& language_pool_name, std::string& validity, const Message& message);
    std::vector<std::string> get_message_and_results(const std::vector<std::set<Message>>& messages, const std::vector<bool>& results);
    void print_message_and_results(std::ostringstream& oss, const std::vector<std::set<Message>>& messages, const std::vector<bool>& results);
    void print_message_and_results(std::ofstream& file, const std::vector<std::set<Message>>& messages, const std::vector<bool>& results);
    void print_message_and_results(const std::vector<std::string>& message_pools_names, AlgorithmRepresentation& algorithm_representation, const std::vector<std::set<Message>>& messages, const std::vector<bool>& results);
    void print_tokens(std::ofstream& file, const std::string& target_string, const std::vector<VariableToken>& combined_tokens);

private:
    omp_lock_t lock;
    static const char* token_types[];
    static const char* complex_token_types[];
    static const char* special_identifier_types[];
};