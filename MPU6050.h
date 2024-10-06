#ifndef CH32V003_LIB_SENSORS_SENSORS_MPU6050
#define CH32V003_LIB_SENSORS_SENSORS_MPU6050

#include "GeneralDefs.h"
#include "Vec3.h"

typedef enum {
    MPU6050_PM2G  = 0,  // ±2g
    MPU6050_PM4G  = 1,
    MPU6050_PM8G  = 2,
    MPU6050_PM16G = 3,
} MPU6050_AccelRange;

typedef enum {
    MPU6050_PM2000DPS = 3,  // ±2000°/s
    MPU6050_PM1000DPS = 2,
    MPU6050_PM500DPS  = 1,
    MPU6050_PM250DPS  = 0,
} MPU6050_GyroRange;

typedef struct {
    MPU6050_AccelRange accel_range;
    MPU6050_GyroRange gyro_range;
    uint8_t addr;
} MPU6050;

typedef struct {
    Vec3f accel;
    Vec3f gyro;
} MPU6050_DataF;

typedef struct {
    Vec3i16 accel;
    Vec3i16 gyro;
} MPU6050_DataI16;

typedef enum {
    MPU6050_I2C_AD0_LOW  = 0b1101000,
    MPU6050_I2C_AD0_HIGH = 0b1101001,
} MPU6050_I2C_Addr;

sensor_error_t mpu6050_init(MPU6050* self, const MPU6050_AccelRange accel_range, const MPU6050_GyroRange gyro_range,
                            const MPU6050_I2C_Addr i2c_addr);
MPU6050_DataF mpu6050_readf(MPU6050* self);
MPU6050_DataI16 mpu6050_readi16(MPU6050* self);

MPU6050_DataF mpu6050_compensate(MPU6050* self, MPU6050_DataI16 data_i16);

Vec3f mpu6050_compensate_accel(MPU6050* self, Vec3i16 data_i16);
Vec3f mpu6050_compensate_gyro(MPU6050* self, Vec3i16 data_i16);
#endif
