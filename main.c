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

#include "hardware/uart.h"
#include "pico/stdlib.h"
#include "pico_library.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
// SIM module Uart
#define UART_ID uart1
#define UART_TX_PIN 8
#define UART_RX_PIN 9
#define BAUDRATE 115200

// Debugging Uart
#define DEBUG_UART_ID uart0
#define DEBUG_TX_PIN 0
#define DEBUG_RX_PIN 1
#define DEBUG_BAUDRATE 115200

// LOG Lines
#define LOG(X) uart_puts(DEBUG_UART_ID, X)

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
PicoLibrary_t mPicoLib;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
int main() {
    stdio_init_all();
    // Set up our UART with the required speed.
    uart_init(DEBUG_UART_ID, DEBUG_BAUDRATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(DEBUG_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(DEBUG_RX_PIN, GPIO_FUNC_UART);

    // Initialize library parameter
    mPicoLib.uartId = UART_ID;
    mPicoLib.baudrate = BAUDRATE;
    mPicoLib.txPin = UART_TX_PIN;
    mPicoLib.rxPin = UART_RX_PIN;

    // Call pico library init
    picolib_init(&mPicoLib);

    // Basic functionality
    LOG("*** Test basic functionality\r\n ***");
    char *Buffer = malloc(128);
    LOG("Check is SIM Inserter\r\n");
    if (sim_is_inserted()) {
        LOG("Sim is inserted!!!\r\n");
    } else {
        LOG("### Please check your sim card again\r\n");
        while(1);
    }
    LOG("Check signal strength\r\n");
    int signal = sim_get_signal_strength();
    sprintf(Buffer, "Signal Strength:%d\r\n", signal);
    LOG(Buffer);
    LOG("Get network Provider\r\n");
    sim_get_network_provider();
    LOG(mPicoLib.NetworkProvider);
    LOG("Get banlance available\r\n");
    sim_check_balance_available("*101*2#");
    LOG(mPicoLib.BalanceAvailable);
    LOG("Configure to LTE mode only instead on automatic\r\n");
    if (sim_configure_network_mode(38)) {
        sprintf(Buffer, "Configure Network successfully:%d\r\n",
                mPicoLib.NetWorkMode);
        LOG(Buffer);
    } else {
        LOG("Uncessfully to configure network mode\r\n");
    }
    LOG("Enable Inernet socket\r\n");
    sim_at_netopen();
    sleep_ms(1000);
    if (sim_is_socket_available()) {
        LOG("Socket is available ==> Try closing\r\n");
        sim_at_netclose();
        sleep_ms(1000);
        if (sim_is_socket_available() == false) {
            LOG("Successfully in closing socket\r\n");
        } else {
            LOG("Can not close the socket\r\n");
        }
    }  else {
            LOG("Unsuccessfully in opening the socket\r\n");
    }
    while(1);
    // SMS example
    LOG("Testing SMS functionality\r\n");
    sms_set_mode(1);
    sms_remove_messages();
    sms_send("YourPhoneNumber", "Hello, great\032");
    LOG("Go to receive message\r\n");
    while (1) {
        if (is_sms_readable()) {
            sprintf(Buffer, "======>SMS:%s\r\n", mPicoLib.SmsMsg);
            LOG(Buffer);
        }
        sleep_ms(500);
    }
    // MQTT example
    // Try to stop MQTT service first
    mqtt_disconnect_server(0, 90);
    mqtt_release_client(0);
    mqtt_stop();
    LOG("Connecting to MQTT Server.....\r\n");
    if (mqtt_start()) {
        LOG("MQTT started successfully!\r\n");
        mqtt_acquire_client(0, "SangTruongTan");
        if (mqtt_connect_server_authenticate(0, "tcp://test.mosquitto.org:1883",
                                             90, 1, "User", "Password")) {
            LOG("Connected to MQTT Server successfully!\r\n");
            // Do something
            if (mqtt_subscribe_topic(0, "ChangeYourTopic", 0)) {
                LOG("Subcribe successfully\r\n");
                mqtt_public_topic(0, "ChangeYourTopic");
                mqtt_public_message(0, "Hello1234");
                mqtt_public_to_server(0, 0, 120);
                char *Buf = malloc(100);
                mqtt_subscribe(0);
                LOG("Going to receive loop\r\n");
                // Receive Rx data
                while (1) {
                    if (mqtt_is_rx_readable()) {
                        sprintf(Buf, "Topic RX:%s\r\n", mPicoLib.RxTopic);
                        LOG(Buf);
                        sprintf(Buf, "RX Payload:%s\r\n", mPicoLib.RxMsg);
                        LOG(Buf);
                    }
                }
            } else {
                LOG("Subcribe unsuccessfully\r\n");
            }
        }
    }
    // Loop forever
    while (1) {
        // Using for debuging Module SIM
        at_command_bio_forward(uart0, uart1);
    }
}
