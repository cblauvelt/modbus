#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#include "modbus/core/messages/exception_response.hpp"
#include "modbus/core/messages/is_message.hpp"
#include "modbus/core/types.hpp"

namespace modbus {

/**
 * @brief This class represents the individual data unit that is sent to the
 * receiving device.
 *
 * @section The tcp_data_unit is defined by the MODBUS standard and contains a
 * PDU which can represent a request or a response.
 */
class tcp_data_unit {

  public:
    /**
     * @brief Creates an empty, invalid tcp_data_unit
     */
    tcp_data_unit();

    /**
     * @brief Creates a tcp_data_unit that contains the PDU.
     * @param transactionId The transaction ID as defined by the MODBUS
     * standard.
     * @param pdu The data payload that the TCP Data Unit will hold. It must
     * meet the requirements of is_message.
     */
    template <typename M>
    tcp_data_unit(uint16_t transactionId, M pdu)
        : buffer_(std::make_shared<buffer_t>())
        , type_(pdu.type()) {
        // Stop compilation if requirements for Message
        static_assert(detail::is_message<M>::value,
                      "Message Requirements not met.");

        auto length = pdu.size();
        buffer_->resize(TCP_HEADER_SIZE + length);
        auto it = buffer_->begin();

        // transaction ID
        *it++ = (uint8_t)(transactionId >> 8);
        *it++ = (uint8_t)(transactionId);

        // protocol ID
        *it++ = (uint8_t)(PROTOCOL_ID >> 8);
        *it++ = (uint8_t)(PROTOCOL_ID);

        // message length
        *it++ = (uint8_t)(length >> 8);
        *it++ = (uint8_t)(length);

        pdu.serialize(it);
    }

    /**
     * @brief Creates a tcp_data_unit
     * @param buffer The buffer that is used reading and writing data to the
     * remote endpoint
     * @param bytesRead The number of bytes within the buffer.
     * @param type An enum that defines whether this is a request or a
     * response.
     */
    tcp_data_unit(buffer_t buffer, size_t bytesRead, message_type type);

    /**
     * @brief Creates a tcp_data_unit
     * @param header The header of the tcp_data_unit.
     * @param payload The data portion of the data unit.
     * @param bytesRead The number of bytes within the payload.
     * @param type An enum that defines whether this is a request or a
     * response.
     */
    tcp_data_unit(buffer_t header, buffer_t payload, size_t bytesRead,
                  message_type type);

    tcp_data_unit(const tcp_data_unit& dataUnit) = default;

    tcp_data_unit(tcp_data_unit&&) noexcept = default;

    tcp_data_unit& operator=(const tcp_data_unit& x) = default;

    tcp_data_unit& operator=(tcp_data_unit&&) noexcept = default;

    /**
     * @return The transaction ID assigned during creation of the object.
     */
    uint16_t transaction_id() const;

    /**
     * @return The length of the PDU + the transaction ID in bytes
     */
    uint16_t message_length() const;

    /**
     * @return The unit ID of the intended recipient of the request.
     */
    uint8_t unit_id() const;

    /**
     * @return The function code.
     */
    function_code_t function_code() const;

    /**
     * @return Whether or not the response is an exception
     */
    bool is_exception() const;

    /**
     * @return The exception code from the response
     */
    exception_code_t exception_code() const;

    /**
     * @return The MODBUS Protocol Data Unit (PDU)
     */
    template <typename T>
    std::optional<typename std::enable_if_t<
        !std::is_same<T, exception_response>::value, T>>
    pdu() const {
        try {
            if (this->is_exception()) {
                return std::nullopt;
            }

            if ((uint8_t)this->function_code() != (uint8_t)T::function_code()) {
                return std::nullopt;
            }

            if (type_ != T::type()) {
                return std::nullopt;
            }

            if (buffer_->empty()) {
                return std::nullopt;
            }

            const_buffer_iterator it = buffer_->begin();
            it += TCP_HEADER_SIZE;

            T pdu(it);
            return pdu;
        } catch (...) {
            return std::nullopt;
        }
    }

    /**
     * @return The MODBUS Protocol Data Unit for exception_response types
     */
    template <typename T>
    std::optional<typename std::enable_if_t<
        std::is_same<T, exception_response>::value, T>>
    pdu() const {
        try {
            if (!this->is_exception()) {
                return std::nullopt;
            }

            if (type_ != T::type()) {
                return std::nullopt;
            }

            if (buffer_->empty()) {
                return std::nullopt;
            }

            const_buffer_iterator it = buffer_->begin();
            it += TCP_HEADER_SIZE;

            T pdu(it);
            return pdu;
        } catch (...) {
            return std::nullopt;
        }
    }

    /**
     * @brief Returns a shared_ptr to the buffer of the data unit.
     * This allows the buffer to be sent to asio::async_write without a copy.
     * @returns shared_ptr<const buffer>
     */
    std::shared_ptr<const modbus::buffer_t> buffer() const;

    /**
     * @returns Whether the data unit contains a request or a response.
     */
    message_type type() const;

  private:
    std::shared_ptr<modbus::buffer_t> buffer_;
    message_type type_;
};

} // namespace modbus
