#pragma once

#include <functional>
#include <tsl/hopscotch_map.h>
#include <boost/signals2.hpp>

template <typename Signature>
class Event;

template <typename F, typename ... Args>
class Event<F(Args...)> {
public:
    using Slot = std::function<F(Args...)>;
    using Token = std::size_t;
    using Signal = boost::signals2::signal<F(Args...)>;
    using Connection = boost::signals2::connection;

    ~Event() noexcept
    {
        clear();
    }

    void clear()
    {
        signal.disconnect_all_slots();
        connections.clear();
        next_id = 0;
    }

    Token operator+=(Slot slot)
    {
        Connection connection = signal.connect(std::move(slot));
        Token id = next_id++;
        connections.emplace(id, std::move(connection));
        return id;
    }

    void operator-=(Token id)
    {
        auto it = connections.find(id);
        if (it != connections.end()) {
            it->second.disconnect();
            connections.erase(it);
        }
    }

    void operator()(Args... args) const
    {
        signal(std::forward<Args>(args)...);
    }

private:
    Signal signal;
    tsl::hopscotch_map<Token, Connection> connections;
    Token next_id = 0;
};