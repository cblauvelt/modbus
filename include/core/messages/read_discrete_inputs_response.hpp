#pragma once

#include <cstdint>
#include <vector>

#include "types.hpp"

namespace modbus {

/**
 * @brief The response is a Read Discrete Inputs response as defined by the
 * MODBUS standard.
 */
struct read_discrete_inputs_response {
    uint8_t unit_id;
    buffer_t inputs;

    read_discrete_inputs_response() = default;

    read_discrete_inputs_response(uint8_t unit_id, buffer_t inputs);

    read_discrete_inputs_response(const_buffer_iterator it);

    static constexpr function_code_t function_code() {
        return function_code_t::read_discrete_inputs;
    }

    static constexpr message_type type() { return message_type::response; }

    size_t size() {
        return sizeof(unit_id) + sizeof(uint8_t) + // Function Code
               sizeof(uint8_t) +                   // Byte Count
               sizeof(uint8_t) * inputs.size();
    }

    buffer_iterator serialize(buffer_iterator it) const;
};

bool operator==(const read_discrete_inputs_response& lhs,
                const read_discrete_inputs_response& rhs);

bool operator!=(const read_discrete_inputs_response& lhs,
                const read_discrete_inputs_response& rhs);

} // namespace modbus
