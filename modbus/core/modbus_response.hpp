#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>

#include "modbus/core/types.hpp"

namespace modbus {

using namespace std;

class modbus_response_t {

  public:
    /**
     * Initializes a default, invalid object
     */
    modbus_response_t();

    /**
     * Initializes a MODBUS data type with the necessary constructors.
     * @param type The data type held in data.
     * @param data The data of the response.
     * @param start_address The starting address of the data
     * @param numValues The number of values stored by the data. This will be
     * identical to data.size() for registers but can be different for coils
     * since 1-8 coils can be stored within a byte.
     */
    modbus_response_t(data_model_t type, shared_ptr<buffer_t> data,
                      uint16_t start_address, uint16_t numValues);

    /**
     * @return The type of data.
     */
    data_model_t data_model() const;

    /**
     * Sets the type of data.
     * @param type The type of data.
     */
    void set_data_model(data_model_t type);

    /**
     * Assignes the data to the data block.
     * @param data The data byte array
     */
    void set_data(shared_ptr<buffer_t> data);

    /**
     * @returns The starting address of the data.
     */
    uint16_t start_address() const;

    /**
     * @returns The number of values stored in data.
     */
    uint16_t size() const;

    /**
     * @return The buffer_length of the data block in bytes.
     */
    uint16_t buffer_length() const;

    /**
     * Returns the byte array of the data for serialization
     * @return The a shared point to the byte array representing
     * the data.
     */
    shared_ptr<buffer_t> data() const;

    /**
     * Copy the register defined by index into data.
     * @param index The index of coils or input statuses to put into data.
     */
    bool getBool(unsigned int index);

    /**
     * Puts an unsigned byte into data.
     * @param index The starting byte index in the data block to write into
     * data.
     */
    uint8_t getUINT8(unsigned int index) const;

    /**
     * Puts a signed byte into data.
     * @param index The starting byte index in the data block to write into
     * data.
     */
    int8_t getINT8(unsigned int index) const;

    /**
     * Puts an unsigned word into data.
     * @param index The starting byte index in the data block to write into
     * data.
     * @param order The byte order of the data point.
     */
    uint16_t getUINT16(unsigned int index,
                       byte_order order = byte_order::normal) const;

    /**
     * Puts a signed word into data.
     * @param index The starting byte index in the data block to write into
     * data.
     * @param order The word order of the data point.
     */
    int16_t getINT16(unsigned int index,
                     byte_order order = byte_order::normal) const;

    /**
     * Puts an unsigned word into data.
     * @param index The starting byte index in the data block to write into
     * data.
     * @param order The word order of the data point.
     */
    uint32_t getUINT32(unsigned int index,
                       byte_order order = byte_order::normal) const;

    /**
     * Puts a signed word into data.
     * @param index The starting byte index in the data block to write into
     * data.
     * @param order The word order of the data point.
     */
    int32_t getINT32(unsigned int index,
                     byte_order order = byte_order::normal) const;

    /**
     * The object is not a valid object if it's data type is Invaliddata_type or
     * id data.size() == 0
     * @return true if the data block is valid, otherwise false
     */
    constexpr bool isValid() const {
        return !(type_ == data_model_t::invalid_data_type || data_ == nullptr ||
                 data_->size() == 0 || num_values_ == 0);
    }

    /**
     * @brief Returns the data type given the function code.
     * @param code The MODBUS function code.
     * @returns The data type of the value within Data as defined by the enum
     * data_model_t.
     */
    static data_model_t data_type(function_code_t code);

    friend ostream& operator<<(ostream& output, const modbus_response_t& data);

  private:
    modbus::data_model_t type_;
    shared_ptr<modbus::buffer_t> data_;
    uint16_t start_address_;
    uint16_t num_values_;
};

} // namespace modbus
