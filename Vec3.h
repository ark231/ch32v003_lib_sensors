#ifndef CH32V003_LIB_SENSORS_SENSORS_VEC3
#define CH32V003_LIB_SENSORS_SENSORS_VEC3
#include <stdint.h>

typedef struct {
    float x;
    float y;
    float z;
} Vec3f;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t z;
} Vec3u16;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} Vec3i16;

Vec3f vec3f_add(Vec3f a, Vec3f b);
Vec3u16 vec3u16_add(Vec3u16 a, Vec3u16 b);
Vec3i16 vec3i16_add(Vec3i16 a, Vec3i16 b);

Vec3f vec3f_sub(Vec3f a, Vec3f b);
Vec3u16 vec3u16_sub(Vec3u16 a, Vec3u16 b);
Vec3i16 vec3i16_sub(Vec3i16 a, Vec3i16 b);

Vec3f vec3f_mul(Vec3f vec, float scaler);
Vec3u16 vec3u16_mul(Vec3u16 vec, uint16_t scaler);
Vec3i16 vec3i16_mul(Vec3i16 vec, int16_t scaler);

Vec3f vec3f_div(Vec3f vec, float scaler);
Vec3u16 vec3u16_div(Vec3u16 vec, uint16_t scaler);
Vec3i16 vec3i16_div(Vec3i16 vec, int16_t scaler);

Vec3f cast_vec3u16_to_vec3f(Vec3u16 vec);
Vec3f cast_vec3i16_to_vec3f(Vec3i16 vec);
#endif
