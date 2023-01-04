/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : The source file of the library
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 Sang Tan Truong.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#include "pico_library.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
#define UART_ID uart1
#define UART_TX_PIN 8
#define UART_RX_PIN 9
#define BAUDRATE 115200

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
PicoLibrary_t mPicoLib;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
int main() {
    stdio_init_all();
    // Initialize library parameter
    mPicoLib.uartId = UART_ID;
    mPicoLib.baudrate = BAUDRATE;
    mPicoLib.txPin = UART_TX_PIN;
    mPicoLib.rxPin = UART_RX_PIN;

    // Call pico library init
    picolib_init(&mPicoLib);

    // MQTT example
    // uart_puts(uart0, "Connecting to MQTT Server.....");
    // mqtt_start();
    // mqtt_acquire_client(0, "SangTruongTan\083");
    // mqtt_connect_server(0, "tcp://test.mosquitto.org:1883", 90, 1);
    //Loop forever
    while(1) {
        // Using for debuging Module SIM
        // at_command_bio_forward(uart0, uart1);
    }
}