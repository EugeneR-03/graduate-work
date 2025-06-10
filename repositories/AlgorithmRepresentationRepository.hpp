#pragma once

#include <set>
#include <iostream>
#include <execution>
#include <pqxx/pqxx>

#include "IRepository.hpp"
#include "../dto/AlgorithmDTO.hpp"
#include "../dto/RepresentationDTO.hpp"
#include "../dto/RepresentationStringDTO.hpp"
#include "../dto/AlgorithmRepresentation.hpp"
#include "SharedRepository.hpp"

class AlgorithmRepresentationRepository : public IRepository<AlgorithmRepresentation> {
public:
    AlgorithmRepresentationRepository();
    ~AlgorithmRepresentationRepository();

    void init();
    void connect(std::string connection) override;
    void connect(pqxx::connection* connection) override;
    void disconnect() override;

    void add(AlgorithmRepresentation object) override;
    void update(AlgorithmRepresentation object) override;
    void remove(AlgorithmRepresentation object) override;

    std::vector<AlgorithmRepresentation> get_all() override;
    std::optional<AlgorithmRepresentation> get_by_id(int id) override;
    std::vector<AlgorithmRepresentation> get_by_field(const std::string& field, const std::string& value) override;

private:
    pqxx::connection* connection;
    IRepository<AlgorithmDTO>* algorithm_repository;
    IRepository<RepresentationDTO>* representation_repository;
    IRepository<RepresentationStringDTO>* representation_string_repository;
};