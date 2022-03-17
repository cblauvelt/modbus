#include "modbus/core/messages/write_multiple_registers_request.hpp"

namespace modbus {

write_multiple_registers_request::write_multiple_registers_request(
    uint8_t unit_id, uint16_t start_address, uint16_t length,
    std::vector<uint16_t> values) {
    this->unit_id = unit_id;
    this->start_address = start_address;
    this->length = length;
    this->values = values;
}

write_multiple_registers_request::write_multiple_registers_request(
    const_buffer_iterator it) {
    unit_id = *it++;
    it++; // skip function code
    start_address = *it++ << 8;
    start_address += *it++;
    length = *it++ << 8;
    length += *it++;

    // Get number of bytes remaining
    uint8_t numBytes = *it++;
    int rvElements = numBytes / 2; // Each value is two bytes
    values.resize(rvElements);

    for (int i = 0; i < rvElements; i++) {
        values[i] = *it++ << 8;
        values[i] += *it++;
    }
}

buffer_iterator
write_multiple_registers_request::serialize(buffer_iterator it) const {
    *it++ = unit_id;
    *it++ = (uint8_t)this->function_code();
    *it++ = start_address >> 8;
    *it++ = start_address & 0x00FF;
    *it++ = length >> 8;
    *it++ = length & 0x00FF;

    // serialize the data
    *it++ = (uint8_t)(values.size() * sizeof(uint16_t)); // byte count
    for (auto word : values) {
        *it++ = word >> 8;
        *it++ = word & 0x00FF;
    }

    return it;
}

bool operator==(const write_multiple_registers_request& lhs,
                const write_multiple_registers_request& rhs) {
    return (lhs.unit_id == rhs.unit_id &&
            lhs.start_address == rhs.start_address && lhs.values == rhs.values);
}

bool operator!=(const write_multiple_registers_request& lhs,
                const write_multiple_registers_request& rhs) {
    return !(lhs == rhs);
}

} // namespace modbus
