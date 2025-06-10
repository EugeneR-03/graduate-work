#include "MenuSharedRepository.hpp"

MenuSharedRepository* MenuSharedRepository::instance = nullptr;

MenuSharedRepository::MenuSharedRepository() {}

MenuSharedRepository::~MenuSharedRepository()
{
    disconnect();
    delete instance;
}

void MenuSharedRepository::init()
{
    auto pool_en = DataUtils::JsonUtils::read("res/values-en/tui_messages.json");
    auto pool_ru = DataUtils::JsonUtils::read("res/values-ru/tui_messages.json");

    message_storage = MessageWStorage();
    pool_en.get_to(message_storage["en"]);
    pool_ru.get_to(message_storage["ru"]);
    message_storage.switch_language(Language::Type::Russian);
}

void MenuSharedRepository::connect()
{
    init();
}

void MenuSharedRepository::disconnect()
{
    message_storage.clear();
}

MenuSharedRepository& MenuSharedRepository::get_instance()
{
    if (!instance) {
        instance = new MenuSharedRepository();
    }
    return *instance;
}

MessageWStorage& MenuSharedRepository::get_message_storage()
{
    return message_storage;
}