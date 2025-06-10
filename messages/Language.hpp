#pragma once

#include <string>

namespace Language {
    enum class Type : short {
        Unknown,
        English,
        Russian
    };

    inline std::string type_to_string(Type type, bool full_names = false)
    {
        if (full_names) {
            switch (type) {
                case Type::English:
                    return "English";
                case Type::Russian:
                    return "Russian";
                default:
                    return "Unknown";
            }
        }
        else {
            switch (type) {
                case Type::English:
                    return "en";
                case Type::Russian:
                    return "ru";
                default:
                    return "Unknown";
            }
        }
    }

    inline Type type_from_string(const std::string& type)
    {
        Type result = Type::English;

        std::string lower_type = type;
        std::transform(
            type.begin(), type.end(), 
            lower_type.begin(), 
            ::tolower
        );

        if (lower_type == "english" || lower_type == "en") {
            result = Type::English;
        }
        else if (lower_type == "russian" || 
                lower_type == "ru" || 
                lower_type == "русский") {
            result = Type::Russian;
        }

        return result;
    }
}