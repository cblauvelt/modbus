
#include <iomanip>

#include "tcp_data_unit.hpp"

namespace modbus {

tcp_data_unit::tcp_data_unit()
    : buffer_(std::make_shared<buffer_t>())
    , type_(message_type::invalid_pdu_type) {}

tcp_data_unit::tcp_data_unit(modbus::buffer_t buffer, size_t bytesRead,
                             message_type type)
    : type_(type) {
    // build message length
    auto messageLength = buffer.at(4) << 8;
    messageLength += buffer.at(5);

    int dataLength = (int)(bytesRead - TCP_HEADER_SIZE);
    if (dataLength <= 0 || dataLength < messageLength) {
        throw std::out_of_range(
            "Message length field and Data length do not match.");
    }

    buffer_ = std::make_shared<modbus::buffer_t>(std::move(buffer));
}

tcp_data_unit::tcp_data_unit(modbus::buffer_t header, modbus::buffer_t payload,
                             size_t bytesRead, message_type type)
    : buffer_(std::make_shared<modbus::buffer_t>())
    , type_(type) {
    if (header.at(2) != 0 || header.at(3) != 0) {
        throw std::invalid_argument("Protocol ID is invalid");
    }

    // build message length
    auto messageLength = header.at(4) << 8;
    messageLength += header.at(5);

    if (bytesRead <= 0 || bytesRead < messageLength) {
        throw std::out_of_range(
            "Message length field and Data length do not match.");
    }

    buffer_->reserve(header.size() + payload.size());
    buffer_->insert(buffer_->begin(), std::make_move_iterator(header.begin()),
                    std::make_move_iterator(header.end()));

    buffer_->insert(buffer_->end(), std::make_move_iterator(payload.begin()),
                    std::make_move_iterator(payload.end()));
}

uint16_t tcp_data_unit::transaction_id() const {
    uint16_t transactionId = buffer_->at(0) << 8;
    transactionId += buffer_->at(1);
    return transactionId;
}

uint16_t tcp_data_unit::message_length() const {
    uint16_t messageLength = buffer_->at(4) << 8;
    messageLength += buffer_->at(5);
    return messageLength;
}

uint8_t tcp_data_unit::unit_id() const { return buffer_->at(6); }

function_code_t tcp_data_unit::function_code() const {
    return (modbus::function_code_t)(buffer_->at(7) & 0x7F);
}

bool tcp_data_unit::is_exception() const { return (buffer_->at(7) & 0x80); }

exception_code_t tcp_data_unit::exception_code() const {
    if (!is_exception()) {
        return exception_code_t::no_exception;
    }

    return (modbus::exception_code_t)buffer_->at(8);
}

message_type tcp_data_unit::type() const { return type_; }

std::shared_ptr<const modbus::buffer_t> tcp_data_unit::buffer() const {
    return std::shared_ptr<const modbus::buffer_t>(buffer_);
}

} // namespace modbus