#include "modbus/core/messages/write_multiple_coils_request.hpp"

namespace modbus {

write_multiple_coils_request::write_multiple_coils_request(
    uint8_t unit_id, uint16_t start_address, uint16_t length,
    std::vector<uint8_t> values) {
    this->unit_id = unit_id;
    this->start_address = start_address;
    this->length = length;
    this->values = values;
}

write_multiple_coils_request::write_multiple_coils_request(
    uint8_t unit_id, uint16_t start_address, std::vector<bool> values) {
    this->unit_id = unit_id;
    this->start_address = start_address;
    this->length = (uint16_t)values.size();

    uint16_t numBytes = (uint16_t)length / 8;

    // if there's overflow, add one byte
    if (length % 8) {
        numBytes++;
    }

    this->values.resize(numBytes);
    int boolNumber = 0;

    for (auto value : values) {
        if (value) {
            this->values[boolNumber / 8] |= 1 << (boolNumber % 8);
        }
        boolNumber++;
    }
}

write_multiple_coils_request::write_multiple_coils_request(
    const_buffer_iterator it) {
    unit_id = *it++;
    it++; // skip function code
    start_address = *it++ << 8;
    start_address += *it++;
    length = *it++ << 8;
    length += *it++;

    uint8_t numBytes = *it++;
    values.resize(numBytes);
    for (int i = 0; i < numBytes; i++) {
        values[i] = *it++;
    }
}

buffer_iterator
write_multiple_coils_request::serialize(buffer_iterator it) const {
    *it++ = unit_id;
    *it++ = (uint8_t)this->function_code();
    *it++ = start_address >> 8;
    *it++ = start_address & 0x00FF;
    *it++ = length >> 8;
    *it++ = length & 0x00FF;

    // serialize the data
    *it++ = (uint8_t)values.size();
    for (auto byte : values) {
        *it++ = byte;
    }

    return it;
}

bool operator==(const write_multiple_coils_request& lhs,
                const write_multiple_coils_request& rhs) {
    return (lhs.unit_id == rhs.unit_id &&
            lhs.start_address == rhs.start_address &&
            lhs.length == rhs.length && lhs.values == rhs.values);
}

bool operator!=(const write_multiple_coils_request& lhs,
                const write_multiple_coils_request& rhs) {
    return !(lhs == rhs);
}

} // namespace modbus
