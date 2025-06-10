#include "./main-blocks/VerificationSystem.hpp"
#include "./utils/Debugger.hpp"
#include "./utils/Settings.hpp"
#include "./utils/DataUtils.hpp"
// #include 
#include "./repositories/SharedRepository.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>

#include <chrono>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

const std::string default_config_path = "config.json";

Settings parse_cmd_options(int argc, char* argv[])
{
    Settings settings;

    MainWorkMode work_mode = MainWorkMode::File;
    std::string debug_mode;
    std::string errors;
    std::string language;

    std::string config_file_path;

    std::string input_file;
    std::string output_file;

    std::string db_connection;
    std::string algorithm_name;
    std::string algorithm_description;
    int dimensionality;
    int iterations;
    std::string representation_file;
    
    po::options_description generic_options("Generic options");
    generic_options.add_options()
        ("help,h", "Print this help message.")
        ("work-mode,W", po::value<std::string>(&debug_mode)->default_value("File"),
            "Work mode. \"File\" or \"DB\".")
        ("config-file,C", po::value<std::string>(&config_file_path)->default_value("config.json"),
            "Config file path. Config contains other options.")
        ("debug-mode,M", po::value<std::string>(&debug_mode)->default_value("Normal"),
            "Debug mode. Available modes:\n"
            "  None - \tno debug\n"
            "  Normal - \tnormal debug\n"
            "  Verbose - \tverbose debug")
        ("errors,E", po::value<std::string>(&errors)->default_value("All"),
            "Errors checking mode. Available modes:\n"
            "  UntilFirst - \tcheck errors until first\n"
            "  AllWithoutInner - \tcheck errors with inner blocks of text\n"
            "  All - \tcheck all errors")
        ("language,L", po::value<std::string>(&language)->default_value("ru"),
            "Language of app. Available languages:\n"
            "  English - \tEnglish language\n"
            "  Russian - \tRussian language")
        ;
    
    po::options_description db_config_options("DB config options");
    db_config_options.add_options()
        ("db-connection,c", po::value<std::string>(&db_connection)->default_value(""),
            "DB connection string.\n"
            "  Example:\n"
            "  \"host=localhost port=5432 dbname=<dbname> user=<user> password=<password>\"")
        ("algorithm-name,n", po::value<std::string>(&algorithm_name)->default_value(""),
            "Algorithm name (string).")
        ("algorithm-description,d", po::value<std::string>(&algorithm_description)->default_value(""),
            "Algorithm description (string).")
        ("size,s", po::value<int>(&dimensionality)->default_value(0),
            "Size in dimensions. Dimensionality (integer).")
        ("iterations,i", po::value<int>(&iterations)->default_value(0),
            "Iterations (integer).")
        ("representation-file,r", po::value<std::string>(&representation_file)->default_value(""),
            "Representation file (loaded from DB).")
        ;
    
    po::options_description config_options("Configuration options");
    config_options.add_options()
        ("output-file,O", po::value<std::string>(&output_file)->default_value("output.txt"),
            "Output file path.")
        ("input-file,I", po::value<std::string>(&input_file)->default_value("input.txt"),
            "Input file path.")
        ;

    po::options_description cmdline_options;
    cmdline_options
        .add(generic_options)
        .add(db_config_options)
        .add(config_options);

    // po::options_description visible_options;
    // visible_options.add(generic_options).add(db_config_options).add(config_options);

    po::parsed_options parsed_options = po::command_line_parser(argc, argv).options(cmdline_options).run();

    po::variables_map vm;
    po::store(parsed_options, vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << cmdline_options << "\n";
        exit(0);
    }
    if (vm.count("config-file")) {
        std::string file_path = vm["config-file"].as<std::string>();
        if (std::filesystem::exists(file_path)) {
            return Settings(file_path);
        }
    }
    else {
        if (std::filesystem::exists(default_config_path)) {
            return Settings(default_config_path);
        }
    }
    if (vm.count("work-mode")) {
        settings.global_settings.work_mode = main_work_mode_from_string(vm["work-mode"].as<std::string>());
    }

    if (vm.count("debug-mode")) {
        settings.global_settings.debug_mode = debug_mode_from_string(vm["debug-mode"].as<std::string>());
    }
    if (vm.count("errors")) {
        settings.global_settings.errors_mode = syntax_block_working_mode_from_string(vm["errors"].as<std::string>());
    }
    if (vm.count("language")) {
        settings.global_settings.language = Language::type_from_string(vm["language"].as<std::string>());
    }

    switch (work_mode) {
        case MainWorkMode::File: {
            if (vm.count("input-file")) {
                settings.file_settings.input_file_path = vm["input-file"].as<std::string>();
            }
            if (vm.count("output-file")) {
                settings.file_settings.output_file_path = vm["output-file"].as<std::string>();
            }
            break;
        }

        case MainWorkMode::DB: {
            if (vm.count("db-connection")) {
                settings.db_settings.db_connection = vm["db-connection"].as<std::string>();
            }
            if (vm.count("algorithm-name")) {
                settings.db_settings.algorithm_name = vm["algorithm-name"].as<std::string>();
            }
            if (vm.count("algorithm-description")) {
                settings.db_settings.algorithm_description = vm["algorithm-description"].as<std::string>();
            }
            if (vm.count("size")) {
                settings.db_settings.dimensionality = vm["size"].as<std::vector<int>>();
            }
            if (vm.count("iterations")) {
                settings.db_settings.iterations = vm["iterations"].as<int>();
            }
            if (vm.count("representation-file")) {
                settings.db_settings.representation_file_path = vm["representation-file"].as<std::string>();
            }
            break;
        }
    }    

    return settings;
}

int main(int argc, char* argv[])
{
    Settings settings = parse_cmd_options(argc, argv);

    switch (settings.global_settings.language) {
        case Language::Type::English:
            SharedRepository::get_instance().get_message_storage().switch_language(Language::Type::English);
            break;
            
        case Language::Type::Russian:
            SharedRepository::get_instance().get_message_storage().switch_language(Language::Type::Russian);
            break;
        
        default:
            break;
    }

    if (settings.global_settings.debug_mode == DebuggerWorkingMode::None) {
        return 0;
    }

    try {
        switch (settings.global_settings.work_mode) {
            case MainWorkMode::File: {
                VerificationSystem v_system = VerificationSystem(settings.global_settings.errors_mode);
                v_system.check_file(settings.file_settings.input_file_path, settings.file_settings.output_file_path);
                break;
            }

            case MainWorkMode::DB: {
                SharedRepository::get_instance().connect(settings.db_settings.db_connection);
                auto algorithm_representations = SharedRepository::get_instance()
                    .get_algorithm_representation_repository()
                    .get_by_field("name", settings.db_settings.algorithm_name);
                
                if (algorithm_representations.size() == 0) {
                    throw std::runtime_error("Algorithm not found by name");
                }
                // std::cout << "Found " << algorithm_representations.size() << " algorithm representations" << std::endl;

                // фильтруем по описанию, размерности и итерациям
                std::vector<AlgorithmRepresentation> filtered_algorithm_representations;
                for (AlgorithmRepresentation algorithm_representation : algorithm_representations) {
                    std::string current_lang = settings.global_settings.language == Language::Type::English ? "en" : "ru";
                    if (!algorithm_representation.algorithm.description[current_lang].empty() &&
                            algorithm_representation.algorithm.description[current_lang] != "-" &&
                            algorithm_representation.algorithm.description[current_lang] != settings.db_settings.algorithm_description) {
                        continue;
                    }
                    if (algorithm_representation.representation.dimensionality != settings.db_settings.dimensionality) {
                        continue;
                    }
                    if (algorithm_representation.representation.iterations != settings.db_settings.iterations) {
                        continue;
                    }
                    filtered_algorithm_representations.push_back(algorithm_representation);
                }
                // std::cout << "Filtered. " << filtered_algorithm_representations.size() << " algorithm representations remains" << std::endl;

                if (filtered_algorithm_representations.size() == 0) {
                    throw std::runtime_error("Algorithm not found");
                }
                auto filtered_algorithm_representation = filtered_algorithm_representations[0];

                VerificationSystem v_system = VerificationSystem(settings.global_settings.errors_mode);
                v_system.check_db_representation(filtered_algorithm_representation);
                break;
            }
        }
    }
    catch (const pqxx::broken_connection& e) {
        // std::cerr << "Error: " << e.what() << std::endl;
        return 10;
    }
    catch (const pqxx::sql_error& e) {
        // std::cerr << "Error: " << e.what() << std::endl;
        return 11;
    }
    catch (const std::runtime_error& e) {
        if (std::strcmp(e.what(), "Algorithm not found") == 0) {
            // std::cerr << "Error: Algorithm not found" << std::endl;
            return 12;
        }
        if (std::strcmp(e.what(), "DataUtils::FileUtils::read_strings_from_file() failed to open file") == 0) {
            // std::cerr << "Error: DataUtils::FileUtils::read_strings_from_file() failed to open file" << std::endl;
            return 10;
        }
        return 1;
    }
    catch (std::exception& e) {
        // std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}