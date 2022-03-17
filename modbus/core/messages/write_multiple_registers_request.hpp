#pragma once

#include "modbus/core/types.hpp"

namespace modbus {

/**
 * @brief The response is a Write Multiple Registers request as defined by the
 * MODBUS standard.
 */
struct write_multiple_registers_request {
    uint8_t unit_id;
    uint16_t start_address;
    uint16_t length;
    std::vector<uint16_t> values;

    write_multiple_registers_request() = default;

    write_multiple_registers_request(uint8_t unit_id, uint16_t start_address,
                                     uint16_t length,
                                     std::vector<uint16_t> values);

    write_multiple_registers_request(const_buffer_iterator it);

    static constexpr function_code_t function_code() {
        return function_code_t::write_multiple_registers;
    }

    static constexpr message_type type() { return message_type::request; }

    size_t size() {
        return sizeof(unit_id) + sizeof(uint8_t) + // Function code
               sizeof(start_address) + sizeof(length) +
               sizeof(uint8_t) + // The number of data bytes in the
                                 // values array
               sizeof(uint16_t) * values.size();
    }

    buffer_iterator serialize(buffer_iterator it) const;
};

bool operator==(const write_multiple_registers_request& lhs,
                const write_multiple_registers_request& rhs);

bool operator!=(const write_multiple_registers_request& lhs,
                const write_multiple_registers_request& rhs);

} // namespace modbus
