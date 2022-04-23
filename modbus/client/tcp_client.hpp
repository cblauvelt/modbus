#pragma once

#include <memory>

#include <boost/asio.hpp>
#include <cpool/connection_pool.hpp>
#include <cpool/tcp_connection.hpp>

#include "modbus/client/client_config.hpp"
#include "modbus/core/error.hpp"
#include "modbus/core/tcp_data_unit.hpp"
#include "modbus/core/types.hpp"

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
     * @brief Reserves a transaction ID and creates a tcp_data_unit that can be
     * sent using send_request.
     * @param request An object that meets the requirements of is_message and
     * describes a MODBUS request.
     * @returns tcp_data_unit The MODBUS request as a TCP Data Unit
     *
     */
    template <typename M> tcp_data_unit create_request(const M& request) {
        return tcp_data_unit(reserve_transaction_id(), request);
    }

    /**
     * @brief Reserves a transaction ID for creation of a tcp_data_unit
     * @return uint16_t The transaction ID to use in the request.
     */
    uint16_t reserve_transaction_id();

    /**
     * @brief Instructs the interface to send a request.
     * @param request The request to send to the remote endpoint.
     * @param timeout The time to wait for a response before declaring a request
     * a failure.
     * @return awaitable<read_response_t> An awaitable tuple
     * with the response and an error if any
     */
    [[nodiscard]] awaitable<read_response_t>
    send_request(const tcp_data_unit& request,
                 std::chrono::milliseconds timeout = std::chrono::milliseconds::max);

    /**
     * @brief Sends the request.
     *
     * @param connection The connection to use to make the request.
     * @return awaitable<cpool::error> An awaitable tuple
     * with the response and an error if any
     */
    [[nodiscard]] awaitable<cpool::error>
    send_request(cpool::tcp_connection* connection, const buffer_t& buf);

    /**
     * @brief Reads the response.
     *
     * @param connection The connection to use to make the request.
     * @return awaitable<read_response_t> An awaitable tuple
     * with the response and an error if any
     */
    [[nodiscard]] awaitable<read_response_t>
    read_response(cpool::tcp_connection* connection);

    /**
     * @brief Compares a request to the response and determines if any illegal
     * conditions have occurred.
     */
    static std::error_code
    validate_response(const tcp_data_unit& requestDataUnit,
                      const tcp_data_unit& responseDataUnit);

    [[nodiscard]] awaitable<batteries::errors::error>
    clear_buffer(cpool::tcp_connection* connection);

  private:
    std::unique_ptr<cpool::tcp_connection> connection_ctor();

    [[nodiscard]] awaitable<batteries::errors::error>
    on_connection_state_change(cpool::tcp_connection* conn,
                               const cpool::client_connection_state state);

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
