#include "BME280.h"

#include <math.h>
#include <stdint.h>

#include "i2c_helper.h"

#define BME280_REG_CTRL_HUM  0xf2
#define BME280_REG_CTRL_MEAS 0xf4
#define BME280_REG_CONFIG    0xf5
#define BME280_REG_PRESS_MSB 0xf7
#define BME280_REG_CALIB00   0x88
#define BME280_REG_CALIB25   0xa1
#define BME280_REG_CALIB26   0xe1

void bme280_begin(BME280* self, TemperatureOversampling osrs_t, PressureOversampling osrs_p,
                  HumidityOversampling osrs_h, Mode mode, Tstandby t_sb, Filter filter, SPI3WireEnable spi3w_en) {
    uint8_t ctrl_meas_reg = (osrs_t << 5) | (osrs_p << 2) | mode;    // 温度、圧力オーバーサンプリング
    uint8_t config_reg    = (t_sb << 5) | (filter << 2) | spi3w_en;  // スタンバイ時間とIIRフィルター設定
    uint8_t ctrl_hum_reg  = osrs_h;                                  // 湿度オーバーサンプリング

    i2c_write_byte(self->BME280_ADDRESS_, BME280_REG_CTRL_HUM, ctrl_hum_reg);
    i2c_write_byte(self->BME280_ADDRESS_, BME280_REG_CTRL_MEAS, ctrl_meas_reg);
    i2c_write_byte(self->BME280_ADDRESS_, BME280_REG_CONFIG, config_reg);

    bme280_read_trim(self);
}

void bme280_read_trim(BME280* self) {
    uint8_t inBuf[32];
    i2c_read(self->BME280_ADDRESS_, BME280_REG_CALIB00, inBuf, 24);
    i2c_read(self->BME280_ADDRESS_, BME280_REG_CALIB25, inBuf + 24, 1);
    i2c_read(self->BME280_ADDRESS_, BME280_REG_CALIB26, inBuf + 24 + 1, 7);

    self->dig_T1_ = (inBuf[1] << 8) | inBuf[0];
    self->dig_T2_ = (inBuf[3] << 8) | inBuf[2];
    self->dig_T3_ = (inBuf[5] << 8) | inBuf[4];
    self->dig_P1_ = (inBuf[7] << 8) | inBuf[6];
    self->dig_P2_ = (inBuf[9] << 8) | inBuf[8];
    self->dig_P3_ = (inBuf[11] << 8) | inBuf[10];
    self->dig_P4_ = (inBuf[13] << 8) | inBuf[12];
    self->dig_P5_ = (inBuf[15] << 8) | inBuf[14];
    self->dig_P6_ = (inBuf[17] << 8) | inBuf[16];
    self->dig_P7_ = (inBuf[19] << 8) | inBuf[18];
    self->dig_P8_ = (inBuf[21] << 8) | inBuf[20];
    self->dig_P9_ = (inBuf[23] << 8) | inBuf[22];
    self->dig_H1_ = inBuf[24];
    self->dig_H2_ = (inBuf[26] << 8) | inBuf[25];
    self->dig_H3_ = inBuf[27];
    self->dig_H4_ = (inBuf[28] << 4) | (0x0F & inBuf[29]);
    self->dig_H5_ = (inBuf[30] << 4) | ((inBuf[29] >> 4) & 0x0F);
    self->dig_H6_ = inBuf[31];
}

float bme280_read_pressure(BME280* self, uint8_t* raw_data) {
    uint32_t pres_raw = (raw_data[0] << 12) | (raw_data[1] << 4) | (raw_data[2] >> 4);
    float result =
        bme280_compute_pres64(self, pres_raw) / 256.0;  // pa シフトしてしまうと小数点以下が失われるので、割り算する
    result *= 0.01;                                     // hpa
    return result;
}

float bme280_read_temperature(BME280* self, uint8_t* raw_data) {
    uint32_t temp_raw = (raw_data[0] << 12) | (raw_data[1] << 4) | (raw_data[2] >> 4);
    return bme280_compute_temp64(self, temp_raw) / 100.0;  // °C
}

float bme280_read_humidity(BME280* self, uint8_t* raw_data) {
    uint32_t hum_raw = (raw_data[0] << 8 | raw_data[1]);
    return bme280_compute_hum64(self, hum_raw) / 1024.0;
}

uint32_t bme280_compute_pres64(BME280* self, int32_t adc_p) {
    int64_t var1, var2, p;
    var1 = ((int64_t)self->t_fine_) - 128000;
    var2 = var1 * var1 * (int64_t)self->dig_P6_;
    var2 = var2 + ((var1 * (int64_t)self->dig_P5_) << 17);
    var2 = var2 + (((int64_t)self->dig_P4_) << 35);
    var1 = ((var1 * var1 * (int64_t)self->dig_P3_) >> 8) + ((var1 * (int64_t)self->dig_P2_) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)self->dig_P1_) >> 33;
    if (var1 == 0) {
        return 0;  // ゼロ除算による例外を避ける。
    }
    p    = 1048576 - adc_p;
    p    = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)self->dig_P9_) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)self->dig_P8_) * p) >> 19;
    p    = ((p + var1 + var2) >> 8) + (((int64_t)self->dig_P7_) << 4);
    return (uint32_t)p;
}

uint32_t bme280_compute_temp64(BME280* self, int32_t adc_t) {
    int32_t var1, var2, t;
    var1 = ((((adc_t >> 3) - ((int32_t)self->dig_T1_ << 1))) * ((int32_t)self->dig_T2_)) >> 11;
    var2 = (((((adc_t >> 4) - ((int32_t)self->dig_T1_)) * ((adc_t >> 4) - ((int32_t)self->dig_T1_))) >> 12) *
            ((int32_t)self->dig_T3_)) >>
           14;
    self->t_fine_ = var1 + var2;
    t             = (self->t_fine_ * 5 + 128) >> 8;
    return t;
}

uint32_t bme280_compute_hum64(BME280* self, int32_t adc_h) {
    int32_t v_x1_u32r;
    v_x1_u32r = (self->t_fine_ - ((int32_t)76800));
    v_x1_u32r = (((((adc_h << 14) - (((int32_t)self->dig_H4_) << 20) - (((int32_t)self->dig_H5_) * v_x1_u32r)) +
                   ((int32_t)16384)) >>
                  15) *
                 (((((((v_x1_u32r * ((int32_t)self->dig_H6_)) >> 10) *
                      (((v_x1_u32r * ((int32_t)self->dig_H3_)) >> 11) + ((int32_t)32768))) >>
                     10) +
                    ((int32_t)2097152)) *
                       ((int32_t)self->dig_H2_) +
                   8192) >>
                  14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)self->dig_H1_)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    return (uint32_t)(v_x1_u32r >> 12);
}

BME280_DataTypeF bme280_readf(BME280* self) {
    BME280_DataTypeF result;
    uint8_t data[8];
    i2c_read(self->BME280_ADDRESS_, BME280_REG_PRESS_MSB, data, 8);
    // 気圧データの補正には気温のデータ(t_fine_)が必要なので、先に気温のデータを補正する
    result.temperature = bme280_read_temperature(self, data + 3);
    result.pressure    = bme280_read_pressure(self, data);
    result.humidity    = bme280_read_humidity(self, data + 6);
    return result;
}
