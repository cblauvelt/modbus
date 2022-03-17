
#include "modbus/core/messages/mask_write_register_request.hpp"

namespace modbus {

mask_write_register_request::mask_write_register_request(uint8_t unitId,
                                                         uint16_t startAddress,
                                                         uint16_t and_mask,
                                                         uint16_t or_mask) {
    this->unit_id = unitId;
    this->start_address = startAddress;
    this->and_mask = and_mask;
    this->or_mask = or_mask;
}

mask_write_register_request::mask_write_register_request(
    const_buffer_iterator it) {
    unit_id = *it++;
    it++; // skip function code
    start_address = *it++ << 8;
    start_address += *it++;
    and_mask = *it++ << 8;
    and_mask += *it++;
    or_mask = *it++ << 8;
    or_mask += *it++;
}

buffer_iterator
mask_write_register_request::serialize(buffer_iterator it) const {
    *it++ = unit_id;
    *it++ = (uint8_t)this->function_code();
    *it++ = start_address >> 8;
    *it++ = start_address & 0x00FF;
    *it++ = and_mask >> 8;
    *it++ = and_mask & 0x00FF;
    *it++ = or_mask >> 8;
    *it++ = or_mask & 0x00FF;

    return it;
}

bool operator==(const mask_write_register_request& lhs,
                const mask_write_register_request& rhs) {
    return (lhs.unit_id == rhs.unit_id &&
            lhs.start_address == rhs.start_address &&
            lhs.and_mask == rhs.and_mask && lhs.or_mask == rhs.or_mask);
}

bool operator!=(const mask_write_register_request& lhs,
                const mask_write_register_request& rhs) {
    return !(lhs == rhs);
}

} // namespace modbus
