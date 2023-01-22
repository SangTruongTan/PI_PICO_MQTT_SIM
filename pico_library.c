/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : pico_library.c
 * @brief          : The source file of the library
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
/* Includes ------------------------------------------------------------------*/
#include "pico_library.h"

/* Private includes ----------------------------------------------------------*/
#include <string.h>

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#ifdef PICO_LIB_DEBUG
#define LOG(X) uart_puts(uart0, X)
#else
#define LOG(X)
#endif

#define FLASH_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
PicoLibrary_t *mPico;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
void picolib_init(PicoLibrary_t *PicoParam) {
    mPico = PicoParam;
    // uart Ring Buffer initialize
    mPico->RingHandler.uartId = mPico->uartId;
    Ring_Init(&mPico->RingHandler, mPico->baudrate, mPico->txPin, mPico->rxPin);
    // Sim Initialize
    sim_init(mPico->uartId);
}

void sim_init(uart_inst_t *Uart) {
    LOG("Initializing SIM module\r\n");
    sim_send_test_command(Uart);
    sim_forward_command(Uart, "AT&F\r");
    sim_forward_command(Uart, "ATE0\r");
}

bool picolib_process(char *Buffer) {
    bool retval = false;
    LOG(Buffer);
    if (strstr(Buffer, ">")) {
        LOG(Buffer);
        mPico->MorethanSymbol = true;
        retval = true;
    }
    if (strstr(Buffer, "OK")) {
        mPico->OkDetected = true;
        retval = true;
    } else if (strcmp(Buffer, "\r\n") == 0) {
        retval = false;
    } else if (strstr(Buffer, "+CMQTTSTART:")) {
        if (strstr(Buffer, "0")) {
            mPico->MqttStarted = true;
            retval = true;
        }
    } else if (strstr(Buffer, "+CMQTTCONNECT:")) {
        if (strstr(Buffer, ",0") || strlen(Buffer) > 25) {
            mPico->ConnectionAvailable = true;
            retval = true;
        }
    } else if (strstr(Buffer, "+CMQTTPUB:")) {
        if (strstr(Buffer, ",0")) {
            mPico->PublicMsgSuc = true;
            retval = true;
        }
    } else if (strstr(Buffer, "+CMQTTRXSTART:")) {
        mPico->RxDetected = true;
        retval = true;
    } else if (strstr(Buffer, "+CMQTTRXTOPIC:")) {
        mPico->IsRxTopic = true;
        mPico->pRxTopic = 0;
        retval = true;
    } else if (strstr(Buffer, "+CMQTTRXPAYLOAD:")) {
        mPico->IsRxTopic = false;
        mPico->IsRxPayload = true;
        mPico->pRxMsg = 0;
        retval = true;
    } else if (strstr(Buffer, "+CMQTTRXEND:")) {
        mPico->IsRxTopic = false;
        mPico->IsRxPayload = false;
        retval = true;
    } else if (strstr(Buffer, "+CMQTTSTOP:")) {
        if (strstr(Buffer, "0")) {
            mPico->MqttStarted = false;
            retval = true;
        }
    } else if (strstr(Buffer, "+CMQTTDISC:")) {
        if (strstr(Buffer, "0")) {
            mPico->ConnectionAvailable = false;
            retval = true;
        }
    } else if (strstr(Buffer, "+CMGL:")) {
        mPico->SmsDetected = true;
        retval = true;
    } else if (strstr(Buffer, "+CSQ:") && strstr(Buffer, ",")) {
        if (strtok(Buffer, ": ") != NULL) {
            char *ptr = strtok(NULL, ",");
            mPico->SignalStrength = atoi(ptr);
        }
        retval = true;
    } else if (strstr(Buffer, "+CPIN:")) {
        if (strstr(Buffer, "+CPIN: READY")) {
            mPico->IsSimInserted = true;
        } else {
            mPico->IsSimInserted = false;
        }
        retval = true;
    } else if (strstr(Buffer, "+CSPN:") && strstr(Buffer, ",")) {
        if (strtok(Buffer, ": ") != NULL) {
            char *ptr = strtok(NULL, ",");
            if (ptr != NULL) {
                strcpy(mPico->NetworkProvider, ptr);
            }
        }
        retval = true;
    } else if (strstr(Buffer, "+NETOPEN:")) {
        if (strstr(Buffer, "0")) {
            mPico->IsSocketAvailable = true;
        } else {
            mPico->IsSocketAvailable = false;
        }
        retval = true;
    } else if (strstr(Buffer, "+NETCLOSE:")) {
        mPico->IsSocketAvailable = false;
        retval = true;
    } else if (strstr(Buffer, "+CNMP:")) {
        char *ptr = strtok(Buffer, ": ");
        if (ptr != NULL) {
            ptr = strtok(NULL, "\r");
            if (ptr != NULL) {
                mPico->NetWorkMode = atoi(ptr);
            }
        }
        retval = true;
    } else if (mPico->IsRxTopic) {
        if (PICO_RX_TOPIC_LENGTH - mPico->pRxTopic >= strlen(Buffer)) {
            strcpy(mPico->RxTopic + mPico->pRxTopic, Buffer);
            mPico->pRxTopic += strlen(Buffer);
            retval = true;
        }
    } else if (mPico->IsRxPayload) {
        if (PICO_RX_MSG_LENGTH - mPico->pRxMsg >= strlen(Buffer)) {
            strcpy(mPico->RxMsg + mPico->pRxMsg, Buffer);
            mPico->pRxMsg += strlen(Buffer);
            retval = true;
        }
    } else if (mPico->SmsDetected) {
            strcpy(mPico->SmsMsg, Buffer);
            mPico->SmsDetected = false;
            mPico->is_sms_readable = true;
            retval = true;
    }
    return retval;
}

void at_command_bio_forward(uart_inst_t *DebugUart, uart_inst_t *UartSim) {
    LOG("Disable Uart ISR before comming to the loop...");
    int UART_IRQ = mPico->uartId == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_enabled(UART_IRQ, false);
    uart_set_irq_enables(mPico->uartId, false, false);
    while (1) {
        if (uart_is_readable(DebugUart)) {
            uart_putc(UartSim, uart_getc(DebugUart));
        }
        if (uart_is_readable(UartSim)) {
            uart_putc(DebugUart, uart_getc(UartSim));
        }
    }
}

void sim_send_test_command(uart_inst_t *Uart) {
    char *Cmd = "AT\r";
    char *Buffer = malloc(10);
    char *Out = malloc(30);
    while (1) {
        sim_send_at_command(Uart, Cmd);
        for (int i = 0; i < 4; i++) {
            memset(Buffer, '\0', 10);
            if (sim_receive_at_command(Uart, Buffer, '\n')) {
                sprintf(Out, "Len:%d=>%s\r\n", strlen(Buffer), Buffer);
                LOG(Out);
                if (strstr(Buffer, "OK")) {
                    LOG("Sim is ready\r\n");
                    free(Buffer);
                    free(Out);
                    return;
                }
            }
        }
        LOG("Connecting with SIM failed, try again...\r\n");
        sleep_ms(2000);
    }
}

void sim_forward_command(uart_inst_t *Uart, char *Cmd) {
    char *Buffer = malloc(32);
    sim_send_at_command(Uart, Cmd);
    LOG(Cmd);
    sleep_ms(1000);
    handle_buffer();
    free(Buffer);
}

void sim_send_at_command(uart_inst_t *Uart, char *buffer) {
    uart_puts(Uart, buffer);
}

bool sim_receive_at_command(uart_inst_t *Uart, char *Buffer, char Delimiter) {
    int timeout = 10;
    while (timeout > 0) {
        if (Is_available(&mPico->RingHandler)) {
            int Length =
                Get_String_NonBlocking(&mPico->RingHandler, Buffer, '\n');
            if (Length > 2) break;
        } else {
            sleep_ms(1);
            timeout--;
        }
    }
    if (timeout > 0)
        return true;
    else
        return false;
}

void sim_at_netclose() {
    char *Cmd = "AT+NETCLOSE\r";
    sim_send_at_command(mPico->uartId, Cmd);
    LOG(Cmd);
    sleep_ms(1000);
    handle_buffer();
}

void sim_at_netopen() {
    char *Cmd = "AT+NETOPEN\r";
    sim_send_at_command(mPico->uartId, Cmd);
    LOG(Cmd);
    sleep_ms(1000);
    handle_buffer();
}

bool sim_is_socket_available(void) {
    handle_buffer();
    return mPico->IsSocketAvailable;
}

int sim_get_signal_strength(void) {
    char *Cmd = "AT+CSQ\r";
    sim_send_at_command(mPico->uartId, Cmd);
    LOG(Cmd);
    sleep_ms(500);
    handle_buffer();
    return mPico->SignalStrength;
}

bool sim_is_inserted(void) {
    char *Cmd = "AT+CPIN?\r";
    sim_send_at_command(mPico->uartId, Cmd);
    LOG(Cmd);
    sleep_ms(500);
    handle_buffer();
    return mPico->IsSimInserted;
}

void sim_get_network_provider(void) {
    char *Cmd = "AT+CSPN?\r";
    sim_send_at_command(mPico->uartId, Cmd);
    LOG(Cmd);
    sleep_ms(500);
    handle_buffer();
}

bool sim_configure_network_mode(int Mode) {
    char *Head = "AT+CNMP=";
    char *Cmd = "AT+CNMP?\r";
    char *Buffer = malloc(32);
    sprintf(Buffer, "%s%u\r", Head, Mode);
    sim_send_at_command(mPico->uartId, Buffer);
    LOG(Buffer);
    LOG("Wait until 10s after configuring the network mode\r\n");
    sleep_ms(10000);
    free(Buffer);
    sim_send_at_command(mPico->uartId, Cmd);
    LOG(Cmd);
    mPico->NetWorkMode = -1;
    sleep_ms(1000);
    handle_buffer();
    if (mPico->NetWorkMode == -1) return false;
    else return true;
}

bool sim_check_balance_available(char *Dial) {
    bool retval = false;
    char *Buffer = malloc(64);
    // Check all un-read message
    is_sms_readable();
    // Remove storage
    sms_remove_messages();
    // Make a call
    sprintf(Buffer, "ATD%s;\r", Dial);
    sim_send_at_command(mPico->uartId, Buffer);
    LOG(Buffer);
    LOG("Waiting 5 seconds for sms response...\r\n");
    sleep_ms(5000);
    if (is_sms_readable()) {
        strcpy(mPico->BalanceAvailable, mPico->SmsMsg);
        retval = true;
    }
    return retval;
}

bool mqtt_start() {
    char *Cmd = "AT+CMQTTSTART\r";
    sim_send_at_command(mPico->uartId, Cmd);
    LOG(Cmd);
    sleep_ms(2000);
    mPico->MqttStarted = false;
    handle_buffer();
    if (mPico->MqttStarted)
        return true;
    else
        return false;
}

bool mqtt_is_service_available(void) {
    handle_buffer();
    return mPico->MqttStarted;
}

bool mqtt_stop() {
    char *Cmd = "AT+CMQTTSTOP\r";
    sim_send_at_command(mPico->uartId, Cmd);
    LOG(Cmd);
    sleep_ms(2000);
    handle_buffer();
    if (mPico->MqttStarted == false)
        return true;
    else
        return false;
}

void mqtt_acquire_client(uint8_t ClientIdx, char *ClientId) {
    char *Head = "AT+CMQTTACCQ=";
    char *Buffer = malloc(50);
    sprintf(Buffer, "%s%u,\"%s\"\r", Head, ClientIdx, ClientId);
    sim_send_at_command(mPico->uartId, Buffer);
    LOG(Buffer);
    sleep_ms(2000);
    handle_buffer();
    free(Buffer);
}

void mqtt_release_client(uint8_t ClientIdx) {
    char *Head = "AT+CMQTTREL=";
    char *Buffer = malloc(50);
    sprintf(Buffer, "%s%u\r", Head, ClientIdx);
    sim_send_at_command(mPico->uartId, Buffer);
    LOG(Buffer);
    sleep_ms(2000);
    handle_buffer();
    free(Buffer);
}

bool mqtt_will_topic(uint8_t ClientIdx, char *WillTopic) {
    bool retval = false;
    char *Head = "AT+CMQTTWILLTOPIC=";
    char *Buffer = malloc(100);
    sprintf(Buffer, "%s%u,%d\r", Head, ClientIdx, strlen(WillTopic));
    LOG(Buffer);
    if (mqtt_support_send(Buffer, WillTopic)) retval = true;
    free(Buffer);
    return retval;
}

bool mqtt_will_message(uint8_t ClientIdx, char *Message, int Qos) {
    bool retval = false;
    char *Head = "AT+CMQTTWILLMSG=";
    char *Buffer = malloc(100);
    sprintf(Buffer, "%s%u,%d,%d\r", Head, ClientIdx, strlen(Message), Qos);
    LOG(Buffer);
    if (mqtt_support_send(Buffer, Message)) retval = true;
    free(Buffer);
    return retval;
}

bool mqtt_connect_server(uint8_t ClientIdx, char *Server,
                         uint16_t KeepAliveTime, uint8_t CleanSession) {
    char *Head = "AT+CMQTTCONNECT=";
    char *Buffer = malloc(100);
    sprintf(Buffer, "%s%u,\"%s\",%u,%u\r", Head, ClientIdx, Server,
            KeepAliveTime, CleanSession);
    sim_send_at_command(mPico->uartId, Buffer);
    LOG(Buffer);
    sleep_ms(2000);
    free(Buffer);
    mPico->ConnectionAvailable = false;
    handle_buffer();
    if (mPico->ConnectionAvailable)
        return true;
    else
        return false;
}

bool mqtt_connect_server_authenticate(uint8_t ClientIdx, char *Server,
                                      uint16_t KeepAliveTime,
                                      uint8_t CleanSession, char *User,
                                      char *Password) {
    char *Head = "AT+CMQTTCONNECT=";
    char *Buffer = malloc(128);
    sprintf(Buffer, "%s%u,\"%s\",%u,%u,\"%s\",\"%s\"\r", Head, ClientIdx, Server,
            KeepAliveTime, CleanSession, User, Password);
    sim_send_at_command(mPico->uartId, Buffer);
    LOG(Buffer);
    sleep_ms(2000);
    free(Buffer);
    mPico->ConnectionAvailable = false;
    handle_buffer();
    if (mPico->ConnectionAvailable)
        return true;
    else
        return false;
}

bool mqtt_is_connection_available(void) {
    char *Cmd = "AT+CMQTTCONNECT?\r";
    sim_send_at_command(mPico->uartId, Cmd);
    LOG(Cmd);
    sleep_ms(500);
    handle_buffer();
    return mPico->ConnectionAvailable;
}

bool mqtt_disconnect_server(uint8_t ClientIdx, uint8_t Timeout) {
    bool retval = false;
    char *Head = "AT+CMQTTDISC=";
    char *Buffer = malloc(32);
    sprintf(Buffer, "%s%u,%u\r", Head, ClientIdx, Timeout);
    sleep_ms(2000);
    sim_send_at_command(mPico->uartId, Buffer);
    LOG(Buffer);
    free(Buffer);
    sleep_ms(2000);
    handle_buffer();
    if (mPico->ConnectionAvailable == false) retval = true;
    return retval;
}

bool mqtt_subscribe_topic(uint8_t ClientIdx, char *SubTopic, int Qos) {
    bool retval = false;
    char *Head = "AT+CMQTTSUBTOPIC=";
    char *Buffer = malloc(100);
    sprintf(Buffer, "%s%u,%d,%d\r", Head, ClientIdx, strlen(SubTopic), Qos);
    LOG(Buffer);
    if (mqtt_support_send(Buffer, SubTopic)) retval = true;
    free(Buffer);
    return retval;
}

bool mqtt_subscribe_message(uint8_t ClientIdx, char *Message, int Qos) {
    bool retval = false;
    char *Head = "AT+CMQTTSUB=";
    char *Buffer = malloc(100);
    sprintf(Buffer, "%s%u,%d,%d,1\r", Head, ClientIdx, strlen(Message), Qos);
    if (mqtt_support_send(Buffer, Message)) retval = true;
    free(Buffer);
    return retval;
}
bool mqtt_subscribe(uint8_t ClientIdx) {
    bool retval = false;
    char *Head = "AT+CMQTTSUB=";
    char *Buffer = malloc(32);
    handle_buffer();
    sprintf(Buffer, "%s%u\r", Head, ClientIdx);
    LOG(Buffer);
    sim_forward_command(mPico->uartId, Buffer);
    sleep_ms(500);
    handle_buffer();
    if (mPico->OkDetected) {
        retval = true;
    }
    free(Buffer);
    return true;
}

bool mqtt_unsubscribe_topic(uint8_t ClientIdx, char *UnSubTopic, int Qos) {
    bool retval = false;
    char *Head = "AT+CMQTTUNSUBTOPIC=";
    char *Buffer = malloc(100);
    sprintf(Buffer, "%s%u,%d,%d\r", Head, ClientIdx, strlen(UnSubTopic), Qos);
    LOG(Buffer);
    if (mqtt_support_send(Buffer, UnSubTopic)) retval = true;
    free(Buffer);
    return retval;
}

bool mqtt_unsubscribe_message(uint8_t ClientIdx, char *Message) {
    bool retval = false;
    char *Head = "AT+CMQTTUNSUB=";
    char *Buffer = malloc(100);
    sprintf(Buffer, "%s%u,%d,1\r", Head, ClientIdx, strlen(Message));
    if (mqtt_support_send(Buffer, Message)) retval = true;
    free(Buffer);
    return retval;
}

bool mqtt_public_topic(uint8_t ClientIdx, char *PubTopic) {
    bool retval = false;
    char *Head = "AT+CMQTTTOPIC=";
    char *Buffer = malloc(100);
    sprintf(Buffer, "%s%u,%d\r", Head, ClientIdx, strlen(PubTopic));
    LOG(Buffer);
    if (mqtt_support_send(Buffer, PubTopic)) retval = true;
    free(Buffer);
    return retval;
}

bool mqtt_public_message(uint8_t ClientIdx, char *Message) {
    bool retval = false;
    char *Head = "AT+CMQTTPAYLOAD=";
    char *Buffer = malloc(100);
    sprintf(Buffer, "%s%u,%d\r", Head, ClientIdx, strlen(Message));
    LOG(Buffer);
    if (mqtt_support_send(Buffer, Message)) retval = true;
    free(Buffer);
    return retval;
}

bool mqtt_public_to_server(uint8_t ClientIdx, int Qos, uint8_t PubTimeout) {
    bool retval = false;
    char *Head = "AT+CMQTTPUB=";
    char *Buffer = malloc(100);
    handle_buffer();
    sprintf(Buffer, "%s%u,%d,%d\r", Head, ClientIdx, Qos, PubTimeout);
    LOG(Buffer);
    sim_forward_command(mPico->uartId, Buffer);
    sleep_ms(100);
    mPico->PublicMsgSuc = false;
    handle_buffer();
    if (mPico->PublicMsgSuc) {
        retval = true;
    }
    free(Buffer);
    return true;
}

bool mqtt_configure_context(uint8_t ClientIdx, uint8_t CheckUtf8Flag) {
    bool retval = false;
    char *Head = "AT+CMQTTCFG=\"checkUTF8\",";
    char *Buffer = malloc(100);
    handle_buffer();
    sprintf(Buffer, "%s%u,%u\r", Head, ClientIdx, CheckUtf8Flag);
    LOG(Buffer);
    sim_forward_command(mPico->uartId, Buffer);
    sleep_ms(100);
    handle_buffer();
    if (mPico->OkDetected) {
        retval = true;
    }
    free(Buffer);
    return true;
}

bool mqtt_is_rx_readable() {
    if (mPico->RxDetected == true) {
        mPico->RxDetected = false;
        return true;
    }
    handle_buffer();
    sleep_ms(1000);
    bool retval = mPico->RxDetected;
    mPico->RxDetected = false;
    return retval;
}

void sms_set_mode(uint8_t Mode) {
    char *Head = "AT+CMGF=";
    char *Buffer = malloc(16);
    sprintf(Buffer, "%s%u\r", Head, Mode);
    sim_send_at_command(mPico->uartId, Buffer);
    LOG(Buffer);
    sleep_ms(200);
    handle_buffer();
    sleep_ms(200);
}

bool sms_send(char *PhoneNumber, char *Text) {
    bool retval = false;
    char *Head = "AT+CMGS=";
    char *Buffer = malloc(64);
    sprintf(Buffer, "%s\"%s\"\r", Head, PhoneNumber);
    if (mqtt_support_send(Buffer, Text)) retval = true;
    free(Buffer);
    return retval;
}

bool is_sms_readable() {
    bool retval = false;
    handle_buffer();
    sleep_ms(2000);
    handle_buffer();
    if (mPico->is_sms_readable) {
        retval = true;
    }
    mPico->is_sms_readable = false;
    return retval;
}

bool sms_remove_messages() {
    char *Cmd = " AT+CMGD=,1\r";
    sim_send_at_command(mPico->uartId, Cmd);
    LOG(Cmd);
    sleep_ms(1000);
    handle_buffer();
    sleep_ms(200);
}

void handle_buffer() {
    char *Buffer = malloc(128);
    reset_flags();
    if (Buffer == NULL) return;
    while (Detect_Char(&mPico->RingHandler, '\n')) {
        Get_String_NonBlocking(&mPico->RingHandler, Buffer, '\n');
        picolib_process(Buffer);
    }
    while (Detect_Char(&mPico->RingHandler, '>')) {
        Get_String_NonBlocking(&mPico->RingHandler, Buffer, '>');
        picolib_process(Buffer);
    }
    free(Buffer);
}

bool mqtt_support_send(char *Cmd, char *Message) {
    bool retval = false;
    handle_buffer();
    sim_send_at_command(mPico->uartId, Cmd);
    LOG(Cmd);
    sleep_ms(500);
    handle_buffer();
    if (mPico->MorethanSymbol) {
        sim_send_at_command(mPico->uartId, Message);
        LOG(Message);
        LOG("\r\n");
        sleep_ms(1000);
        handle_buffer();
        if (mPico->OkDetected == true)
            retval = true;
    } else {
        retval = false;
    }
    return retval;
}

void reset_flags() {
    mPico->MorethanSymbol = false;
    mPico->OkDetected = false;
}
