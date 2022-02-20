#include "types.hpp"

namespace modbus {

/**
 * @brief The response is a Read Holding Registers request as defined by the
 * MODBUS standard.
 */
struct read_holding_registers_request {
    uint8_t unit_id;
    uint16_t start_address;
    uint16_t length;

    read_holding_registers_request() = default;

    read_holding_registers_request(uint8_t unit_id, uint16_t start_address,
                                   uint16_t length);

    read_holding_registers_request(const_buffer_iterator it);

    static constexpr function_code_t function_code() {
        return function_code_t::read_holding_registers;
    }

    static constexpr message_type type() { return message_type::request; }

    static constexpr size_t size() {
        return sizeof(unit_id) + sizeof(uint8_t) + // function code
               sizeof(start_address) + sizeof(length);
    }

    buffer_iterator serialize(buffer_iterator it) const;
};

inline bool operator==(const read_holding_registers_request& lhs,
                       const read_holding_registers_request& rhs) {
    return (lhs.unit_id == rhs.unit_id &&
            lhs.start_address == rhs.start_address && lhs.length == rhs.length);
}

inline bool operator!=(const read_holding_registers_request& lhs,
                       const read_holding_registers_request& rhs) {
    return !(lhs == rhs);
}

} // namespace modbus
