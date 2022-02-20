#pragma once

#include <memory>

#include <boost/asio.hpp>
#include <cpool/connection_pool.hpp>
#include <cpool/tcp_connection.hpp>

#include "client_config.hpp"
#include "error.hpp"
#include "tcp_data_unit.hpp"
#include "types.hpp"

namespace modbus {

namespace asio = boost::asio;
using namespace std::chrono_literals;

using error_code = boost::system::error_code;
using boost::asio::awaitable;

class tcp_client {
  public:
    /**
     * @brief Creates a tcp_client using properties from config.
     * @param exec The Asio executor to use for event handling.
     * @param config The configuration object
     */
    tcp_client(cpool::net::any_io_executor exec, client_config config);

    tcp_client(const tcp_client&) = delete;
    tcp_client& operator=(const tcp_client&) = delete;

    /**
     * @brief Sets the configuration object if the client is not running.
     * @param config The configuration object that holds the configuration
     * details.
     */
    void set_config(client_config config);

    /**
     * @brief The current configuration of the client.
     */
    client_config config() const;

    /**
     * @brief Instructs the interface to send a request.
     * @param request The request to send to the remote endpoint.
     * @param timeout The time to wait for a response before declaring a request
     * a failure.
     * @return awaitable<send_response_t> An awaitable tuple
     * with the response and an error if any
     */
    template <typename M>
    [[nodiscard]] awaitable<send_response_t>
    send_request(const M& request, std::chrono::milliseconds timeout = 5s) {
        on_log_(log_level::debug,
                fmt::format("connecting to {}:{}", config_.host, config_.port));
        auto connection = co_await con_pool_->get_connection();

        // setup timeout
        on_log_(log_level::debug,
                fmt::format("setting timeout of {}ms", timeout.count()));
        connection->expires_after(timeout);

        int transaction_id = transaction_id_++;
        tcp_data_unit request_data_unit(transaction_id, request);
        auto buf = request_data_unit.buffer();

        on_log_(log_level::debug,
                fmt::format("sending request with ID {}", transaction_id));
        auto reply = co_await send_request(connection, *buf);

        // reset timeout
        connection->expires_never();

        con_pool_->release_connection(connection);

        auto response_data_unit = std::get<0>(reply);
        auto error = std::get<1>(reply);
        // return if error before validation
        if (error) {
            co_return reply;
        }

        // validate response
        if (request_data_unit.transaction_id() !=
            response_data_unit->transaction_id()) {
            co_return std::make_tuple(
                response_data_unit, modbus_client_error_code::invalid_response);
        }

        if (request_data_unit.function_code() !=
            response_data_unit->function_code()) {
            co_return std::make_tuple(
                response_data_unit, modbus_client_error_code::invalid_response);
        }

        co_return reply;
    }

    /**
     * @brief Sends a request.
     *
     * @param connection The connection to use to make the request.
     * @param buf The buffer that contains the request.
     * @param timeout The time after which the request is considered to expire.
     * Note that this does not expire while waiting for a connection.
     * @return awaitable<send_response_t> An awaitable tuple
     * with the response and an error if any
     */
    [[nodiscard]] awaitable<send_response_t>
    send_request(cpool::tcp_connection* connection, const buffer_t& buf);

    /**
     * @brief Compares a request to the response and determines if any illegal
     * conditions have occurred.
     */
    static std::error_code
    validate_response(const tcp_data_unit& requestDataUnit,
                      const tcp_data_unit& responseDataUnit);

  private:
    /// The io_service that is used to schedule asynchronous events.
    cpool::net::any_io_executor exec_;

    /// The configuration options of the client.
    client_config config_;

    /// The connection to the server. @see cpool::tcp_connection.
    std::unique_ptr<cpool::connection_pool<cpool::tcp_connection>> con_pool_;

    // The transaction id for the request
    std::atomic<uint16_t> transaction_id_;

    // event handlers
    /// Called when there is a call to log_message. Does nothing if set to
    /// nullptr.
    logging_handler_t on_log_;
};

} // namespace modbus
