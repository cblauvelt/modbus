#include "modbus/core/messages/read_write_registers_response.hpp"

namespace modbus {

read_write_registers_response::read_write_registers_response(
    uint8_t unit_id, std::vector<uint16_t> values) {
    this->unit_id = unit_id;
    this->values = values;
}

read_write_registers_response::read_write_registers_response(
    const_buffer_iterator it) {
    unit_id = *it++;
    it++; // skip function code

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
read_write_registers_response::serialize(buffer_iterator it) const {
    *it++ = unit_id;
    *it++ = (uint8_t)this->function_code();

    // serialize the data
    *it++ = (uint8_t)(values.size() * sizeof(uint16_t)); // byte count
    for (auto word : values) {
        *it++ = word >> 8;
        *it++ = word & 0x00FF;
    }

    return it;
}

bool operator==(const read_write_registers_response& lhs,
                const read_write_registers_response& rhs) {
    return (lhs.unit_id == rhs.unit_id && lhs.values == rhs.values);
}

bool operator!=(const read_write_registers_response& lhs,
                const read_write_registers_response& rhs) {
    return !(lhs == rhs);
}

} // namespace modbus
