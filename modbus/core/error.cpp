#include "modbus/core/error.hpp"

namespace modbus {

namespace detail {

struct modbus_error_code_category : std::error_category {
    const char* name() const noexcept override { return "modbus_error_code"; }

    std::string message(int ev) const override {
        switch (static_cast<modbus_error_code>(ev)) {
        case modbus_error_code::success:
            return "Success";
        case modbus_error_code::not_supported:
            return "The requested action is not yet supported";
        case modbus_error_code::internal_error:
            return "An exception was thrown and caught by the program";
        case modbus_error_code::malformed_message:
            return "The client/server has received a response/request that "
                   "does not conform to the standard";
        case modbus_error_code::crc_check_failed:
            return "The CRC check of the PDU failed";
        case modbus_error_code::lrc_check_failed:
            return "The LRC check of the PDU failed";
        default:
            return "(unrecognized error)";
        }
    }
};

struct modbus_client_error_code_category : std::error_category {
    const char* name() const noexcept override {
        return "ModbusClientErrorCode";
    }

    std::string message(int ev) const override {
        switch (static_cast<modbus_client_error_code>(ev)) {
        case modbus_client_error_code::success:
            return "Success";
        case modbus_client_error_code::write_timeout:
            return "The timeout for sending the request has expired";
        case modbus_client_error_code::read_timeout:
            return "The timeout for receiving a response has expired";
        case modbus_client_error_code::invalid_response:
            return "A response was received that does not pass verification "
                   "checks";
        case modbus_client_error_code::disconnected:
            return "The client was disconnected";
        case modbus_client_error_code::stopped:
            return "The client was stopped";
        default:
            return "(unrecognized error)";
        }
    }
};

struct modbus_server_error_code_category : std::error_category {
    const char* name() const noexcept override {
        return "ModbusServerErrorCode";
    }

    std::string message(int ev) const override {
        switch (static_cast<modbus_server_error_code>(ev)) {
        case modbus_server_error_code::success:
            return "Success";
        case modbus_server_error_code::exceeded_max_sessions:
            return "The maximum number of sessions were exceeded";
        default:
            return "(unrecognized error)";
        }
    }
};

const modbus_error_code_category theModbusErrorCodeCategory{};
const modbus_client_error_code_category theModbusClientErrorCodeCategory{};
const modbus_server_error_code_category theModbusServerErrorCodeCategory{};

} // namespace detail

std::error_code make_error_code(modbus_error_code e) {
    return {static_cast<int>(e), detail::theModbusErrorCodeCategory};
}

std::error_code make_error_code(modbus_client_error_code e) {
    return {static_cast<int>(e), detail::theModbusClientErrorCodeCategory};
}

std::error_code make_error_code(modbus_server_error_code e) {
    return {static_cast<int>(e), detail::theModbusServerErrorCodeCategory};
}

} // namespace modbus
