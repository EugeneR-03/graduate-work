#pragma once

#include <string>
#include <fstream>
#include <tsl/hopscotch_map.h>
#include <nlohmann/json.hpp>

#include "../messages/Language.hpp"
#include "../main-blocks/SyntaxBlock.hpp"
#include "PasswordHasher.hpp"
#include "Debugger.hpp"

enum class MainWorkMode : short {
    File,
    DB
};

inline std::string main_work_mode_to_string(MainWorkMode mode)
{
    switch (mode) {
        case MainWorkMode::File:
            return "File";
        case MainWorkMode::DB:
            return "DB";
        default:
            throw std::invalid_argument("Unknown work mode");
    }
}

inline MainWorkMode main_work_mode_from_string(const std::string& mode)
{
    if (mode == "File") {
        return MainWorkMode::File;
    }
    else if (mode == "DB") {
        return MainWorkMode::DB;
    }
    else {
        throw std::invalid_argument("Unknown work mode");
    }
}

class Settings {
public:
    struct Global {
        MainWorkMode work_mode;
        DebuggerWorkingMode debug_mode;
        SyntaxBlockWorkingMode errors_mode;
        Language::Type language;
    };

    struct File {
        std::string input_file_path;
        std::string output_file_path;
    };

    struct DB {
        std::string db_connection;
        std::string algorithm_name;
        std::string algorithm_description;
        std::vector<int> dimensionality;
        int iterations;
        std::string representation_file_path;
    };

    Global global_settings;
    File file_settings;
    DB db_settings;

    Settings();
    explicit Settings(const std::string& config_file_path);
    void write_settings(const std::string& config_file_path);
    static tsl::hopscotch_map<std::string, std::string> parse_db_params(const std::string& db_connection);
    static std::string db_params_to_string(const tsl::hopscotch_map<std::string, std::string>& db_params);

private:
    void read_json_file(const std::string& config_file_path);
    void write_json_file(const std::string& config_file_path);
};