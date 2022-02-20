#include "messages/read_holding_registers_response.hpp"

namespace modbus {

read_holding_registers_response::read_holding_registers_response(
    uint8_t unit_id, std::vector<uint16_t> values) {
    this->unit_id = unit_id;

    auto it = values.begin();
    // Get number of bytes remaining
    int rvElements = values.size() * sizeof(uint16_t);
    this->values.reserve(rvElements);

    for (int i = 0; i < rvElements; i += 2) {
        this->values.emplace_back((uint8_t)(*it >> 8));
        this->values.emplace_back((uint8_t)(*it & 0x00FF));
        it++;
    }
}

read_holding_registers_response::read_holding_registers_response(
    uint8_t unit_id, buffer_t values) {
    this->unit_id = unit_id;
    this->values = values;
}

read_holding_registers_response::read_holding_registers_response(
    const_buffer_iterator it) {
    unit_id = *it++;
    it++; // skip function code

    // Get number of bytes remaining
    uint8_t numBytes = *it++;
    values.resize(numBytes);

    for (int i = 0; i < numBytes; i++) {
        values[i] = *it++;
    }
}

buffer_iterator
read_holding_registers_response::serialize(buffer_iterator it) const {
    *it++ = unit_id;
    *it++ = (uint8_t)this->function_code();

    // serialize the data
    *it++ = (uint8_t)(values.size()); // byte count
    for (auto byte : values) {
        *it++ = byte;
    }

    return it;
}

bool operator==(const read_holding_registers_response& lhs,
                const read_holding_registers_response& rhs) {
    return (lhs.unit_id == rhs.unit_id && lhs.values == rhs.values);
}

bool operator!=(const read_holding_registers_response& lhs,
                const read_holding_registers_response& rhs) {
    return !(lhs == rhs);
}

} // namespace modbus
