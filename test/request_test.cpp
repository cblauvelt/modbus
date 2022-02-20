#include "requests.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {

using namespace modbus;
using namespace std;

TEST(requests, read_coils_request) {
    read_coils_request request{17, 19, 37};

    EXPECT_EQ(request.unit_id, 17);
    EXPECT_EQ(request.start_address, 19);
    EXPECT_EQ(request.length, 37);
    EXPECT_EQ(request.function_code(), function_code_t::read_coils);
    EXPECT_EQ(request.type(), message_type::request);

    read_coils_request eqRequest = request;
    read_coils_request neRequest1{2, 19, 37};
    read_coils_request neRequest2{17, 20, 37};
    read_coils_request neRequest3{17, 19, 38};

    EXPECT_EQ(request, eqRequest);
    EXPECT_NE(request, neRequest1);
    EXPECT_NE(request, neRequest2);
    EXPECT_NE(request, neRequest3);

    buffer_t data{0x11, 0x01, 0x00, 0x13, 0x00, 0x25};
    EXPECT_EQ(request.size(), data.size());

    buffer_t retData;
    retData.resize(request.size());
    buffer_iterator it = retData.begin();
    it = request.serialize(it);

    EXPECT_EQ(data, retData);

    read_coils_request dataRequest{data.begin()};
    EXPECT_EQ(dataRequest, request);
}

TEST(requests, read_discrete_inputs_request) {
    read_discrete_inputs_request request{17, 196, 22};

    EXPECT_EQ(request.unit_id, 17);
    EXPECT_EQ(request.start_address, 196);
    EXPECT_EQ(request.length, 22);
    EXPECT_EQ(request.function_code(), function_code_t::read_discrete_inputs);
    EXPECT_EQ(request.type(), message_type::request);

    read_discrete_inputs_request eqRequest = request;
    read_discrete_inputs_request neRequest1{18, 196, 22};
    read_discrete_inputs_request neRequest2{17, 197, 22};
    read_discrete_inputs_request neRequest3{17, 196, 23};

    EXPECT_EQ(request, eqRequest);
    EXPECT_NE(request, neRequest1);
    EXPECT_NE(request, neRequest2);
    EXPECT_NE(request, neRequest3);

    buffer_t data{0x11, 0x02, 0x00, 0xC4, 0x00, 0x16};
    EXPECT_EQ(request.size(), data.size());

    buffer_t retData;
    retData.resize(request.size());
    buffer_iterator it = retData.begin();
    it = request.serialize(it);

    EXPECT_EQ(data, retData);

    read_discrete_inputs_request dataRequest{data.begin()};
    EXPECT_EQ(dataRequest, request);
}

TEST(requests, read_holding_reg_request) {
    read_holding_registers_request request{17, 107, 3};

    EXPECT_EQ(request.unit_id, 17);
    EXPECT_EQ(request.start_address, 107);
    EXPECT_EQ(request.length, 3);
    EXPECT_EQ(request.function_code(), function_code_t::read_holding_registers);
    EXPECT_EQ(request.type(), message_type::request);

    read_holding_registers_request eqRequest = request;
    read_holding_registers_request neRequest1{18, 107, 3};
    read_holding_registers_request neRequest2{17, 108, 3};
    read_holding_registers_request neRequest3{17, 107, 4};

    EXPECT_EQ(request, eqRequest);
    EXPECT_NE(request, neRequest1);
    EXPECT_NE(request, neRequest2);
    EXPECT_NE(request, neRequest3);

    buffer_t data{0x11, 0x03, 0x00, 0x6B, 0x00, 0x03};
    EXPECT_EQ(request.size(), data.size());

    buffer_t retData;
    retData.resize(request.size());
    buffer_iterator it = retData.begin();
    it = request.serialize(it);

    EXPECT_EQ(data, retData);

    read_holding_registers_request dataRequest{data.begin()};
    EXPECT_EQ(dataRequest, request);
}

TEST(requests, read_input_reg_request) {
    read_input_registers_request request{17, 8, 1};

    EXPECT_EQ(request.unit_id, 17);
    EXPECT_EQ(request.start_address, 8);
    EXPECT_EQ(request.length, 1);
    EXPECT_EQ(request.function_code(), function_code_t::read_input_registers);
    EXPECT_EQ(request.type(), message_type::request);

    read_input_registers_request eqRequest = request;
    read_input_registers_request neRequest1{18, 8, 1};
    read_input_registers_request neRequest2{17, 9, 1};
    read_input_registers_request neRequest3{17, 8, 2};

    EXPECT_EQ(request, eqRequest);
    EXPECT_NE(request, neRequest1);
    EXPECT_NE(request, neRequest2);
    EXPECT_NE(request, neRequest3);

    buffer_t data{0x11, 0x04, 0x00, 0x08, 0x00, 0x01};
    EXPECT_EQ(request.size(), data.size());

    buffer_t retData;
    retData.resize(request.size());
    buffer_iterator it = retData.begin();
    it = request.serialize(it);

    EXPECT_EQ(data, retData);

    read_input_registers_request dataRequest{data.begin()};
    EXPECT_EQ(dataRequest, request);
}

TEST(requests, write_single_coil_request) {
    write_single_coil_request request{17, 172, coil_status_t::on};

    EXPECT_EQ(request.unit_id, 17);
    EXPECT_EQ(request.start_address, 172);
    EXPECT_EQ(request.value, coil_status_t::on);
    EXPECT_EQ(request.function_code(), function_code_t::write_single_coil);
    EXPECT_EQ(request.type(), message_type::request);

    write_single_coil_request eqRequest = request;
    write_single_coil_request neRequest1{18, 172, coil_status_t::on};
    write_single_coil_request neRequest2{17, 173, coil_status_t::on};
    write_single_coil_request neRequest3{17, 172, coil_status_t::off};

    EXPECT_EQ(request, eqRequest);
    EXPECT_NE(request, neRequest1);
    EXPECT_NE(request, neRequest2);
    EXPECT_NE(request, neRequest3);

    buffer_t data{0x11, 0x05, 0x00, 0xAC, 0xFF, 0x00};
    EXPECT_EQ(request.size(), data.size());

    buffer_t retData;
    retData.resize(request.size());
    buffer_iterator it = retData.begin();
    it = request.serialize(it);

    EXPECT_EQ(data, retData);

    write_single_coil_request dataRequest{data.begin()};
    EXPECT_EQ(dataRequest, request);
}

TEST(requests, write_single_reg_request) {
    write_single_register_request request{17, 1, 3};

    EXPECT_EQ(request.unit_id, 17);
    EXPECT_EQ(request.start_address, 1);
    EXPECT_EQ(request.value, 3);
    EXPECT_EQ(request.function_code(), function_code_t::write_single_register);
    EXPECT_EQ(request.type(), message_type::request);

    write_single_register_request eqRequest = request;
    write_single_register_request neRequest1{18, 1, 3};
    write_single_register_request neRequest2{17, 2, 3};
    write_single_register_request neRequest3{17, 1, 4};

    EXPECT_EQ(request, eqRequest);
    EXPECT_NE(request, neRequest1);
    EXPECT_NE(request, neRequest2);
    EXPECT_NE(request, neRequest3);

    buffer_t data{0x11, 0x06, 0x00, 0x01, 0x00, 0x03};
    EXPECT_EQ(request.size(), data.size());

    buffer_t retData;
    retData.resize(request.size());
    buffer_iterator it = retData.begin();
    it = request.serialize(it);

    EXPECT_EQ(data, retData);

    write_single_register_request dataRequest{data.begin()};
    EXPECT_EQ(dataRequest, request);
}

TEST(requests, write_multi_coil_request) {
    write_multiple_coils_request request{17, 19, 10,
                                         std::vector<uint8_t>{0xCD, 0x01}};

    EXPECT_EQ(request.unit_id, 17);
    EXPECT_EQ(request.start_address, 19);
    EXPECT_EQ(request.length, 10);
    EXPECT_EQ(request.function_code(), function_code_t::write_multiple_coils);
    EXPECT_EQ(request.type(), message_type::request);

    write_multiple_coils_request eqRequest = request;
    write_multiple_coils_request neRequest1{18, 19, 10,
                                            std::vector<uint8_t>{0xCD, 0x01}};
    write_multiple_coils_request neRequest2{17, 20, 10,
                                            std::vector<uint8_t>{0xCD, 0x01}};
    write_multiple_coils_request neRequest3{17, 19, 11,
                                            std::vector<uint8_t>{0xCD, 0x01}};
    write_multiple_coils_request neRequest4{17, 19, 10,
                                            std::vector<uint8_t>{0xCD, 0x02}};

    EXPECT_EQ(request, eqRequest);
    EXPECT_NE(request, neRequest1);
    EXPECT_NE(request, neRequest2);
    EXPECT_NE(request, neRequest3);
    EXPECT_NE(request, neRequest4);

    buffer_t data{0x11, 0x0F, 0x00, 0x13, 0x00, 0x0A, 0x02, 0xCD, 0x01};
    EXPECT_EQ(request.size(), data.size());

    buffer_t retData;
    retData.resize(request.size());

    buffer_iterator it = retData.begin();
    it = request.serialize(it);

    EXPECT_EQ(data, retData);

    write_multiple_coils_request dataRequest{data.begin()};
    EXPECT_EQ(dataRequest, request);
}

TEST(requests, write_multi_reg_request) {
    write_multiple_registers_request request{17, 1, 2,
                                             std::vector<uint16_t>{10, 258}};

    EXPECT_EQ(request.unit_id, 17);
    EXPECT_EQ(request.start_address, 1);
    EXPECT_EQ(request.length, 2);
    EXPECT_EQ(request.function_code(),
              function_code_t::write_multiple_registers);
    EXPECT_EQ(request.type(), message_type::request);

    write_multiple_registers_request eqRequest = request;
    write_multiple_registers_request neRequest1{18, 1, 2,
                                                std::vector<uint16_t>{11, 258}};
    write_multiple_registers_request neRequest2{17, 2, 2,
                                                std::vector<uint16_t>{11, 258}};
    write_multiple_registers_request neRequest3{17, 1, 3,
                                                std::vector<uint16_t>{11, 258}};
    write_multiple_registers_request neRequest4{17, 1, 2,
                                                std::vector<uint16_t>{11, 259}};

    EXPECT_EQ(request, eqRequest);
    EXPECT_NE(request, neRequest1);
    EXPECT_NE(request, neRequest2);
    EXPECT_NE(request, neRequest3);
    EXPECT_NE(request, neRequest4);

    buffer_t data{0x11, 0x10, 0x00, 0x01, 0x00, 0x02,
                  0x04, 0x00, 0x0A, 0x01, 0x02};
    EXPECT_EQ(request.size(), data.size());

    buffer_t retData;
    retData.resize(request.size());
    buffer_iterator it = retData.begin();
    it = request.serialize(it);

    EXPECT_EQ(data, retData);

    write_multiple_registers_request dataRequest{data.begin()};
    EXPECT_EQ(dataRequest, request);
}

TEST(requests, mask_write_reg_request) {
    mask_write_register_request request{17, 4, 0x00F2, 0x0025};

    EXPECT_EQ(request.unit_id, 17);
    EXPECT_EQ(request.start_address, 4);
    EXPECT_EQ(request.and_mask, 0x00F2);
    EXPECT_EQ(request.or_mask, 0x0025);
    EXPECT_EQ(request.function_code(), function_code_t::mask_write_register);
    EXPECT_EQ(request.type(), message_type::request);

    mask_write_register_request eqRequest = request;
    mask_write_register_request neRequest1{18, 4, 0x00F2, 0x0025};
    mask_write_register_request neRequest2{17, 5, 0x00F2, 0x0025};
    mask_write_register_request neRequest3{17, 4, 0x00F3, 0x0025};
    mask_write_register_request neRequest4{17, 4, 0x00F2, 0x0026};

    EXPECT_EQ(request, eqRequest);
    EXPECT_NE(request, neRequest1);
    EXPECT_NE(request, neRequest2);
    EXPECT_NE(request, neRequest3);
    EXPECT_NE(request, neRequest4);

    buffer_t data{0x11, 0x16, 0x00, 0x04, 0x00, 0xF2, 0x00, 0x25};
    EXPECT_EQ(request.size(), data.size());

    buffer_t retData;
    retData.resize(request.size());
    buffer_iterator it = retData.begin();
    it = request.serialize(it);

    EXPECT_EQ(data, retData);

    mask_write_register_request dataRequest{data.begin()};
    EXPECT_EQ(dataRequest, request);
}

TEST(requests, read_write_multi_reg_request) {
    read_write_registers_request request{
        17, 4, 6, 15, std::vector<uint16_t>{0x00FF, 0x00FF, 0x00FF}};

    EXPECT_EQ(request.unit_id, 17);
    EXPECT_EQ(request.read_start_address, 4);
    EXPECT_EQ(request.read_num_registers, 6);
    EXPECT_EQ(request.write_start_address, 15);
    EXPECT_EQ(request.function_code(),
              function_code_t::read_write_multiple_registers);
    EXPECT_EQ(request.type(), message_type::request);

    // Test equality and inequality operators
    read_write_registers_request eqRequest = request;
    read_write_registers_request neRequest1{
        18, 4, 6, 15, std::vector<uint16_t>{0x00FF, 0x00FF, 0x00FF}};
    read_write_registers_request neRequest2{
        17, 5, 6, 15, std::vector<uint16_t>{0x00FF, 0x00FF, 0x00FF}};
    read_write_registers_request neRequest3{
        17, 4, 7, 15, std::vector<uint16_t>{0x00FF, 0x00FF, 0x00FF}};
    read_write_registers_request neRequest4{
        17, 4, 6, 16, std::vector<uint16_t>{0x00FF, 0x00FF, 0x00FF}};
    read_write_registers_request neRequest5{
        17, 4, 6, 16, std::vector<uint16_t>{0x00FF, 0x00FF}};
    read_write_registers_request neRequest6{
        17, 4, 6, 16, std::vector<uint16_t>{0x00FE, 0x00FF, 0x00FF}};

    EXPECT_EQ(request, eqRequest);
    EXPECT_NE(request, neRequest1);
    EXPECT_NE(request, neRequest2);
    EXPECT_NE(request, neRequest3);
    EXPECT_NE(request, neRequest4);
    EXPECT_NE(request, neRequest5);
    EXPECT_NE(request, neRequest6);

    // Test serialize function
    buffer_t data{0x11, 0x17, 0x00, 0x04, 0x00, 0x06, 0x00, 0x0F, 0x00,
                  0x03, 0x06, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
    EXPECT_EQ(request.size(), data.size());

    buffer_t retData;
    retData.resize(request.size());
    buffer_iterator it = retData.begin();
    it = request.serialize(it);

    EXPECT_EQ(data, retData);

    // Test constructor from iterator
    read_write_registers_request dataRequest{data.begin()};
    EXPECT_EQ(dataRequest, request);
}

} // namespace
