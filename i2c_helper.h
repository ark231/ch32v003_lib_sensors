#ifndef CH32V003_LIB_SENSORS_SENSORS_I2C_HELPER
#define CH32V003_LIB_SENSORS_SENSORS_I2C_HELPER
#include <lib_i2c.h>
#include <stdint.h>
typedef enum {
    ENDIAN_LITTLE,
    ENDIAN_BIG,
} Endiannness;
i2c_err_t i2c_write_byte(const uint8_t addr, const uint8_t reg, const uint8_t value);
uint16_t read_16_from_bytes(const uint8_t *buf, Endiannness endian);
#endif
