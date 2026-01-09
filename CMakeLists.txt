# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(appkituesamer)

target_sources(
    app PRIVATE src/main.c
    src/ssd1306/ssd1306.c
    src/sensors/sensors.c
    src/mpu6050/mpu6050.c
    src/bmp280/bmp280.c
    src/utilities/utilities.c)
