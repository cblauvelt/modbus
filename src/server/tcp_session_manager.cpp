#include "server/tcp_session_manager.hpp"

#include <mutex>

namespace modbus {

tcp_session_manager::tcp_session_manager(asio::any_io_executor exec,
                                         logging_handler_t on_log)
    : mtx_()
    , exec_(exec)
    , sessions_()
    , on_log_(on_log)
    , max_sessions_(DEFAULT_SERVER_MAX_CONNECTIONS) {}

awaitable<std::error_code> tcp_session_manager::start(tcp_session_ptr session) {
    std::lock_guard<std::mutex> lock{mtx_};
    if (sessions_.size() > max_sessions_) {

        session->stop();
        co_return modbus_server_error_code::exceeded_max_sessions;
    }

    sessions_.insert(session);
    co_spawn(exec_, session->start(), detached);
    co_return modbus_server_error_code::no_error;
}

void tcp_session_manager::stop(tcp_session_ptr session) {
    std::lock_guard<std::mutex> lock{mtx_};
    sessions_.erase(session);
    session->stop();
}

void tcp_session_manager::stop_all() {
    on_log_(log_level::info, "closing all connections");
    std::lock_guard<std::mutex> lock{mtx_};
    for (auto session : sessions_) {
        session->stop();
    }
    sessions_.clear();
}

void tcp_session_manager::set_max_sessions(int max_sessions) {
    max_sessions_ = max_sessions;
}
int tcp_session_manager::max_sessions() const { return max_sessions_; }
} // namespace modbus
