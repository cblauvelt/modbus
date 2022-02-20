#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include <cpool/error.hpp>
#include <cpool/types.hpp>

// #include "error.h"

namespace modbus {

// pre-define
class tcp_data_unit;

/// log_level Defines the various log levels.
enum class log_level : uint8_t {
    trace = 0,
    debug = 1,
    info = 2,
    warn = 3,
    error = 4,
    critical = 5
};

/// Defines what data structure to use to read and write bytes to the socket
using buffer_t = std::vector<uint8_t>;
/// The iterator to use on the data
using buffer_iterator = buffer_t::iterator;
/// The const iterator on the data
using const_buffer_iterator = buffer_t::const_iterator;

// Function handlers
/// The function object to handle an error message.
using logging_handler_t =
    std::function<void(log_level level, std::string_view message)>;

/// Defines the response type from send
using send_response_t =
    std::tuple<std::shared_ptr<tcp_data_unit>, cpool::error>;

/// Defines the maximum size of an Application Data Unit (APU)
constexpr int MAX_APU_SIZE = 256;
/// Defines the minimum Pdu size including the unit id
constexpr int MIN_PDU_SIZE = 4;
/// Defines the maximum size of a pdu
constexpr int max_pdu_size = 253;
/// Defines the minimum Pdu size for an exception response including the unit id
constexpr int EXCEPTION_PDU_SIZE = 3;
/// Defines the protocol id. This is always 0x0000
constexpr uint16_t PROTOCOL_ID = 0x0000;
// According to the standard it's 7 but we use the number 6 because we
// exclude the unit_id
constexpr int TCP_HEADER_SIZE = 6;

/**
 * This enum contains all supported MODBUS function codes.
 */
enum class function_code_t : uint8_t {
    /// invalid_function_code An invalid code.
    invalid_function_code = 0,

    // Bit access
    /// read_discrete_inputs Reads a discrete input register.
    read_discrete_inputs = 2,
    /// read_coils Reads a control coil
    read_coils = 1,
    /// write_single_coil Forces a single coil on or off
    write_single_coil = 5,
    /// write_multiple_coils Forces multiple coils on or off if supported
    write_multiple_coils = 15,

    // 16-bit register access
    /// read_input_registers Read a 16-bit input register
    read_input_registers = 4,
    /// read_holding_registers Read a 16-bit holding register
    read_holding_registers = 3,
    /// write_single_register Write a 16-bit holding register
    write_single_register = 6,
    /// write_multiple_registers Write multiple 16-bit holding registers if
    /// supported
    write_multiple_registers = 16,
    /// read_write_multiple_registers Write multiple holding registers and then
    /// read back their contents
    read_write_multiple_registers = 23,
    /// mask_write_register Perform a bit-wise AND operation on the register
    mask_write_register = 22,
    /// read_fifo_queue Read a FIFO Queue
    read_fifo_queue = 24
};

enum class exception_code_t : uint8_t {
    /// no_exception no_exception was returned
    no_exception = 0,
    /// illegal_function The function code is not an allowable action for the
    /// server
    illegal_function = 0x01,
    /// illegal_data_address The data address is not an allowable address for
    /// the server
    illegal_data_address = 0x02,
    /// illegal_data_value A value contained in the query is not an allowable
    /// value
    illegal_data_value = 0x03,
    /// server_device_failure An unrecoverable error occurred while the server
    /// was attempting to perform the requested action
    server_device_failure = 0x04,
    /// acknowledge The server has accepted the request and is processing it
    acknowledge = 0x05,
    /// server_device_busy The server is busy and cannot process the requested
    /// command. The client should retransmit at a later time.
    server_device_busy = 0x06,
    /// negative_acknowledge The server cannot perform the program function.
    negative_acknowledge = 0x07,
    /// memory_parity_error The server detected that the extended file area
    /// failed to pass a parity check.
    memory_parity_error = 0x08,
    /// gateway_path_unavailable The gateway was unable to allocate an internal
    /// communications path.
    gateway_path_unavailable = 0x0A,
    /// gateway_target_device_failed_to_respond The gateway received no response
    /// from the target device.
    gateway_target_device_failed_to_respond = 0x0B
};

enum class coil_status_t : uint16_t {
    // off The output is off
    off = 0x0000,
    // on The output is on
    on = 0xFF00
};

enum class data_model_t : uint8_t {
    /// invalid_data_type An invalid data_model set by the default constructor.
    /// Do not
    /// use.
    invalid_data_type = 0,
    /// input_status This data_model of data can be provided by an I/O system.
    input_status = 1,
    /// coil This data_model of data can be alterable by an application program
    coil = 2,
    /// input_register This data_model of data can be provided by an I/O system
    input_register = 3,
    /// holding_register This data_model of data can be alterable by an
    /// application
    /// program.
    holding_register = 4,
    /// This contains a confirmation that the associated coils/registers were
    /// written
    write_confirmation = 5,
};

enum class message_type : uint8_t {
    /// An invalid PDU data_model. Do not use.
    invalid_pdu_type = 0,
    /// The PDU is a request
    request = 1,
    /// The PDU is a response to a request
    response = 2,
};

enum class byte_order : uint8_t {
    /// normal BigEndian byte order
    normal = 1,
    /// byte_swapped Least significant byte first
    byte_swapped = 2
};

inline std::string to_string(const log_level level) {
    switch (level) {
    case log_level::trace:
        return "Trace";

    case log_level::debug:
        return "Debug";

    case log_level::info:
        return "Info";

    case log_level::warn:
        return "Warn";

    case log_level::error:
        return "Error";

    case log_level::critical:
        return "Critical";

    default:
        return "uknown state";
    }
}

inline std::string to_string(const exception_code_t& code) {
    switch (code) {
    case exception_code_t::illegal_function:
        return "illegal function";
    case exception_code_t::illegal_data_address:
        return "illegal data address";
    case exception_code_t::illegal_data_value:
        return "illegal data value";
    case exception_code_t::server_device_failure:
        return "server device failure";
    case exception_code_t::acknowledge:
        return "acknowledge";
    case exception_code_t::server_device_busy:
        return "server device busy";
    case exception_code_t::negative_acknowledge:
        return "negative acknowledge";
    case exception_code_t::memory_parity_error:
        return "memory parity error";
    case exception_code_t::gateway_path_unavailable:
        return "gateway path unavailable";
    case exception_code_t::gateway_target_device_failed_to_respond:
        return "gateway target device failed to respond";
    default:
        return "unknown";
    }
}

/// A logging handler that does nothing
constexpr void null_logging_handler(log_level level,
                                    std::string_view message){};

inline void print_logging_handler(log_level level, std::string_view message) {
    std::cout << "[" << to_string(level) << "] " << message << std::endl;
}

} // namespace modbus
