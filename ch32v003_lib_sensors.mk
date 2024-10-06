ch32v003_lib_sensors?=./
ADDITIONAL_C_FILES+=$(ch32v003_lib_sensors)/MPU6050.c $(ch32v003_lib_sensors)/Vec3.c $(ch32v003_lib_sensors)/i2c_helper.c
EXTRA_CFLAGS+=-I$(ch32v003_lib_sensors)
