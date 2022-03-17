#include "modbus/server/server_helper.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {

using namespace modbus;
using namespace std;

TEST(reg_ops, copy_registers_from_memory) {
    vector<uint8_t> data, expected;
    buffer_t memorySpace = {0x0F, 0xF0, 0x0F, 0xF0, 0x00, 0x00, 0x0F, 0xF0};

    data = copy_data_registers(memorySpace, 0, 2);
    expected = {0x0F, 0xF0, 0x0F, 0xF0};
    EXPECT_EQ(data, expected);

    data = copy_data_registers(memorySpace, 3, 1);
    expected = {0x0F, 0xF0};
    EXPECT_EQ(data, expected);
}

TEST(reg_ops, write_single_register_to_memory) {
    vector<uint8_t> expected;
    buffer_t memorySpace = {0x0F, 0xF0, 0x0F, 0xF0, 0x00, 0x00, 0x0F, 0xF0};

    write_register(0xAAAA, memorySpace, 0);
    expected = {0xAA, 0xAA, 0x0F, 0xF0, 0x00, 0x00, 0x0F, 0xF0};
    EXPECT_EQ(memorySpace, expected);

    write_register(0x9999, memorySpace, 3);
    expected = {0xAA, 0xAA, 0x0F, 0xF0, 0x00, 0x00, 0x99, 0x99};
    EXPECT_EQ(memorySpace, expected);
}

TEST(reg_ops, write_multiple_register_to_memory) {
    buffer_t expected;
    vector<uint16_t> data{0xFF00, 0x00FF};
    buffer_t memorySpace = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    write_registers(data, memorySpace, 0, 2);
    expected = {0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(memorySpace, expected);

    write_registers(data, memorySpace, 2, 2);
    expected = {0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF};
    EXPECT_EQ(memorySpace, expected);
}

} // namespace