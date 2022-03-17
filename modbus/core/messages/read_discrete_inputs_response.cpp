#include "modbus/core/messages/read_discrete_inputs_response.hpp"

namespace modbus {

read_discrete_inputs_response::read_discrete_inputs_response(uint8_t unit_id,
                                                             buffer_t inputs) {
    this->unit_id = unit_id;
    this->inputs = inputs;
}

read_discrete_inputs_response::read_discrete_inputs_response(
    const_buffer_iterator it) {
    unit_id = *it++;
    it++; // skip function code

    uint8_t byteCount = *it++;
    inputs.resize(byteCount);
    for (int i = 0; i < byteCount; i++) {
        inputs[i] = *it++;
    }
}

buffer_iterator
read_discrete_inputs_response::serialize(buffer_iterator it) const {
    *it++ = unit_id;
    *it++ = (uint8_t)this->function_code();
    *it++ = (uint8_t)inputs.size();

    for (auto byte : inputs) {
        *it++ = byte;
    }

    return it;
}

bool operator==(const read_discrete_inputs_response& lhs,
                const read_discrete_inputs_response& rhs) {
    return (lhs.unit_id == rhs.unit_id && lhs.inputs == rhs.inputs);
}

bool operator!=(const read_discrete_inputs_response& lhs,
                const read_discrete_inputs_response& rhs) {
    return !(lhs == rhs);
}

} // namespace modbus
