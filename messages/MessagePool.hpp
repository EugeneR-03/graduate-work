#pragma once

#include <tsl/hopscotch_map.h>
#include <boost/nowide/convert.hpp>

using MessageRecord = tsl::hopscotch_map<std::string, std::string>;
using MessageWRecord = tsl::hopscotch_map<std::string, std::wstring>;

template <typename T>
class MessagePool {
public:
    T& operator[](const std::string& key)
    {
        return pool[key];
    }

    const T& operator[](const std::string& key) const
    {
        return pool.at(key);
    }

    T& at(const std::string& key)
    {
        return pool[key];
    }

    const T& at(const std::string& key) const
    {
        return pool.at(key);
    }

    void insert(const std::string& key, const T& value)
    {
        pool[key] = value;
    }

    void erase(const std::string& key)
    {
        pool.erase(key);
    }

    void clear()
    {
        pool.clear();
    }

    bool contains(const std::string& key) const
    {
        return pool.find(key) != pool.end();
    }

    size_t size() const
    {
        return pool.size();
    }

    bool empty() const
    {
        return pool.empty();
    }

    std::vector<std::string> keys() const
    {
        std::vector<std::string> result(pool.size());
        result.reserve(pool.size());
        for (auto it = pool.begin(); it != pool.end(); ++it) result.push_back(it->first);
        return result;
    }

    std::vector<T> values() const
    {
        std::vector<T> result(pool.size());
        result.reserve(pool.size());
        for (auto it = pool.begin(); it != pool.end(); ++it) result.push_back(it->second);
        return result;
    }

    MessagePool() {}

private:
    tsl::hopscotch_map<std::string, T> pool;
};

namespace nlohmann {
    template <>
    struct adl_serializer<MessagePool<MessageRecord>> {
        static void from_json(const json& j, MessagePool<MessageRecord>& pool) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string theme = it.key();
                json value = it.value();
                MessageRecord record = value.template get<MessageRecord>();
                pool[theme] = record;
            }
        }
    };

    template <>
    struct adl_serializer<MessagePool<MessageWRecord>> {
        static void from_json(const json& j, MessagePool<MessageWRecord>& pool) {
            for (auto& [theme, sub] : j.items()) {
                MessageWRecord rec;
                for (auto& [key, val] : sub.items()) {
                    const std::string utf8_str = val.get<std::string>();
                    const std::wstring ws = boost::nowide::widen(utf8_str);
                    rec.emplace(key, std::move(ws));
                }
                pool.insert(theme, std::move(rec));
            }
        }
    };
}