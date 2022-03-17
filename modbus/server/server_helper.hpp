#include "modbus/core/types.hpp"

namespace modbus {

using namespace std;

/**
 * @brief A helper function to easily create a bitmap that annotates which
 * registers are allowed to be queried.
 * @param startAddress The starting address of the coil or register where access
 * is allowed.
 * @param numRegisters The number of registers or coils after startAddress where
 * access is allowed.
 * @return std::buffer_t The bitmap to determine access to
 * registers/coils.
 */
inline buffer_t create_request_bit_map(int startAddress, int numRegisters) {
    // Used to form the bitmask
    const uint8_t bitMasks[]{0b00000000, 0b00000001, 0b00000011,
                             0b00000111, 0b00001111, 0b00011111,
                             0b00111111, 0b01111111, 0b11111111};

    buffer_t requestBitmap;

    // If no registers are requested, return an empty bitmap
    if (numRegisters == 0) {
        return requestBitmap;
    }

    int startByte = startAddress / 8;
    int startBit = startAddress % 8;
    int numBytes = ((numRegisters + startBit) / 8) + 1;
    int numHighBits =
        (numBytes == 1) ? numRegisters : (numRegisters % 8) + startBit;

    // Increase the size to hold the request
    requestBitmap.resize(numBytes);

    // Fill out the bitmap
    for (int i = 0; i < numBytes - 1; i++) {
        requestBitmap[i] = 0xFF;
    }
    requestBitmap[numBytes - 1] = bitMasks[numHighBits];
    requestBitmap[0] <<= startBit;

    return requestBitmap;
}

/**
 * @brief Uses an access bit map to determine whether access to a register or
 * coil is allowed.
 * @param accessBitMask The access bitmask for the server.
 * @param requestBitmap The bitmask which represents the requested
 * registers/coils
 * @param startAddress The start address of the request.
 */
inline bool legal_address(const buffer_t& accessBitMask,
                          const buffer_t& requestBitmap, int startAddress) {
    if (requestBitmap.size() == 0) {
        return false;
    }

    int startByte = startAddress / 8;
    int numBytes = requestBitmap.size();

    // Bounds checking
    if (startByte + numBytes > accessBitMask.size()) {
        return false;
    }

    // Compare against the access bitmap
    for (int i = 0; i < requestBitmap.size(); i++) {
        if ((requestBitmap[i] & accessBitMask[i + startByte]) !=
            requestBitmap[i]) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Copies the requested bit values from one array to another.
 * @param from The memory space that is being copied.
 * @param requestBitmap The request bitmap.
 * @param startAddress The starting address of the coil.
 * @param numRegisters The number of registers to copy starting at startAddress.
 * @return std::buffer_t The coils that were requested.
 */
inline buffer_t copy_data_bits(const buffer_t& from,
                               const buffer_t& requestBitmap, int startAddress,
                               int numRegisters) {

    // Turn bit numbers into byte numbers
    int startByte = startAddress / 8;
    int startBit = startAddress % 8;
    int numBytes = requestBitmap.size();
    int numHighBits = numRegisters % 8;
    buffer_t to(numBytes);

    // Copy Data
    for (int i = 0; i < numBytes; i++) {
        to[i] = from[i + startByte] & requestBitmap[i];
    }

    // Format data
    for (int i = 0; i < numBytes - 1; i++) {
        to[i] = (to[i] >> startBit) | (to[i + 1] << (8 - startBit));
    }
    to[numBytes - 1] >>= startBit;

    // The number of bytes returned should be equal to the requested bytes,
    // not where they lie in the data map. This can be different when the
    // offset means the data lies on two separate bytes. This can only
    // vary by 1 byte.
    int numDataBytes = ((numRegisters - 1) / 8) + 1;
    if (numDataBytes != to.size()) {
        to.pop_back();
    }

    return to;
}

/**
 * @brief Sets a value of a bit in memory.
 * @param data The data that represents a bit in memory.
 * @param coilStatus A value representing On or Off.
 * @param coilAddress The address of the bit to write.
 */
inline void write_coil(buffer_t& data, coil_status_t coilStatus,
                       int coilAddress) {
    int startByte = coilAddress / 8;
    int startBit = coilAddress % 8;
    uint8_t byte = 1 << (startBit);

    if (coilStatus == coil_status_t::on) {
        data[startByte] |= byte; // set bit
    } else {
        data[startByte] &= ~byte; // unset bit
    }
}

/**
 * @brief Writes several bits in memory
 * @param data The data that represents a bit in memory.
 * @param requestBitmap The request bitmap.
 * @param coilStatuses The values of the bits to set in memory.
 * @param startAddress Which bit in data to start writing bits.
 */
inline void write_coils(buffer_t& data, const buffer_t& requestBitmap,
                        buffer_t coilStatuses, int startAddress) {
    int startByte = startAddress / 8;
    int startBit = startAddress % 8;
    int numBytes = requestBitmap.size();

    // Format data
    for (int i = 1; i < numBytes; i++) {
        coilStatuses[i] = (coilStatuses[i] << startBit) |
                          (coilStatuses[i - 1] >> (8 - startBit));
    }
    coilStatuses[0] <<= startBit;

    for (int i = 0; i < numBytes; i++) {
        data[i + startByte] &=
            coilStatuses[i] | ~requestBitmap[i]; // Clear unset bits
        data[i + startByte] |=
            coilStatuses[i] & requestBitmap[i]; // Set the set bits
    }
}

/**
 * @return The requested data registers.
 * @param from The memory space holding the registers.
 * @param startAddress The register to start copying.
 * @param numRegisters The number of registers to copy.
 */
inline buffer_t copy_data_registers(const buffer_t& from, int startAddress,
                                    int numRegisters) {

    // Turn register numbers into byte numbers
    int startByte = startAddress * 2;
    int numBytes = numRegisters * 2;

    buffer_t to(numBytes);

    // Copy Data
    for (int i = 0; i < numBytes; i++) {
        to[i] = from[i + startByte];
    }

    return to;
}

/**
 * @brief Sets a register value.
 * @param value The value to set the register to.
 * @param to The memory space where the new register value will be stored.
 * @param startAddress The address of the register to write
 */
inline void write_register(uint16_t value, buffer_t& to, int startAddress) {

    // Turn register numbers into byte numbers
    int startByte = startAddress * 2;

    // Copy Data
    to[startByte] = (uint8_t)(value >> 8);
    to[startByte + 1] = (uint8_t)(value & 0xFF);
}

/**
 * @brief Writes multiple registers to a memory space.
 * @param from The register values to write.
 * @param to The memory space in which the register values will be stored.
 * @param numRegisters The number of registers to copy.
 */
inline void write_registers(const vector<uint16_t>& from, buffer_t& to,
                            int startAddress, int numRegisters) {
    // Turn register numbers into byte numbers
    int startByte = startAddress * 2;

    // Copy Data
    for (int i = 0; i < numRegisters; i++) {
        to[2 * i + startByte] = (uint8_t)(from[i] >> 8);
        to[2 * i + startByte + 1] = (uint8_t)(from[i] & 0x00FF);
    }
}

} // namespace modbus
