# RTOS PROJECT

## Goals

The goal of the project is to display the value of a temperature sensor and and a gyro sensor on a small monochrome display using an stm32 microcontroller using ZephyrRTOS.

## Materials

- Temperature sensor bmp280
- Accelereometer and gyro sensor mpu6050
- Monochrome OLED display 128*64px sssd1306
- Devboard nucleo_g474re

## Results

For now the code version of the master branch is a version tha does not work in "real time" terms, it is a simple sequential code to test the functionning of each part and works like a charm.

The version on the task branch work using mutex.

## Additionnal note

The name of the project has been carefully choosen with the teacher because : "Notre app elle tue sa mère !"
