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
#include "string.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#ifdef PICO_LIB_DEBUG
#define LOG(X) uart_puts(uart0, X)
#else
#define LOG(X)
#endif

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
    if (strlen(Buffer) > 2) {
        LOG(Buffer);
        if (strstr(Buffer, "OK")) {
            retval = false;
        } else if (strstr(Buffer, "+CMQTTSTART:")) {
            if (strstr(Buffer, "0")) {
                mPico->MqttStarted = true;
                retval = true;
            }
        } else if (strstr(Buffer, "+CMQTTCONNECT:")) {
            if (strstr(Buffer, ",0")) {
                mPico->ConnectionAvailable = true;
                retval = true;
            }
        } else if (strstr(Buffer, "+CMQTTRXTOPIC:")) {
            mPico->RxTopic = true;
            retval = true;
        } else if (strstr(Buffer, "+CMQTTRXPAYLOAD:")) {
            mPico->RxPayload = true;
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
        }
    } else if (strstr(Buffer, ">")) {
        LOG(Buffer);
        mPico->MorethanSymbol = true;
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
    while (Detect_Char(&mPico->RingHandler, '\n')) {
        if (sim_receive_at_command(mPico->uartId, Buffer, '\n')) {
            LOG(Buffer);
        }
    }
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

void sim_at_netclose(uart_inst_t *Uart) {
    char *Cmd = "AT+NETCLOSE\r";
    char *Buffer = malloc(30);
    sim_send_at_command(Uart, Buffer);
    LOG(Cmd);
    int i = 2000;
    while (i > 0) {
        sleep_ms(1);
        handle_buffer();
        i--;
    }
    free(Buffer);
}

void sim_at_netopen(uart_inst_t *Uart) {
    char *Cmd = "AT+NETOPEN\r";
    char *Buffer = malloc(30);
    sim_send_at_command(Uart, Buffer);
    LOG(Cmd);
    int i = 2000;
    while (i > 0) {
        sleep_ms(1);
        handle_buffer();
        i--;
    }
    sleep_ms(100);
    free(Buffer);
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

bool mqtt_stop() {
    char *Cmd = "AT+CMQTTSTOP\r";
    sim_send_at_command(mPico->uartId, Cmd);
    LOG(Cmd);
    sleep_ms(1000);
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
    free(Buffer);
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

bool mqtt_disconnect_server(uint8_t ClientIdx, uint8_t Timeout) {
    bool retval = false;
    char *Head = "AT+CMQTTDISC=";
    char *Buffer = malloc(100);
    sprintf("%s%u,%u\r", Head, ClientIdx, Timeout);
    sim_send_at_command(mPico->uartId, Buffer);
    LOG(Buffer);
    free(Buffer);
    sleep_ms(500);
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
    sprintf(Buffer, "%s%u,%d,%d\r", Head, ClientIdx, Qos, PubTimeout);
    LOG(Buffer);
    sim_forward_command(mPico->uartId, Buffer);
    free(Buffer);
    return true;
}

bool sms_send(char *PhoneNumber, char *Text) {
    bool retval = false;
    char *Head = "AT+CMGS=";
    char *Buffer = malloc(50);
    sprintf(Buffer, "%s%s\r", Head, PhoneNumber);
    if (mqtt_support_send(Buffer, Text)) retval = true;
    free(Buffer);
    return retval;
}

void handle_buffer() {
    char *Buffer = malloc(128);
    while (Detect_Char(&mPico->RingHandler, '\n')) {
        Get_String_NonBlocking(&mPico->RingHandler, Buffer, '\n');
        picolib_process(Buffer);
    }
    free(Buffer);
}

bool mqtt_support_send(char *Cmd, char *Message) {
    handle_buffer();
    sim_send_at_command(mPico->uartId, Cmd);
    sleep_ms(100);
    mPico->MorethanSymbol = false;
    if (mPico->MorethanSymbol) {
        sim_send_at_command(mPico->uartId, Message);
        return true;
    } else {
        return false;
    }
}
