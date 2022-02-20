#pragma once

#include <cstdint>
#include <vector>

#include "types.hpp"

namespace modbus {

/**
 * @brief The response is a Read Coils request as defined by the MODBUS
 * standard.
 */
struct read_coils_request {
    uint8_t unit_id;
    uint16_t start_address;
    uint16_t length;

    read_coils_request() = default;

    read_coils_request(uint8_t unit_id, uint16_t start_address,
                       uint16_t length);

    read_coils_request(const_buffer_iterator it);

    static constexpr function_code_t function_code() {
        return function_code_t::read_coils;
    }

    static constexpr modbus::message_type type() { return message_type::request; }

    static constexpr size_t size() {
        return sizeof(unit_id) + sizeof(uint8_t) + sizeof(start_address) +
               sizeof(length);
    }

    buffer_iterator serialize(buffer_iterator it) const;
};

bool operator==(const read_coils_request& lhs, const read_coils_request& rhs);

bool operator!=(const read_coils_request& lhs, const read_coils_request& rhs);

} // namespace modbus
