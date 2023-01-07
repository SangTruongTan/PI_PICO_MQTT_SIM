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
RingHandler_t mRingHandler;
/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
void picolib_init(PicoLibrary_t *PicoParam) {
    mPico = PicoParam;
    // uart Ring Buffer initialize
    mRingHandler.uartId = mPico->uartId;
    Ring_Init(&mRingHandler, mPico->baudrate, mPico->txPin, mPico->rxPin);
    // Sim Initialize
    sim_init(mPico->uartId);
}

void sim_init(uart_inst_t *Uart) {
    LOG("Initializing SIM module\r\n");
    sim_send_test_command(Uart);
    sim_forward_command(Uart, "AT&F");
    sim_forward_command(Uart, "ATE0");
    sim_forward_buffer(Uart);
    sim_at_netclose(Uart);
    sleep_ms(2000);  // Wait 2s before turning on network
    sim_at_netopen(Uart);
}

void at_command_bio_forward(uart_inst_t *DebugUart, uart_inst_t *UartSim) {
    char Output[2] = {'\0', '\0'};
    while (1) {
        if (uart_is_readable(DebugUart)) {
            uart_putc(UartSim, uart_getc(DebugUart));
            sim_forward_buffer(UartSim);
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
                if (strcmp(Buffer, "OK\r\n")) {
                    LOG("Sim is ready\r\n");
                    break;
                }
            }
        }
        LOG("Connecting with SIM failed, try again...\r\n");
        sleep_ms(2000);
    }
    free(Buffer);
    free(Out);
}

void sim_forward_command(uart_inst_t *Uart, char *Cmd) {
    char *Buffer = malloc(10);
    sim_send_at_command(Uart, Cmd);
    LOG(Cmd);
    for (int i = 0; i < 4; i++) {
        memset(Buffer, '\0', 10);
        if (sim_receive_at_command(Uart, Buffer, '\n')) {
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
        if (Is_available(&mRingHandler)) {
            int Length = Get_String_NonBlocking(&mRingHandler, Buffer, '\n');
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

void sim_forward_buffer(uart_inst_t *Uart) {
    char Output[2] = {'\0', '\0'};
    while (uart_is_readable(Uart)) {
        Output[0] = uart_getc(Uart);
        LOG(Output);
    }
}

void sim_at_netclose(uart_inst_t *Uart) {
    char *Cmd = "AT+NETCLOSE\r";
    char *Buffer = malloc(30);
    sim_send_at_command(Uart, Buffer);
    LOG(Cmd);
    int i = 2000;
    while (i > 0) {
        sleep_ms(1);
        sim_forward_buffer(Uart);
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
        sim_forward_buffer(Uart);
        i--;
    }
    sleep_ms(100);
    free(Buffer);
}

void mqtt_start() {
    char *Cmd = "AT+CMQTTSTART\r";
    sim_send_at_command(mPico->uartId, Cmd);
    LOG(Cmd);
    sleep_ms(2000);
    sim_forward_buffer(mPico->uartId);
}

void mqtt_acquire_client(uint8_t ClientIdx, char *ClientId) {
    char *Head = "AT+CMQTTACCQ=";
    char *Buffer = malloc(50);
    sprintf(Buffer, "%s%u,\"%s\"\r", Head, ClientIdx, ClientId);
    sim_send_at_command(mPico->uartId, Buffer);
    LOG(Buffer);
    sleep_ms(2000);
    sim_forward_buffer(mPico->uartId);
    free(Buffer);
}

void mqtt_connect_server(uint8_t ClientIdx, char *Server,
                         uint16_t KeepAliveTime, uint8_t CleanSession) {
    char *Head = "AT+CMQTTCONNECT=";
    char *Buffer = malloc(100);
    sprintf(Buffer, "%s%u,\"%s\",%u,%u\r", Head, ClientIdx, Server, KeepAliveTime,
            CleanSession);
    sim_send_at_command(mPico->uartId, Buffer);
    LOG(Buffer);
    sleep_ms(2000);
    sim_forward_buffer(mPico->uartId);
    free(Buffer);
}
