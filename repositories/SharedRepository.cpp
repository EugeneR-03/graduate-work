#include "SharedRepository.hpp"

SharedRepository* SharedRepository::instance;

SharedRepository::SharedRepository() 
    : connection(nullptr)
{
    init();
}

SharedRepository::~SharedRepository()
{
    disconnect();
    delete algorithm_repository;
    delete representation_repository;
    delete representation_string_repository;
    delete algorithm_representation_repository;
    delete instance;
}

void SharedRepository::init()
{
    algorithm_repository = new AlgorithmRepository();
    representation_repository = new RepresentationRepository();
    representation_string_repository = new RepresentationStringRepository();
    algorithm_representation_repository = new AlgorithmRepresentationRepository();

    auto pool_en = DataUtils::JsonUtils::read("res/values-en/messages.json");
    auto pool_ru = DataUtils::JsonUtils::read("res/values-ru/messages.json");

    message_storage = MessageStorage();
    pool_en.get_to(message_storage["en"]);
    pool_ru.get_to(message_storage["ru"]);
    message_storage.switch_language(Language::Type::Russian);
}


void SharedRepository::connect(std::string connection)
{
    this->connection = new pqxx::connection(connection);
    connect(this->connection);
}

void SharedRepository::connect(pqxx::connection* connection)
{
    algorithm_repository->connect(connection);
    representation_repository->connect(connection);
    representation_string_repository->connect(connection);
    algorithm_representation_repository->connect(connection);
}

void SharedRepository::disconnect() {
    algorithm_repository->disconnect();
    representation_repository->disconnect();
    representation_string_repository->disconnect();
    algorithm_representation_repository->disconnect();
}

SharedRepository& SharedRepository::get_instance()
{
    if (!instance) {
        instance = new SharedRepository();
    }
    return *instance;
}

MessageStorage& SharedRepository::get_message_storage()
{
    return message_storage;
}

IRepository<AlgorithmDTO>& SharedRepository::get_algorithm_repository()
{
    return *algorithm_repository;
}

IRepository<RepresentationDTO>& SharedRepository::get_representation_repository()
{
    return *representation_repository;
}

IRepository<RepresentationStringDTO>& SharedRepository::get_representation_string_repository()
{
    return *representation_string_repository;
}

IRepository<AlgorithmRepresentation>& SharedRepository::get_algorithm_representation_repository()
{
    return *algorithm_representation_repository;
}