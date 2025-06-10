#include <locale.h>
#include <signal.h>

#include <io.h>
#include <fcntl.h>
#include <fstream>
#include <windows.h>
#include <boost/nowide/args.hpp>
#include <boost/nowide/fstream.hpp>

#include "./../utils/Settings.hpp"
#include "./../utils/DataUtils.hpp"
#include "./../messages/Language.hpp"
#include "./../main-blocks/VerificationSystem.hpp"
#include "./../repositories/MenuSharedRepository.hpp"

#include "./windows/BaseWindow.hpp"
#include "./windows/TitleWindow.hpp"
#include "./windows/MenuSubwindow.hpp"
#include "./windows/FormSubwindow.hpp"
#include "./windows/WindowView.hpp"
#include "./windows/ButtonView.hpp"
#include "./windows/MenuView.hpp"
#include "./windows/FormView.hpp"
#include "./windows/TextEditView.hpp"
#include "./windows/ModalView.hpp"
#include "./NcursesUtils.hpp"

const std::wstring main_exe = L"main.exe";
const std::string default_config_path = "config.json";

DWORD WINAPI LaunchProcess(const std::wstring& applicationName)
{
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessW(
        applicationName.c_str(),
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    )) {
        std::cerr << "Error: " << GetLastError() << std::endl;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
        std::cerr << "Error: " << GetLastError() << std::endl;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return exitCode;
}

struct IRepresentation {
    virtual ~IRepresentation() = default;
    virtual std::vector<std::string> get_strings() = 0;
    virtual void set_strings(const std::vector<std::string>& strings) = 0;
    virtual void save() = 0;
    virtual void check() = 0;
    virtual std::vector<std::string> get_validity(std::string pool_name) = 0;
};

class RepresentationFromFile : public IRepresentation {
public:
    RepresentationFromFile(Settings settings) : settings(settings)
    {
        this->strings = DataUtils::FileUtils::read_strings_from_file(settings.file_settings.input_file_path);
    }

    std::vector<std::string> get_strings() override
    {
        return strings;
    }

    void set_strings(const std::vector<std::string>& strings) override
    {
        this->strings = strings;
    }

    void save() override
    {
        DataUtils::FileUtils::write_strings_to_file(settings.file_settings.input_file_path, strings);
    }

    void check() override
    {
        VerificationSystem v_system = VerificationSystem(settings.global_settings.errors_mode);
        v_system.check_strings(strings, settings.file_settings.output_file_path);
    }

    std::vector<std::string> get_validity(std::string pool_name) override
    {
        return DataUtils::FileUtils::read_strings_from_file(settings.file_settings.output_file_path);
    }

private:
    Settings settings;
    std::vector<std::string> strings;
};

class RepresentationFromDB : public IRepresentation {
public:
    RepresentationFromDB(Settings settings) : settings(settings)
    {
        SharedRepository::get_instance().connect(settings.db_settings.db_connection);
        auto algorithm_representations = SharedRepository::get_instance()
            .get_algorithm_representation_repository()
            .get_by_field("name", settings.db_settings.algorithm_name);
        
        if (algorithm_representations.size() == 0) {
            throw std::runtime_error("Algorithm not found by name");
        }

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

        if (filtered_algorithm_representations.size() == 0) {
            throw std::runtime_error("Algorithm not found");
        }
        if (filtered_algorithm_representations.size() > 1) {
            throw std::runtime_error("Multiple representations found");
        }
        representation = filtered_algorithm_representations[0];
    }

    std::vector<std::string> get_strings() override
    {
        return representation.representation_strings
            | std::ranges::views::transform([](const auto& representation_string) { return representation_string.content; })
            | std::ranges::to<std::vector<std::string>>();
    }

    void set_strings(const std::vector<std::string>& strings) override
    {
        if (representation.representation_strings.size() != strings.size()) {
            throw std::runtime_error("RepresentationFromDB::set_strings(): strings.size() != representation.representation_strings.size()");
        }
        for (int i = 0; i < (int)strings.size(); i++) {
            representation.representation_strings[i].content = strings[i];
        }
    }

    void save() override
    {
        SharedRepository::get_instance().get_algorithm_representation_repository().update(representation);
    }

    void check() override
    {
        VerificationSystem v_system = VerificationSystem(settings.global_settings.errors_mode);
        v_system.check_db_representation(representation);
    }

    std::vector<std::string> get_validity(std::string pool_name) override
    {
        auto current_pool = SharedRepository::get_instance().get_message_storage().get_current_pool();
        std::vector<std::string> validity;
        for (int i = 0; i < (int)representation.representation_strings.size(); i++) {
            if (representation.representation_strings[i].validity.has_value()) {
                std::string str = representation.representation_strings[i].validity.value()[pool_name];
                // делаем split через std::ranges
                auto buf = str
                    | std::ranges::views::split('\n')
                    | std::ranges::to<std::vector<std::string>>();
                auto string_message = current_pool["other"]["string"];
                string_message += ": " + std::to_string(i + 1) + "\n";
                validity.push_back(string_message);

                for (auto it = buf.begin(); it != buf.end(); it++) {
                    validity.push_back("\t" + *it);
                }
            }
            else {
                auto string_message = current_pool["other"]["string"];
                string_message += ": " + std::to_string(i + 1) + "\n";
                validity.push_back(string_message);
            }
        }
        return validity;
    }

private:
    Settings settings;
    AlgorithmRepresentation representation;
};

void init(Settings settings)
{
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

    MenuSharedRepository::get_instance().connect();
}

int main(int argc, char* argv[]) {
    boost::nowide::args _(argc, argv);
    setlocale(LC_ALL, ".UTF8");

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    Settings settings = Settings(default_config_path);
    
    init(settings);

    auto& message_storage = MenuSharedRepository::get_instance().get_message_storage();
    message_storage.switch_language(settings.global_settings.language);
    auto& current_pool = message_storage.get_current_pool();

    std::shared_ptr<IRepresentation> representation { nullptr };

    nu::init(nu::Default);

    // модальные окна

    Size default_modal_size = ModalView::get_default_size();

    ModalView::ptr modal_win_success_info = ModalView::create_default(
        TextW(current_pool["modal_window_success_info"]["title"], Align::Center, COLOR_PAIR(nu::Cyan)),
        TextW(current_pool["modal_window_success_info"]["message"], Align::Justify, COLOR_PAIR(nu::White)),
        TextW(current_pool["modal_window_success_info"]["ok_text"], Align::Center, COLOR_PAIR(nu::Cyan)),
        nu::display::center.start_y - default_modal_size.height / 2, 
        nu::display::center.start_x - default_modal_size.width / 2
    );

    ModalView::ptr modal_win_restart_info = ModalView::create_default(
        TextW(current_pool["modal_window_restart_info"]["title"], Align::Center, COLOR_PAIR(nu::Cyan)),
        TextW(current_pool["modal_window_restart_info"]["message"], Align::Justify, COLOR_PAIR(nu::White)),
        TextW(current_pool["modal_window_restart_info"]["ok_text"], Align::Center, COLOR_PAIR(nu::Cyan)),
        nu::display::center.start_y - default_modal_size.height / 2, 
        nu::display::center.start_x - default_modal_size.width / 2
    );

    ModalView::ptr modal_win_not_loaded_error = ModalView::create_default(
        TextW(current_pool["modal_window_not_loaded_error"]["title"], Align::Center, COLOR_PAIR(nu::Red)),
        TextW(current_pool["modal_window_not_loaded_error"]["message"], Align::Justify, COLOR_PAIR(nu::White)),
        TextW(current_pool["modal_window_not_loaded_error"]["ok_text"], Align::Center, COLOR_PAIR(nu::Cyan)),
        nu::display::center.start_y - default_modal_size.height / 2, 
        nu::display::center.start_x - default_modal_size.width / 2
    );

    ModalView::ptr modal_win_load_error = ModalView::create_default(
        TextW(current_pool["modal_window_load_error"]["title"], Align::Center, COLOR_PAIR(nu::Red)),
        TextW(current_pool["modal_window_load_error"]["message"], Align::Justify, COLOR_PAIR(nu::White)),
        TextW(current_pool["modal_window_load_error"]["ok_text"], Align::Center, COLOR_PAIR(nu::Cyan)),
        nu::display::center.start_y - default_modal_size.height / 2, 
        nu::display::center.start_x - default_modal_size.width / 2
    );

    // обычные окна

    TextEditView::ptr text_edit_win;
    bool text_edit_win_representation_loaded = false;

    MenuView::ptr main_menu_win;

    MenuView::ptr text_menu_win;

    MenuView::ptr text_source_menu_win;

    MenuView::ptr text_db_source_params_menu_win;

    MenuView::ptr text_file_source_params_menu_win;

    FormView::ptr representation_loading_form_win;

    FormView::ptr db_connection_form_win;

    FormView::ptr file_params_form_win;

    MenuView::ptr options_menu_win;

    MenuView::ptr work_mode_options_menu_win;

    MenuView::ptr language_options_menu_win;

    // инициализация обычных окон

    auto text_edit_win_size = Size(nu::display::size.height - 2, nu::display::size.width - 2);
    auto text_edit_win_pos = Position(1, 1);
    text_edit_win = TextEditView::create(
        text_edit_win_size,
        text_edit_win_pos,
        TextW(current_pool["text_edit_window"]["title"], Align::Center, COLOR_PAIR(nu::Cyan)), 3
    );
    text_edit_win->set_unfocused_border(nu::borders::single_);
    text_edit_win->add_exit_callback([&]() {
        text_edit_win->hide();
        text_menu_win->run();
    }, { K_ESCAPE });
    text_edit_win->on_hide += [&]() {
        if (representation && text_edit_win_representation_loaded) {
            representation->set_strings(text_edit_win->get_lines());
        }
    };

    auto main_menu_size = Size(10, 16);
    auto main_menu_pos = Position(nu::display::center.start_y - main_menu_size.height / 2, 10); // 5, 10
    main_menu_win = MenuView::create(
        main_menu_size, 
        main_menu_pos,
        TextW(current_pool["main_menu"]["title"], Align::Center, COLOR_PAIR(nu::Cyan)), 3
    );
    main_menu_win->set_unfocused_border(nu::borders::double_);
    main_menu_win->set_custom_menu_mark(L"❱ ");
    main_menu_win->set_active_items_attrs(COLOR_PAIR(nu::Green));
    main_menu_win->set_unfocused_items_attrs(COLOR_PAIR(nu::Green) | A_REVERSE);
    main_menu_win->set_menu_margins(1, 0, 1, 1);
    main_menu_win->add_menu_item(current_pool["main_menu"]["launch"], L"", [&]() {
        main_menu_win->stop();
        text_menu_win->show();
        text_menu_win->run();
    }, { KEY_ENTER, KEY_RIGHT });
    main_menu_win->add_menu_item(current_pool["main_menu"]["options"], L"", [&]() {
        main_menu_win->stop();
        options_menu_win->show();
        options_menu_win->run();
    }, { KEY_ENTER, KEY_RIGHT });
    main_menu_win->add_menu_item(current_pool["main_menu"]["exit"], L"", [&]() {
        main_menu_win->stop();
    }, { KEY_ENTER });

    auto text_menu_size = Size(13, 30);
    auto text_menu_pos = Position(nu::display::center.start_y - text_menu_size.height / 2, 26); //4 26
    text_menu_win = MenuView::create(
        text_menu_size, 
        text_menu_pos,
        TextW(current_pool["text_menu"]["title"], Align::Center, COLOR_PAIR(nu::Cyan)), 3  
    );
    text_menu_win->set_unfocused_border(nu::borders::double_);
    text_menu_win->set_custom_menu_mark(L"❱ ");
    text_menu_win->set_active_items_attrs(COLOR_PAIR(nu::Green));
    text_menu_win->set_unfocused_items_attrs(COLOR_PAIR(nu::Green) | A_REVERSE);
    text_menu_win->set_menu_margins(1, 0, 1, 1);
    text_menu_win->add_menu_item(current_pool["text_menu"]["text_source_menu"], L"", [&]() {
        text_menu_win->stop();
        text_source_menu_win->show();
        text_source_menu_win->run();
    }, { KEY_ENTER, KEY_RIGHT });
    text_menu_win->add_menu_item(current_pool["text_menu"]["text_source_params_menu"], L"", [&]() {
        text_menu_win->stop();
        switch (settings.global_settings.work_mode) {
            case MainWorkMode::DB:
                text_db_source_params_menu_win->show();
                text_db_source_params_menu_win->run();
                break;
            case MainWorkMode::File:
                text_file_source_params_menu_win->show();
                text_file_source_params_menu_win->run();
                break;
            default:
                break;
        }
    }, { KEY_ENTER, KEY_RIGHT });
    text_menu_win->add_menu_item(current_pool["text_menu"]["load"], L"", [&]() {
        try {
            switch (settings.global_settings.work_mode) {
                case MainWorkMode::DB:
                    representation = std::make_shared<RepresentationFromDB>(settings);
                    break;
                
                case MainWorkMode::File:
                    representation = std::make_shared<RepresentationFromFile>(settings);
                    break;
            }

            modal_win_success_info->show();
            modal_win_success_info->run();
        }
        catch (const std::exception& e) {
            modal_win_load_error->show();
            modal_win_load_error->run();
        }
    }, { KEY_ENTER });
    text_menu_win->add_menu_item(current_pool["text_menu"]["redact"], L"", [&]() {
        if (representation) {
            text_edit_win_representation_loaded = true;
            text_edit_win->load(representation->get_strings());
            text_edit_win->show();
            text_edit_win->run();
        }
        else {
            modal_win_not_loaded_error->show();
            modal_win_not_loaded_error->run();
        }
    }, { KEY_ENTER });
    text_menu_win->add_menu_item(current_pool["text_menu"]["check"], L"", [&]() {
        try {
            if (representation) {
                representation->check();
                text_edit_win_representation_loaded = false;
                text_edit_win->load(representation->get_validity(message_storage.get_current_pool_name()));
                text_edit_win->show();
                text_edit_win->run_non_editable();
            }
            else {
                modal_win_not_loaded_error->show();
                modal_win_not_loaded_error->run();
            }
        }
        catch (const std::exception& e) {
            modal_win_load_error->show();
            modal_win_load_error->run();
        }  
    }, { KEY_ENTER });
    text_menu_win->add_menu_item(current_pool["text_menu"]["load_and_check"], L"", [&]() {
        try {
            switch (settings.global_settings.work_mode) {
                case MainWorkMode::DB:
                    representation = std::make_shared<RepresentationFromDB>(settings);
                    break;
                
                case MainWorkMode::File:
                    representation = std::make_shared<RepresentationFromFile>(settings);
                    break;
            }

            representation->check();
            text_edit_win_representation_loaded = false;
            text_edit_win->load(representation->get_validity(message_storage.get_current_pool_name()));
            text_edit_win->show();
            text_edit_win->run_non_editable();
        }
        catch (const std::exception& e) {
            modal_win_load_error->show();
            modal_win_load_error->run();
        }
    }, { KEY_ENTER });
    text_menu_win->add_menu_item(current_pool["text_menu"]["save"], L"", [&]() {
        if (!representation) {
            modal_win_not_loaded_error->show();
            modal_win_not_loaded_error->run();
        }
        else {
            representation->save();
            modal_win_success_info->show();
            modal_win_success_info->run();
        }
    }, { KEY_ENTER });
    text_menu_win->add_exit_callback([&]() {
        text_menu_win->hide();
        main_menu_win->run();
    }, { K_ESCAPE, KEY_LEFT });

    auto text_source_menu_size = Size(10, 30);
    auto text_source_menu_pos = Position(nu::display::center.start_y - text_source_menu_size.height / 2, 26); //4 26
    text_source_menu_win = MenuView::create(
        text_source_menu_size, 
        text_source_menu_pos,
        TextW(current_pool["text_source_menu"]["title"], Align::Center, COLOR_PAIR(nu::Cyan)), 3
    );
    text_source_menu_win->set_unfocused_border(nu::borders::double_);
    text_source_menu_win->set_custom_menu_mark(L"❱ ");
    text_source_menu_win->set_active_items_attrs(COLOR_PAIR(nu::Green));
    text_source_menu_win->set_unfocused_items_attrs(COLOR_PAIR(nu::Green) | A_REVERSE);
    text_source_menu_win->set_defined_item_attrs(COLOR_PAIR(nu::Yellow));
    text_source_menu_win->set_menu_margins(1, 0, 1, 1);
    text_source_menu_win->add_menu_item(current_pool["text_source_menu"]["db"], L"", [&]() {
        if (settings.global_settings.work_mode != MainWorkMode::DB) {
            representation = nullptr;
            settings.global_settings.work_mode = MainWorkMode::DB;
            settings.write_settings(default_config_path);
            representation = nullptr;
            text_source_menu_win->set_defined_item(0);
            text_source_menu_win->show();
        }
    }, { KEY_ENTER, KEY_RIGHT });
    text_source_menu_win->add_menu_item(current_pool["text_source_menu"]["file"], L"", [&]() {
        if (settings.global_settings.work_mode != MainWorkMode::File) {
            representation = nullptr;
            settings.global_settings.work_mode = MainWorkMode::File;
            settings.write_settings(default_config_path);
            representation = nullptr;
            text_source_menu_win->set_defined_item(1);
            text_source_menu_win->show();
        }
    }, { KEY_ENTER, KEY_RIGHT });
    text_source_menu_win->add_exit_callback([&]() {
        text_source_menu_win->hide();
        text_menu_win->run();
    }, { K_ESCAPE, KEY_LEFT });
    switch (settings.global_settings.work_mode) {
        case MainWorkMode::DB:
            text_source_menu_win->set_defined_item(0);
            break;
        case MainWorkMode::File:
            text_source_menu_win->set_defined_item(1);
            break;
        default:
            break;
    }

    auto text_db_source_params_menu_size = Size(10, 30);
    auto text_db_source_params_menu_pos = Position(nu::display::center.start_y - text_db_source_params_menu_size.height / 2, 26); //4 26
    text_db_source_params_menu_win = MenuView::create(
        text_db_source_params_menu_size, 
        text_db_source_params_menu_pos,
        TextW(current_pool["text_db_source_params_menu"]["title"], Align::Center, COLOR_PAIR(nu::Cyan)), 3
    );
    text_db_source_params_menu_win->set_unfocused_border(nu::borders::double_);
    text_db_source_params_menu_win->set_custom_menu_mark(L"❱ ");
    text_db_source_params_menu_win->set_active_items_attrs(COLOR_PAIR(nu::Green));
    text_db_source_params_menu_win->set_unfocused_items_attrs(COLOR_PAIR(nu::Green) | A_REVERSE);
    text_db_source_params_menu_win->set_menu_margins(1, 0, 1, 1);
    text_db_source_params_menu_win->add_menu_item(current_pool["text_db_source_params_menu"]["loading_params"], L"", [&]() {
        text_db_source_params_menu_win->stop();
        representation_loading_form_win->show();
        representation_loading_form_win->run();
    }, { KEY_ENTER, KEY_RIGHT });
    text_db_source_params_menu_win->add_menu_item(current_pool["text_db_source_params_menu"]["db_connection_params"], L"", [&]() {
        text_db_source_params_menu_win->stop();
        db_connection_form_win->show();
        db_connection_form_win->run();
    }, { KEY_ENTER, KEY_RIGHT });
    text_db_source_params_menu_win->add_exit_callback([&]() {
        text_db_source_params_menu_win->hide();
        text_menu_win->run();
    }, { K_ESCAPE, KEY_LEFT });

    auto text_file_source_params_menu_size = Size(10, 30);
    auto text_file_source_params_menu_pos = Position(nu::display::center.start_y - text_file_source_params_menu_size.height / 2, 26); //4 26
    text_file_source_params_menu_win = MenuView::create(
        text_file_source_params_menu_size, 
        text_file_source_params_menu_pos,
        TextW(current_pool["text_file_source_params_menu"]["title"], Align::Center, COLOR_PAIR(nu::Cyan)), 3
    );
    text_file_source_params_menu_win->set_unfocused_border(nu::borders::double_);
    text_file_source_params_menu_win->set_custom_menu_mark(L"❱ ");
    text_file_source_params_menu_win->set_active_items_attrs(COLOR_PAIR(nu::Green));
    text_file_source_params_menu_win->set_unfocused_items_attrs(COLOR_PAIR(nu::Green) | A_REVERSE);
    text_file_source_params_menu_win->set_menu_margins(1, 0, 1, 1);
    text_file_source_params_menu_win->add_menu_item(current_pool["text_file_source_params_menu"]["file_params"], L"", [&]() {
        text_file_source_params_menu_win->stop();
        file_params_form_win->show();
        file_params_form_win->run();
    }, { KEY_ENTER, KEY_RIGHT });
    text_file_source_params_menu_win->add_exit_callback([&]() {
        text_file_source_params_menu_win->hide();
        text_menu_win->run();
    }, { K_ESCAPE, KEY_LEFT });

    auto representation_loading_form_win_size = Size(14, 50);
    auto representation_loading_form_win_pos = Position(nu::display::center.start_y - representation_loading_form_win_size.height / 2, 56); //4 56
    representation_loading_form_win = FormView::create(
        representation_loading_form_win_size,
        representation_loading_form_win_pos,
        TextW(current_pool["representation_loading_form"]["title"], Align::Center, COLOR_PAIR(nu::Cyan)), 3
    );
    representation_loading_form_win->set_unfocused_border(nu::borders::double_);
    representation_loading_form_win->set_form_margins(1, 0, 1, 1);
    auto representation_loading_form_menu_exit_callback = [&]() {
        representation_loading_form_win->hide();
        text_db_source_params_menu_win->run();
    };
    representation_loading_form_win->add_field(current_pool["representation_loading_form"]["name"], 1, 15, L"", 2, 30);
    representation_loading_form_win->set_field_content(0, settings.db_settings.algorithm_name);
    representation_loading_form_win->add_field(current_pool["representation_loading_form"]["description"], 1, 15, L"", 2, 30);
    representation_loading_form_win->set_field_content(1, settings.db_settings.algorithm_description);
    representation_loading_form_win->add_field(current_pool["representation_loading_form"]["dimensionality"], 1, 15, L"", 2, 30);
    representation_loading_form_win->set_field_content(2, std::to_string(settings.db_settings.dimensionality[0]));
    representation_loading_form_win->add_field(current_pool["representation_loading_form"]["iterations"], 1, 15, L"", 2, 30);
    representation_loading_form_win->set_field_content(3, std::to_string(settings.db_settings.iterations));
    representation_loading_form_win->add_exit_callback(representation_loading_form_menu_exit_callback, { K_ESCAPE });
    representation_loading_form_win->on_hide += [&]() {
        auto algorithm_name = representation_loading_form_win->get_field_content(0);
        auto algorithm_description = representation_loading_form_win->get_field_content(1);
        // числа, разделенные запятыми
        auto dimensionality_str = representation_loading_form_win->get_field_content(2);
        std::vector<int> dimensionality = dimensionality_str
            | std::ranges::views::split(',')
            | std::ranges::views::transform([](auto&& str) { return std::stoi(std::string(str.begin(), str.end())); })
            | std::ranges::to<std::vector<int>>();
        auto iterations = representation_loading_form_win->get_field_content(3);
        if (algorithm_name != settings.db_settings.algorithm_name ||
                algorithm_description != settings.db_settings.algorithm_description ||
                dimensionality != settings.db_settings.dimensionality ||
                iterations != std::to_string(settings.db_settings.iterations)) {
            settings.db_settings.algorithm_name = algorithm_name;
            settings.db_settings.algorithm_description = algorithm_description;
            settings.db_settings.dimensionality = dimensionality;
            settings.db_settings.iterations = std::stoi(iterations);
            settings.write_settings(default_config_path);
            representation = nullptr;
        }
    };

    auto db_connection = settings.db_settings.db_connection;
    auto db_params = Settings::parse_db_params(db_connection);

    auto db_connection_form_win_size = Size(15, 50);
    auto db_connection_form_win_pos = Position(nu::display::center.start_y - db_connection_form_win_size.height / 2, 56);   //5 54
    db_connection_form_win = FormView::create(
        db_connection_form_win_size,
        db_connection_form_win_pos,
        TextW(current_pool["db_connection_form"]["title"], Align::Center, COLOR_PAIR(nu::Cyan)), 3
    );
    db_connection_form_win->set_unfocused_border(nu::borders::double_);
    db_connection_form_win->set_form_margins(1, 0, 1, 1);
    auto db_connection_form_exit_callback = [&]() {
        db_connection_form_win->hide();
        text_db_source_params_menu_win->run();
    };
    db_connection_form_win->add_field(current_pool["db_connection_form"]["db_name"], 1, 15, L"", 2, 30);
    db_connection_form_win->set_field_content(0, db_params["dbname"]);
    db_connection_form_win->add_field(current_pool["db_connection_form"]["db_login"], 1, 15, L"", 2, 30);
    db_connection_form_win->set_field_content(1, db_params["user"]);
    db_connection_form_win->add_field(current_pool["db_connection_form"]["db_password"], 1, 15, L"", 2, 30, O_PASSWORD);
    db_connection_form_win->set_field_content(2, db_params["password"]);
    db_connection_form_win->add_field(current_pool["db_connection_form"]["db_host"], 1, 15, L"", 2, 30);
    db_connection_form_win->set_field_content(3, db_params["host"]);
    db_connection_form_win->add_field(current_pool["db_connection_form"]["db_port"], 1, 15, L"", 1, 30);
    db_connection_form_win->set_field_content(4, db_params["port"]);
    db_connection_form_win->add_exit_callback(db_connection_form_exit_callback, { K_ESCAPE });
    db_connection_form_win->on_hide += [&]() {
        auto db_name = db_connection_form_win->get_field_content(0);
        auto db_login = db_connection_form_win->get_field_content(1);
        auto db_password = db_connection_form_win->get_field_content(2);
        auto db_host = db_connection_form_win->get_field_content(3);
        auto db_port = db_connection_form_win->get_field_content(4);
        db_connection = "dbname=" + db_name + " user=" + db_login + " password=" + db_password + " host=" + db_host + " port=" + db_port;
        if (db_name != db_params["dbname"] || 
                db_login != db_params["user"] || 
                db_password != db_params["password"] || 
                db_host != db_params["host"] || 
                db_port != db_params["port"]) {
            db_params["dbname"] = db_name;
            db_params["user"] = db_login;
            db_params["password"] = db_password;
            db_params["host"] = db_host;
            db_params["port"] = db_port;
            settings.db_settings.db_connection = db_connection;    
            settings.write_settings(default_config_path);
            representation = nullptr;
        }
    };

    auto file_params_form_win_size = Size(12, 50);
    auto file_params_form_win_pos = Position(nu::display::center.start_y - file_params_form_win_size.height / 2, 56);   //5 54
    file_params_form_win = FormView::create(
        file_params_form_win_size,
        file_params_form_win_pos,
        TextW(current_pool["file_params_form"]["title"], Align::Center, COLOR_PAIR(nu::Cyan)), 3
    );
    file_params_form_win->set_unfocused_border(nu::borders::double_);
    file_params_form_win->set_form_margins(1, 0, 1, 1);
    auto file_params_form_exit_callback = [&]() {
        file_params_form_win->hide();
        text_file_source_params_menu_win->run();
    };
    file_params_form_win->add_field(current_pool["file_params_form"]["input_file"], 1, 15, L"", 3, 30);
    file_params_form_win->set_field_content(0, settings.file_settings.input_file_path);
    file_params_form_win->add_field(current_pool["file_params_form"]["output_file"], 1, 15, L"", 3, 30);
    file_params_form_win->set_field_content(1, settings.file_settings.output_file_path);
    file_params_form_win->add_exit_callback(file_params_form_exit_callback, { K_ESCAPE });
    file_params_form_win->on_hide += [&]() {
        auto input_file = file_params_form_win->get_field_content(0);
        auto output_file = file_params_form_win->get_field_content(1);
        if (input_file != settings.file_settings.input_file_path || output_file != settings.file_settings.output_file_path) {
            settings.file_settings.input_file_path = input_file;
            settings.file_settings.output_file_path = output_file;
            settings.write_settings(default_config_path);
            representation = nullptr;
        }
    };

    auto options_menu_size = Size(10, 28);
    auto options_menu_pos = Position(nu::display::center.start_y - options_menu_size.height / 2, 26);   //5 26
    options_menu_win = MenuView::create(
        options_menu_size,
        options_menu_pos,
        TextW(current_pool["options_menu"]["title"], Align::Center, COLOR_PAIR(nu::Cyan)), 3
    );
    options_menu_win->set_unfocused_border(nu::borders::double_);
    options_menu_win->set_custom_menu_mark(L"❱ ");
    options_menu_win->set_active_items_attrs(COLOR_PAIR(nu::Green));
    options_menu_win->set_unfocused_items_attrs(COLOR_PAIR(nu::Green) | A_REVERSE);
    options_menu_win->set_menu_margins(1, 0, 1, 1);
    auto options_menu_exit_callback = [&]() {
        options_menu_win->hide();
        main_menu_win->run();
    };
    options_menu_win->add_menu_item(current_pool["options_menu"]["check_options"], L"", [&]() {
        options_menu_win->stop();
        work_mode_options_menu_win->show();
        work_mode_options_menu_win->run();
    }, { KEY_ENTER, KEY_RIGHT });
    options_menu_win->add_menu_item(current_pool["options_menu"]["language_options"], L"", [&]() {
        options_menu_win->stop();
        language_options_menu_win->show();
        language_options_menu_win->run();
    }, { KEY_ENTER, KEY_RIGHT });
    options_menu_win->add_exit_callback(options_menu_exit_callback, { K_ESCAPE, KEY_LEFT });

    auto work_mode_options_menu_win_size = Size(10, 30);
    auto work_mode_options_menu_win_pos = Position(nu::display::center.start_y - work_mode_options_menu_win_size.height / 2, 54);   //5 84
    work_mode_options_menu_win = MenuView::create(
        work_mode_options_menu_win_size,
        work_mode_options_menu_win_pos,
        TextW(current_pool["work_mode_options_menu"]["title"], Align::Center, COLOR_PAIR(nu::Cyan)), 3
    );
    work_mode_options_menu_win->set_unfocused_border(nu::borders::double_);
    work_mode_options_menu_win->set_custom_menu_mark(L"❱ ");
    work_mode_options_menu_win->set_active_items_attrs(COLOR_PAIR(nu::Green));
    work_mode_options_menu_win->set_unfocused_items_attrs(COLOR_PAIR(nu::Green) | A_REVERSE);
    work_mode_options_menu_win->set_defined_item_attrs(COLOR_PAIR(nu::Yellow));
    work_mode_options_menu_win->set_menu_margins(1, 0, 1, 1);
    auto work_mode_options_menu_set_errors_mode_callback = [&]() {
        int current_item_index = work_mode_options_menu_win->get_current_item_index();
        work_mode_options_menu_win->set_defined_item(current_item_index);
        work_mode_options_menu_win->show();
        if (settings.global_settings.errors_mode != (SyntaxBlockWorkingMode)current_item_index) {
            settings.global_settings.errors_mode = (SyntaxBlockWorkingMode)current_item_index;
            settings.write_settings(default_config_path);
            representation = nullptr;
        }
    };
    auto work_mode_options_menu_exit_callback = [&]() {
        work_mode_options_menu_win->hide();
        options_menu_win->run();
    };
    work_mode_options_menu_win->add_menu_item(current_pool["work_mode_options_menu"]["until_first_error"], L"", work_mode_options_menu_set_errors_mode_callback, { KEY_ENTER });
    work_mode_options_menu_win->add_menu_item(current_pool["work_mode_options_menu"]["all_errors"], L"", work_mode_options_menu_set_errors_mode_callback, { KEY_ENTER });
    work_mode_options_menu_win->add_menu_item(current_pool["work_mode_options_menu"]["all_errors_detailed"], L"", work_mode_options_menu_set_errors_mode_callback, { KEY_ENTER });
    work_mode_options_menu_win->add_exit_callback(work_mode_options_menu_exit_callback, { K_ESCAPE, KEY_LEFT });
    int work_mode_options_menu_win_defined_item_index = (int)settings.global_settings.errors_mode;
    work_mode_options_menu_win->set_defined_item(work_mode_options_menu_win_defined_item_index);

    auto language_options_menu_win_size = Size(10, 24);
    auto language_options_menu_win_pos = Position(nu::display::center.start_y - language_options_menu_win_size.height / 2, 54); //5 84
    language_options_menu_win = MenuView::create(
        language_options_menu_win_size,
        language_options_menu_win_pos,
        TextW(current_pool["language_options_menu"]["title"], Align::Center, COLOR_PAIR(nu::Cyan)), 3
    );
    language_options_menu_win->set_unfocused_border(nu::borders::double_);
    language_options_menu_win->set_custom_menu_mark(L"❱ ");
    language_options_menu_win->set_active_items_attrs(COLOR_PAIR(nu::Green));
    language_options_menu_win->set_unfocused_items_attrs(COLOR_PAIR(nu::Green) | A_REVERSE);
    language_options_menu_win->set_defined_item_attrs(COLOR_PAIR(nu::Yellow));
    language_options_menu_win->set_menu_margins(1, 0, 1, 1);
    auto language_options_menu_set_language = [&]() {
        int current_item_index = language_options_menu_win->get_current_item_index();
        language_options_menu_win->set_defined_item(current_item_index);
        language_options_menu_win->show();
        bool switched = false;
        switch (current_item_index) {
            case 0:
                switched = settings.global_settings.language != Language::Type::Russian;
                settings.global_settings.language = Language::Type::Russian;
                break;
            case 1:
                switched = settings.global_settings.language != Language::Type::English;
                settings.global_settings.language = Language::Type::English;
                break;
            default:
                break;
        }
        if (switched) {
            settings.write_settings(default_config_path);
            representation = nullptr;
            modal_win_restart_info->show();
            modal_win_restart_info->run();
        }
    };
    auto language_options_menu_exit_callback = [&]() {
        language_options_menu_win->hide();
        options_menu_win->run();
    };
    language_options_menu_win->add_menu_item(current_pool["language_options_menu"]["russian"], L"", language_options_menu_set_language, { KEY_ENTER });
    language_options_menu_win->add_menu_item(current_pool["language_options_menu"]["english"], L"", language_options_menu_set_language, { KEY_ENTER });
    language_options_menu_win->add_exit_callback(language_options_menu_exit_callback, { K_ESCAPE, KEY_LEFT });
    switch (settings.global_settings.language) {
        case Language::Type::Russian:
            language_options_menu_win->set_defined_item(0);
            break;
        case Language::Type::English:
            language_options_menu_win->set_defined_item(1);
            break;
        default:
            break;
    }
    
    refresh();
    
    main_menu_win->show();
    main_menu_win->run();

    nu::end();
    return 0;
}