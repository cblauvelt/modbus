#pragma once

#include <cstdint>
#include <string>

#include "server_types.hpp"
#include "types.hpp"

namespace modbus {

struct server_config {
    std::string endpoint;
    uint16_t port;
    uint16_t max_connections;
    logging_handler_t logging_handler;

    server_config()
        : endpoint("0.0.0.0")
        , port(502)
        , max_connections(DEFAULT_SERVER_MAX_CONNECTIONS)
        , logging_handler(null_logging_handler) {}

    server_config(std::string endpoint, uint16_t port)
        : endpoint(endpoint)
        , port(port)
        , max_connections(DEFAULT_SERVER_MAX_CONNECTIONS)
        , logging_handler(null_logging_handler) {}

    server_config set_endpoint(std::string endpoint) {
        this->endpoint = endpoint;
        return *this;
    }

    server_config set_port(uint16_t port) {
        this->port = port;
        return *this;
    }

    server_config set_max_connections(uint16_t max_connections) {
        this->max_connections = max_connections;
        return *this;
    }

    server_config set_logging_handler(logging_handler_t handler) {
        this->logging_handler = handler;
        return *this;
    }
};

} // namespace modbus
