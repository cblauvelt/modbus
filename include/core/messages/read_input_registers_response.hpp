#pragma once

#include "types.hpp"

namespace modbus {

/**
 * @brief The response is a Read Input Registers response as defined by the
 * MODBUS standard.
 */
struct read_input_registers_response {
    uint8_t unit_id;
    std::vector<uint8_t> values;

    read_input_registers_response() = default;

    read_input_registers_response(uint8_t unit_id, std::vector<uint8_t> values);

    read_input_registers_response(uint8_t unit_id,
                                  std::vector<uint16_t> values);

    read_input_registers_response(const_buffer_iterator it);

    static constexpr function_code_t function_code() {
        return function_code_t::read_input_registers;
    }

    static constexpr message_type type() { return message_type::response; }

    size_t size() {
        return sizeof(unit_id) + sizeof(uint8_t) + // Function Code
               sizeof(uint8_t) +                   // Byte Count
               values.size();
    }

    buffer_iterator serialize(buffer_iterator it) const;
};

bool operator==(const read_input_registers_response& lhs,
                const read_input_registers_response& rhs);

bool operator!=(const read_input_registers_response& lhs,
                const read_input_registers_response& rhs);

} // namespace modbus
