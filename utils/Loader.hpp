#pragma once

#include <string>
#include <vector>
#include <memory>
#include <windows.h>

#include "Settings.hpp"
#include "./../repositories/SharedRepository.hpp"

class SharedMemoryFile {
public:
    SharedMemoryFile(const SharedMemoryFile& other) = delete;
    SharedMemoryFile& operator=(const SharedMemoryFile& other) = delete;
    
    SharedMemoryFile(SharedMemoryFile&& other) noexcept
        : name(other.name), 
          handle(std::move(other.handle)), 
          view(std::move(other.view)), 
          size(other.size) {}
    
    SharedMemoryFile& operator=(SharedMemoryFile&& other) noexcept {
        name = other.name;
        handle = std::move(other.handle);
        view = std::move(other.view);
        size = other.size;

        other.view.reset();
        return *this;
    }

    SharedMemoryFile(const std::wstring& name, const std::vector<std::string>& data)
        : name(name)
    {
        size = calculate_required_size(data);
        handle.reset(CreateFileMappingW(
            INVALID_HANDLE_VALUE, nullptr,
            PAGE_READWRITE,
            static_cast<DWORD>(size >> 32),
            static_cast<DWORD>(size & 0xFFFFFFFF),
            name.c_str()
        ));
        if(!handle) {
            throw std::runtime_error("CreateFileMappingW failed");
        }

        view.reset(MapViewOfFile(
            handle.get(), 
            FILE_MAP_ALL_ACCESS, 
            0, 
            0, 
            size
        ));
        if(!view) {
            throw std::runtime_error("MapViewOfFile failed");
        }

        write(data);
    }

    explicit SharedMemoryFile(const std::wstring& name)
        : name(name)
    {
        handle.reset(OpenFileMappingW(
            FILE_MAP_ALL_ACCESS,
            false,
            name.c_str()
        ));
        if (!handle) {
            throw std::runtime_error("OpenFileMapping() failed");
        }

        view.reset(MapViewOfFile(
            handle.get(), 
            FILE_MAP_ALL_ACCESS, 
            0, 
            0, 
            0
        ));
        if (!view) {
            throw std::runtime_error("MapViewOfFile() failed");
        }

        MEMORY_BASIC_INFORMATION info;
        if (!VirtualQuery(
            view.get(),
            &info,
            sizeof(info)
        )) {
            throw std::runtime_error("VirtualQuery() failed");
        }
        size = info.RegionSize;
    }

    std::vector<std::string> read() const
    {
        const char* ptr = static_cast<const char*>(view.get());
        std::vector<std::string> result;
        
        // читаем количество строк
        uint32_t count;
        std::memcpy(&count, ptr, sizeof(count));
        ptr += sizeof(count);
        
        // читаем сами строки
        result.reserve(count);
        for (uint32_t i = 0; i < count; ++i) {
            result.emplace_back(ptr);
            ptr += result.back().size() + 1;
        }
        
        return result;
    }

    void write(const std::vector<std::string>& data)
    {
        size_t required_size = calculate_required_size(data);
        if (size < required_size) {
            throw std::runtime_error("Not enough memory");
        }

        char* ptr = static_cast<char*>(view.get());

        // записываем количество строк
        uint32_t count = static_cast<uint32_t>(data.size());
        std::memcpy(ptr, &count, sizeof(count));
        ptr += sizeof(count);
        
        // записываем сами строки
        for (const auto& str : data) {
            std::memcpy(ptr, str.c_str(), str.size() + 1);
            ptr += str.size() + 1;
        }

        FlushViewOfFile(view.get(), 0);
    }

    size_t count() const
    {
        uint32_t count;
        std::memcpy(&count, view.get(), sizeof(count));
        return count;
    }

    bool empty() const
    {
        return count() == 0;
    }

    void close()
    {
        view.reset();
        handle.reset();
    }

private:
    struct HandleDeleter {
        void operator()(HANDLE handle) const noexcept {
            if (handle) {
                CloseHandle(handle);
            }
        }
    };

    struct ViewDeleter {
        void operator()(void* ptr) const noexcept {
            if (ptr) {
                UnmapViewOfFile(ptr);
            }
        }
    };

    static size_t calculate_required_size(const std::vector<std::string>& data)
    {
        size_t size = sizeof(uint32_t);     // количество строк
        for (const auto& str : data) {
            size += str.size() + 1;         // строка + нуль-терминатор
        }
        return size;
    }

    size_t size;
    std::wstring name;
    std::unique_ptr<std::remove_pointer_t<HANDLE>, HandleDeleter> handle {nullptr};
    std::unique_ptr<void, ViewDeleter> view {nullptr};
};

class Loader {
public:
    static void load(Settings settings, std::wstring shared_memory_file_name)
    {
        switch (settings.global_settings.work_mode) {
            case MainWorkMode::File: {
                std::vector<std::string> strings = DataUtils::FileUtils::read_strings_from_file(settings.file_settings.input_file_path);
                shared_memory_file = std::make_unique<SharedMemoryFile>(shared_memory_file_name, strings);
                break;
            }

            case MainWorkMode::DB: {
                SharedRepository::get_instance().connect(settings.db_settings.db_connection);
                auto algorithm_representations = SharedRepository::get_instance()
                    .get_algorithm_representation_repository()
                    .get_by_field("name", settings.db_settings.algorithm_name);
                
                if (algorithm_representations.size() == 0) {
                    throw std::runtime_error("Algorithm not found by name");
                }
                // std::cout << "Found " << algorithm_representations.size() << " algorithm representations" << std::endl;

                // фильтруем по описанию, размерности и итерациям
                std::vector<AlgorithmRepresentation> filtered_algorithm_representations;
                for (AlgorithmRepresentation algorithm_representation : algorithm_representations) {
                    std::string current_lang = settings.global_settings.language == Language::Type::English ? "en" : "ru";
                    if (!algorithm_representation.algorithm.description[current_lang].empty() &&
                            algorithm_representation.algorithm.description[current_lang] != "-" &&
                            algorithm_representation.algorithm.description[current_lang] != settings.db_settings.algorithm_description) {
                        continue;
                    }
                    if (algorithm_representation.representation.dimensionality != settings.db_settings.dimensionality) {
                        continue;
                    }
                    if (algorithm_representation.representation.iterations != settings.db_settings.iterations) {
                        continue;
                    }
                    filtered_algorithm_representations.push_back(algorithm_representation);
                }
                // std::cout << "Filtered. " << filtered_algorithm_representations.size() << " algorithm representations remains" << std::endl;

                if (filtered_algorithm_representations.size() == 0) {
                    throw std::runtime_error("Algorithm not found");
                }
                auto filtered_algorithm_representation = filtered_algorithm_representations[0];

                shared_memory_file = std::make_unique<SharedMemoryFile>(shared_memory_file_name, filtered_algorithm_representation.representation_strings);
                break;
            }
        }
    }

    static std::vector<std::string> read(std::wstring shared_memory_file_name) {
        if (!shared_memory_file) {
            shared_memory_file = std::make_unique<SharedMemoryFile>(shared_memory_file_name);
        }
        if (shared_memory_file->empty()) {
            throw std::runtime_error("Shared memory file is empty");
        }
        return shared_memory_file->read();
    }

private:
    static inline std::unique_ptr<SharedMemoryFile> shared_memory_file;
};