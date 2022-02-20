#include "server/server_helper.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {

using namespace modbus;
using namespace std;

TEST(legal_address, create_bitmap) {
    vector<uint8_t> expected;

    expected = vector<uint8_t>{0x01};
    EXPECT_EQ(create_request_bit_map(0, 1), expected);

    expected = vector<uint8_t>{0x06};
    EXPECT_EQ(create_request_bit_map(1, 2), expected);

    expected = vector<uint8_t>{0x02};
    EXPECT_EQ(create_request_bit_map(1, 1), expected);

    expected = vector<uint8_t>{0xFE, 0x01};
    EXPECT_EQ(create_request_bit_map(1, 8), expected);

    expected = vector<uint8_t>{0xFE, 0x03};
    EXPECT_EQ(create_request_bit_map(1, 9), expected);

    expected = vector<uint8_t>{0xFE, 0xFF, 0x03};
    EXPECT_EQ(create_request_bit_map(1, 17), expected);

    expected = vector<uint8_t>{0x0C};
    EXPECT_EQ(create_request_bit_map(10, 2), expected);

    expected = vector<uint8_t>{0xFC, 0xFF, 0x07};
    EXPECT_EQ(create_request_bit_map(10, 17), expected);
}

TEST(legal_address, test_bitmap) {
    vector<uint8_t> bitmask;

    bitmask = {0xF9, 0xFF, 0xFF, 0xFF};
    EXPECT_FALSE(legal_address(bitmask, create_request_bit_map(1, 2), 1));

    bitmask = {0x06, 0x00, 0x00, 0x00};
    EXPECT_TRUE(legal_address(bitmask, create_request_bit_map(1, 2), 1));

    bitmask = {0x02, 0x00, 0x00, 0x00};
    EXPECT_TRUE(legal_address(bitmask, create_request_bit_map(1, 1), 1));

    bitmask = {0x01, 0xFE, 0xFF, 0xFF};
    EXPECT_FALSE(legal_address(bitmask, create_request_bit_map(1, 8), 1));
    bitmask = {0xFE, 0x01, 0x00, 0x00};
    EXPECT_TRUE(legal_address(bitmask, create_request_bit_map(1, 8), 1));

    bitmask = {0x01, 0xFC, 0xFF, 0xFF};
    EXPECT_FALSE(legal_address(bitmask, create_request_bit_map(1, 9), 1));
    bitmask = {0xFE, 0x03, 0x06, 0x00};
    EXPECT_TRUE(legal_address(bitmask, create_request_bit_map(1, 9), 1));

    bitmask = {0x01, 0x00, 0xFC, 0xFF};
    EXPECT_FALSE(legal_address(bitmask, create_request_bit_map(1, 17), 1));
    bitmask = {0xFE, 0xFF, 0x03, 0x00};
    EXPECT_TRUE(legal_address(bitmask, create_request_bit_map(1, 17), 1));

    bitmask = {0xFF, 0xF3, 0xFF, 0xFF};
    EXPECT_FALSE(legal_address(bitmask, create_request_bit_map(10, 2), 10));
    bitmask = {0x00, 0x0C, 0x00, 0x00};
    EXPECT_TRUE(legal_address(bitmask, create_request_bit_map(10, 2), 10));

    bitmask = {0xFF, 0xF9, 0x00, 0x06};
    EXPECT_FALSE(legal_address(bitmask, create_request_bit_map(10, 17), 10));
    bitmask = {0x00, 0xFC, 0xFF, 0x07};
    EXPECT_TRUE(legal_address(bitmask, create_request_bit_map(10, 17), 10));
}

} // namespace
