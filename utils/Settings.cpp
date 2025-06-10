#include "Settings.hpp"

Settings::Settings()
    : global_settings(), file_settings(), db_settings() {}

Settings::Settings(const std::string& config_file_path)
{
    if (config_file_path.ends_with(".json")) {
        read_json_file(config_file_path);
    }
}

void Settings::write_settings(const std::string& config_file_path)
{
    if (config_file_path.ends_with(".json")) {
        write_json_file(config_file_path);
    }
}

tsl::hopscotch_map<std::string, std::string> Settings::parse_db_params(const std::string& db_connection)
{
    auto buf = db_connection
        | std::ranges::views::split(' ')
        | std::ranges::views::transform([](auto&& word) {
              return std::string(word.begin(), word.end());
          })
        | std::ranges::to<std::vector<std::string>>();
    
    tsl::hopscotch_map<std::string, std::string> db_params;
    for (auto&& param : buf) {
        auto param_buf = param
            | std::ranges::views::split('=')
            | std::ranges::views::transform([](auto&& word) {
                  return std::string(word.begin(), word.end());
              })
            | std::ranges::to<std::vector<std::string>>();
        if (param_buf.size() > 1) {
            db_params[param_buf[0]] = param_buf[1];
        }
        else {
            db_params[param_buf[0]] = "";
        }
    }
    return db_params;
}

std::string Settings::db_params_to_string(const tsl::hopscotch_map<std::string, std::string>& db_params)
{
    std::string db_connection;
    db_connection += "user=" + db_params.at("user") + " ";
    db_connection += "password=" + db_params.at("password") + " ";
    db_connection += "host=" + db_params.at("host") + " ";
    db_connection += "port=" + db_params.at("port") + " ";
    db_connection += "dbname=" + db_params.at("dbname");
    return db_connection;
}

void Settings::read_json_file(const std::string& config_file_path)
{
    std::ifstream configFile(config_file_path);
    nlohmann::json config;
    configFile >> config;
    configFile.close();

    this->global_settings.work_mode = main_work_mode_from_string(config["Settings"]["Global"]["WorkMode"]);
    this->global_settings.debug_mode = debug_mode_from_string(config["Settings"]["Global"]["DebugMode"]);
    this->global_settings.errors_mode = syntax_block_working_mode_from_string(config["Settings"]["Global"]["Errors"]);
    this->global_settings.language = Language::type_from_string(config["Settings"]["Global"]["Language"]);

    this->file_settings.input_file_path = config["Settings"]["File"]["InputFile"];
    this->file_settings.output_file_path = config["Settings"]["File"]["OutputFile"];

    auto db_params = parse_db_params(config["Settings"]["DB"]["DBConnection"]);
    try {
        db_params["password"] = PasswordHasher::decrypt(db_params["password"]);
    }
    catch (const std::exception& e) {
        db_params["password"] = "";
    }
    this->db_settings.db_connection = db_params_to_string(db_params);
    this->db_settings.algorithm_name = config["Settings"]["DB"]["AlgorithmName"];
    this->db_settings.algorithm_description = config["Settings"]["DB"]["AlgorithmDescription"];
    this->db_settings.dimensionality = config["Settings"]["DB"]["Dimensionality"].get<std::vector<int>>();
    this->db_settings.iterations = config["Settings"]["DB"]["Iterations"];
    this->db_settings.representation_file_path = config["Settings"]["DB"]["RepresentationFile"];
}

void Settings::write_json_file(const std::string& config_file_path)
{
    nlohmann::json config;

    config["Settings"]["Global"]["WorkMode"] = main_work_mode_to_string(this->global_settings.work_mode);
    config["Settings"]["Global"]["DebugMode"] = debug_mode_to_string(this->global_settings.debug_mode);
    config["Settings"]["Global"]["Errors"] = syntax_block_working_mode_to_string(this->global_settings.errors_mode);
    config["Settings"]["Global"]["Language"] = Language::type_to_string(this->global_settings.language);

    config["Settings"]["File"]["InputFile"] = this->file_settings.input_file_path;
    config["Settings"]["File"]["OutputFile"] = this->file_settings.output_file_path;

    auto db_params = parse_db_params(this->db_settings.db_connection);
    try {
        db_params["password"] = PasswordHasher::encrypt(db_params["password"]);
    }
    catch (const std::exception& e) {
        db_params["password"] = "";
    }
    config["Settings"]["DB"]["DBConnection"] = db_params_to_string(db_params);
    config["Settings"]["DB"]["AlgorithmName"] = this->db_settings.algorithm_name;
    config["Settings"]["DB"]["AlgorithmDescription"] = this->db_settings.algorithm_description;
    config["Settings"]["DB"]["Dimensionality"] = this->db_settings.dimensionality;
    config["Settings"]["DB"]["Iterations"] = this->db_settings.iterations;
    config["Settings"]["DB"]["RepresentationFile"] = this->db_settings.representation_file_path;

    std::ofstream configFile(config_file_path);
    configFile << config.dump(4);
    configFile.close();
}