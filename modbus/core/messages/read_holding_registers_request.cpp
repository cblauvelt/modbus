#include "modbus/core/messages/read_holding_registers_request.hpp"

namespace modbus {

read_holding_registers_request::read_holding_registers_request(
    uint8_t unit_id, uint16_t start_address, uint16_t length) {
    this->unit_id = unit_id;
    this->start_address = start_address;
    this->length = length;
}

read_holding_registers_request::read_holding_registers_request(
    const_buffer_iterator it) {
    unit_id = *it++;
    it++; // skip function code
    start_address = *it++ << 8;
    start_address += *it++;
    length = *it++ << 8;
    length += *it;
}

buffer_iterator
read_holding_registers_request::serialize(buffer_iterator it) const {
    *it++ = unit_id;
    *it++ = (uint8_t)this->function_code();
    *it++ = start_address >> 8;
    *it++ = start_address & 0x00FF;
    *it++ = length >> 8;
    *it++ = length & 0x00FF;

    return it;
}

} // namespace modbus
