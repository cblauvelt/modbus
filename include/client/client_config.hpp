#pragma once

#include <cstdint>
#include <string>

#include "types.hpp"

namespace modbus {

constexpr uint16_t DEFAULT_CLIENT_MAX_CONNECTIONS = 1;

struct client_config {
    std::string host;
    uint16_t port;
    uint16_t max_connections;
    logging_handler_t logging_handler;

    client_config()
        : host("127.0.0.1")
        , port(502)
        , max_connections(DEFAULT_CLIENT_MAX_CONNECTIONS)
        , logging_handler(null_logging_handler) {}

    client_config(std::string host, uint16_t port)
        : host(host)
        , port(port)
        , max_connections(DEFAULT_CLIENT_MAX_CONNECTIONS) {}

    client_config set_host(std::string host) {
        this->host = host;
        return *this;
    }

    client_config set_port(uint16_t port) {
        this->port = port;
        return *this;
    }

    client_config set_max_connections(uint16_t max_connections) {
        this->max_connections = max_connections;
        return *this;
    }

    client_config set_logging_handler(logging_handler_t handler) {
        this->logging_handler = handler;
        return *this;
    }
};

} // namespace modbus
