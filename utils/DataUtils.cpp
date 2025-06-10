#include "DataUtils.hpp"

json DataUtils::JsonUtils::read(const std::string& path)
{
    auto file = std::ifstream(path);
    if (!file.is_open()) {
        throw std::runtime_error("DataUtils::JsonUtils::read() failed to open file");
    }

    auto result = json::parse(file);
    file.close();
    
    return result;
}

void DataUtils::JsonUtils::write(const std::string& path, json data, int indent)
{
    auto file = std::ofstream(path);
    file << data.dump(indent);
    file.close();
}

std::vector<std::string> DataUtils::FileUtils::read_strings_from_file(std::ifstream& file)
{
    if (!file.is_open()) {
        throw std::runtime_error("DataUtils::FileUtils::read_strings_from_file() failed to open file");
    }

    std::vector<std::string> result;
    std::string line;
    while (std::getline(file, line)) {
        result.push_back(line);
    }
    return result;
}

std::vector<std::string> DataUtils::FileUtils::read_strings_from_file(const std::string& path)
{
    auto file = std::ifstream(path);
    auto strings = DataUtils::FileUtils::read_strings_from_file(file);
    file.close();
    return strings;
}

void DataUtils::FileUtils::write_strings_to_file(std::ofstream& file, const std::vector<std::string>& strings)
{
    if (!file.is_open()) {
        throw std::runtime_error("DataUtils::FileUtils::write_strings_to_file() failed to open file");
    }

    for (const auto& line : strings) {
        file << line << std::endl;
    }
}

void DataUtils::FileUtils::write_strings_to_file(const std::string& path, const std::vector<std::string>& strings)
{
    auto file = std::ofstream(path);
    DataUtils::FileUtils::write_strings_to_file(file, strings);
    file.close();
}

std::vector<std::string> DataUtils::FileUtils::get_folders_list_from_folder(std::string folderPath)
{
    if (!std::filesystem::exists(folderPath)) {
        throw std::runtime_error("Директория не существует");
    }

    std::vector<std::string> result;

    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_directory()) {
            result.push_back(entry.path().string());
        }
    }

    return result;
}

std::vector<std::string> DataUtils::FileUtils::get_txt_files_list_from_folder(std::string folderPath)
{
    if (!std::filesystem::exists(folderPath)) {
        throw std::runtime_error("Директория не существует");
    }

    std::vector<std::string> result;

    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".txt") {
            result.push_back(entry.path().string());
        }
    }

    return result;
}

std::string DataUtils::FileUtils::get_input_file(std::string text)
{
    std::string filePath;
    
    while (true) {
        std::cout << "Текущая директория: " << std::filesystem::current_path() << std::endl;
        std::cout << "Нажмите Esc для выхода." << std::endl;
        std::cout << text << std::endl;

        char ch = _getch();
        while (ch != (int)KeyCode::ENTER && ch != (int)KeyCode::ESC) {
            if (ch == (int)KeyCode::BACKSPACE) {
                filePath.resize(0, filePath.length() - 1);
                std::cout << "\b \b";
            }
            else {
                filePath += ch;
                std::cout << ch;
            }
            ch = _getch();
        }

        if (ch == (int)KeyCode::ESC) {
            filePath = "";
            break;
        }
        
        if (std::filesystem::exists(filePath) && filePath.ends_with(".txt")) {
            break;
        }
        else {
            std::cout << std::endl;
            std::cout << "Файл не существует или некорректного формата (должен быть .txt). Попробуйте ещё раз." << std::endl;
            std::cout << "Для продолжения нажмите любую клавишу." << std::endl;
            _getch();
            filePath = "";
            std::system("cls");
        }
    }
    return filePath;
}

std::string DataUtils::FileUtils::get_output_file(std::string text)
{
    std::string filePath;
    
    while (true) {
        std::cout << "Текущая директория: " << std::filesystem::current_path() << std::endl;
        std::cout << "Нажмите Esc для выхода." << std::endl;
        std::cout << text << std::endl;

        char ch = _getch();
        while (ch != (int)KeyCode::ENTER && ch != (int)KeyCode::ESC) {
            if (ch == (int)KeyCode::BACKSPACE) {
                filePath = filePath.substr(0, filePath.length() - 1);
                std::cout << "\b \b";
            }
            else {
                filePath += ch;
                std::cout << ch;
            }
            ch = _getch();
        }

        if (ch == (int)KeyCode::ESC) {
            filePath = "";
            break;
        }
        
        if (filePath.ends_with(".txt")) {
            break;
        }
        else {
            std::cout << std::endl;
            std::cout << "Файл некорректного формата (должен быть .txt). Попробуйте ещё раз." << std::endl;
            std::cout << "Для продолжения нажмите любую клавишу." << std::endl;
            _getch();
            filePath = "";
            std::system("cls");
        }
    }
    return filePath;
}