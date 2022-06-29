#pragma once

#include "modbus/core/types.hpp"

namespace modbus {

/**
 * @brief The response is a Write Multiple Registers response as defined by the
 * MODBUS standard.
 */
/**
 * @brief The response is a Write Single Coils response as defined by the MODBUS
 * standard.
 */
struct write_single_coil_response {
    uint8_t unit_id;
    uint16_t start_address;
    bool value;

    write_single_coil_response() = default;

    write_single_coil_response(uint8_t unit_id, uint16_t start_address,
                               bool value);

    write_single_coil_response(const_buffer_iterator it);

    static constexpr function_code_t function_code() {
        return function_code_t::write_single_coil;
    }

    static constexpr message_type type() { return message_type::response; }

    static constexpr size_t size() {
        return sizeof(unit_id) + sizeof(uint8_t) + sizeof(start_address) +
               sizeof(uint16_t);
    }

    buffer_iterator serialize(buffer_iterator it) const;
};

bool operator==(const write_single_coil_response& lhs,
                const write_single_coil_response& rhs);

bool operator!=(const write_single_coil_response& lhs,
                const write_single_coil_response& rhs);

} // namespace modbus
