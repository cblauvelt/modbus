#include "modbus/client/tcp_client.hpp"

namespace modbus {

tcp_client::tcp_client(cpool::net::any_io_executor exec, client_config config)
    : exec_(exec)
    , config_(config)
    , con_pool_(nullptr)
    , transaction_id_(1)
    , on_log_(config_.logging_handler) {

    con_pool_ = std::make_unique<cpool::connection_pool<cpool::tcp_connection>>(
        exec_, std::bind(&tcp_client::connection_ctor, this),
        config_.max_connections);
}

void tcp_client::set_config(client_config config) {
    config_ = config;
    on_log_ = config.logging_handler;

    con_pool_ = std::make_unique<cpool::connection_pool<cpool::tcp_connection>>(
        exec_, std::bind(&tcp_client::connection_ctor, this),
        config_.max_connections);
}

client_config tcp_client::config() const { return config_; }

uint16_t tcp_client::reserve_transaction_id() { return transaction_id_++; }

awaitable<read_response_t>
tcp_client::send_request(const tcp_data_unit& request,
                         std::chrono::milliseconds timeout) {
    on_log_(modbus::log_level::trace,
            fmt::format("getting connection - connections {} - idle {}",
                        con_pool_->size(), con_pool_->size_idle()));
    auto connection = co_await con_pool_->get_connection();
    if (connection == nullptr) {
        co_return read_response_t(
            tcp_data_unit(), cpool::error(modbus_client_error_code::stopped));
    }

    // clear buffer to remove responses that may have appeared after a timeout
    auto error = co_await clear_buffer(connection);
    if (error) {
        co_return read_response_t(tcp_data_unit(), error);
    }

    // setup timeout
    on_log_(log_level::trace,
            fmt::format("setting read timeout of {}ms", timeout.count()));

    // set timeout for response
    connection->expires_after(timeout);

    auto buf = request.buffer();

    on_log_(log_level::debug, fmt::format("sending request with ID {}",
                                          request.transaction_id()));
    error = co_await send_request(connection, *buf);
    if (error) {
        if (error == boost::system::error_code(cpool::net::error::timed_out)) {
            co_return read_response_t(tcp_data_unit(),
                                      modbus_client_error_code::write_timeout);
        }
        co_return read_response_t(tcp_data_unit(), error);
    }

    // we've cleared the buffer but a response could have come in between
    // clearing the buffer and reading the response from our request iterate
    // until we've received our response or timedout
    tcp_data_unit response;
    do {
        std::tie(response, error) = co_await read_response(connection);
        if (error) {
            break;
        }
        on_log_(log_level::debug, fmt::format("received response with ID {}",
                                              response.transaction_id()));
    } while (!error && response.transaction_id() < request.transaction_id());

    connection->expires_never();

    con_pool_->release_connection(connection);

    co_return read_response_t(response, error);
}

awaitable<cpool::error>
tcp_client::send_request(cpool::tcp_connection* connection,
                         const buffer_t& buf) {

    // write request
    auto [write_error, bytes_written] =
        co_await connection->async_write(asio::buffer(buf));
    if (write_error) {
        on_log_(modbus::log_level::debug,
                fmt::format("write_error: {}", write_error.message()));
        co_return write_error;
    }
    if (bytes_written != buf.size()) {
        co_return cpool::error(fmt::format(
            "tried to write {0} bytes, wrote {1}", buf.size(), bytes_written));
    }

    co_return cpool::error();
}

awaitable<read_response_t>
tcp_client::read_response(cpool::tcp_connection* connection) {
    buffer_t readBuffer(MAX_APU_SIZE);
    // read header of response
    auto [read_error, bytes_read] = co_await connection->async_read(
        asio::buffer(readBuffer, TCP_HEADER_SIZE));
    if (read_error) {
        co_return read_response_t(tcp_data_unit(), read_error);
    }
    if (bytes_read != TCP_HEADER_SIZE) {
        co_return read_response_t(
            tcp_data_unit(),
            cpool::error(modbus_error_code::malformed_message));
    }

    // read the response
    int message_length = readBuffer[4] << 8;
    message_length += readBuffer[5];
    if (message_length + TCP_HEADER_SIZE > MAX_APU_SIZE) {
        co_return read_response_t(
            tcp_data_unit(),
            cpool::error(modbus_client_error_code::invalid_response));
    }

    std::tie(read_error, bytes_read) =
        co_await connection->async_read(asio::buffer(
            asio::buffer(readBuffer) + TCP_HEADER_SIZE, message_length));
    if (read_error) {
        co_return read_response_t(tcp_data_unit(), read_error);
    }
    if (bytes_read != message_length) {
        co_return read_response_t(
            tcp_data_unit(),
            cpool::error(modbus_client_error_code::disconnected,
                         "failed to read the response"));
    }

    auto response = tcp_data_unit(readBuffer, bytes_read + TCP_HEADER_SIZE,
                                  message_type::response);

    co_return read_response_t(response, cpool::error());
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

    return modbus_client_error_code::success;
}

awaitable<cpool::error>
tcp_client::clear_buffer(cpool::tcp_connection* connection) {
    // clear out buffer
    // sometimes the client times out but the server eventually responds
    // this will have an old transaction id and response type
    auto [bytes_available, err] = connection->bytes_available();
    if (err) {
        co_return modbus_client_error_code::disconnected;
    }

    if (bytes_available > 0) {
        buffer_t ignore_buf(bytes_available);
        auto [error, bytes_read] =
            co_await connection->async_read(asio::buffer(ignore_buf));
        if (error) {
            co_return error;
        }
        if (bytes_read != bytes_available) {
            co_return modbus_client_error_code::disconnected;
        }
    }

    co_return modbus_client_error_code::success;
}

std::unique_ptr<cpool::tcp_connection> tcp_client::connection_ctor() {
    auto conn = std::make_unique<cpool::tcp_connection>(exec_, config_.host,
                                                        config_.port);
    conn->set_state_change_handler(
        std::bind(&tcp_client::on_connection_state_change, this,
                  std::placeholders::_1, std::placeholders::_2));
    return conn;
}

[[nodiscard]] awaitable<batteries::errors::error>
tcp_client::on_connection_state_change(
    cpool::tcp_connection* conn, const cpool::client_connection_state state) {
    switch (state) {
    case cpool::client_connection_state::disconnected:
        on_log_(log_level::info, fmt::format("disconnected from {0}:{1}",
                                             conn->host(), conn->port()));
        break;

    case cpool::client_connection_state::resolving:
        on_log_(log_level::info, fmt::format("resolving {0}", conn->host()));
        break;

    case cpool::client_connection_state::connecting:
        on_log_(log_level::info, fmt::format("connecting to {0}:{1}",
                                             conn->host(), conn->port()));
        break;

    case cpool::client_connection_state::connected:
        on_log_(log_level::info, fmt::format("connected to {0}:{1}",
                                             conn->host(), conn->port()));
        break;

    case cpool::client_connection_state::disconnecting:
        on_log_(log_level::info, fmt::format("disconnecting from {0}:{1}",
                                             conn->host(), conn->port()));
        break;

    default:
        on_log_(
            log_level::warn,
            fmt::format("unknown client_connection_state: {0}", (int)state));
    }

    co_return batteries::errors::error();
}

} // namespace modbus
