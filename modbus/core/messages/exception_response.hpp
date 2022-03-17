#pragma once

#include <cstdint>
#include <vector>

#include "modbus/core/types.hpp"

namespace modbus {

/**
 * @brief The response is an exception code as defined by the MODBUS standard.
 */
struct exception_response {
    uint8_t unit_id;
    function_code_t func_code;
    exception_code_t exception_code;

    exception_response() = default;

    exception_response(uint8_t unit_id, function_code_t function_code,
                       exception_code_t exception_code);

    exception_response(const_buffer_iterator it);

    constexpr function_code_t function_code() { return func_code; }

    static constexpr message_type type() { return message_type::response; }

    size_t size() {
        return sizeof(unit_id) + sizeof(uint8_t) + // Function Code
               sizeof(exception_code_t);
    }

    buffer_iterator serialize(buffer_iterator it) const;
};

bool operator==(const exception_response& lhs, const exception_response& rhs);

bool operator!=(const exception_response& lhs, const exception_response& rhs);

} // namespace modbus
