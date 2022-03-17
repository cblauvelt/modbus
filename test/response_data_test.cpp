#include "modbus/core/modbus_response.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {

using namespace modbus;
using namespace std;

TEST(modbus_response, default_constructor) {
    modbus_response_t resp;
    EXPECT_FALSE(resp.isValid());
    EXPECT_EQ(resp.buffer_length(), 0);
    EXPECT_EQ(resp.data_model(), data_model_t::invalid_data_type);
}

TEST(modbus_response, known_values) {
    shared_ptr<buffer_t> byteArray =
        make_shared<buffer_t>(buffer_t{0xCD, 0x6B, 0xB2, 0x0E, 0x1B});
    uint16_t start_address = 2;
    modbus_response_t resp(data_model_t::coil, byteArray, start_address,
                           (uint16_t)byteArray->size());
    auto retData = resp.data();
    uint8_t testByte = 0;
    int8_t testSignedByte = 0;
    uint16_t testWord = 0;
    int16_t testSignedWord = 0;
    uint32_t testDWord = 0;
    int32_t testSignedDWord = 0;

    EXPECT_TRUE(resp.isValid());
    EXPECT_EQ(resp.buffer_length(), 5);
    EXPECT_EQ(retData->size(), 5);
    EXPECT_EQ(resp.data_model(), data_model_t::coil);

    // boolean checks
    EXPECT_TRUE(resp.getBool(0));
    EXPECT_FALSE(resp.getBool(4));
    EXPECT_TRUE(resp.getBool(11));
    EXPECT_FALSE(resp.getBool(16));

    // 8-bit checks
    EXPECT_EQ(resp.getUINT8(1), 0x6B);
    EXPECT_EQ(resp.getINT8(2), -78);

    // 16-bit checks
    EXPECT_EQ(resp.getUINT16(0), 0xCD6B);
    EXPECT_EQ(resp.getUINT16(1, byte_order::byte_swapped), 0x0EB2);
    EXPECT_EQ(resp.getINT16(0, byte_order::byte_swapped), 27597);
    EXPECT_EQ(resp.getINT16(1), -19954);

    // 32-bit checks
    EXPECT_EQ(resp.getUINT32(0), 3446387214);
    EXPECT_EQ(resp.getUINT32(0, byte_order::byte_swapped), 246574029);
    EXPECT_EQ(resp.getINT32(0), -848580082);
    EXPECT_EQ(resp.getINT32(0, byte_order::byte_swapped), 246574029);

    // start address checks
    EXPECT_EQ(start_address, resp.start_address());
}

} // namespace