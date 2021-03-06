#pragma once

#include "modbus/core/types.hpp"

namespace modbus {

/**
 * @brief The response is a Write Single Register request as defined by the
 * MODBUS standard.
 */
struct write_single_register_request {
    uint8_t unit_id;
    uint16_t start_address;
    uint16_t value;

    write_single_register_request() = default;

    write_single_register_request(uint8_t unit_id, uint16_t start_address,
                                  uint16_t value);

    write_single_register_request(const_buffer_iterator it);

    static constexpr function_code_t function_code() {
        return function_code_t::write_single_register;
    }

    static constexpr message_type type() { return message_type::request; }

    static constexpr size_t size() {
        return sizeof(unit_id) + sizeof(uint8_t) + sizeof(start_address) +
               sizeof(value);
    }

    buffer_iterator serialize(buffer_iterator it) const;
};

bool operator==(const write_single_register_request& lhs,
                const write_single_register_request& rhs);

bool operator!=(const write_single_register_request& lhs,
                const write_single_register_request& rhs);

} // namespace modbus
