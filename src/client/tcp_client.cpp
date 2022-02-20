#include "client/tcp_client.hpp"

namespace modbus {

tcp_client::tcp_client(cpool::net::any_io_executor exec, client_config config)
    : exec_(exec)
    , config_(config)
    , con_pool_(nullptr)
    , transaction_id_(1)
    , on_log_(config_.logging_handler) {
    auto conn_creator = [&]() -> std::unique_ptr<cpool::tcp_connection> {
        auto conn = std::make_unique<cpool::tcp_connection>(exec_, config_.host,
                                                            config_.port);
        return conn;
    };

    con_pool_ = std::make_unique<cpool::connection_pool<cpool::tcp_connection>>(
        exec_, conn_creator, config_.max_connections);
}

void tcp_client::set_config(client_config config) {
    config_ = config;
    on_log_ = config.logging_handler;

    auto conn_creator = [&]() -> std::unique_ptr<cpool::tcp_connection> {
        auto conn = std::make_unique<cpool::tcp_connection>(exec_, config_.host,
                                                            config_.port);
        return conn;
    };

    con_pool_ = std::make_unique<cpool::connection_pool<cpool::tcp_connection>>(
        exec_, conn_creator, config_.max_connections);
}

client_config tcp_client::config() const { return config_; }

awaitable<send_response_t>
tcp_client::send_request(cpool::tcp_connection* connection,
                         const buffer_t& buf) {
    auto nullDataUnit = std::make_shared<tcp_data_unit>();
    buffer_t readBuffer(MAX_APU_SIZE);

    // write request
    auto [write_error, bytes_written] =
        co_await connection->async_write(asio::buffer(buf));
    if (write_error) {
        co_return send_response_t(nullDataUnit, write_error);
    }
    if (bytes_written != buf.size()) {
        auto error = cpool::error(fmt::format(
            "tried to write {0} bytes, wrote {1}", buf.size(), bytes_written));
        co_return send_response_t(nullDataUnit, error);
    }

    // read header of response
    auto [read_error, bytes_read] = co_await connection->async_read(
        asio::buffer(readBuffer, TCP_HEADER_SIZE));
    if (read_error) {
        co_return send_response_t(nullDataUnit, read_error);
    }
    if (bytes_read != TCP_HEADER_SIZE) {
        co_return send_response_t(
            nullDataUnit, cpool::error(modbus_error_code::malformed_message));
    }

    // read the response
    int message_length = readBuffer[4] << 8;
    message_length += readBuffer[5];
    if (message_length + TCP_HEADER_SIZE > MAX_APU_SIZE) {
        co_return send_response_t(
            nullDataUnit,
            cpool::error(modbus_client_error_code::invalid_response));
    }

    std::tie(read_error, bytes_read) =
        co_await connection->async_read(asio::buffer(
            asio::buffer(readBuffer) + TCP_HEADER_SIZE, message_length));
    if (read_error) {
        co_return send_response_t(nullDataUnit, read_error);
    }

    auto response = std::make_shared<tcp_data_unit>(
        readBuffer, bytes_read + TCP_HEADER_SIZE, message_type::response);
    co_return send_response_t(response, cpool::error());
}

std::error_code
tcp_client::validate_response(const tcp_data_unit& requestDataUnit,
                              const tcp_data_unit& responseDataUnit) {
    // Do some error checking
    function_code_t functionCode = responseDataUnit.function_code();

    if (requestDataUnit.transaction_id() != responseDataUnit.transaction_id()) {
        return modbus_client_error_code::invalid_response;
    }

    if (requestDataUnit.function_code() != responseDataUnit.function_code()) {
        return modbus_client_error_code::invalid_response;
    }

    if ((functionCode == function_code_t::write_single_coil ||
         functionCode == function_code_t::write_single_register) &&
        !responseDataUnit.is_exception()) {
        // Data should equal the request
        if (*(requestDataUnit.buffer()) != *(responseDataUnit.buffer())) {
            return modbus_client_error_code::invalid_response;
        }
    }

    return modbus_client_error_code::no_error;
}

} // namespace modbus
