/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : uartRingBuffer.h
 * @brief          : Header for uartRingBuffer.c file.
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
#ifndef __UARTRINGBUFFER_H_
#define __UARTRINGBUFFER_H_

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"

#include "hardware/uart.h"
#include "pico/stdlib.h"
#include "hardware/irq.h"

/* Private defines -----------------------------------------------------------*/
#define RING_BUFFER_SIZE 256
#define RING_RX_BUFFER 64

// define UART_RING_BUFFER_DEBUG 1

/* Exported types ------------------------------------------------------------*/
typedef struct RingHandler_t {
    bool enable;
    uart_inst_t *uartId;
    int Head;
    int Tail;
    uint8_t MainBuffer[RING_BUFFER_SIZE];
    uint8_t RxBuffer[RING_RX_BUFFER];
} RingHandler_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Initialize the Ring Buffer.
 * @param Handler The pointer of the Ring Buffer Handler.
 * @param baudrate Uart baudrate.
 * @param txPin Tx Pin.
 * @param rxPin Rx Pin.
 * @retval void
 */
void Ring_Init(RingHandler_t *RingHandler, uint baudrate, uint txPin,
               uint rxPin);

/**
 * @brief Detect the particular character.
 * @param RingHandler The Ring Handler.
 * @param Deli The character needs to find.
 * @retval bool True means detected and False is remaining case.
 */
bool Detect_Char(RingHandler_t *RingHandler, const char Deli);

/**
 * @brief Check the data available or not.
 * @param RingHandler The Ring Handler.
 * @retval int: zero meant empty, > 0 mean available data.
 */
int Is_available(RingHandler_t *RingHandler);

/**
 * @brief Get char from the buffer.
 * @param RingHandler The Ring Handler.
 * @retval uint8_t
 */
uint8_t get_char(RingHandler_t *RingHandler);

/**
 * @brief Get string from the buffer.
 * @note This function is difference with the get_peek function. This function
 * will increse the tail of the buffer.
 * @param RingHandler The Ring Handler.
 * @param Buffer The pointer of the buffer.
 * @param Size The number char to read.
 * @retval int The number of char in the buffer.
 */
int get_string(RingHandler_t *RingHandler, uint8_t *Buffer, uint16_t Size);

/**
 * @brief Get peek string from the buffer.
 * @param RingHandler The Ring Handler.
 * @param Buffer The pointer of the buffer.
 * @param Size The number char to read.
 * @retval int The number of char in the buffer.
 */
int get_peek(RingHandler_t *RingHandler, uint8_t *Buffer, uint16_t Size);

/**
 * @brief The function handle the UART interrupt.
 * @retval void
 */
void uart_RxEventCallback();

/**
 * @brief Restart the Uart.
 * @param RingHandler The pointer of the Ring Handler.
 * @retval void
 */
void Ring_Restart_Uart(RingHandler_t *RingHandle);

/**
 * @brief Increase the Tail.
 * @param Ring The pointer of the Ring.
 * @param Size The size to increase.
 * @param void
 */
void Ring_Increase_Tail(RingHandler_t *Ring, uint16_t Size);

/**
 * @brief Copy the main buffer to the user buffer.
 * @param Ring The pointer of the Ring Handler.
 * @param Buffer The pointer of the buffer.
 * @param Size The size to copy.
 * @retval void
 */
void Ring_Copy_Buffer(RingHandler_t *Ring, uint8_t *Buffer, uint16_t Size);

/**
 * @brief Calculate the Index of the character.
 * @param Buffer The pointer of the buffer.
 * @param Chr The char to find.
 * @param Size The size of the buffer to find.
 * @retval int The position of the Character.
 */
int IndexOf(char *Buffer, char Chr, size_t Size);

/**
 * @brief Calculate the Index of the string.
 * @note Must be confirm the buffer finish with the '\0' char.
 * @param Buffer The pointer of the buffer.
 * @param Chr The char to find.
 * @retval int The position of the Character.
 */
int IndexOfString(uint8_t *Buffer, char Chr);

/**
 * @brief Get the string contains the terminate character with Blocking.
 * @param Ring The Ring Handler.
 * @param Buffer The reading buffer.
 * @param Terminate The terminated character.
 * @retval int The length of the string was read. -1 meant timeout or Error.
 */
int Get_String_NonBlocking(RingHandler_t *Ring, char *Buffer,
                           const char Terminate);

/**
 * @brief Get the String with the specific termante.
 * @param Buffer The pointer to the buffer.
 * @param Source The pointer of the Source.
 * @param Terminate The terminate character.
 * @param Size The size of the buffer.
 * @retval int The length of the string. Return -1 when can not find the
 * termanated.
 */
int Get_String_Util(char *Buffer, char *Source, const char Terminate,
                    size_t Size);

#endif /* __UARTRINGBUFFER_H_ */
