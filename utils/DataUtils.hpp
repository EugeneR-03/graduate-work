#pragma once

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <conio.h>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class KeyCode {
    ESC = 27,
    ESCAPE = 1,
    ENTER = 13,
    BACKSPACE = 8,
    UP = 72,
    LEFT = 75,
    RIGHT = 77,
    DOWN = 80,
};

namespace DataUtils {
    namespace JsonUtils {
        json read(const std::string& path);
        void write(const std::string& path, json data, int indent = 4);
    };

    namespace FileUtils {
        std::vector<std::string> read_strings_from_file(std::ifstream& file);
        std::vector<std::string> read_strings_from_file(const std::string& path);
        void write_strings_to_file(std::ofstream& file, const std::vector<std::string>& strings);
        void write_strings_to_file(const std::string& path, const std::vector<std::string>& strings);
        std::vector<std::string> get_folders_list_from_folder(std::string folder_path);
        std::vector<std::string> get_txt_files_list_from_folder(std::string folder_path);
        std::string get_input_file(std::string text);
        std::string get_output_file(std::string text);
    };
};