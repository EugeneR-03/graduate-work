#include "RepresentationStringRepository.hpp"

RepresentationStringRepository::RepresentationStringRepository() 
    : connection(nullptr) {}

RepresentationStringRepository::~RepresentationStringRepository()
{
    disconnect();
}

void RepresentationStringRepository::connect(std::string connection)
{
    this->connection = new pqxx::connection(connection);
}

void RepresentationStringRepository::connect(pqxx::connection* connection)
{
    this->connection = connection;
}

void RepresentationStringRepository::disconnect()
{
    if (this->connection == nullptr) return;

    delete this->connection;
    this->connection = nullptr;
}

void RepresentationStringRepository::add(RepresentationStringDTO object)
{
    pqxx::work tx(*connection);

    auto object_validity = object.validity.has_value() ? std::make_optional(object.validity.value().dump()) : std::nullopt;
    
    tx.exec(R"(
        INSERT INTO representation_strings_data (representation_id, content, validity)
        VALUES ($1, $2, $3::jsonb);
    )", pqxx::params{
        object.representation_id, 
        object.content, 
        object_validity
    });
    tx.commit();
}

void RepresentationStringRepository::update(RepresentationStringDTO object)
{
    pqxx::work tx(*connection);

    auto object_validity = object.validity.has_value() ? std::make_optional(object.validity.value().dump()) : std::nullopt;
    tx.exec(R"(
        UPDATE representation_strings_data
        SET content = $1, validity = $2::jsonb
        WHERE representation_string_id = $3;
    )", pqxx::params{
        object.content, 
        object_validity,
        object.representation_string_id
    });
    tx.commit();
}

void RepresentationStringRepository::remove(RepresentationStringDTO object)
{
    pqxx::work tx(*connection);

    tx.exec(R"(
        DELETE FROM representation_strings_data
        WHERE representation_string_id = $1;
    )", pqxx::params{object.representation_string_id});
    tx.commit();
}

std::vector<RepresentationStringDTO> RepresentationStringRepository::get_all()
{
    pqxx::work tx(*connection);

    pqxx::result rows = tx.exec(R"(
        SELECT * FROM representation_strings_data;
    )");

    std::vector<RepresentationStringDTO> representation_strings_data;
    representation_strings_data.reserve(rows.size());

    for (const auto& row : rows) {
        auto [representation_string_id, representation_id, content, validity_optional] = row.as<int, int, std::string, std::optional<std::string>>();
        
        auto validity = validity_optional.has_value() ? std::make_optional(json::parse(validity_optional.value())) : std::nullopt;
        RepresentationStringDTO representation_string(representation_string_id, representation_id, content, validity);

        representation_strings_data.push_back(representation_string);
    }

    return representation_strings_data;
}

std::optional<RepresentationStringDTO> RepresentationStringRepository::get_by_id(int id)
{
    pqxx::work tx(*connection);

    pqxx::result rows = tx.exec(R"(
        SELECT * FROM representation_strings_data
        WHERE representation_string_id = $1;
    )", pqxx::params{id});

    if (rows.empty()) return std::nullopt;

    auto [representation_string_id, representation_id, content, validity_optional] = rows.front().as<int, int, std::string, std::optional<std::string>>();

    auto validity = validity_optional.has_value() ? std::make_optional(json::parse(validity_optional.value())) : std::nullopt;
    return RepresentationStringDTO(representation_string_id, representation_id, content, validity);
}

std::vector<RepresentationStringDTO> RepresentationStringRepository::get_by_field(const std::string& field, const std::string& value)
{
    pqxx::work tx(*connection);

    auto regex = make_regex_by_words(value);
    pqxx::result rows = tx.exec(R"(
        SELECT * FROM representation_strings_data
        WHERE $1 ~* $2;
    )", pqxx::params{field, regex});

    std::vector<RepresentationStringDTO> representation_strings_data;
    representation_strings_data.reserve(rows.size());

    for (const auto& row : rows) {
        auto [representation_string_id, representation_id, content, validity_optional] = row.as<int, int, std::string, std::optional<std::string>>();
        
        auto validity = validity_optional.has_value() ? std::make_optional(json::parse(validity_optional.value())) : std::nullopt;
        RepresentationStringDTO representation_string(representation_string_id, representation_id, content, validity);
        representation_strings_data.push_back(representation_string);
    }

    return representation_strings_data;
}