#pragma once

#include <functional>

#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/as_tuple.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>

#include "modbus/core/tcp_data_unit.hpp"

namespace modbus {

class tcp_session;

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::experimental::as_tuple;

constexpr uint16_t DEFAULT_SERVER_MAX_CONNECTIONS = 4;

using request_handler_t =
    std::function<awaitable<tcp_data_unit>(const tcp_data_unit&)>;

} // namespace modbus
