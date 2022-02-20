#include "types.hpp"

namespace modbus {

/**
 * @brief The response is a Write Multiple Registers response as defined by the
 * MODBUS standard.
 */
struct write_multiple_registers_response {
    uint8_t unit_id;
    uint16_t start_address;
    uint16_t length;

    write_multiple_registers_response() = default;

    write_multiple_registers_response(uint8_t unit_id, uint16_t start_address,
                                      uint16_t length);

    write_multiple_registers_response(const_buffer_iterator it);

    static constexpr function_code_t function_code() {
        return function_code_t::write_multiple_registers;
    }

    static constexpr message_type type() { return message_type::response; }

    size_t size() {
        return sizeof(unit_id) + sizeof(uint8_t) + // Function code
               sizeof(start_address) + sizeof(length);
    }

    buffer_iterator serialize(buffer_iterator it) const;
};

bool operator==(const write_multiple_registers_response& lhs,
                const write_multiple_registers_response& rhs);

bool operator!=(const write_multiple_registers_response& lhs,
                const write_multiple_registers_response& rhs);

} // namespace modbus
