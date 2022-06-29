#include "modbus/core/requests.hpp"
#include "modbus/core/tcp_data_unit.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {

using namespace modbus;
using namespace std;

TEST(tcp_data_unit, empty) {
    tcp_data_unit data_unit;
    EXPECT_THROW(data_unit.is_exception(), std::out_of_range);
    EXPECT_THROW(data_unit.exception_code(), std::out_of_range);
    EXPECT_THROW(data_unit.transaction_id(), std::out_of_range);
    EXPECT_THROW(data_unit.message_length(), std::out_of_range);
    EXPECT_THROW(data_unit.unit_id(), std::out_of_range);
    EXPECT_THROW(data_unit.function_code(), std::out_of_range);
}

TEST(tcp_data_unit, read_coils_request) {
    read_coils_request request = read_coils_request{17, 19, 37};
    buffer_t data{0x00, 0x01, 0x00, 0x00, 0x00, 0x06,
                  0x11, 0x01, 0x00, 0x13, 0x00, 0x25};

    uint16_t transaction_id = 0x0001;
    uint8_t unit_id = 0x11;

    tcp_data_unit dataUnit(transaction_id, request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<read_coils_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_discrete_inputs_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, read_coils_request_buffer) {
    read_coils_request request = read_coils_request{17, 19, 37};
    buffer_t data{0x00, 0x01, 0x00, 0x00, 0x00, 0x06,
                  0x11, 0x01, 0x00, 0x13, 0x00, 0x25};

    uint16_t transaction_id = 0x0001;
    uint8_t unit_id = 0x11;

    tcp_data_unit dataUnit(data, data.size(), message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<read_coils_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_discrete_inputs_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, read_coils_request_two_buffers) {
    read_coils_request request = read_coils_request{17, 19, 37};
    buffer_t data{0x00, 0x01, 0x00, 0x00, 0x00, 0x06,
                  0x11, 0x01, 0x00, 0x13, 0x00, 0x25};
    buffer_t header{0x00, 0x01, 0x00, 0x00, 0x00, 0x06};
    buffer_t payload{0x11, 0x01, 0x00, 0x13, 0x00, 0x25};

    uint16_t transaction_id = 0x0001;
    uint8_t unit_id = 0x11;

    tcp_data_unit dataUnit(header, payload, payload.size(),
                           message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<read_coils_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_discrete_inputs_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, read_discrete_inputs_request_struct) {
    read_discrete_inputs_request request{11, 196, 22};
    buffer_t data{0x00, 0x02, 0x00, 0x00, 0x00, 0x06,
                  0x0B, 0x02, 0x00, 0xC4, 0x00, 0x16};

    uint16_t transaction_id = 0x0002;
    uint8_t unit_id = 0x11;

    tcp_data_unit dataUnit(transaction_id, request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<read_discrete_inputs_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, read_discrete_inputs_request_buffer) {
    read_discrete_inputs_request request{11, 196, 22};
    buffer_t data{0x00, 0x02, 0x00, 0x00, 0x00, 0x06,
                  0x0B, 0x02, 0x00, 0xC4, 0x00, 0x16};

    uint16_t transaction_id = 0x0002;
    uint8_t unit_id = 0x11;

    tcp_data_unit dataUnit(data, data.size(), message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<read_discrete_inputs_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, read_discrete_inputs_request_two_buffers) {
    read_discrete_inputs_request request{11, 196, 22};
    buffer_t data{0x00, 0x02, 0x00, 0x00, 0x00, 0x06,
                  0x0B, 0x02, 0x00, 0xC4, 0x00, 0x16};
    buffer_t header{0x00, 0x02, 0x00, 0x00, 0x00, 0x06};
    buffer_t payload{0x0B, 0x02, 0x00, 0xC4, 0x00, 0x16};

    uint16_t transaction_id = 0x0002;
    uint8_t unit_id = 0x11;

    tcp_data_unit dataUnit(header, payload, payload.size(),
                           message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<read_discrete_inputs_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, read_holding_registers_request_struct) {
    read_holding_registers_request request{12, 107, 3};
    buffer_t data{0x00, 0x03, 0x00, 0x00, 0x00, 0x06,
                  0x0C, 0x03, 0x00, 0x6B, 0x00, 0x03};

    uint16_t transaction_id = 0x0003;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(transaction_id, request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<read_holding_registers_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, read_holding_registers_request_buffer) {
    read_holding_registers_request request{12, 107, 3};
    buffer_t data{0x00, 0x03, 0x00, 0x00, 0x00, 0x06,
                  0x0C, 0x03, 0x00, 0x6B, 0x00, 0x03};

    uint16_t transaction_id = 0x0003;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(data, data.size(), message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<read_holding_registers_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, read_holding_registers_request_two_buffers) {
    read_holding_registers_request request{12, 107, 3};
    buffer_t data{0x00, 0x03, 0x00, 0x00, 0x00, 0x06,
                  0x0C, 0x03, 0x00, 0x6B, 0x00, 0x03};
    buffer_t header{0x00, 0x03, 0x00, 0x00, 0x00, 0x06};
    buffer_t payload{0x0C, 0x03, 0x00, 0x6B, 0x00, 0x03};

    uint16_t transaction_id = 0x0003;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(header, payload, payload.size(),
                           message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<read_holding_registers_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, read_input_registers_request_struct) {
    read_input_registers_request request{12, 8, 1};
    buffer_t data{0x00, 0x04, 0x00, 0x00, 0x00, 0x06,
                  0x0C, 0x04, 0x00, 0x08, 0x00, 0x01};

    uint16_t transaction_id = 0x0004;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(transaction_id, request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<read_input_registers_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, read_input_registers_request_buffer) {
    read_input_registers_request request{12, 8, 1};
    buffer_t data{0x00, 0x04, 0x00, 0x00, 0x00, 0x06,
                  0x0C, 0x04, 0x00, 0x08, 0x00, 0x01};

    uint16_t transaction_id = 0x0004;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(data, data.size(), message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<read_input_registers_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, read_input_registers_request_two_buffers) {
    read_input_registers_request request{12, 8, 1};
    buffer_t data{0x00, 0x04, 0x00, 0x00, 0x00, 0x06,
                  0x0C, 0x04, 0x00, 0x08, 0x00, 0x01};
    buffer_t header{0x00, 0x04, 0x00, 0x00, 0x00, 0x06};
    buffer_t payload{0x0C, 0x04, 0x00, 0x08, 0x00, 0x01};

    uint16_t transaction_id = 0x0004;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(header, payload, payload.size(),
                           message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<read_input_registers_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, write_single_register_request_struct) {
    write_single_coil_request request{12, 172, true};
    buffer_t data{0x00, 0x05, 0x00, 0x00, 0x00, 0x06,
                  0x0C, 0x05, 0x00, 0xAC, 0xFF, 0x00};

    uint16_t transaction_id = 0x0005;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(transaction_id, request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<write_single_coil_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, write_single_coil_request_buffer) {
    write_single_coil_request request{12, 172, true};
    buffer_t data{0x00, 0x05, 0x00, 0x00, 0x00, 0x06,
                  0x0C, 0x05, 0x00, 0xAC, 0xFF, 0x00};

    uint16_t transaction_id = 0x0005;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(data, data.size(), message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<write_single_coil_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, write_single_coil_request_two_buffers) {
    write_single_coil_request request{12, 172, true};
    buffer_t data{0x00, 0x05, 0x00, 0x00, 0x00, 0x06,
                  0x0C, 0x05, 0x00, 0xAC, 0xFF, 0x00};
    buffer_t header{0x00, 0x05, 0x00, 0x00, 0x00, 0x06};
    buffer_t payload{0x0C, 0x05, 0x00, 0xAC, 0xFF, 0x00};

    uint16_t transaction_id = 0x0005;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(header, payload, payload.size(),
                           message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<write_single_coil_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, write_single_coil_request_struct) {
    write_single_register_request request{17, 1, 3};
    buffer_t data{0x00, 0x06, 0x00, 0x00, 0x00, 0x06,
                  0x11, 0x06, 0x00, 0x01, 0x00, 0x03};

    uint16_t transaction_id = 0x0006;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(transaction_id, request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<write_single_register_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, write_single_register_request_buffer) {
    write_single_register_request request{17, 1, 3};
    buffer_t data{0x00, 0x06, 0x00, 0x00, 0x00, 0x06,
                  0x11, 0x06, 0x00, 0x01, 0x00, 0x03};

    uint16_t transaction_id = 0x0006;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(data, data.size(), message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<write_single_register_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, write_single_register_request_two_buffers) {
    write_single_register_request request{17, 1, 3};
    buffer_t data{0x00, 0x06, 0x00, 0x00, 0x00, 0x06,
                  0x11, 0x06, 0x00, 0x01, 0x00, 0x03};
    buffer_t header{0x00, 0x06, 0x00, 0x00, 0x00, 0x06};
    buffer_t payload{0x11, 0x06, 0x00, 0x01, 0x00, 0x03};

    uint16_t transaction_id = 0x0006;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(header, payload, payload.size(),
                           message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<write_single_register_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, write_multiple_coil_request_struct) {
    write_multiple_coils_request request{17, 19, 10,
                                         std::vector<uint8_t>{0xCD, 0x01}};
    buffer_t data{0x00, 0x0F, 0x00, 0x00, 0x00, 0x09, 0x11, 0x0F,
                  0x00, 0x13, 0x00, 0x0A, 0x02, 0xCD, 0x01};

    uint16_t transaction_id = 0x000F;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(transaction_id, request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<write_multiple_coils_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, write_multiple_coil_request_buffer) {
    write_multiple_coils_request request{17, 19, 10,
                                         std::vector<uint8_t>{0xCD, 0x01}};
    buffer_t data{0x00, 0x0F, 0x00, 0x00, 0x00, 0x09, 0x11, 0x0F,
                  0x00, 0x13, 0x00, 0x0A, 0x02, 0xCD, 0x01};

    uint16_t transaction_id = 0x000F;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(data, data.size(), message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<write_multiple_coils_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, write_multiple_coil_request_two_buffers) {
    write_multiple_coils_request request{17, 19, 10,
                                         std::vector<uint8_t>{0xCD, 0x01}};
    buffer_t data{0x00, 0x0F, 0x00, 0x00, 0x00, 0x09, 0x11, 0x0F,
                  0x00, 0x13, 0x00, 0x0A, 0x02, 0xCD, 0x01};
    buffer_t header{0x00, 0x0F, 0x00, 0x00, 0x00, 0x09};
    buffer_t payload{0x11, 0x0F, 0x00, 0x13, 0x00, 0x0A, 0x02, 0xCD, 0x01};

    uint16_t transaction_id = 0x000F;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(header, payload, payload.size(),
                           message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<write_multiple_coils_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, write_multiple_register_request_struct) {
    write_multiple_registers_request request{17, 1, 2,
                                             std::vector<uint16_t>{10, 258}};
    buffer_t data{0x00, 0x0F, 0x00, 0x00, 0x00, 0x0B, 0x11, 0x10, 0x00,
                  0x01, 0x00, 0x02, 0x04, 0x00, 0x0A, 0x01, 0x02};

    uint16_t transaction_id = 0x000F;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(transaction_id, request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<write_multiple_registers_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, write_multiple_register_request_buffer) {
    write_multiple_registers_request request{17, 1, 2,
                                             std::vector<uint16_t>{10, 258}};
    buffer_t data{0x00, 0x0F, 0x00, 0x00, 0x00, 0x0B, 0x11, 0x10, 0x00,
                  0x01, 0x00, 0x02, 0x04, 0x00, 0x0A, 0x01, 0x02};

    uint16_t transaction_id = 0x000F;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(data, data.size(), message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<write_multiple_registers_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, write_multiple_register_request_two_buffers) {
    write_multiple_registers_request request{17, 1, 2,
                                             std::vector<uint16_t>{10, 258}};
    buffer_t data{0x00, 0x0F, 0x00, 0x00, 0x00, 0x0B, 0x11, 0x10, 0x00,
                  0x01, 0x00, 0x02, 0x04, 0x00, 0x0A, 0x01, 0x02};
    buffer_t header{0x00, 0x0F, 0x00, 0x00, 0x00, 0x0B};
    buffer_t payload{0x11, 0x10, 0x00, 0x01, 0x00, 0x02,
                     0x04, 0x00, 0x0A, 0x01, 0x02};

    uint16_t transaction_id = 0x000F;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(header, payload, payload.size(),
                           message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<write_multiple_registers_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, mask_write_register_request_struct) {
    mask_write_register_request request{17, 4, 0x00F2, 0x0025};
    buffer_t data{0x00, 0x16, 0x00, 0x00, 0x00, 0x08, 0x11,
                  0x16, 0x00, 0x04, 0x00, 0xF2, 0x00, 0x25};

    uint16_t transaction_id = 0x0016;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(transaction_id, request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<mask_write_register_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, mask_write_register_request_buffer) {
    mask_write_register_request request{17, 4, 0x00F2, 0x0025};
    buffer_t data{0x00, 0x16, 0x00, 0x00, 0x00, 0x08, 0x11,
                  0x16, 0x00, 0x04, 0x00, 0xF2, 0x00, 0x25};

    uint16_t transaction_id = 0x0016;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(data, data.size(), message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<mask_write_register_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, mask_write_register_request_two_buffers) {
    mask_write_register_request request{17, 4, 0x00F2, 0x0025};
    buffer_t data{0x00, 0x16, 0x00, 0x00, 0x00, 0x08, 0x11,
                  0x16, 0x00, 0x04, 0x00, 0xF2, 0x00, 0x25};
    buffer_t header{0x00, 0x16, 0x00, 0x00, 0x00, 0x08};
    buffer_t payload{0x11, 0x16, 0x00, 0x04, 0x00, 0xF2, 0x00, 0x25};

    uint16_t transaction_id = 0x0016;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(header, payload, payload.size(),
                           message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<mask_write_register_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, read_write_multiple_register_request_struct) {
    read_write_registers_request request{
        17, 4, 6, 15, std::vector<uint16_t>{0x00FF, 0x00FF, 0x00FF}};
    buffer_t data{0x00, 0x17, 0x00, 0x00, 0x00, 0x11, 0x11, 0x17,
                  0x00, 0x04, 0x00, 0x06, 0x00, 0x0F, 0x00, 0x03,
                  0x06, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};

    uint16_t transaction_id = 0x0017;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(transaction_id, request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<read_write_registers_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, read_write_multiple_register_request_buffer) {
    read_write_registers_request request{
        17, 4, 6, 15, std::vector<uint16_t>{0x00FF, 0x00FF, 0x00FF}};
    buffer_t data{0x00, 0x17, 0x00, 0x00, 0x00, 0x11, 0x11, 0x17,
                  0x00, 0x04, 0x00, 0x06, 0x00, 0x0F, 0x00, 0x03,
                  0x06, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};

    uint16_t transaction_id = 0x0017;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(data, data.size(), message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<read_write_registers_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, read_write_multiple_register_request_two_buffers) {
    read_write_registers_request request{
        17, 4, 6, 15, std::vector<uint16_t>{0x00FF, 0x00FF, 0x00FF}};
    buffer_t data{0x00, 0x17, 0x00, 0x00, 0x00, 0x11, 0x11, 0x17,
                  0x00, 0x04, 0x00, 0x06, 0x00, 0x0F, 0x00, 0x03,
                  0x06, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
    buffer_t header{0x00, 0x17, 0x00, 0x00, 0x00, 0x11};
    buffer_t payload{0x11, 0x17, 0x00, 0x04, 0x00, 0x06, 0x00, 0x0F, 0x00,
                     0x03, 0x06, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
    uint16_t transaction_id = 0x0017;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(header, payload, payload.size(),
                           message_type::request);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), request.size());
    EXPECT_EQ(dataUnit.unit_id(), request.unit_id);

    auto optionalRequest = dataUnit.pdu<read_write_registers_request>();
    EXPECT_TRUE(optionalRequest);
    auto request2 = optionalRequest.value();
    EXPECT_EQ(request, request2);

    auto request3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(request3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, exception_response_struct) {
    exception_response response{17, function_code_t::read_coils,
                                exception_code_t::illegal_data_address};
    buffer_t data{0x00, 0x17, 0x00, 0x00, 0x00, 0x03, 0x11, 0x81, 0x02};

    uint16_t transaction_id = 0x0017;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(transaction_id, response);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), response.size());
    EXPECT_EQ(dataUnit.unit_id(), response.unit_id);

    auto optionalResponse = dataUnit.pdu<exception_response>();
    EXPECT_TRUE(optionalResponse);
    auto response2 = optionalResponse.value();
    EXPECT_EQ(response, response2);

    auto response3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(response3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, exception_response_buffer) {
    exception_response response{17, function_code_t::read_coils,
                                exception_code_t::illegal_data_address};
    buffer_t data{0x00, 0x17, 0x00, 0x00, 0x00, 0x03, 0x11, 0x81, 0x02};

    uint16_t transaction_id = 0x0017;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(data, data.size(), message_type::response);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), response.size());
    EXPECT_EQ(dataUnit.unit_id(), response.unit_id);

    auto optionalResponse = dataUnit.pdu<exception_response>();
    EXPECT_TRUE(optionalResponse);
    auto response2 = optionalResponse.value();
    EXPECT_EQ(response, response2);

    auto response3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(response3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

TEST(tcp_data_unit, exception_response_two_buffers) {
    exception_response response{17, function_code_t::read_coils,
                                exception_code_t::illegal_data_address};
    buffer_t data{0x00, 0x17, 0x00, 0x00, 0x00, 0x03, 0x11, 0x81, 0x02};
    buffer_t header{0x00, 0x17, 0x00, 0x00, 0x00, 0x03};
    buffer_t payload{0x11, 0x81, 0x02};
    uint16_t transaction_id = 0x0017;
    uint8_t unit_id = 0x12;

    tcp_data_unit dataUnit(header, payload, payload.size(),
                           message_type::response);
    EXPECT_EQ(dataUnit.transaction_id(), transaction_id);
    EXPECT_EQ(dataUnit.message_length(), response.size());
    EXPECT_EQ(dataUnit.unit_id(), response.unit_id);

    auto optionalResponse = dataUnit.pdu<exception_response>();
    EXPECT_TRUE(optionalResponse);
    auto response2 = optionalResponse.value();
    EXPECT_EQ(response, response2);

    auto response3 = dataUnit.pdu<read_coils_request>();
    EXPECT_FALSE(response3);

    std::shared_ptr<const buffer_t> buffer = dataUnit.buffer();
    EXPECT_EQ(*buffer, data);
}

} // namespace