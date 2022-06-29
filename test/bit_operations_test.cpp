#include "modbus/server/server_helper.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {

using namespace modbus;
using namespace std;

TEST(legal_address, copy_bits) {
    vector<uint8_t> requestBitmap, data, expected;
    vector<uint8_t> memorySpace = {0xA9, 0x9A, 0x06, 0x00};

    requestBitmap = create_request_bit_map(2, 2);
    data = copy_data_bits(memorySpace, requestBitmap, 2, 2);
    expected = {0x02};
    EXPECT_EQ(data, expected);

    requestBitmap = create_request_bit_map(0, 1);
    data = copy_data_bits(memorySpace, requestBitmap, 0, 1);
    expected = {0x01};
    EXPECT_EQ(data, expected);

    requestBitmap = create_request_bit_map(1, 8);
    data = copy_data_bits(memorySpace, requestBitmap, 1, 8);
    expected = {0x54};
    EXPECT_EQ(data, expected);

    requestBitmap = create_request_bit_map(1, 9);
    data = copy_data_bits(memorySpace, requestBitmap, 1, 9);
    expected = {0x54, 0x01};
    EXPECT_EQ(data, expected);

    requestBitmap = create_request_bit_map(1, 17);
    data = copy_data_bits(memorySpace, requestBitmap, 1, 17);
    expected = {0x54, 0x4D, 0x01};
    EXPECT_EQ(data, expected);

    requestBitmap = create_request_bit_map(10, 2);
    data = copy_data_bits(memorySpace, requestBitmap, 10, 2);
    expected = {0x02};
    EXPECT_EQ(data, expected);

    requestBitmap = create_request_bit_map(10, 17);
    data = copy_data_bits(memorySpace, requestBitmap, 10, 17);
    expected = {0xA6, 0x01, 0x00};
    EXPECT_EQ(data, expected);
}

TEST(legal_address, write_single_coil) {
    vector<uint8_t> coils, expected;

    coils = {0x00, 0x00, 0x00};
    write_coil(coils, true, 0);
    expected = {0x01, 0x00, 0x00};
    EXPECT_EQ(coils, expected);

    write_coil(coils, true, 2);
    expected = {0x05, 0x00, 0x00};
    EXPECT_EQ(coils, expected);

    write_coil(coils, true, 8);
    expected = {0x05, 0x01, 0x00};
    EXPECT_EQ(coils, expected);

    write_coil(coils, true, 10);
    expected = {0x05, 0x05, 0x00};
    EXPECT_EQ(coils, expected);

    write_coil(coils, true, 17);
    expected = {0x05, 0x05, 0x02};
    EXPECT_EQ(coils, expected);

    write_coil(coils, true, 19);
    expected = {0x05, 0x05, 0x0A};
    EXPECT_EQ(coils, expected);

    write_coil(coils, false, 19);
    expected = {0x05, 0x05, 0x02};
    EXPECT_EQ(coils, expected);

    write_coil(coils, false, 17);
    expected = {0x05, 0x05, 0x00};
    EXPECT_EQ(coils, expected);

    write_coil(coils, false, 10);
    expected = {0x05, 0x01, 0x00};
    EXPECT_EQ(coils, expected);

    write_coil(coils, false, 8);
    expected = {0x05, 0x00, 0x00};
    EXPECT_EQ(coils, expected);

    write_coil(coils, false, 2);
    expected = {0x01, 0x00, 0x00};
    EXPECT_EQ(coils, expected);

    write_coil(coils, false, 0);
    expected = {0x00, 0x00, 0x00};
    EXPECT_EQ(coils, expected);
}

TEST(legal_address, write_multi_coil) {
    vector<uint8_t> coils, expected, requestBitMap;
    vector<uint8_t> memorySpace{0x9A, 0xA9, 0x01};

    // Test setting 1 bit
    coils = {0x01};
    requestBitMap = create_request_bit_map(2, 2);
    write_coils(memorySpace, requestBitMap, coils, 2);
    expected = {0x96, 0xA9, 0x01};
    EXPECT_EQ(memorySpace, expected);

    // Test settings and unseting multiple bits
    coils = {0x09};
    memorySpace = {0x9A, 0xA9, 0x01};
    requestBitMap = create_request_bit_map(2, 4);
    write_coils(memorySpace, requestBitMap, coils, 2);
    expected = {0xA6, 0xA9, 0x01};
    EXPECT_EQ(memorySpace, expected);

    // Test setting and unsetting bits across a byte alignment
    coils = {0xA9, 0x01};
    memorySpace = {0x9A, 0xA9, 0x01};
    requestBitMap = create_request_bit_map(2, 9);
    write_coils(memorySpace, requestBitMap, coils, 2);
    expected = {0xA6, 0xAE, 0x01};
    EXPECT_EQ(memorySpace, expected);

    // Test settings and unsetting bits across a byte alignment
    // starting not on the first byte
    coils = {0xA9, 0x01};
    memorySpace = {0x9A, 0xA9, 0x01};
    requestBitMap = create_request_bit_map(10, 9);
    write_coils(memorySpace, requestBitMap, coils, 10);
    expected = {0x9A, 0xA5, 0x06};
    EXPECT_EQ(memorySpace, expected);
}

} // namespace
