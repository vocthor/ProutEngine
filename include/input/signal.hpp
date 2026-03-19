#pragma once

#include <functional>
#include <utility>
#include <map>

namespace
{
    // -------------------------------------------------------------------------
    // RAII connection handle — disconnects automatically on destruction
    // -------------------------------------------------------------------------
    class Connection
    {
    public:
        Connection() = default;

        explicit Connection(std::function<void()> disconnector)
            : disconnector_(std::move(disconnector)) {}

        ~Connection() { disconnect(); }

        Connection(const Connection &) = delete;
        Connection &operator=(const Connection &) = delete;

        Connection(Connection &&o) noexcept
            : disconnector_(std::move(o.disconnector_))
        {
            o.disconnector_ = nullptr;
        }

        Connection &operator=(Connection &&o) noexcept
        {
            disconnect();
            disconnector_ = std::move(o.disconnector_);
            o.disconnector_ = nullptr;
            return *this;
        }

        void disconnect()
        {
            if (disconnector_)
            {
                disconnector_();
                disconnector_ = nullptr;
            }
        }

    private:
        std::function<void()> disconnector_;
    };

    // -------------------------------------------------------------------------
};

template <typename... Args>
class Signal
{
public:
    using Callback = std::function<void(Args...)>;

    // Non-copyable, non-movable: Connections hold a raw pointer to this Signal
    Signal() = default;
    Signal(const Signal &) = delete;
    Signal &operator=(const Signal &) = delete;
    Signal(Signal &&) = delete;
    Signal &operator=(Signal &&) = delete;

    [[nodiscard]] Connection connect(Callback callback)
    {
        int id = nextId_++;
        callbacks_.emplace(id, std::move(callback));
        return Connection([this, id]
                          { disconnectById(id); });
    }

    void emit(Args... args) const
    {
        for (auto &[id, callback] : callbacks_)
            callback(args...);
    }

private:
    void disconnectById(int id)
    {
        callbacks_.erase(id);
    }

    std::map<int, Callback> callbacks_;
    int nextId_ = 0;
};
