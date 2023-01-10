/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : uartRingBuffer.c
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
#include "uartRingBuffer.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#ifdef UART_RING_BUFFER_DEBUG
#define LOG(X) uart_puts(uart0, X)
#else
#define LOG(X)
#endif

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
RingHandler_t *mHandler;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
void Ring_Init(RingHandler_t *RingHandler, uint baudrate, uint txPin,
               uint rxPin) {
    mHandler = RingHandler;
    // Initialize for Pi Pico Uart
    uart_init(mHandler->uartId, baudrate);
    // Port gpio pins as uart pins
    gpio_set_function(txPin, GPIO_FUNC_UART);
    gpio_set_function(rxPin, GPIO_FUNC_UART);
    uart_set_hw_flow(mHandler->uartId, false, false);
    uart_set_fifo_enabled(mHandler->uartId, false);
    // And set up and enable the interrupt handlers
    uint UART_IRQ = mHandler->uartId == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, uart_RxEventCallback);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(mHandler->uartId, true, false);

    // Reset for the ring buffer
    mHandler->Head = 0;
    mHandler->Tail = 0;
    memset(mHandler->MainBuffer, '\0', RING_BUFFER_SIZE);
    memset(mHandler->RxBuffer, '\0', RING_RX_BUFFER);
}

bool Detect_Char(RingHandler_t *RingHandler, const char Deli) {
    bool retval = false;
    uint8_t *Temp = NULL;
    int Availabe;
    Availabe = Is_available(RingHandler);
    if (Availabe > 0) {
        Temp = malloc(RING_BUFFER_SIZE);
        if (Temp == NULL) return -1;
        get_peek(RingHandler, Temp, Availabe);
        int Index = IndexOf(Temp, Deli, Availabe);
        if (Index != -1) {
            retval = true;
        }
        free(Temp);
    }
    return retval;
}

int Is_available(RingHandler_t *RingHandler) {
    uint16_t Head = RingHandler->Head;
    uint16_t Tail = RingHandler->Tail;
    int available = 0;
    if (Head > Tail) {
        available = Head - Tail;
    } else if (Head < Tail) {
        available = RING_BUFFER_SIZE - Tail + Head;
    }
    return available;
}

uint8_t get_char(RingHandler_t *RingHandler) {
    uint8_t retval = '\0';
    uint16_t Tail = RingHandler->Tail;
    if (RingHandler->Head != Tail) {
        retval = RingHandler->MainBuffer[Tail];
        Ring_Increase_Tail(RingHandler, 1);
    }

    return retval;
}

int get_string(RingHandler_t *RingHandler, uint8_t *Buffer, uint16_t Size) {
    int DataCanRead = get_peek(RingHandler, Buffer, Size);
    if (DataCanRead != 0) {
        Ring_Increase_Tail(RingHandler, DataCanRead);
    }
    return DataCanRead;
}

int get_peek(RingHandler_t *RingHandler, uint8_t *Buffer, uint16_t Size) {
    int available = Is_available(RingHandler);
    int DataToRead = Size;
    if (Size > available) {
        DataToRead = available;
    }
    Ring_Copy_Buffer(RingHandler, Buffer, DataToRead);
    return DataToRead;
}

void uart_RxEventCallback() {
    int Size = 0;
    // Read data from uart termianal
    while (uart_is_readable(mHandler->uartId) &&
           Size < RING_RX_BUFFER) {
        mHandler->RxBuffer[Size] = uart_getc(mHandler->uartId);
        Size++;
    }
    if (Size == 0) return;
    int pos = mHandler->Head;
    // Calculate remaining size
    int Remain = mHandler->Tail - mHandler->Head;
    if (Remain <= 0) {
        Remain += RING_BUFFER_SIZE;
    }
    // Check if the new data exceeds the remain buffer size
    if (pos + Size >= RING_BUFFER_SIZE) {
        uint16_t DataToCopy = RING_BUFFER_SIZE - pos;
        memcpy((mHandler->MainBuffer + pos), mHandler->RxBuffer, DataToCopy);
        pos = 0;
        memcpy(mHandler->MainBuffer, (mHandler->RxBuffer + DataToCopy),
               Size - DataToCopy);
        mHandler->Head = Size - DataToCopy;
    } else {
        memcpy(mHandler->MainBuffer + pos, mHandler->RxBuffer, Size);
        mHandler->Head = pos + Size;
    }
    // Increase the Tail pointer above the Head 1
    if (Size >= Remain) {
        mHandler->Tail = mHandler->Head + 1;
    }
}

void Ring_Increase_Tail(RingHandler_t *Ring, uint16_t Size) {
    uint16_t Tail = Ring->Tail;
    if (Tail + Size >= RING_BUFFER_SIZE) {
        Ring->Tail = Tail + Size - RING_BUFFER_SIZE;
    } else {
        Ring->Tail += Size;
    }
}

void Ring_Copy_Buffer(RingHandler_t *Ring, uint8_t *Buffer, uint16_t Size) {
    uint16_t Tail = Ring->Tail;
    uint16_t Remain = RING_BUFFER_SIZE - Tail;
    if (Size > Remain) {
        memcpy(Buffer, (const void *)&Ring->MainBuffer[Tail], Remain);
        memcpy(Buffer + Remain, (const void *)Ring->MainBuffer, Size - Remain);
    } else {
        memcpy(Buffer, (const void *)&Ring->MainBuffer[Tail], Size);
    }
}

int IndexOf(char *Buffer, char Chr, size_t Size) {
    int Index = -1;
    void *Result = memchr((const void *)Buffer, Chr, Size);
    if (Result == NULL) {
        return -1;
    }
    Index = Result - (void *)Buffer;
    return Index;
}

int IndexOfString(uint8_t *Buffer, char Chr) {
    int Index = IndexOf(Buffer, Chr, strlen((const char *)Buffer));
    return Index;
}

int Get_String_NonBlocking(RingHandler_t *Ring, char *Buffer,
                           const char Terminate) {
    char *Temp = NULL;
    int Length = -1;
    int Availabe;
    Temp = malloc(RING_BUFFER_SIZE);
    if (Temp == NULL) return -1;
    Availabe = Is_available(Ring);
    if (Availabe > 0) {
        get_peek(Ring, Temp, Availabe);
        Length = Get_String_Util(Buffer, Temp, Terminate, Availabe);
        Ring_Increase_Tail(Ring, Length);
    }
    free(Temp);
    return Length;
}

int Get_String_Util(char *Buffer, char *Source, const char Terminate,
                    size_t Size) {
    int Index = IndexOf(Source, Terminate, Size);
    if (Index >= 0) {
        memset(Buffer + Index + 1, '\0', 1);
        memcpy(Buffer, Source, Index + 1);
    }
    return Index + 1;
}
