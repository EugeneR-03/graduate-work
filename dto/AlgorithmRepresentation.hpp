#pragma once

#include <string>
#include "AlgorithmDTO.hpp"
#include "RepresentationDTO.hpp"
#include "RepresentationStringDTO.hpp"

struct AlgorithmRepresentation {
    AlgorithmDTO algorithm;
    RepresentationDTO representation;
    std::vector<RepresentationStringDTO> representation_strings;

    AlgorithmRepresentation() {}
    AlgorithmRepresentation(AlgorithmDTO algorithm, RepresentationDTO representation, std::vector<RepresentationStringDTO> representation_strings) :
        algorithm(algorithm), representation(representation), representation_strings(representation_strings) {}

    // AlgorithmRepresentation(int algorithm_id, std::string name, std::string description, int representationId, int dimensionality, int iterations, std::string content, std::string validity) :

    bool operator<(const AlgorithmRepresentation& other) const {
        return representation.representation_id < other.representation.representation_id;
    }

    bool operator==(const AlgorithmRepresentation& other) const {
        return representation.representation_id == other.representation.representation_id;
    }
};