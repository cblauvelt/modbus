#pragma once

#include "modbus/core/types.hpp"

namespace modbus {

/**
 * @brief The response is a Read Write Multiple Registers request as defined by
 * the MODBUS standard.
 */
struct read_write_registers_request {
    uint8_t unit_id;
    uint16_t read_start_address;
    uint16_t read_num_registers;
    uint16_t write_start_address;
    uint16_t write_num_registers;
    std::vector<uint16_t> values;

    read_write_registers_request() = default;

    read_write_registers_request(uint8_t unit_id, uint16_t read_start_address,
                                 uint16_t read_num_registers,
                                 uint16_t write_start_address,
                                 std::vector<uint16_t> values);

    read_write_registers_request(const_buffer_iterator it);

    static constexpr function_code_t function_code() {
        return function_code_t::read_write_multiple_registers;
    }

    static constexpr message_type type() { return message_type::request; }

    size_t size() {
        return sizeof(unit_id) + sizeof(uint8_t) + // function code
               sizeof(read_start_address) + sizeof(read_num_registers) +
               sizeof(write_start_address) + sizeof(write_num_registers) +
               sizeof(uint8_t) + // byte count of values
               sizeof(uint16_t) * values.size();
    }

    buffer_iterator serialize(buffer_iterator it) const;
};

bool operator==(const read_write_registers_request& lhs,
                const read_write_registers_request& rhs);

bool operator!=(const read_write_registers_request& lhs,
                const read_write_registers_request& rhs);

} // namespace modbus
