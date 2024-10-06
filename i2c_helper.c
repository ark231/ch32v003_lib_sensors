#include "i2c_helper.h"

i2c_err_t i2c_write_byte(const uint8_t addr, const uint8_t reg, const uint8_t value) {
    const uint8_t buf[] = {value};
    return i2c_write(addr, reg, buf, 1);
}

uint16_t read_16_from_bytes(const uint8_t *buf, Endiannness endian) {
    uint16_t result = 0;
    result |= ((uint16_t)buf[0]) << (endian == ENDIAN_LITTLE ? 0 : 8);
    result |= ((uint16_t)buf[1]) << (endian == ENDIAN_LITTLE ? 8 : 0);
    return result;
}
