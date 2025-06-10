#pragma once

#include <string>
#include <iostream>
#include "../messages/Language.hpp"
#include "../messages/MessagePool.hpp"

class MessageStorage {
public:
    MessageStorage() 
        : language(Language::Type::Unknown) {}
    ~MessageStorage() {}

    std::string get_current_pool_name()
    {
        return language_str;
    }
    
    MessagePool<MessageRecord>& get_current_pool()
    {
        return message_pools[language_str];
    }

    void switch_language(Language::Type language)
    {
        this->language = language;
        this->language_str = Language::type_to_string(language);

        if (!message_pools.contains(language_str)) {
            message_pools[language_str] = MessagePool<MessageRecord>();
        }
    }

    MessagePool<MessageRecord>& operator[](const std::string& key)
    {
        return message_pools[key];
    }

    const MessagePool<MessageRecord>& operator[](const std::string& key) const
    {
        return message_pools.at(key);
    }

    MessagePool<MessageRecord>& at(const std::string& key)
    {
        return message_pools[key];
    }

    const MessagePool<MessageRecord>& at(const std::string& key) const
    {
        return message_pools.at(key);
    }

    void insert(const std::string& key, const MessagePool<MessageRecord>& value)
    {
        message_pools[key] = value;
    }

    void erase(const std::string& key)
    {
        message_pools.erase(key);
    }

    void clear()
    {
        message_pools.clear();
    }

    bool contains(const std::string& key) const
    {
        return message_pools.contains(key);
    }

protected:
    Language::Type language;
    std::string language_str;
    tsl::hopscotch_map<std::string, MessagePool<MessageRecord>> message_pools;
};