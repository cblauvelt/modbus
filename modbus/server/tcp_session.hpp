#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <vector>

#include "modbus/core/types.hpp"
#include "server_types.hpp"

namespace modbus {

class tcp_session_manager;

class tcp_session : public std::enable_shared_from_this<tcp_session> {

  public:
    explicit tcp_session(tcp::socket socket,
                         tcp_session_manager& session_manager,
                         request_handler_t handler, logging_handler_t on_log);

    tcp_session(const tcp_session&) = delete;
    tcp_session& operator=(const tcp_session&) = delete;

    /// Start the first asynchronous operation for the session.
    awaitable<void> start();

    /// Stop all asynchronous operations associated with the session.
    void stop();

  private:
    /// Socket for the session
    tcp::socket socket_;

    /// The manager for this session
    tcp_session_manager& session_manager_;

    request_handler_t request_handler_;
    logging_handler_t on_log_;

    std::atomic_bool stop_;
};

} // namespace modbus
