/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : configure.c
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
#include "configure.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#ifdef PICO_DEVICES
#define LOG_OUTPUT(X) uart_puts(uart0, X)
#else
#define LOG_OUTPUT(X) printf(X);
#endif

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
Configuration_t *pConfigure;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
void configure_init(Configuration_t *Configure) { pConfigure = Configure; }

bool is_receive_sms(void) {
    // Fix me: Detect when receive the new sms
    return true;
}

bool is_master_number(void) {
    if (strcmp(pConfigure->Sender, pConfigure->PhoneNumber[0]) == 0)
        return true;
    return false;
}

bool is_master_empty(void) {
    if (pConfigure->PhoneNumber[0][0] = 0) return true;
    return false;
}

void add_master_number(void) {}

void process_configure_sms(void) {}

void LOG_DETAILS(const char *File, const char *Func, int Line,
                 const char *format, ...) {
    char *Buffer = calloc(LOG_BUFFER, sizeof(char));
    sprintf(Buffer, "%s:%d:%s() =>", File, Line,
            Func);
    LOG_OUTPUT(Buffer);
    va_list vl;
    va_start(vl, format);
    vsprintf(Buffer, format, vl);
    LOG_OUTPUT(Buffer);
    LOG_OUTPUT("\r\n");
    va_end(vl);
    free(Buffer);
    free(Buffer);
}

void LOG(const char *format, ...) {
    char *Buffer = calloc(LOG_BUFFER, sizeof(char));
    va_list vl;
    va_start(vl, format);
    vsprintf(Buffer, format, vl);
    LOG_OUTPUT(Buffer);
    LOG_OUTPUT("\r\n");
    va_end(vl);
    free(Buffer);
}