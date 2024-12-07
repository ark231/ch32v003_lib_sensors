#ifndef CH32V003_LIB_SENSORS_ENVIRONMENTSENSORDATA
#define CH32V003_LIB_SENSORS_ENVIRONMENTSENSORDATA
#include <stdint.h>
typedef struct {
    float pressure;
    float temperature;
    float humidity;
} EnvironmentSensorDataF;

typedef struct {
    int16_t pressure;
    int16_t temperature;
    int16_t humidity;
} EnvironmentSensorDataI16;

typedef struct {
    uint16_t pressure;
    uint16_t temperature;
    uint16_t humidity;
} EnvironmentSensorDataU16;
#endif
