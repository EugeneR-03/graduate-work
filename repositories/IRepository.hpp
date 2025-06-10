#pragma once

#include <optional>
#include <string>
#include <vector>
#include <pqxx/pqxx>

// данный репозиторий является вариантом DAO (Data Access Object)
// (не является паттерном репозитория из DDD (Domain Driven Design))
template <typename T>
struct IRepository {
    virtual ~IRepository() = default;

    virtual void connect(std::string connection) = 0;
    virtual void connect(pqxx::connection* connection) = 0;
    virtual void disconnect() = 0;

    virtual void add(T object) = 0;
    virtual void update(T object) = 0;
    virtual void remove(T object) = 0;

    virtual std::vector<T> get_all() = 0;
    virtual std::optional<T> get_by_id(int id) = 0;
    virtual std::vector<T> get_by_field(const std::string& field, const std::string& value) = 0;
};

inline std::string make_regex_by_words(std::string_view value)
{
    return value
        | std::views::split(' ')
        | std::views::transform([](auto&& word) {
            std::string_view sv(word.begin(), word.end());
            return "(?=.*" + std::string(sv) + ")";
          })
        | std::views::join
        | std::ranges::to<std::string>();
}