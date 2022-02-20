#include "modbus_response.hpp"

namespace modbus {

modbus_response_t::modbus_response_t()
    : type_(data_model_t::invalid_data_type)
    , data_(nullptr)
    , start_address_(0)
    , num_values_(0) {}

modbus_response_t::modbus_response_t(modbus::data_model_t type,
                                     shared_ptr<buffer_t> data,
                                     uint16_t start_address, uint16_t numValues)
    : type_(type)
    , data_(data)
    , start_address_(start_address)
    , num_values_(numValues) {}

data_model_t modbus_response_t::data_model() const { return type_; }

void modbus_response_t::set_data_model(modbus::data_model_t type) {
    type_ = type;
}

void modbus_response_t::set_data(shared_ptr<buffer_t> data) {
    data_ = std::move(data);
}

uint16_t modbus_response_t::start_address() const { return start_address_; }

shared_ptr<buffer_t> modbus_response_t::data() const { return data_; }

uint16_t modbus_response_t::buffer_length() const {
    if (!isValid()) {
        return 0;
    }
    return (uint16_t)data_->size();
}

bool modbus_response_t::getBool(unsigned int index) {
    if (!isValid()) {
        return false;
    }

    unsigned int registerOffset = index / 8;
    int bit = index % 8;

    uint8_t mask = (uint8_t)1 << bit;

    return (bool)(data_->at(registerOffset) & mask);
}

uint8_t modbus_response_t::getUINT8(unsigned int index) const {
    if (!isValid()) {
        return 0;
    }
    return data_->at(index);
}

int8_t modbus_response_t::getINT8(unsigned int index) const {
    if (!isValid()) {
        return 0;
    }
    return (int8_t)data_->at(index);
}

uint16_t modbus_response_t::getUINT16(unsigned int index,
                                      byte_order order) const {
    if (!isValid()) {
        return 0;
    }

    uint16_t data;
    switch (order) {
    case byte_order::normal:
        data = data_->at(sizeof(data) * index) << 8;
        data += data_->at(sizeof(data) * index + 1);
        break;
    case byte_order::byte_swapped:
        data = data_->at(sizeof(data) * index + 1) << 8;
        data += data_->at(sizeof(data) * index);
        break;
    }

    return data;
}

int16_t modbus_response_t::getINT16(unsigned int index,
                                    byte_order order) const {
    if (!isValid()) {
        return 0;
    }

    uint16_t data;
    switch (order) {
    case byte_order::normal:
        data = data_->at(sizeof(data) * index) << 8;
        data += data_->at(sizeof(data) * index + 1);
        break;
    case byte_order::byte_swapped:
        data = data_->at(sizeof(data) * index + 1) << 8;
        data += data_->at(sizeof(data) * index);
        break;
    }

    return (int16_t)data;
}

uint32_t modbus_response_t::getUINT32(unsigned int index,
                                      byte_order order) const {
    if (!isValid()) {
        return 0;
    }

    uint32_t data;
    switch (order) {
    case byte_order::normal:
        data = data_->at(sizeof(data) * index) << 24;
        data += data_->at(sizeof(data) * index + 1) << 16;
        data += data_->at(sizeof(data) * index + 2) << 8;
        data += data_->at(sizeof(data) * index + 3);
        break;
    case byte_order::byte_swapped:
        data = data_->at(sizeof(data) * index + 3) << 24;
        data += data_->at(sizeof(data) * index + 2) << 16;
        data += data_->at(sizeof(data) * index + 1) << 8;
        data += data_->at(sizeof(data) * index);
        break;
    }

    return data;
}

int32_t modbus_response_t::getINT32(unsigned int index,
                                    byte_order order) const {
    if (!isValid()) {
        return 0;
    }

    uint32_t data;
    switch (order) {
    case byte_order::normal:
        data = data_->at(sizeof(data) * index) << 24;
        data += data_->at(sizeof(data) * index + 1) << 16;
        data += data_->at(sizeof(data) * index + 2) << 8;
        data += data_->at(sizeof(data) * index + 3);
        break;
    case byte_order::byte_swapped:
        data = data_->at(sizeof(data) * index + 3) << 24;
        data += data_->at(sizeof(data) * index + 2) << 16;
        data += data_->at(sizeof(data) * index + 1) << 8;
        data += data_->at(sizeof(data) * index);
        break;
    }

    return (int32_t)data;
}

ostream& operator<<(ostream& output, const modbus_response_t& data) {
    output << "{" << endl;
    output << "Type: " << (uint8_t)data.type_ << "," << endl;

    output << "data: ";
    std::for_each(data.data_->begin(), data.data_->end(),
                  [&](const uint8_t& i) { output << (int)i; });
    output << endl << "}" << endl;

    return output;
}

data_model_t modbus_response_t::data_type(function_code_t code) {
    switch (code) {
    case function_code_t::read_coils:
        return data_model_t::coil;

    case function_code_t::read_discrete_inputs:
        return data_model_t::input_status;

    case function_code_t::read_input_registers:
        return data_model_t::input_register;

    case function_code_t::read_holding_registers:
        return data_model_t::holding_register;

    case function_code_t::write_single_coil:
    case function_code_t::write_multiple_coils:
    case function_code_t::write_single_register:
    case function_code_t::write_multiple_registers:
        return data_model_t::write_confirmation;

    default:
        return data_model_t::invalid_data_type;
    }

    return data_model_t::invalid_data_type;
}

} // namespace modbus