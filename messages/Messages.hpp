#pragma once

#include <string>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Message {
    int string_index;
    int token_index;
    std::string token_value;
    std::string message_pool;
    std::string message_pool_identifier;
    json message_text;

    Message()
        : string_index(-1), token_index(-1), token_value(""), message_pool(""), message_pool_identifier("") {}
    Message(int string_index, int token_index, const std::string& message_pool, const std::string& message_pool_identifier)
        : string_index(string_index), token_index(token_index), token_value(""), message_pool(message_pool), message_pool_identifier(message_pool_identifier) {}
    Message(int string_index, int token_index, const std::string& token_value, const std::string& message_pool, const std::string& message_pool_identifier)
        : string_index(string_index), token_index(token_index), token_value(token_value), message_pool(message_pool), message_pool_identifier(message_pool_identifier) {}

    bool operator<(const Message& other) const
    {
        return token_index < other.token_index;
    }

    bool operator>(const Message& other) const
    {
        return token_index > other.token_index;
    }

    bool operator==(const Message& other) const
    {
        return string_index == other.string_index && 
            token_index == other.token_index && 
            token_value == other.token_value && 
            message_pool == other.message_pool &&
            message_pool_identifier == other.message_pool_identifier;
    }
};