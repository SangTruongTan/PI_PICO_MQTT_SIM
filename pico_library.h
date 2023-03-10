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

#include "configure.h"
#include "hardware/uart.h"
#include "pico/stdlib.h"
#include "uartRingBuffer.h"
/* Private defines -----------------------------------------------------------*/
#define PICO_LIB_DEBUG 1  // Comment if there is no debuging enable

// Definition of RX Topic and Payload length
#define PICO_RX_TOPIC_LENGTH 32
#define PICO_RX_MSG_LENGTH 64
#define PICO_SMS_LENGTH 32
#define PICO_BALANCE_LENGTH 128

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
    bool RxDetected;
    bool IsRxTopic;
    bool IsRxPayload;
    bool OkDetected;
    bool SmsDetected;
    bool SmsCMTIDetected;
    bool is_sms_readable;
    bool PublicMsgSuc;
    bool IsSimInserted;
    bool IsSocketAvailable;
    int NetWorkMode;  // - 1 means reading not successfully
    int SignalStrength;
    int pRxTopic;
    int pRxMsg;
    char RxTopic[PICO_RX_TOPIC_LENGTH];
    char RxMsg[PICO_RX_MSG_LENGTH];
    char *SmsMsg;
    char *SmsSender;
    char BalanceAvailable[PICO_BALANCE_LENGTH];
    char NetworkProvider[32];
    int HTTPResponseCode;
    int HTTPDataLength;
    char *HTTPData;
} PicoLibrary_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Initialize the library.
 * @param PicoParam The vector of Libary parameter.
 * @retval bool
 */
bool picolib_init(PicoLibrary_t *PicoParam);

/**
 * @brief Initialize module SIM.
 * @param Uart The pointer of uart instance.
 * @retval bool
 */
bool sim_init(uart_inst_t *Uart);

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
 * @retval bool
 */
bool sim_send_test_command(uart_inst_t *Uart);

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
 * @retval void
 */
void sim_at_netclose();

/**
 * @brief Send AT to Open Network
 * @retval void
 */
void sim_at_netopen(void);

/**
 * @brief Is socket available
 * @retval bool
 */
bool sim_is_socket_available(void);

/**
 * @brief Get signal strength
 * @retval int The value that describe the signal strength.
 */
int sim_get_signal_strength(void);

/**
 * @brief Is sim inserted
 * @retval bool
 */
bool sim_is_inserted(void);

/**
 * @brief Get sim network provider
 * @retval void
 */
void sim_get_network_provider(void);

/**
 * @brief Configure Network Mode
 * @param Mode The network mode refer in datasheet. 2 - Auto, 13 - GSM only, 14
 * ??? WCDMA Only, 38 ??? LTE Only
 * @retval bool
 */
bool sim_configure_network_mode(int Mode);

/* MQTT Support functions */

/**
 * @brief MQTT Start
 * @retval bool True means started succesfully and opposite for False.
 */
bool mqtt_start();

/**
 * @brief Is MQTT Service available
 * @retval bool
 */
bool mqtt_is_service_available(void);

/**
 * @brief Check balance available of the SIM card
 * @param Dial Your dial number to check. e.g, "*101#"
 * @retval bool True means successfully checking and opposite for False.
 */
bool sim_check_balance_available(char *Dial);

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
 * @param Session The session_id to operate. It???s from 0 to 1.
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
bool mqtt_will_topic(uint8_t ClientIdx, char *WillTopic);

/**
 * @brief MQTT WILL message
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param Message The payload of will message.
 * @param Qos The publish message???s qos. The range is from 0 to 2.
 * @retval bool True means subscribe succesfully and opposite for False.
 */
bool mqtt_will_message(uint8_t ClientIdx, char *Message, int Qos);

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
 * @brief MQTT Connect Server with authentication (User and Password)
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param Server The server address. For instance,
 * "tcp://test.mosquitto.org:1883"
 * @param KeepAliveTime Keep Alive Time parameter.
 * @param CleanSession Clean Session parameter.
 * @param User The user name specified in the private broker.
 * @param Password The password of the user
 * @retval bool True means connected succesfully and opposite for False.
 */
bool mqtt_connect_server_authenticate(uint8_t ClientIdx, char *Server,
                                      uint16_t KeepAliveTime,
                                      uint8_t CleanSession, char *User,
                                      char *Password);

/**
 * @brief Is connection available
 * @retval bool
 */
bool mqtt_is_connection_available(void);

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
 * @param Qos The publish message???s qos. The range is from 0 to 2.
 * @retval bool True means subscribe succesfully and opposite for False.
 */
bool mqtt_subscribe_topic(uint8_t ClientIdx, char *SubTopic, int Qos);

/**
 * @brief MQTT SUBSCRIBE message
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param Message The message of SUBSCRIBE message.
 * @param Qos The publish message???s qos. The range is from 0 to 2.
 * @retval bool True means send unsubscribe msg succesfully and opposite for
False.
 */
bool mqtt_subscribe_message(uint8_t ClientIdx, char *Message, int Qos);

/**
 * @brief MQTT SUBSCRIBE only
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @retval bool True means send command succesfully and opposite for False.
 */
bool mqtt_subscribe(uint8_t ClientIdx);

/**
 * @brief MQTT UNSUBSCRIBE TOPIC message
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param UnsubTopic The topic of UNSUBSRIBE message.
 * @param Qos The publish message???s qos. The range is from 0 to 2.
 * @retval bool True means set unsubcribe topic succesfully and opposite for
False.
 */
bool mqtt_unsubscribe_topic(uint8_t ClientIdx, char *UnSubTopic, int Qos);

/**
 * @brief MQTT UNSUBSCRIBE message
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param Message The message of UNSUBSCRIBE message.
 * @retval bool True means unsubscribe succesfully and opposite for False.
 */
bool mqtt_unsubscribe_message(uint8_t ClientIdx, char *Message);

/**
 * @brief MQTT PUBLISH TOPIC message
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param PubTopic The topic of PUBLISH message.
 * @retval bool True means set topic succesfully and opposite for False.
 */
bool mqtt_public_topic(uint8_t ClientIdx, char *PubTopic);

/**
 * @brief MQTT PUBLISH Payload message
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param Message The message of PUBLISH message.
 * @retval bool True means public succesfully and opposite for False.
 */
bool mqtt_public_message(uint8_t ClientIdx, char *Message);

/**
 * @brief MQTT PUBLISH message to server
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param Qos The publish message???s qos. The range is from 0 to 2.
 * @param PubTimeout The publishing timeout interval value. The range is from
60s to 180s.
 * @retval bool True means public succesfully and opposite for False.
 */
bool mqtt_public_to_server(uint8_t ClientIdx, int Qos, uint8_t PubTimeout);

/**
 * @brief MQTT Configure Context
 * @param ClientIdx The client index. A numeric parameter that identifies a
client. The range of permitted values is 0 to 1.
 * @param CheckUtf8Flag The flag to indicate whether to check the string is UTF8
coding or not, the default value is 1. 0 means not check and opposite side.
 * @param bool True means configure succesfully and opposite for False.
 */
bool mqtt_configure_context(uint8_t ClientIdx, uint8_t CheckUtf8Flag);

/**
 * @brief Is MQTT Rx data ready to read
 * @retval bool True means the RX data ready to read and oppsite for False.
 */
bool mqtt_is_rx_readable();

/* HTTP(s) APIs*/
/**
 * @brief Start HTTP(s) Service
 */
void http_start(void);

/**
 * @brief Stop HTTP(s) Service
 */
void http_stop(void);

/**
 * @brief Set URL HTTP(s)
 * @param Url Example: "https://github.com"
 */
void http_set_param_url(char *Url);

/**
 * @brief Send HTTP(s) action
 * @param Action 0. GET; 1. POST; 2.HEAD; 3.DELETE
 * @retval -2 means Not enough memory
 * @retval -1 means request but no response
 * @retval 0 means receive response with error
 * @retval Otherwise Code Response
 */
int http_action(int Action);

/**
 * @brief Read the Head response from action. Should be call http_action()
 * first.
 * @return char* The pointer of data (Should be free() after using).
 */
char *http_read_head();

/* SMS Support Functions */
/**
 * @brief Sms set mode
 * @param Mode 0 for PDU and 1 for text mode
 * @void
 */
void sms_set_mode(uint8_t Mode);

/**
 * @brief SMS select message format. Default as Text mode.
 * @param PhoneNumber The destination phone number. For instance,
 * "+8412345789".
 * @param Text The text message. "ABCD\032" - '\032' is very important.
 * @param bool True means send sms succesfully and opposite for False.
 */
bool sms_send(char *PhoneNumber, char *Text);

/**
 * @brief Send message back to Sender
 * @param Text Text message
 */
bool sms_get_back(char *Text);

/**
 * @brief Whether available SMS Read unread message
 * @retval bool True means sms message available succesfully and opposite for
 * False.
 */
bool is_sms_readable();

/**
 * @brief Remove all messages in the SIM's storage
 * void
 */
bool sms_remove_messages();

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

/**
 * @brief Reset all flags
 * @retval void
 */
void reset_flags();

#endif /* __PICO_LIBRARY_H_ */
