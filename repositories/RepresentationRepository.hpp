#pragma once

#include <string>
#include <vector>
#include <pqxx/pqxx>

#include "IRepository.hpp"
#include "../dto/RepresentationDTO.hpp"

class RepresentationRepository : public IRepository<RepresentationDTO> {
public:
    RepresentationRepository();
    ~RepresentationRepository();

    void connect(std::string connection) override;
    void connect(pqxx::connection* connection) override;
    void disconnect() override;

    void add(RepresentationDTO object) override;
    void update(RepresentationDTO object) override;
    void remove(RepresentationDTO object) override;

    std::vector<RepresentationDTO> get_all() override;
    std::optional<RepresentationDTO> get_by_id(int id) override;
    std::vector<RepresentationDTO> get_by_field(const std::string& field, const std::string& value) override;

private:
    pqxx::connection* connection;
};