/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : configure.h
 * @brief          : Header for confiure.c file.
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
#ifndef __CONFIGURE_H_
#define __CONFIGURE_H_

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* Private defines -----------------------------------------------------------*/
#define MQTT_TOPIC_LENGTH 32
#define MQTT_USER_LENGTH 32
#define MQTT_PASSWORD_LENGTH 32

/* Exported types ------------------------------------------------------------*/
typedef enum {
    UNINITIALIZE_STATE = 0,
    READY_STATE = 1,
    SELECTION_STATE = 2,
    ADDING_NUMBER_STATE = 3,
    DELETE_NUMBER_STATE = 4,
    LIST_OUT_NUMBER_STATE = 5,
} configuration_state_t;

typedef struct {
    configuration_state_t State;
    char *SmsBuffer;    // The buffer of the sms message
    char *Sender;       // The sender telephone
    char PhoneNumber[10][10];
    uint8_t ModbusPacket[5][8];
    char MqttTopic[5][32];
    char MqttUser[MQTT_USER_LENGTH];
    char MqttPassword[MQTT_PASSWORD_LENGTH];
    uint8_t FourTwentyConfi[4];
} Configuration_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief For initializing the library
 * @param Configure The pointer of configure member variable
 */
void configure_init(Configuration_t *Configure);

/**
 * @brief Check whether there is a message received
 * @return true Just receive a sms
 * @return false There is no sms received
 */
bool is_receive_sms(void);

/**
 * @brief Check whether there is a master phone number
 * @return true Is master phone number
 * @return false Not is master phone number
 */
bool is_master_number(void);

/**
 * @brief Check whether the master phone number not configured yet
 * @return true Not configured yet
 * @return false The master phone number is existing
 */
bool is_master_empty(void);

/**
 * @brief For adding master phone number only
 */
void add_master_number(void);

/**
 * @brief Process the configuring sms
 */
void process_configure_sms(void);

#endif /* __CONFIGURE_H_ */
