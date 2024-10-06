#include "Vec3.h"

Vec3f vec3f_add(Vec3f a, Vec3f b) {
    Vec3f result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}
Vec3u16 vec3u16_add(Vec3u16 a, Vec3u16 b) {
    Vec3u16 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}
Vec3i16 vec3i16_add(Vec3i16 a, Vec3i16 b) {
    Vec3i16 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

Vec3f vec3f_sub(Vec3f a, Vec3f b) {
    Vec3f result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}
Vec3u16 vec3u16_sub(Vec3u16 a, Vec3u16 b) {
    Vec3u16 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}
Vec3i16 vec3i16_sub(Vec3i16 a, Vec3i16 b) {
    Vec3i16 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

Vec3f vec3f_mul(Vec3f vec, float scaler) {
    Vec3f result;
    result.x = vec.x * scaler;
    result.y = vec.y * scaler;
    result.z = vec.z * scaler;
    return result;
}
Vec3u16 vec3u16_mul(Vec3u16 vec, uint16_t scaler) {
    Vec3u16 result;
    result.x = vec.x * scaler;
    result.y = vec.y * scaler;
    result.z = vec.z * scaler;
    return result;
}
Vec3i16 vec3i16_mul(Vec3i16 vec, int16_t scaler) {
    Vec3i16 result;
    result.x = vec.x * scaler;
    result.y = vec.y * scaler;
    result.z = vec.z * scaler;
    return result;
}

Vec3f vec3f_div(Vec3f vec, float scaler) {
    Vec3f result;
#ifdef POOR_MANS_SENSORS_NO_FAST_DIV
    result.x = vec.x / scaler;
    result.y = vec.y / scaler;
    result.z = vec.z / scaler;
#else
    float inv = 1 / scaler;
    result.x  = vec.x * inv;
    result.y  = vec.y * inv;
    result.z  = vec.z * inv;
#endif
    return result;
}
Vec3u16 vec3u16_div(Vec3u16 vec, uint16_t scaler) {
    Vec3u16 result;
    result.x = vec.x / scaler;
    result.y = vec.y / scaler;
    result.z = vec.z / scaler;
    return result;
}
Vec3i16 vec3i16_div(Vec3i16 vec, int16_t scaler) {
    Vec3i16 result;
    result.x = vec.x / scaler;
    result.y = vec.y / scaler;
    result.z = vec.z / scaler;
    return result;
}

Vec3f cast_vec3u16_to_vec3f(Vec3u16 vec) {
    Vec3f result = {vec.x, vec.y, vec.z};
    return result;
}

Vec3f cast_vec3i16_to_vec3f(Vec3i16 vec) {
    Vec3f result = {vec.x, vec.y, vec.z};
    return result;
}
