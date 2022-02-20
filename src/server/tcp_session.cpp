#include "server/tcp_session.hpp"
#include "tcp_session_manager.hpp"

namespace modbus {

tcp_session::tcp_session(tcp::socket socket,
                         tcp_session_manager& session_manager,
                         request_handler_t handler, logging_handler_t on_log)
    : socket_(std::move(socket))
    , session_manager_(session_manager)
    , request_handler_(handler)
    , on_log_(on_log)
    , stop_(false) {}

awaitable<void> tcp_session::start() {
    auto endpoint = socket_.remote_endpoint().address().to_string();
    buffer_t buf(MAX_APU_SIZE);

    while (!stop_) {
        on_log_(log_level::trace, "waiting for request");
        auto [read_err, bytes_read] = co_await socket_.async_read_some(
            asio::buffer(buf), as_tuple(use_awaitable));
        on_log_(log_level::debug, fmt::format("read {} bytes", bytes_read));
        if (read_err && read_err != asio::error::operation_aborted) {
            if (read_err == asio::error::eof) {
                on_log_(log_level::info,
                        fmt::format("client {} disconnected", endpoint));
            } else {
                on_log_(
                    log_level::error,
                    fmt::format("unanticipated error from read {}; client: {}",
                                read_err.message(), endpoint));
            }

            session_manager_.stop(shared_from_this());
            break;
        } else if (read_err == asio::error::operation_aborted) {
            break;
        }

        if (bytes_read < TCP_HEADER_SIZE + MIN_PDU_SIZE) {
            on_log_(log_level::error,
                    fmt::format("malformed message from {}", endpoint));
            continue;
        }

        auto request = tcp_data_unit{buf, bytes_read, message_type::request};
        on_log_(log_level::trace, "processing request");
        auto response = co_await request_handler_(request);
        on_log_(log_level::trace, "created response");

        auto [write_err, bytes_written] = co_await socket_.async_write_some(
            asio::buffer(*(response.buffer())), as_tuple(use_awaitable));

        on_log_(log_level::debug, fmt::format("wrote {} bytes", bytes_written));
        if (write_err && write_err != asio::error::operation_aborted) {
            session_manager_.stop(shared_from_this());
            on_log_(log_level::error,
                    fmt::format("unanticipated write error {}; client: {}",
                                read_err.message(), endpoint));
            break;
        } else if (read_err == asio::error::operation_aborted) {
            break;
        }
    }
}

void tcp_session::stop() {
    stop_ = true;
    boost::system::error_code ignored_err;
    socket_.close(ignored_err);
}

} // namespace modbus
