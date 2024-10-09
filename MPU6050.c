#include "MPU6050.h"

#include <math.h>

#include "Vec3.h"
#include "i2c_helper.h"

#define REG_CONFIG       0x1a
#define REG_GYRO_CONFIG  0x1b
#define REG_ACCEL_CONFIG 0x1c
#define REG_ACCEL_XOUT_H 0x3b
#define REG_GYRO_XOUT_H  0x43
#define REG_PWR_MGMT_1   0x6b

// NOLINTBEGIN(readability-identifier-naming) : private member function
Vec3i16 mpu6050_read_accel_i16_(MPU6050 *self, uint8_t *buf);
Vec3i16 mpu6050_read_gyro_i16_(MPU6050 *self, uint8_t *buf);
// NOLINTEND(readability-identifier-naming) : private member function
// NOLINTNEXTLINE(readability-identifier-naming) : this function returns constant values, like dynamic #define
float mpu6050_LSB_PER_DPS(MPU6050_GyroRange gyro_range) {
    // ±2000°/s→16.4LSB/°/s, ±1000°/s→32.8LSB/°/s, ±500°/s→65.5LSB/°/s,
    // ±250°/s→131LSB/°/s
    switch (gyro_range) {
        case MPU6050_PM2000DPS:
            return 16.4;
        case MPU6050_PM1000DPS:
            return 32.8;
        case MPU6050_PM500DPS:
            return 65.5;
        case MPU6050_PM250DPS:
            return 131;
        default:
            return nan("");
    }
}
// NOLINTNEXTLINE(readability-identifier-naming) :  this function returns constant values, like dynamic #define
int mpu6050_LSB_PER_G(MPU6050_AccelRange accel_range) {
    // ±2g→16384LSB/g, ±4g→8192LSB/g, ±8g→4096LSB/g, ±16g→2048LSB/g
    switch (accel_range) {
        case MPU6050_PM2G:
            return 16384;
        case MPU6050_PM4G:
            return 8192;
        case MPU6050_PM8G:
            return 4096;
        case MPU6050_PM16G:
            return 2048;
        default:
            return 0;
    }
}

sensor_error_t mpu6050_init(MPU6050 *self, const MPU6050_AccelRange accel_range, const MPU6050_GyroRange gyro_range,
                            const MPU6050_I2C_Addr i2c_addr) {
    self->accel_range = accel_range;
    self->gyro_range  = gyro_range;
    self->addr        = i2c_addr;
    i2c_err_t res     = I2C_OK;
    res += i2c_write_byte(self->addr, REG_GYRO_CONFIG, self->gyro_range << 3);
    res += i2c_write_byte(self->addr, REG_ACCEL_CONFIG, self->accel_range << 3);
    res += i2c_write_byte(self->addr, REG_CONFIG, 0x00);      // FSYNC: disabled, DLPF: disabled
    res += i2c_write_byte(self->addr, REG_PWR_MGMT_1, 0x00);  // SLEEP: disabled <- デフォルトではenabled
    return res == I2C_OK ? SENSOR_OK : SENSOR_I2C_ERROR;
}

MPU6050_DataF mpu6050_readf(MPU6050 *self) { return mpu6050_compensate(self, mpu6050_readi16(self)); }

MPU6050_DataI16 mpu6050_readi16(MPU6050 *self) {
    MPU6050_DataI16 result;
    uint8_t buf[2 * (3 + 1 + 3)];  // accel,temp,gyro
    i2c_read(self->addr, REG_ACCEL_XOUT_H, buf, sizeof(buf));
    result.accel = mpu6050_read_accel_i16_(self, buf);
    result.gyro  = mpu6050_read_gyro_i16_(self, buf + (2 * (3 + 1)));
    return result;
}

Vec3i16 mpu6050_read_accel_i16_(MPU6050 *self, uint8_t *buf) {
    int16_t ax16, ay16, az16;
    ax16 = read_16_from_bytes(buf, ENDIAN_BIG);
    ay16 = read_16_from_bytes(buf + 2, ENDIAN_BIG);
    az16 = read_16_from_bytes(buf + 4, ENDIAN_BIG);

    Vec3i16 result = {.x = ax16, .y = ay16, .z = az16};
    return result;
}
Vec3i16 mpu6050_read_gyro_i16_(MPU6050 *self, uint8_t *buf) {
    int16_t gx16, gy16, gz16;
    gx16           = read_16_from_bytes(buf, ENDIAN_BIG);
    gy16           = read_16_from_bytes(buf + 2, ENDIAN_BIG);
    gz16           = read_16_from_bytes(buf + 4, ENDIAN_BIG);
    Vec3i16 result = {.x = gx16, .y = gy16, .z = gz16};
    return result;
}

MPU6050_DataF mpu6050_compensate(MPU6050 *self, MPU6050_DataI16 data_i16) {
    MPU6050_DataF result;
    result.accel = mpu6050_compensate_accel(self, data_i16.accel);
    result.gyro  = mpu6050_compensate_gyro(self, data_i16.gyro);
    return result;
}

Vec3f mpu6050_compensate_accel(MPU6050 *self, Vec3i16 data_i16) {
    return vec3f_div(cast_vec3i16_to_vec3f(data_i16), mpu6050_LSB_PER_G(self->accel_range));
}

Vec3f mpu6050_compensate_gyro(MPU6050 *self, Vec3i16 data_i16) {
    return vec3f_div(cast_vec3i16_to_vec3f(data_i16), mpu6050_LSB_PER_DPS(self->gyro_range));
}
