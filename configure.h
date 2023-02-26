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
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Private defines -----------------------------------------------------------*/
#define LOG_BUFFER 320
// #define PICO_DEVICES 1      // Comment when you're testing on PC
#define CONFIGURE_DEBUG 1  // Comment if there is no debuging enable
#define DETAIL_LOGGING 1  // Comment if use undetails logging

#ifdef CONFIGURE_DEBUG
#ifdef DETAIL_LOGGING
#define LOGUF(Format) LOG_DETAILS(__FILE__, __func__, __LINE__, Format)
#define LOGF(Format, ...) \
    LOG_DETAILS(__FILE__, __func__, __LINE__, Format, __VA_ARGS__)
#else
#define LOGUF(Format) LOG(Format)
#define LOGF(Format, ...) LOG(Format, __VA_ARGS__)
#endif
#else
#define LOGUF(Format)
#define LOGF(Format, ...)
#endif

#define MQTT_TOPIC_LENGTH 32
#define MQTT_USER_LENGTH 32
#define MQTT_PASSWORD_LENGTH 32
#define SMS_MESSAGE_LENGTH 128
#define PHONE_LENGTH 10
#define PHONE_LIST 10
#define MODBUS_PACKAGE 5
#define MODBUS_SIZE_PACKAGE 8
#define MQTT_SIZE_TOPIC 5

/* Exported types ------------------------------------------------------------*/
typedef struct {
    bool isAvailable;
    char *Target;
} substr_t;

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
    char Sender[PHONE_LENGTH + 1];           // The sender telephone
    char SmsBuffer[SMS_MESSAGE_LENGTH + 1];  // The buffer of the sms message
    char PhoneNumber[PHONE_LIST][PHONE_LENGTH + 1];
    unsigned char ModbusPacket[MODBUS_PACKAGE][MODBUS_SIZE_PACKAGE];
    char MqttTopic[MQTT_SIZE_TOPIC][MQTT_TOPIC_LENGTH + 1];
    char MqttUser[MQTT_USER_LENGTH + 1];
    char MqttPassword[MQTT_PASSWORD_LENGTH + 1];
    int FourTwentySensor;
    bool (*get_back)(char *);
    bool (*send_sms)(char *, char *);
} Configuration_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
/* API functions */
/**
 * @brief For initializing the library
 * @param Configure The pointer of configure member variable
 */
void configure_init(Configuration_t *Configure);

/**
 * @brief Main process of the library
 */
void configure_main_process(void);

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

/**
 * @brief List all the phone in the storage
 */
void list_phone(void);

/**
 * @brief Add the slave phone
 * @return true Add phone successfully.
 * @return false Can not add the phone number due to the wrong syntax or not
 * enough storage.
 */
bool add_phone(void);

/**
 * @brief Delete the phone number in the list
 * @return true Delete successfully.
 * @return false Can not delete the phone number due to the wrong syntax or not
 * enough storage.
 */
bool delete_phone(void);

/**
 * @brief Add mobus master package
 * @return true Add successfully
 * @return false Can not add the package due to the wrong syntax or not
 * enough storage.
 */
bool add_modbus(void);

/**
 * @brief Add mqtt topic
 * @return true Add successfully
 * @return false Can not add the package due to the wrong syntax or not
 * enough storage.
 */
bool add_mqtt_topic(void);

/**
 * @brief Configure 4-20 mA Analog Sensor
 * @return true Configure successfully
 * @return false Can not add the package due to the wrong syntax or not
 * enough storage.
 */
bool configure_4_20_sensor(void);

/**
 * @brief Add MQTT Authentication User and Password
 * @return true Add successfully
 * @return false Can not add the package due to the wrong syntax or not
 */
bool add_user_password(void);

/* Alert */
bool alert_status(float Value, float UpperThresold, float LowerThreshold);

/* Support functions */
/**
 * @brief Log debugging with file, function, line and format
 * @param File String name of File
 * @param Func String name of Function
 * @param Line Integer type of Line
 * @param format Formatted output
 * @param ... Args
 */
void LOG_DETAILS(const char *File, const char *Func, int Line,
                 const char *format, ...);

void LOG(const char *format, ...);

/**
 * @brief Get subsequence string between two delimeter
 * @param Text Buffer of the original string
 * @param HeadDelim Head Delimeter
 * @param TailDelim Tail Delimeter
 * @return substr_t Return Substr_t struct
 */
substr_t substr(char *Text, const char *HeadDelim, const char *TailDelim);

/**
 * @brief Convert String hexadecimal format to byte array
 * @param Text Source buffer
 * @param Target Target buffer
 * @return true When convert successfully
 * @return false When convert unsuccessfully
 */
bool hexa_string_to_bytes(char *Text, unsigned char *Target);

#endif /* __CONFIGURE_H_ */
