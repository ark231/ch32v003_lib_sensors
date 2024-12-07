#ifndef CH32V003_LIB_SENSORS_BME280
#define CH32V003_LIB_SENSORS_BME280

#include <stdint.h>

#include "EnvironmentSensorData.h"
typedef enum {
    BME280_TO_SKIP = 0b000,
    BME280_TO_TM1  = 0b001,
    BME280_TO_TM2  = 0b010,  // Temperature oversampling x 2
    BME280_TO_TM4  = 0b011,
    BME280_TO_TM8  = 0b100,
    BME280_TO_TM16 = 0b101,
} TemperatureOversampling;
typedef enum {
    BME280_PO_SKIP = 0b000,
    BME280_PO_TM1  = 0b001,
    BME280_PO_TM2  = 0b010,
    BME280_PO_TM4  = 0b011,
    BME280_PO_TM8  = 0b100,
    BME280_PO_TM16 = 0b101,  // Pressure oversampling    x 16
} PressureOversampling;
typedef enum {
    BME280_HO_SKIP = 0b000,
    BME280_HO_TM1  = 0b001,  // Humidity oversampling    x 1
    BME280_HO_TM2  = 0b010,
    BME280_HO_TM4  = 0b011,
    BME280_HO_TM8  = 0b100,
    BME280_HO_TM16 = 0b101,
} HumidityOversampling;
typedef enum {
    BME280_MODE_SLEEP  = 0b00,
    BME280_MODE_FORCED = 0b01,  // 0b10も同じ
    BME280_MODE_NORMAL = 0b11,  // Normal mode
} Mode;
typedef enum {
    BME280_TSTDBY_MS0_5  = 0b000,  // Tstandby 0.5ms
    BME280_TSTDBY_MS62_5 = 0b001,
    BME280_TSTDBY_MS125  = 0b010,
    BME280_TSTDBY_MS250  = 0b011,
    BME280_TSTDBY_MS500  = 0b100,
    BME280_TSTDBY_MS1000 = 0b101,
    BME280_TSTDBY_MS10   = 0b110,
    BME280_TSTDBY_MS20   = 0b111,
} Tstandby;
typedef enum {
    BME280_FILT_OFF   = 0b000,
    BME280_FILT_IIR2  = 0b001,
    BME280_FILT_IIR4  = 0b010,
    BME280_FILT_IIR8  = 0b011,
    BME280_FILT_IIR16 = 0b100,  // IIR Filter x16
} Filter;
typedef enum {
    BME280_SPI3W_DISABLE = 0,  // 3-wire SPI Disable
    BME280_SPI3W_ENABLE  = 1,
} SPI3WireEnable;
typedef struct {
    // NOLINTBEGIN(readability-identifier-naming)
    uint8_t BME280_ADDRESS_;  //= 0x76_;

    int8_t dig_H6_;
    uint8_t dig_H1_, dig_H3_;
    int16_t dig_T2_, dig_T3_, dig_P2_, dig_P3_, dig_P4_, dig_P5_, dig_P6_, dig_P7_, dig_P8_, dig_P9_;
    int16_t dig_H2_, dig_H4_, dig_H5_;
    uint16_t dig_T1_, dig_P1_;

    int32_t t_fine_;
    // NOLINTEND(readability-identifier-naming)
} BME280;
/// @brief BME280を起動する
/// @param osrs_t 温度計のオーバーサンプリング量
/// @param osrs_p 気圧計のオーバーサンプリング量
/// @param osrs_h 湿度計のオーバーサンプリング量
/// @param mode モード
/// @param t_sb スタンバイ時間
/// @param filter IIRフィルター設定
/// @param spi3w_en 3-wire SPI 有効/無効
void bme280_init(BME280* self, TemperatureOversampling osrs_t, PressureOversampling osrs_p, HumidityOversampling osrs_h,
                 Mode mode, Tstandby t_sb, Filter filter, SPI3WireEnable spi3w_en);

typedef EnvironmentSensorDataF BME280_DataTypeF;
typedef EnvironmentSensorDataI16 BME280_DataTypeI16;

BME280_DataTypeF bme280_readf(BME280* self);
BME280_DataTypeI16 bme280_readi16(BME280* self);

bool bme280_read_raw(BME280* self, uint8_t buf);

// private
void bme280_read_trim(BME280* self);

/// @brief 生のバイト列から気圧の値を読み取る
/// @param rawData 生の気圧データ
/// @warning ~rawData[0]~が気圧データの最初のバイトになるようにポインタを進めておくこと
/// @return 補正等もされた気圧の値
float bme280_read_pressure(BME280* self, uint8_t* raw_data);

/// @brief 生のバイト列から気温の値を読み取る
/// @param rawData 生の気温データ
/// @warning ~rawData[0]~が気温データの最初のバイトになるようにポインタを進めておくこと
/// @return 補正等もされた気温の値
float bme280_read_temperature(BME280* self, uint8_t* raw_data);

/// @brief 生のバイト列から気圧の値を読み取る
/// @param rawData 生の気圧データ
/// @warning ~rawData[0]~が気圧データの最初のバイトになるようにポインタを進めておくこと
/// @return 補正等もされた気圧の値
float bme280_read_humidity(BME280* self, uint8_t* raw_data);

/// @brief 気圧の生データを、補正データを用いて補正する
/// @param adc_P
/// @return 上位24bitが整数部分、下位8ビットが小数点以下の気圧データ、つまり256倍された値。単位はPa
/// @note BOSCH社が公開しているBME280のデータシートの4.2.3 Compensation formulas (p.23) 参照
uint32_t bme280_compute_pres64(BME280* self, int32_t adc_p);

/// @brief 気温の生データを、補正データを用いて補正する
/// @param adc_T
/// @return 100倍されたセ氏温度(e.g. 12.34°C → 1234)
/// @note ~t_fine_~を更新するので、~t_fine_~が必要な処理より前に呼ぶこと。
/// @note BOSCH社が公開しているBME280のデータシートの4.2.3 Compensation formulas (p.23) 参照
uint32_t bme280_compute_temp64(BME280* self, int32_t adc_t);

/// @brief 湿度の生データを補正データを用いて補正する
/// @param adc_H
/// @return 上位22bitが整数部分、下位10bitが小数点以下の気圧データ、つまり1024倍された値。単位は%RH
uint32_t bme280_compute_hum64(BME280* self, int32_t adc_h);

#endif
