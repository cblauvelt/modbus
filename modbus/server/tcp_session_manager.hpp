#pragma once

#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

#include <boost/asio.hpp>

#include "modbus/core/error.hpp"
#include "modbus/core/types.hpp"
#include "server_types.hpp"
#include "tcp_session.hpp"

namespace modbus {
class tcp_session_manager {

    using tcp_session_ptr = std::shared_ptr<tcp_session>;

  public:
    tcp_session_manager(asio::any_io_executor exec, logging_handler_t on_log);

    tcp_session_manager(const tcp_session_manager&) = delete;
    tcp_session_manager& operator=(const tcp_session_manager&) = delete;

    /// Add the specified session to the manager and start it.
    awaitable<std::error_code> start(tcp_session_ptr session);

    /// Stop the specified connection
    void stop(tcp_session_ptr session);

    /// Stop all sessions
    void stop_all();

    /// Sets the maximum number of connections
    void set_max_sessions(int max_sessions);

    /// Gets the maximum number of connections
    int max_sessions() const;

  private:
    /// The managed sessions
    std::mutex mtx_;
    asio::any_io_executor exec_;
    std::set<tcp_session_ptr> sessions_;
    logging_handler_t on_log_;
    std::atomic<int> max_sessions_;
};

} // namespace modbus
