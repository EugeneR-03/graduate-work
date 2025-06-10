#pragma once

#include <iostream>
#include <pqxx/pqxx>

#include "IRepository.hpp"
#include "../dto/AlgorithmDTO.hpp"
#include "../dto/RepresentationDTO.hpp"
#include "../dto/RepresentationStringDTO.hpp"
#include "AlgorithmRepository.hpp"
#include "RepresentationRepository.hpp"
#include "RepresentationStringRepository.hpp"
#include "AlgorithmRepresentationRepository.hpp"
#include "MessageStorage.hpp"
#include "../messages/MessagePool.hpp"
#include "../utils/DataUtils.hpp"

// синглтон
// фасад для репозиториев
class SharedRepository {
public:
    SharedRepository();
    ~SharedRepository();
    SharedRepository(const SharedRepository&) = delete;
    SharedRepository& operator=(const SharedRepository&) = delete;

    void connect(std::string connection);
    void connect(pqxx::connection* connection);
    void disconnect();

    static SharedRepository& get_instance();
    MessageStorage& get_message_storage();
    IRepository<AlgorithmDTO>& get_algorithm_repository();
    IRepository<RepresentationDTO>& get_representation_repository();
    IRepository<RepresentationStringDTO>& get_representation_string_repository();
    IRepository<AlgorithmRepresentation>& get_algorithm_representation_repository();

private:
    static SharedRepository* instance;
    IRepository<AlgorithmDTO>* algorithm_repository;
    IRepository<RepresentationDTO>* representation_repository;
    IRepository<RepresentationStringDTO>* representation_string_repository;
    IRepository<AlgorithmRepresentation>* algorithm_representation_repository;

    MessageStorage message_storage;

    pqxx::connection *connection;

    void init();
};