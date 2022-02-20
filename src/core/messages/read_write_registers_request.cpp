#include "messages/read_write_registers_request.hpp"

namespace modbus {

read_write_registers_request::read_write_registers_request(
    uint8_t unit_id, uint16_t read_start_address, uint16_t read_num_registers,
    uint16_t write_start_address, std::vector<uint16_t> values) {
    this->unit_id = unit_id;
    this->read_start_address = read_start_address;
    this->read_num_registers = read_num_registers;
    this->write_start_address = write_start_address;
    this->write_num_registers = values.size();
    this->values = values;
}

read_write_registers_request::read_write_registers_request(
    const_buffer_iterator it) {
    unit_id = *it++;
    it++; // skip function code
    read_start_address = *it++ << 8;
    read_start_address += *it++;
    read_num_registers = *it++ << 8;
    read_num_registers += *it++;
    write_start_address = *it++ << 8;
    write_start_address += *it++;
    write_num_registers = *it++ << 8;
    write_num_registers += *it++;

    // Get number of bytes remaining
    uint8_t numBytes = *it++;
    int rvElements = numBytes / 2; // Each value is two bytes
    values.resize(rvElements);

    for (int i = 0; i < rvElements; i++) {
        values[i] = *it++ << 8;
        values[i] += *it++;
    }

    write_num_registers = values.size();
}

buffer_iterator
read_write_registers_request::serialize(buffer_iterator it) const {
    *it++ = unit_id;
    *it++ = (uint8_t)this->function_code();
    *it++ = read_start_address >> 8;
    *it++ = read_start_address & 0x00FF;
    *it++ = read_num_registers >> 8;
    *it++ = read_num_registers & 0x00FF;
    *it++ = write_start_address >> 8;
    *it++ = write_start_address & 0x00FF;
    *it++ = write_num_registers >> 8;
    *it++ = write_num_registers & 0x00FF;

    // serialize the data
    *it++ = (uint8_t)(values.size() * sizeof(uint16_t)); // byte count
    for (auto word : values) {
        *it++ = word >> 8;
        *it++ = word & 0x00FF;
    }

    return it;
}

bool operator==(const read_write_registers_request& lhs,
                const read_write_registers_request& rhs) {
    return (lhs.unit_id == rhs.unit_id &&
            lhs.read_start_address == rhs.read_start_address &&
            lhs.read_num_registers == rhs.read_num_registers &&
            lhs.write_start_address == rhs.write_start_address &&
            lhs.write_num_registers == rhs.write_num_registers &&
            lhs.values == rhs.values);
}

bool operator!=(const read_write_registers_request& lhs,
                const read_write_registers_request& rhs) {
    return !(lhs == rhs);
}

} // namespace modbus
