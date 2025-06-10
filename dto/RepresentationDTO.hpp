#pragma once

#include <string>
#include <vector>

struct RepresentationDTO {
    int representation_id;
    int algorithm_id;
    std::vector<int> dimensionality;
    int iterations;

    RepresentationDTO() {}
    RepresentationDTO(int id, int algorithm_id, const std::vector<int>& dimensionality, int iterations) : 
        representation_id(id), algorithm_id(algorithm_id), dimensionality(dimensionality), iterations(iterations) {}

    // перегрузим оператор <
    bool operator<(const RepresentationDTO& other) const {
        return representation_id < other.representation_id;
    }

    bool operator==(const RepresentationDTO& other) const {
        return representation_id == other.representation_id;
    }
};