#pragma once

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct RepresentationStringDTO {
    long int representation_string_id;
    int representation_id;
    std::string content;
    std::optional<json> validity;

    RepresentationStringDTO() {}
    RepresentationStringDTO(long int id, int representation_id, const std::string& content, std::optional<json> validity) : 
        representation_string_id(id), representation_id(representation_id), content(content), validity(validity) {}
    
    bool operator<(const RepresentationStringDTO& other) const {
        return representation_string_id < other.representation_string_id;
    }

    bool operator==(const RepresentationStringDTO& other) const {
        return representation_string_id == other.representation_string_id;
    }
};