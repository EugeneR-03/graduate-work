#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "MessageWStorage.hpp"
#include "../utils/DataUtils.hpp"

class MenuSharedRepository {
public:
    MenuSharedRepository();
    ~MenuSharedRepository();
    MenuSharedRepository(const MenuSharedRepository&) = delete;
    MenuSharedRepository& operator=(const MenuSharedRepository&) = delete;

    void connect();
    void disconnect();

    static MenuSharedRepository& get_instance();

    MessageWStorage& get_message_storage();

private:
    static MenuSharedRepository* instance;
    MessageWStorage message_storage;

    void init();
};