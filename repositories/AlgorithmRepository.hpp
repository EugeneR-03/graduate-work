#pragma once

#include <string>
#include <vector>
#include <pqxx/pqxx>

#include "IRepository.hpp"
#include "../dto/AlgorithmDTO.hpp"

class AlgorithmRepository : public IRepository<AlgorithmDTO> {
public:
    AlgorithmRepository();
    ~AlgorithmRepository();

    void connect(std::string connection) override;
    void connect(pqxx::connection* connection) override;
    void disconnect() override;

    void add(AlgorithmDTO object) override;
    void update(AlgorithmDTO object) override;
    void remove(AlgorithmDTO object) override;

    std::vector<AlgorithmDTO> get_all() override;
    std::optional<AlgorithmDTO> get_by_id(int id) override;
    std::vector<AlgorithmDTO> get_by_field(const std::string& field, const std::string& value) override;

private:
    pqxx::connection *connection;
};