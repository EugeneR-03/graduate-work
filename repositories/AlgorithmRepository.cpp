#include "AlgorithmRepository.hpp"

AlgorithmRepository::AlgorithmRepository() 
    : connection(nullptr) {}

AlgorithmRepository::~AlgorithmRepository() {
    disconnect();
}

void AlgorithmRepository::connect(std::string connection)
{
    this->connection = new pqxx::connection(connection);
}

void AlgorithmRepository::connect(pqxx::connection* connection)
{
    this->connection = connection;
}

void AlgorithmRepository::disconnect()
{
    if (connection == nullptr) return;

    delete connection;
    connection = nullptr;
}

void AlgorithmRepository::add(AlgorithmDTO object)
{
    pqxx::work tx(*connection);

    tx.exec(R"(
        INSERT INTO algorithms_data (name, description)
        VALUES ($1::jsonb, $2::jsonb);
    )", pqxx::params{object.name.dump(), object.description.dump()});
    tx.commit();
}

void AlgorithmRepository::update(AlgorithmDTO object)
{
    pqxx::work tx(*connection);

    tx.exec(R"(
        UPDATE algorithms_data
        SET name = $1::jsonb, description = $2::jsonb
        WHERE algorithm_id = $3;
    )", pqxx::params{object.name.dump(), object.description.dump(), object.algorithm_id});
    tx.commit();
}

void AlgorithmRepository::remove(AlgorithmDTO object)
{
    pqxx::work tx(*connection);

    tx.exec(R"(
        DELETE FROM algorithms_data
        WHERE algorithm_id = $1;
    )", pqxx::params{object.algorithm_id});
    tx.commit();
}

std::vector<AlgorithmDTO> AlgorithmRepository::get_all()
{
    pqxx::work tx(*connection);

    pqxx::result rows = tx.exec(R"(
        SELECT * FROM algorithms_data;
    )");

    std::vector<AlgorithmDTO> algorithms_data;
    algorithms_data.reserve(rows.size());

    for (const auto& row : rows) {
        auto [algorithm_id, name, description] = row.as<int, std::string, std::string>();
        algorithms_data.push_back(AlgorithmDTO(algorithm_id, json::parse(name), json::parse(description)));
    }

    return algorithms_data;
}

std::optional<AlgorithmDTO> AlgorithmRepository::get_by_id(int id)
{
    pqxx::work tx(*connection);

    pqxx::result rows = tx.exec(R"(
        SELECT * FROM algorithms_data
        WHERE algorithm_id = $1;
    )", pqxx::params{id});

    if (rows.empty()) return std::nullopt;

    auto [algorithm_id, name, description] = rows.front().as<int, std::string, std::string>();
    return AlgorithmDTO(algorithm_id, json::parse(name), json::parse(description));
}

std::vector<AlgorithmDTO> AlgorithmRepository::get_by_field(const std::string& field, const std::string& value)
{
    pqxx::work tx(*connection);

    auto regex = make_regex_by_words(value);
    pqxx::result rows = tx.exec(R"(
        SELECT * FROM algorithms_data
        WHERE $1 ~ $2;
    )", pqxx::params{field, regex});

    std::vector<AlgorithmDTO> algorithms_data;
    algorithms_data.reserve(rows.size());

    for (const auto& row : rows) {
        auto [algorithm_id, name, description] = row.as<int, std::string, std::string>();
        algorithms_data.push_back(AlgorithmDTO(algorithm_id, json::parse(name), json::parse(description)));
    }

    return algorithms_data;
}
