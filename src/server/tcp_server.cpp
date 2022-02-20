#include "tcp_server.hpp"

#include <string>

namespace modbus {

tcp_server::tcp_server(asio::any_io_executor exec, request_handler_t handler,
                       server_config config)
    : exec_(exec)
    , config_(config)
    , acceptor_(exec)
    , session_manager_(exec, config.logging_handler)
    , request_handler_(handler)
    , on_log_(config.logging_handler)
    , stop_(false) {}

/// Start the server and start processing request.
awaitable<void> tcp_server::start() {
    // bind to address
    tcp::resolver resolver(exec_);
    auto result = co_await resolver.async_resolve(
        config_.endpoint, std::to_string(config_.port), use_awaitable);
    tcp::endpoint endpoint = *result.begin();
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();
    on_log_(log_level::info,
            fmt::format("listening on {}:{}", endpoint.address().to_string(),
                        endpoint.port()));

    while (!stop_) {

        auto [err, socket] =
            co_await acceptor_.async_accept(as_tuple(use_awaitable));
        if (err) {
            on_log_(log_level::error,
                    fmt::format("error listening on interface {0}:{1} {2}",
                                config_.endpoint, config_.port, err.message()));
            break;
        }
        on_log_(log_level::info,
                fmt::format("connection from {0}",
                            socket.remote_endpoint().address().to_string()));
        auto session = std::make_shared<tcp_session>(
            std::move(socket), session_manager_, request_handler_,
            config_.logging_handler);
        co_spawn(exec_, session_manager_.start(session), detached);
    }
}

/// Stop the server and cancel all requests.
void tcp_server::stop() {
    stop_ = true;
    acceptor_.close();
    session_manager_.stop_all();
}

} // namespace modbus