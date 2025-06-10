#include "RepresentationRepository.hpp"

RepresentationRepository::RepresentationRepository() 
    : connection(nullptr) {}

RepresentationRepository::~RepresentationRepository()
{
    disconnect();
}

void RepresentationRepository::connect(std::string connection)
{
    this->connection = new pqxx::connection(connection);
}

void RepresentationRepository::connect(pqxx::connection* connection)
{
    this->connection = connection;
}

void RepresentationRepository::disconnect()
{
    if (this->connection == nullptr) return;

    delete this->connection;
    this->connection = nullptr;
}

void RepresentationRepository::add(RepresentationDTO object)
{
    pqxx::work tx(*connection);

    tx.exec(R"(
        INSERT INTO representations_data (algorithm_id, dimensionality, iterations) 
        VALUES ($1, $2, $3);
    )", pqxx::params{object.algorithm_id, object.dimensionality, object.iterations});
    tx.commit();
}

void RepresentationRepository::update(RepresentationDTO object)
{
    pqxx::work tx(*connection);

    tx.exec(R"(
        UPDATE representations_data
        SET dimensionality = $1, iterations = $2
        WHERE representation_id = $3;
    )", pqxx::params{object.dimensionality, object.iterations, object.representation_id});
    tx.commit();
}

void RepresentationRepository::remove(RepresentationDTO object)
{
    pqxx::work tx(*connection);

    tx.exec(R"(
        DELETE FROM representations_data
        WHERE representation_id = $1;
    )", pqxx::params{object.representation_id});
    tx.commit();
}

std::vector<RepresentationDTO> RepresentationRepository::get_all()
{
    pqxx::work tx(*connection);

    pqxx::result rows = tx.exec(R"(
        SELECT * FROM representations_data;
    )");

    std::vector<RepresentationDTO> representations_data;
    representations_data.reserve(rows.size());

    for (const auto& row : rows) {
        int representation_id = row["representation_id"].as<int>();
        int algorithm_id = row["algorithm_id"].as<int>();
        auto dimensionality_arr = row["dimensionality"].as_sql_array<int>();
        std::vector<int> dimensionality(dimensionality_arr.cbegin(), dimensionality_arr.cend());
        int iterations = row["iterations"].as<int>();
        RepresentationDTO representation(representation_id, algorithm_id, dimensionality, iterations);
        representations_data.push_back(representation);
    }

    return representations_data;
}

std::optional<RepresentationDTO> RepresentationRepository::get_by_id(int id)
{
    pqxx::work tx(*connection);

    pqxx::result rows = tx.exec(R"(
        SELECT * FROM representations_data
        WHERE representation_id = $1;
    )", pqxx::params{id});

    if (rows.empty()) return std::nullopt;

    auto row = rows.front();
    int representation_id = row["representation_id"].as<int>();
    int algorithm_id = row["algorithm_id"].as<int>();
    auto dimensionality_arr = row["dimensionality"].as_sql_array<int>();
    std::vector<int> dimensionality(dimensionality_arr.cbegin(), dimensionality_arr.cend());
    int iterations = row["iterations"].as<int>();
    return RepresentationDTO(representation_id, algorithm_id, dimensionality, iterations);
}

std::vector<RepresentationDTO> RepresentationRepository::get_by_field(const std::string& field, const std::string& value)
{
    pqxx::work tx(*connection);

    auto regex = make_regex_by_words(value);
    pqxx::result rows = tx.exec(R"(
        SELECT * FROM representations_data
        WHERE $1 ~ $2;
    )", pqxx::params{field, regex});

    std::vector<RepresentationDTO> representations_data;
    representations_data.reserve(rows.size());

    for (const auto& row : rows) {
        int representation_id = row["representation_id"].as<int>();
        int algorithm_id = row["algorithm_id"].as<int>();
        auto dimensionality_arr = row["dimensionality"].as_sql_array<int>();
        std::vector<int> dimensionality(dimensionality_arr.cbegin(), dimensionality_arr.cend());
        int iterations = row["iterations"].as<int>();
        RepresentationDTO representation(representation_id, algorithm_id, dimensionality, iterations);
        representations_data.push_back(representation);
    }

    return representations_data;
}