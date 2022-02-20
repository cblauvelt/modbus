
#include <cstdint>
#include <vector>

#include "types.hpp"

namespace modbus {

/**
 * @brief The response is an Mask Write Register Response as defined by the
 * MODBUS standard.
 */
struct mask_write_register_response {
    uint8_t unit_id;
    uint16_t start_address;
    uint16_t and_mask;
    uint16_t or_mask;

    mask_write_register_response() = default;

    mask_write_register_response(uint8_t unit_id, uint16_t start_address,
                                 uint16_t and_mask, uint16_t or_mask);

    mask_write_register_response(const_buffer_iterator it);

    static constexpr function_code_t function_code() {
        return function_code_t::mask_write_register;
    }

    static constexpr message_type type() { return message_type::response; }

    static constexpr size_t size() {
        return sizeof(unit_id) + sizeof(uint8_t) + sizeof(start_address) +
               sizeof(and_mask) + sizeof(or_mask);
    }

    buffer_iterator serialize(buffer_iterator it) const;
};

bool operator==(const mask_write_register_response& lhs,
                const mask_write_register_response& rhs);

bool operator!=(const mask_write_register_response& lhs,
                const mask_write_register_response& rhs);

} // namespace modbus
