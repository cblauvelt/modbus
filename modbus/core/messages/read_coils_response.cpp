#include "modbus/core/messages/read_coils_response.hpp"

namespace modbus {

read_coils_response::read_coils_response(uint8_t unit_id,
                                         std::vector<uint8_t> values) {
    this->unit_id = unit_id;
    this->values = values;
}

read_coils_response::read_coils_response(const_buffer_iterator it) {
    unit_id = *it++;
    it++; // skip function code

    uint8_t byteCount = *it++;
    values.resize(byteCount);
    for (int i = 0; i < byteCount; i++) {
        values[i] = *it++;
    }
}

buffer_iterator read_coils_response::serialize(buffer_iterator it) const {
    *it++ = unit_id;
    *it++ = (uint8_t)this->function_code();
    *it++ = (uint8_t)values.size();

    for (auto byte : values) {
        *it++ = byte;
    }

    return it;
}

bool operator==(const read_coils_response& lhs,
                const read_coils_response& rhs) {
    return (lhs.unit_id == rhs.unit_id && lhs.values == rhs.values);
}

bool operator!=(const read_coils_response& lhs,
                const read_coils_response& rhs) {
    return !(lhs == rhs);
}

} // namespace modbus
