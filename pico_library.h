/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : pico_library.h
 * @brief          : Header for pico_library.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2023 Sang Tan Truong.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PICO_LIBRARY_H_
#define __PICO_LIBRARY_H_

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "hardware/uart.h"
#include "pico/stdlib.h"

/* Private defines -----------------------------------------------------------*/
#define PICO_LIB_DEBUG 1  // Comment if there is no debuging enable

/* Exported types ------------------------------------------------------------*/
typedef struct {
    uart_inst_t *uartId;
    uint baudrate;
    uint txPin;
    uint rxPin;
} PicoLibrary_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Initialize the library.
 * @param PicoParam The vector of Libary parameter.
 * @retval void
 */
void picolib_init(PicoLibrary_t *PicoParam);

/**
 * @brief Initialize module SIM.
 * @param Uart the pointer of uart instance.
 * @retval void
 */
void sim_init(uart_inst_t *Uart);

/* Module SIM Hardware Abstract Layer (HAL) functions */
/**
 * @brief Test AT command with Module SIM.
 * @param DebugUart Terminal Uart.
 * @param UartSim The Uart ID that is connecting with Module Sim.
 * @retval void
 */
void at_command_bio_forward(uart_inst_t *DebugUart, uart_inst_t *UartSim);

/**
 * @brief Send test AT command and wait until successful.
 * @param Uart The pointer of uart instance.
 * @retval void
 */
void sim_send_test_command(uart_inst_t *Uart);

/**
 * @brief Forwarding AT command and reponse from SIM.
 * @param Uart the pointer of uart instance.
 * @param Cmd String of command.
 * @retval void
 */
void sim_forward_command(uart_inst_t *Uart, char *Cmd);

/**
 * @brief Send AT command.
 * @param Uart the pointer of uart instance.
 * @param buffer the pointer of the buffer.
 * @retval void
 */
void sim_send_at_command(uart_inst_t *Uart, char *buffer);

/**
 * @brief Receive AT command
 * @param Uart The pointer of uart instance.
 * @param buffer The pointer of the buffer.
 * @param Delimiter Determine the character to end up the string.
 * @retval bool
 */
bool sim_receive_at_command(uart_inst_t *Uart, char *buffer, char Delimiter);

/**
 * @brief Flushing buffer by forwarding all to terminal if enabled.
 * @param Uart The pointer of uart instance.
 * @retval void
 */
void sim_forward_buffer(uart_inst_t *Uart);

/**
 * @brief Send AT Netclose
 * @param Uart The pointer of uart instance.
 * @retval void
 */
void sim_at_netclose(uart_inst_t *Uart);

/**
 * @brief Send AT to Open Network
 * @param Uart The pointer of uart instance.
 * @retval void
 */
void sim_at_netopen(uart_inst_t *Uart);

/* MQTT Support functions */
/**
 * @brief MQTT Start
 * @retval void
 */
void mqtt_start();

/**
 * @brief MQTT Acquire Client
 * @param ClientIdx The client index.
 * @param ClientId The client ID.
 * @retval void
 */
void mqtt_acquire_client(uint8_t ClientIdx, char *ClientId);

/**
 * @brief MQTT Connect Server
 * @param ClientIdx The client index.
 * @param Server The server address. For instance,
 * "tcp://test.mosquitto.org:1883"
 * @param KeepAliveTime Keep Alive Time parameter.
 * @param CleanSession Clean Session parameter.
 * @retval void
 */
void mqtt_connect_server(uint8_t ClientIdx, char *Server,
                         uint16_t KeepAliveTime, uint8_t CleanSession);

#endif /* __PICO_LIBRARY_H_ */
