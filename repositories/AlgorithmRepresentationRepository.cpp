#include "AlgorithmRepresentationRepository.hpp"

AlgorithmRepresentationRepository::AlgorithmRepresentationRepository() 
    : connection(nullptr), 
      algorithm_repository(nullptr), 
      representation_repository(nullptr), 
      representation_string_repository(nullptr) {}

AlgorithmRepresentationRepository::~AlgorithmRepresentationRepository()
{
    disconnect();
}

void AlgorithmRepresentationRepository::init()
{
    algorithm_repository = &SharedRepository::get_instance().get_algorithm_repository();
    representation_repository = &SharedRepository::get_instance().get_representation_repository();
    representation_string_repository = &SharedRepository::get_instance().get_representation_string_repository();
}

void AlgorithmRepresentationRepository::connect(std::string connection)
{
    this->connection = new pqxx::connection(connection);
    init();
}

void AlgorithmRepresentationRepository::connect(pqxx::connection* connection)
{
    this->connection = connection;
    init();
}

void AlgorithmRepresentationRepository::disconnect()
{
    if (this->connection == nullptr) return;

    delete this->connection;
    this->connection = nullptr;

    this->algorithm_repository = nullptr;
    this->representation_repository = nullptr;
    this->representation_string_repository = nullptr;
}

void AlgorithmRepresentationRepository::add(AlgorithmRepresentation object)
{
    algorithm_repository->add(object.algorithm);
    representation_repository->add(object.representation);

    for (auto representationString : object.representation_strings) {
        representation_string_repository->add(representationString);
    }
}

void AlgorithmRepresentationRepository::update(AlgorithmRepresentation object)
{
    algorithm_repository->update(object.algorithm);
    representation_repository->update(object.representation);

    for (auto representationString : object.representation_strings) {
        representation_string_repository->update(representationString);
    }
}

void AlgorithmRepresentationRepository::remove(AlgorithmRepresentation object)
{
    algorithm_repository->remove(object.algorithm);
}

std::vector<AlgorithmRepresentation> AlgorithmRepresentationRepository::get_all()
{
    if (this->connection == nullptr) return {};

    pqxx::work tx(*this->connection);

    pqxx::result rows = tx.exec(R"(
        SELECT
            ad.algorithm_id AS algorithm_id,
            ad.name AS name,
            ad.description AS description,
            rd.representation_id AS representation_id,
            rd.dimensionality AS dimensionality,
            rd.iterations AS iterations
        FROM main.algorithms_data ad
        JOIN main.representations_data rd
            ON ad.algorithm_id = rd.algorithm_id;
    )");
    tx.commit();

    auto representation_strings_data = representation_string_repository->get_all();

    std::unordered_map<int, std::vector<RepresentationStringDTO>> representation_strings_map;
    for (auto representationString : representation_strings_data) {
        representation_strings_map[representationString.representation_id].push_back(representationString);
    }

    std::vector<AlgorithmRepresentation> algorithms_representations;
    algorithms_representations.reserve(rows.size());
    #pragma omp parallel for
    for (const auto& row : rows) {
        int algorithm_id = row["algorithm_id"].as<int>();
        std::string name = row["name"].as<std::string>();
        std::string description = row["description"].as<std::string>();
        int representation_id = row["representation_id"].as<int>();
        auto dimensionality_arr = row["dimensionality"].as_sql_array<int>();
        std::vector<int> dimensionality(dimensionality_arr.cbegin(), dimensionality_arr.cend());
        int iterations = row["iterations"].as<int>();

        AlgorithmDTO algorithm(algorithm_id, json::parse(name), json::parse(description));
        RepresentationDTO representation(representation_id, algorithm_id, dimensionality, iterations);

        #pragma omp critical
        algorithms_representations.push_back(AlgorithmRepresentation(algorithm, representation, representation_strings_map[representation_id]));
    }

    std::sort(std::execution::par_unseq, algorithms_representations.begin(), algorithms_representations.end(), std::less<AlgorithmRepresentation>());
    return algorithms_representations;
}

std::optional<AlgorithmRepresentation> AlgorithmRepresentationRepository::get_by_id(int id)
{
    if (this->connection == nullptr) return std::nullopt;

    pqxx::work tx(*this->connection);

    pqxx::result rows = tx.exec(R"(
        SELECT
            ad.algorithm_id AS algorithm_id,
            ad.name AS name,
            ad.description AS description,
            rd.representation_id AS representation_id,
            rd.dimensionality AS dimensionality,
            rd.iterations AS iterations,
            rsd.representation_string_id AS representation_string_id,
            rsd.content AS content,
            rsd.validity AS validity
        FROM main.algorithms_data ad
        JOIN main.representations_data rd
            ON ad.algorithm_id = rd.algorithm_id
        JOIN main.representation_strings_data rsd
            ON rd.representation_id = rsd.representation_id
        WHERE rd.representation_id = $1;
    )", pqxx::params{id});

    if (rows.empty()) return std::nullopt;

    auto row = rows.front();
    int algorithm_id = row["algorithm_id"].as<int>();
    std::string name = row["name"].as<std::string>();
    std::string description = row["description"].as<std::string>();
    int representation_id = row["representation_id"].as<int>();
    auto dimensionality_arr = row["dimensionality"].as_sql_array<int>();
    std::vector<int> dimensionality(dimensionality_arr.cbegin(), dimensionality_arr.cend());
    int iterations = row["iterations"].as<int>();
    long int representation_string_id = row["representation_string_id"].as<long int>();
    std::string content = row["content"].as<std::string>();
    std::optional<std::string> validity_optional = row["validity"].as<std::optional<std::string>>();

    auto validity = validity_optional.has_value() ? std::make_optional(json::parse(validity_optional.value())) : std::nullopt;
    AlgorithmDTO algorithm(algorithm_id, json::parse(name), json::parse(description));
    RepresentationDTO representation(representation_id, algorithm_id, dimensionality, iterations);
    RepresentationStringDTO representation_string(representation_string_id, representation_id, content, validity);

    AlgorithmRepresentation algorithm_representation(algorithm, representation, {representation_string});
    return algorithm_representation;
}

std::vector<AlgorithmRepresentation> AlgorithmRepresentationRepository::get_by_field(const std::string& field, const std::string& value)
{
    if (this->connection == nullptr) return {};
    if (field != "algorithm_id" && field != "name" && field != "description" && 
        field != "representation_id" && field != "dimensionality" && field != "iterations") return {};
    
    pqxx::work tx(*this->connection);

    auto regex = make_regex_by_words(value);
    
    std::string where_clause;
    if (field == "algorithm_id")
        where_clause = std::format("ad.algorithm_id = {}", regex);
    else if (field == "name")
        where_clause = std::format("ad.name->>main.lang() ~* '{}'", regex);
    else if (field == "description")
        where_clause = std::format("ad.description->>main.lang() ~* '{}'", regex);
    else if (field == "representation_id")
        where_clause = std::format("rd.representation_id = {}", regex);
    else if (field == "dimensionality")
        where_clause = std::format("rd.dimensionality = {}", regex);
    else if (field == "iterations")
        where_clause = std::format("rd.iterations = {}", regex);

    std::string query = std::format(R"(
        SELECT
            ad.algorithm_id AS algorithm_id,
            ad.name AS name,
            ad.description AS description,
            rd.representation_id AS representation_id,
            rd.dimensionality AS dimensionality,
            rd.iterations AS iterations,
            jsonb_agg(rsd) AS representation_strings
        FROM main.algorithms_data ad
        JOIN main.representations_data rd
            ON ad.algorithm_id = rd.algorithm_id
        JOIN main.representation_strings_data rsd
            ON rd.representation_id = rsd.representation_id
        WHERE {0}
        GROUP BY 
            ad.algorithm_id, ad.name, ad.description, rd.representation_id, rd.dimensionality, rd.iterations;
    )", where_clause);

    pqxx::result rows = tx.exec(query);

    std::vector<AlgorithmRepresentation> algorithms_representations;
    algorithms_representations.reserve(rows.size());

    #pragma omp parallel for
    for (const auto& row : rows) {
        int algorithm_id = row["algorithm_id"].as<int>();
        std::string name = row["name"].as<std::string>();
        std::string description = row["description"].as<std::string>();
        int representation_id = row["representation_id"].as<int>();
        auto dimensionality_arr = row["dimensionality"].as_sql_array<int>();
        std::vector<int> dimensionality(dimensionality_arr.cbegin(), dimensionality_arr.cend());
        int iterations = row["iterations"].as<int>();
        std::string representation_strings = row["representation_strings"].as<std::string>();

        json representation_strings_json = json::parse(representation_strings);
        std::vector<RepresentationStringDTO> representation_strings_data;
        for (const auto& representation_string_json : representation_strings_json) {
            std::optional<json> validity;
            try {
            validity = representation_string_json["validity"] == nullptr 
                ? std::nullopt 
                : std::make_optional(representation_string_json["validity"]);
            }
            catch (const std::exception& e) {
                validity = std::nullopt;
            }

            RepresentationStringDTO representation_string(
                representation_string_json["representation_string_id"].get<int>(),
                representation_string_json["representation_id"].get<int>(),
                representation_string_json["content"].get<std::string>(),
                validity
            );
            representation_strings_data.push_back(representation_string);
        }

        // сортируем representation_strings_data по representation_string_id
        // параллельная сортировка
        std::sort(std::execution::par_unseq, representation_strings_data.begin(), representation_strings_data.end(), [](const RepresentationStringDTO& a, const RepresentationStringDTO& b) {
            return a.representation_string_id < b.representation_string_id;
        });

        AlgorithmDTO algorithm(algorithm_id, json::parse(name), json::parse(description));
        RepresentationDTO representation(representation_id, algorithm_id, dimensionality, iterations);
        AlgorithmRepresentation algorithm_representation(algorithm, representation, representation_strings_data);
        #pragma omp critical
        algorithms_representations.push_back(algorithm_representation);
    }

    return algorithms_representations;
}