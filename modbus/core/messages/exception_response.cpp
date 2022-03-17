#include "modbus/core/messages/exception_response.hpp"

namespace modbus {

exception_response::exception_response(uint8_t unit_id,
                                       function_code_t function_code,
                                       exception_code_t exception_code) {
    this->unit_id = unit_id;
    this->func_code = function_code;
    this->exception_code = exception_code;
}

exception_response::exception_response(const_buffer_iterator it) {
    unit_id = *it++;
    func_code = (function_code_t)(*it++ & 0x7F);
    exception_code = (exception_code_t)*it++;
}

buffer_iterator exception_response::serialize(buffer_iterator it) const {
    *it++ = unit_id;
    *it++ = ((uint8_t)func_code) | 0x80;
    *it++ = (uint8_t)exception_code;

    return it;
}

bool operator==(const exception_response& lhs, const exception_response& rhs) {
    return (lhs.unit_id == rhs.unit_id && lhs.func_code == rhs.func_code &&
            lhs.exception_code == rhs.exception_code);
}

bool operator!=(const exception_response& lhs, const exception_response& rhs) {
    return !(lhs == rhs);
}

} // namespace modbus
