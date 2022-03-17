#include "modbus/core/responses.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {

using namespace modbus;
using namespace std;

TEST(responses, read_coils_response) {
    read_coils_response response{
        17, std::vector<uint8_t>{0xCD, 0x6B, 0xB2, 0x0E, 0x1B}};

    EXPECT_EQ(response.unit_id, 17);
    EXPECT_EQ(response.function_code(), function_code_t::read_coils);
    EXPECT_EQ(response.type(), message_type::response);

    read_coils_response eqResponse = response;
    read_coils_response neResponse1{
        18, std::vector<uint8_t>{0xCD, 0x6B, 0xB2, 0x0E, 0x1B}};
    read_coils_response neResponse2{
        17, std::vector<uint8_t>{0xC1, 0x6B, 0xB2, 0x0E, 0x1B}};

    EXPECT_EQ(response, eqResponse);
    EXPECT_NE(response, neResponse1);
    EXPECT_NE(response, neResponse2);

    buffer_t data{0x11, 0x01, 0x05, 0xCD, 0x6B, 0xB2, 0x0E, 0x1B};
    EXPECT_EQ(response.size(), data.size());

    buffer_t retData;
    retData.resize(response.size());
    buffer_iterator it = retData.begin();
    it = response.serialize(it);

    EXPECT_EQ(data, retData);

    read_coils_response dataResponse{data.begin()};
    EXPECT_EQ(dataResponse, response);
}

TEST(responses, read_discrete_inputs_response) {
    read_discrete_inputs_response response{
        17, std::vector<uint8_t>{0xAC, 0xDB, 0x35}};

    EXPECT_EQ(response.unit_id, 17);
    EXPECT_EQ(response.function_code(), function_code_t::read_discrete_inputs);
    EXPECT_EQ(response.type(), message_type::response);

    read_discrete_inputs_response eqResponse = response;
    read_discrete_inputs_response neResponse1{
        18, std::vector<uint8_t>{0xAC, 0xDB, 0x35}};
    read_discrete_inputs_response neResponse2{
        17, std::vector<uint8_t>{0xA1, 0xDB, 0x35}};

    EXPECT_EQ(response, eqResponse);
    EXPECT_NE(response, neResponse1);
    EXPECT_NE(response, neResponse2);

    buffer_t data{0x11, 0x02, 0x03, 0xAC, 0xDB, 0x35};
    EXPECT_EQ(response.size(), data.size());

    buffer_t retData;
    retData.resize(response.size());
    buffer_iterator it = retData.begin();
    it = response.serialize(it);

    EXPECT_EQ(data, retData);

    read_discrete_inputs_response dataResponse{data.begin()};
    EXPECT_EQ(dataResponse, response);
}

TEST(responses, read_holding_reg_response) {
    read_holding_registers_response response16{
        17, std::vector<uint16_t>{0x022B, 0x0000, 0x0064}};
    read_holding_registers_response response(
        17, std::vector<uint8_t>{0x02, 0x2B, 0x00, 0x00, 0x00, 0x64});

    EXPECT_EQ(response, response16);

    EXPECT_EQ(response.unit_id, 17);
    EXPECT_EQ(response.function_code(),
              function_code_t::read_holding_registers);
    EXPECT_EQ(response.type(), message_type::response);

    read_holding_registers_response eqResponse = response;
    read_holding_registers_response neResponse1{
        18, std::vector<uint16_t>{0x022B, 0x0000, 0x0064}};
    read_holding_registers_response neResponse2{
        17, std::vector<uint16_t>{0x0221, 0x0000, 0x0064}};

    EXPECT_EQ(response, eqResponse);
    EXPECT_NE(response, neResponse1);
    EXPECT_NE(response, neResponse2);

    buffer_t data{0x11, 0x03, 0x06, 0x02, 0x2B, 0x00, 0x00, 0x00, 0x64};
    EXPECT_EQ(response.size(), data.size());

    buffer_t retData;
    retData.resize(response.size());
    buffer_iterator it = retData.begin();
    it = response.serialize(it);

    EXPECT_EQ(data, retData);

    read_holding_registers_response dataResponse{data.begin()};
    EXPECT_EQ(dataResponse, response);
}

TEST(responses, read_input_reg_response) {
    read_input_registers_response response16{17, std::vector<uint16_t>{0x000A}};
    read_input_registers_response response{17,
                                           std::vector<uint8_t>{0x00, 0x0A}};

    EXPECT_EQ(response, response16);

    EXPECT_EQ(response.unit_id, 17);
    EXPECT_EQ(response.function_code(), function_code_t::read_input_registers);
    EXPECT_EQ(response.type(), message_type::response);

    read_input_registers_response eqResponse = response;
    read_input_registers_response neResponse1{18,
                                              std::vector<uint16_t>{0x000A}};
    read_input_registers_response neResponse2{17,
                                              std::vector<uint16_t>{0x001A}};

    EXPECT_EQ(response, eqResponse);
    EXPECT_NE(response, neResponse1);
    EXPECT_NE(response, neResponse2);

    buffer_t data{0x11, 0x04, 0x02, 0x00, 0x0A};
    EXPECT_EQ(response.size(), data.size());

    buffer_t retData;
    retData.resize(response.size());
    buffer_iterator it = retData.begin();
    it = response.serialize(it);

    EXPECT_EQ(data, retData);

    read_input_registers_response dataResponse{data.begin()};
    EXPECT_EQ(dataResponse, response);
}

TEST(responses, write_single_coil_response) {
    write_single_coil_response response{17, 172, coil_status_t::on};

    EXPECT_EQ(response.unit_id, 17);
    EXPECT_EQ(response.start_address, 172);
    EXPECT_EQ(response.value, coil_status_t::on);
    EXPECT_EQ(response.function_code(), function_code_t::write_single_coil);
    EXPECT_EQ(response.type(), message_type::response);

    write_single_coil_response eqResponse = response;
    write_single_coil_response neResponse1{18, 172, coil_status_t::on};
    write_single_coil_response neResponse2{17, 173, coil_status_t::on};
    write_single_coil_response neResponse3{17, 172, coil_status_t::off};

    EXPECT_EQ(response, eqResponse);
    EXPECT_NE(response, neResponse1);
    EXPECT_NE(response, neResponse2);
    EXPECT_NE(response, neResponse3);

    buffer_t data{0x11, 0x05, 0x00, 0xAC, 0xFF, 0x00};
    EXPECT_EQ(response.size(), data.size());

    buffer_t retData;
    retData.resize(response.size());
    buffer_iterator it = retData.begin();
    it = response.serialize(it);

    EXPECT_EQ(data, retData);

    write_single_coil_response dataResponse{data.begin()};
    EXPECT_EQ(dataResponse, response);
}

TEST(responses, write_single_reg_response) {
    write_single_register_response response{17, 1, 3};

    EXPECT_EQ(response.unit_id, 17);
    EXPECT_EQ(response.start_address, 1);
    EXPECT_EQ(response.value, 3);
    EXPECT_EQ(response.function_code(), function_code_t::write_single_register);
    EXPECT_EQ(response.type(), message_type::response);

    write_single_register_response eqResponse = response;
    write_single_register_response neResponse1{18, 1, 3};
    write_single_register_response neResponse2{17, 2, 3};
    write_single_register_response neResponse3{17, 1, 4};

    EXPECT_EQ(response, eqResponse);
    EXPECT_NE(response, neResponse1);
    EXPECT_NE(response, neResponse2);
    EXPECT_NE(response, neResponse3);

    buffer_t data{0x11, 0x06, 0x00, 0x01, 0x00, 0x03};
    EXPECT_EQ(response.size(), data.size());

    buffer_t retData;
    retData.resize(response.size());
    buffer_iterator it = retData.begin();
    it = response.serialize(it);

    EXPECT_EQ(data, retData);

    write_single_register_response dataResponse{data.begin()};
    EXPECT_EQ(dataResponse, response);
}

TEST(responses, write_multi_coil_response) {
    write_multiple_coils_response response{17, 19, 10};

    EXPECT_EQ(response.unit_id, 17);
    EXPECT_EQ(response.start_address, 19);
    EXPECT_EQ(response.length, 10);
    EXPECT_EQ(response.function_code(), function_code_t::write_multiple_coils);
    EXPECT_EQ(response.type(), message_type::response);

    write_multiple_coils_response eqResponse = response;
    write_multiple_coils_response neResponse1{18, 19, 10};
    write_multiple_coils_response neResponse2{17, 20, 10};
    write_multiple_coils_response neResponse3{17, 19, 11};

    EXPECT_EQ(response, eqResponse);
    EXPECT_NE(response, neResponse1);
    EXPECT_NE(response, neResponse2);
    EXPECT_NE(response, neResponse3);

    buffer_t data{0x11, 0x0F, 0x00, 0x13, 0x00, 0x0A};
    EXPECT_EQ(response.size(), data.size());

    buffer_t retData;
    retData.resize(response.size());

    buffer_iterator it = retData.begin();
    it = response.serialize(it);

    EXPECT_EQ(data, retData);

    write_multiple_coils_response dataResponse{data.begin()};
    EXPECT_EQ(dataResponse, response);
}

TEST(responses, write_multi_reg_response) {
    write_multiple_registers_response response{17, 1, 2};

    EXPECT_EQ(response.unit_id, 17);
    EXPECT_EQ(response.start_address, 1);
    EXPECT_EQ(response.length, 2);
    EXPECT_EQ(response.function_code(),
              function_code_t::write_multiple_registers);
    EXPECT_EQ(response.type(), message_type::response);

    write_multiple_registers_response eqResponse = response;
    write_multiple_registers_response neResponse1{18, 1, 2};
    write_multiple_registers_response neResponse2{17, 2, 2};
    write_multiple_registers_response neResponse3{17, 1, 3};

    EXPECT_EQ(response, eqResponse);
    EXPECT_NE(response, neResponse1);
    EXPECT_NE(response, neResponse2);
    EXPECT_NE(response, neResponse3);

    buffer_t data{0x11, 0x10, 0x00, 0x01, 0x00, 0x02};
    EXPECT_EQ(response.size(), data.size());

    buffer_t retData;
    retData.resize(response.size());
    buffer_iterator it = retData.begin();
    it = response.serialize(it);

    EXPECT_EQ(data, retData);

    write_multiple_registers_response dataResponse{data.begin()};
    EXPECT_EQ(dataResponse, response);
}

TEST(responses, mask_write_reg_response) {
    mask_write_register_response response{17, 4, 0x00F2, 0x0025};

    EXPECT_EQ(response.unit_id, 17);
    EXPECT_EQ(response.start_address, 4);
    EXPECT_EQ(response.and_mask, 0x00F2);
    EXPECT_EQ(response.or_mask, 0x0025);
    EXPECT_EQ(response.function_code(), function_code_t::mask_write_register);
    EXPECT_EQ(response.type(), message_type::response);

    mask_write_register_response eqResponse = response;
    mask_write_register_response neResponse1{18, 4, 0x00F2, 0x0025};
    mask_write_register_response neResponse2{17, 5, 0x00F2, 0x0025};
    mask_write_register_response neResponse3{17, 4, 0x00F3, 0x0025};
    mask_write_register_response neResponse4{17, 4, 0x00F2, 0x0026};

    EXPECT_EQ(response, eqResponse);
    EXPECT_NE(response, neResponse1);
    EXPECT_NE(response, neResponse2);
    EXPECT_NE(response, neResponse3);
    EXPECT_NE(response, neResponse4);

    buffer_t data{0x11, 0x16, 0x00, 0x04, 0x00, 0xF2, 0x00, 0x25};
    EXPECT_EQ(response.size(), data.size());

    buffer_t retData;
    retData.resize(response.size());
    buffer_iterator it = retData.begin();
    it = response.serialize(it);

    EXPECT_EQ(data, retData);

    mask_write_register_response dataResponse{data.begin()};
    EXPECT_EQ(dataResponse, response);
}

TEST(responses, read_write_multi_reg_response) {
    read_write_registers_response response{
        17,
        std::vector<uint16_t>{0x00FE, 0x0ACD, 0x0001, 0x0003, 0x000D, 0x00FF}};

    EXPECT_EQ(response.unit_id, 17);
    EXPECT_EQ(response.function_code(),
              function_code_t::read_write_multiple_registers);
    EXPECT_EQ(response.type(), message_type::response);

    // Test equality and inequality operators
    read_write_registers_response eqResponse = response;
    read_write_registers_response neResponse1{
        18,
        std::vector<uint16_t>{0x00FE, 0x0ACD, 0x0001, 0x0003, 0x000D, 0x00FF}};
    read_write_registers_response neResponse2{
        17,
        std::vector<uint16_t>{0x01FE, 0x0ACD, 0x0001, 0x0003, 0x000D, 0x00FF}};

    EXPECT_EQ(response, eqResponse);
    EXPECT_NE(response, neResponse1);
    EXPECT_NE(response, neResponse2);

    // Test serialize function
    buffer_t data{17,   0x17, 0x0C, 0x00, 0xFE, 0x0A, 0xCD, 0x00,
                  0x01, 0x00, 0x03, 0x00, 0x0D, 0x00, 0xFF};
    EXPECT_EQ(response.size(), data.size());

    buffer_t retData;
    retData.resize(response.size());
    buffer_iterator it = retData.begin();
    it = response.serialize(it);

    EXPECT_EQ(data, retData);

    // Test constructor from iterator
    read_write_registers_response dataResponse{data.begin()};
    EXPECT_EQ(dataResponse, response);
}

TEST(responses, exception_response) {
    // Test constructor
    exception_response response{17, function_code_t::read_coils,
                                exception_code_t::illegal_data_address};

    EXPECT_EQ(response.unit_id, 17);
    EXPECT_EQ(response.function_code(), function_code_t::read_coils);
    EXPECT_EQ(response.type(), message_type::response);
    EXPECT_EQ(response.exception_code, exception_code_t::illegal_data_address);

    // Test equality and inequality operators
    exception_response eqResponse = response;
    exception_response neResponse1{18, function_code_t::read_coils,
                                   exception_code_t::illegal_data_address};
    exception_response neResponse2{17, function_code_t::read_discrete_inputs,
                                   exception_code_t::illegal_data_address};
    exception_response neResponse3{17, function_code_t::read_coils,
                                   exception_code_t::illegal_function};

    EXPECT_EQ(response, eqResponse);
    EXPECT_NE(response, neResponse1);
    EXPECT_NE(response, neResponse2);
    EXPECT_NE(response, neResponse3);

    // Test serialize function
    buffer_t data{0x11, 0x81, 0x02};
    EXPECT_EQ(response.size(), data.size());

    buffer_t retData;
    retData.resize(response.size());
    buffer_iterator it = retData.begin();
    it = response.serialize(it);

    EXPECT_EQ(data, retData);

    // Test constructor from iterator
    exception_response dataResponse{data.begin()};
    EXPECT_EQ(dataResponse, response);
}

}