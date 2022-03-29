#pragma once

#include <system_error>

namespace modbus {
enum class modbus_error_code : uint8_t {
    /// no_error No error has occurred
    no_error = 0,
    /// not_supported The requested action is not yet supported
    not_supported,
    /// An exception was thrown and caught by the program
    internal_error,
    /// malformed_message The client/server has received a response/request that
    /// does not conform to the standard
    malformed_message,
    /// crc_check_failed The CRC check of the PDU failed
    crc_check_failed,
    /// LrcCheckFailed The LRC check of the PDU failed
    lrc_check_failed
};

enum class modbus_client_error_code : uint8_t {
    /// no_error No error has occurred
    no_error = 0,
    /// timeout_expired The timeout for receiving a response has expired
    timeout_expired,
    /// InvalidResponse A response was received that does not pass verification
    /// checks.
    invalid_response,
    /// Disconnected The client was disconnected. This could be from the server
    /// or as a result of a call to disconnect.
    disconnected,
    /// stopped The client has been stopped. No more requests should be sent to the client.
    stopped
};

enum class modbus_server_error_code : uint8_t {
    /// no_error No error has occurred
    no_error = 0,
    /// ExceededMaxSessions The maximum number of sessions were exceeded.
    exceeded_max_sessions
};

std::error_code make_error_code(modbus_error_code);
std::error_code make_error_code(modbus_client_error_code);
std::error_code make_error_code(modbus_server_error_code);

} // namespace modbus

namespace std {
// Tell the C++ STL metaprogramming that enum modbus_error_code
// is registered with the standard error code system
template <> struct is_error_code_enum<modbus::modbus_error_code> : true_type {};
template <>
struct is_error_code_enum<modbus::modbus_client_error_code> : true_type {};
template <>
struct is_error_code_enum<modbus::modbus_server_error_code> : true_type {};
} // namespace std