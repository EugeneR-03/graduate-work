#pragma once

#include <string>
#include <vector>
#include <execution>
#include <ranges>
#include <pqxx/pqxx>

#include "IRepository.hpp"
#include "../dto/RepresentationStringDTO.hpp"

class RepresentationStringRepository : public IRepository<RepresentationStringDTO> {    
public:
    RepresentationStringRepository();
    ~RepresentationStringRepository();

    void connect(std::string connection) override;
    void connect(pqxx::connection* connection) override;
    void disconnect() override;

    void add(RepresentationStringDTO object) override;
    void update(RepresentationStringDTO object) override;
    void remove(RepresentationStringDTO object) override;

    std::vector<RepresentationStringDTO> get_all() override;
    std::optional<RepresentationStringDTO> get_by_id(int id) override;
    std::vector<RepresentationStringDTO> get_by_field(const std::string& field, const std::string& value) override;

private:
    pqxx::connection* connection;
};