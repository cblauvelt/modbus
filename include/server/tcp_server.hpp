#pragma once

#include <atomic>
#include <boost/asio.hpp>
#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

// Server includes
#include "error.h"
#include "server_config.hpp"
#include "tcp_session.hpp"
#include "tcp_session_manager.hpp"
#include "types.hpp"

namespace modbus {

class tcp_server {

  public:
    tcp_server(asio::any_io_executor exec, request_handler_t handler,
               server_config config);

    tcp_server(const tcp_server&) = delete;

    /// Start the server and start processing request.
    awaitable<void> start();

    /// Stop the server and cancel all requests.
    void stop();

  private:
    asio::any_io_executor exec_;
    server_config config_;
    tcp::acceptor acceptor_;
    tcp_session_manager session_manager_;
    request_handler_t request_handler_;
    logging_handler_t on_log_;
    std::atomic_bool stop_;
};

} // namespace modbus
