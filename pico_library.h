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
#include "uartRingBuffer.h"

/* Private defines -----------------------------------------------------------*/
#define PICO_LIB_DEBUG 1  // Comment if there is no debuging enable

/* Exported types ------------------------------------------------------------*/
typedef struct {
    uart_inst_t *uartId;
    RingHandler_t RingHandler;
    uint baudrate;
    uint txPin;
    uint rxPin;
    bool ConnectionAvailable;
    bool MqttStarted;
    bool MorethanSymbol;
    bool RxTopic;
    bool RxPayload;
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
 * @param Uart The pointer of uart instance.
 * @retval void
 */
void sim_init(uart_inst_t *Uart);

/**
 * @brief Handler the message receive from SIM module
 * @param Buffer The pointer of buffer.
 * @retval bool True means the message contains processing-needed, and vice
 * versa.
 */
bool picolib_process(char *Buffer);

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
 * @param Buffer The pointer of the buffer.
 * @param Delimiter Determine the character to end up the string.
 * @retval bool
 */
bool sim_receive_at_command(uart_inst_t *Uart, char *Buffer, char Delimiter);

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
 * @retval bool True means started succesfully and opposite for False.
 */
bool mqtt_start();

/**
 * @brief MQTT Stop
 * @retval bool True means started succesfully and opposite for False.
 */
bool mqtt_stop();

/**
 * @brief MQTT Acquire Client
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param ClientId The client ID.
 * @retval void
 */
void mqtt_acquire_client(uint8_t ClientIdx, char *ClientId);

/**
 * @brief MQTT Release Client
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @retval void
 */
void mqtt_release_client(uint8_t ClientIdx);

/**
 * @brief MQTT Set SSL context
 * @param Session The session_id to operate. It’s from 0 to 1.
 * @param SSLContextId The SSL context ID which will be used in the SSL
 connection.
* @retval bool True means subscribe succesfully and opposite for False.
 */

/**
 * @brief MQTT WILL TOPIC message
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param WillTopic The Topic of WILL message.
 * @retval bool True means subscribe succesfully and opposite for False.
 */

/**
 * @brief MQTT WILL message
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param Message The payload of will message.
 * @param Qos The publish message’s qos. The range is from 0 to 2.
 * @retval bool True means subscribe succesfully and opposite for False.
 */

/**
 * @brief MQTT Connect Server
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param Server The server address. For instance,
 * "tcp://test.mosquitto.org:1883"
 * @param KeepAliveTime Keep Alive Time parameter.
 * @param CleanSession Clean Session parameter.
 * @retval bool True means connected succesfully and opposite for False.
 */
bool mqtt_connect_server(uint8_t ClientIdx, char *Server,
                         uint16_t KeepAliveTime, uint8_t CleanSession);

/**
 * @brief MQTT Disconnect from Server
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 @ @param Timeout The timeout value for disconnection. The unit is second. The
range is 60s to 180s. The default value is 0s (not set the timeout value)
 */
bool mqtt_disconnect_server(uint8_t ClientIdx, uint8_t Timeout);

/**
 * @brief MQTT SUBSCRIBE TOPIC message
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param SubTopic The topic of SUBSCRIBE message.
 * @param Qos The publish message’s qos. The range is from 0 to 2.
 * @retval bool True means subscribe succesfully and opposite for False.
 */
bool mqtt_subscribe_topic(uint8_t ClientIdx, char *SubTopic, int Qos);

/**
 * @brief MQTT SUBSCRIBE message
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param Message The message of SUBSCRIBE message.
 * @param Qos The publish message’s qos. The range is from 0 to 2.
 * @retval bool True means subscribe succesfully and opposite for False.
 */
bool mqtt_subscribe_message(uint8_t ClientIdx, char *Message, int Qos);

/**
 * @brief MQTT UNSUBSCRIBE TOPIC message
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param UnsubTopic The topic of UNSUBSRIBE message.
 * @param Qos The publish message’s qos. The range is from 0 to 2.
 * @retval bool True means subscribe succesfully and opposite for False.
 */
bool mqtt_unsubscribe_topic(uint8_t ClientIdx, char *UnSubTopic, int Qos);

/**
 * @brief MQTT UNSUBSCRIBE message
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param Message The message of UNSUBSCRIBE message.
 * @retval bool True means subscribe succesfully and opposite for False.
 */
bool mqtt_unsubscribe_message(uint8_t ClientIdx, char *Message);

/**
 * @brief MQTT PUBLISH TOPIC message
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param PubTopic The topic of PUBLISH message.
 * @retval bool True means subscribe succesfully and opposite for False.
 */
bool mqtt_public_topic(uint8_t ClientIdx, char *PubTopic);

/**
 * @brief MQTT PUBLISH Payload message
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param Message The message of PUBLISH message.
 * @retval bool True means subscribe succesfully and opposite for False.
 */
bool mqtt_public_message(uint8_t ClientIdx, char *Message);

/**
 * @brief MQTT PUBLISH message to server
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param Qos The publish message’s qos. The range is from 0 to 2.
 * @param PubTimeout The publishing timeout interval value. The range is from
60s to 180s.
 * @retval bool True means subscribe succesfully and opposite for False.
 */
bool mqtt_public_to_server(uint8_t ClientIdx, int Qos, uint8_t PubTimeout);

/**
 * @brief MQTT Configure Context
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param CheckUtf8Flag The flag to indicate whether to check the string is UTF8
coding or not, the default value is 1. 0 means not check and opposite side.
 * @param bool True means subscribe succesfully and opposite for False.
 */

/* SMS Support Functions */

/**
 * @brief SMS select message format. Default as Text mode.
 * @param PhoneNumber The destination phone number. For instance, "+8412345789".
 * @param Text The text message. "ABCD\032" - '\032' is very important.
 * @param bool True means subscribe succesfully and opposite for False.
 */
bool sms_send(char *PhoneNumber, char *Text);

/**
 * @brief SMS Read unread message
 * @param Buffer The buffer of receiving message in case. Should be largely.
 * @retval bool True means subscribe succesfully and opposite for False.
 */

/* Support Functions */
/**
 * @brief Handle Buffer data
 * @retval void
 */
void handle_buffer();

/**
 * @brief Support function to send mqtt message
 * @param Cmd The command of the message.
 * @param Message The buffer of the message.
 * @retval bool True means subscribe succesfully and opposite for False.
 */
bool mqtt_support_send(char *Cmd, char *Message);

#endif /* __PICO_LIBRARY_H_ */
