#pragma once

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct AlgorithmDTO {
    int algorithm_id;
    json name;
    json description;

    AlgorithmDTO() {}
    AlgorithmDTO(int id, json name, json description) : algorithm_id(id), name(name), description(description) {}

    bool operator<(const AlgorithmDTO& other) const {
        return algorithm_id < other.algorithm_id;
    }

    bool operator==(const AlgorithmDTO& other) const {
        return algorithm_id == other.algorithm_id;
    }
};