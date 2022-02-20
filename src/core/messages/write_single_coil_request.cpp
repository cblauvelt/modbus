#include "messages/write_single_coil_request.hpp"

namespace modbus {

write_single_coil_request::write_single_coil_request(uint8_t unit_id,
                                                     uint16_t start_address,
                                                     coil_status_t value) {
    this->unit_id = unit_id;
    this->start_address = start_address;
    this->value = value;
}

write_single_coil_request::write_single_coil_request(const_buffer_iterator it) {
    unit_id = *it++;
    it++; // skip function code
    start_address = *it++ << 8;
    start_address += *it++;

    uint16_t value;
    value = *it++ << 8;
    value += *it;

    this->value = (coil_status_t)value;
}

buffer_iterator write_single_coil_request::serialize(buffer_iterator it) const {
    *it++ = unit_id;
    *it++ = (uint8_t)this->function_code();
    *it++ = start_address >> 8;
    *it++ = start_address & 0x00FF;

    // Set coil status
    *it++ = (value == coil_status_t::on) ? 0xFF : 0x00;
    *it++ = 0x00;

    return it;
}

bool operator==(const write_single_coil_request& lhs,
                const write_single_coil_request& rhs) {
    return (lhs.unit_id == rhs.unit_id &&
            lhs.start_address == rhs.start_address && lhs.value == rhs.value);
}

bool operator!=(const write_single_coil_request& lhs,
                const write_single_coil_request& rhs) {
    return !(lhs == rhs);
}

} // namespace modbus
