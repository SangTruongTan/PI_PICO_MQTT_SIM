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

#define CONFIGURE_MASTER_PHONE_SYNTAX_HEAD "master=\""
#define CONFIGURE_MASTER_PHONE_SYNTAX "*Syntax: <master=\"xxxxxxxxxx\";PIN=\"yyyyyy\">"
#define CONFIGURE_DEFAULT_PIN "PIN=\"082308\""

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
Configuration_t *pConfigure;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
void configure_init(Configuration_t *Configure) { pConfigure = Configure; }

void configure_main_process(void) {
    if (is_receive_sms()) {
        LOGUF("Receive new sms");
        if (is_master_number()) {
            process_configure_sms();
        } else {
            if (is_master_empty()) {
                LOGUF("Master Phone is empty");
                add_master_number();
            } else {
                LOGUF("It's not the master phone!!!");
            }
        }
    }
}

bool is_receive_sms(void) {
    // Fix me: Detect when receive the new sms
    return true;
}

bool is_master_number(void) {
    LOGF("Sender:%s", pConfigure->Sender);
    LOGF("Master:%s", pConfigure->PhoneNumber[0]);
    if (strcmp(pConfigure->Sender, pConfigure->PhoneNumber[0]) == 0)
        return true;
    return false;
}

bool is_master_empty(void) {
    LOGF("%s", pConfigure->PhoneNumber[0]);
    if (pConfigure->PhoneNumber[0][0] == 0) {
        return true;
    }
    return false;
}

void add_master_number(void) {
    bool is_valid = false;
    LOGUF("In Add master");
    if (strstr(pConfigure->SmsBuffer, CONFIGURE_MASTER_PHONE_SYNTAX_HEAD)) {
        LOGUF("Right head syntax");
        if (strstr(pConfigure->SmsBuffer, CONFIGURE_DEFAULT_PIN)) {
            LOGUF("Right PIN");
            char *Buffer = calloc(SMS_MESSAGE_LENGTH, sizeof(char));
            strcpy(Buffer, pConfigure->SmsBuffer);
            strtok(Buffer, "\"");
            char *tail = strtok(NULL, "\"");
            if (strlen(tail) == PHONE_LENGTH) {
                strcpy(pConfigure->PhoneNumber[0], tail);
                LOGF("Add Master Phone successfull with:%s",
                    pConfigure->PhoneNumber[0]);
                sprintf(Buffer, "*Configure Master Phone successfully:\"%s\"",
                        pConfigure->PhoneNumber[0]);
                pConfigure->get_back(Buffer);
                is_valid = true;
            }
            free(Buffer);
        } else {
            LOGUF("Wrong PIN");
            pConfigure->get_back("*Wrong PIN!");
            is_valid = true;
        }
    }
    if (is_valid == false) {
        pConfigure->get_back(CONFIGURE_MASTER_PHONE_SYNTAX);
    }
}

void process_configure_sms(void) { LOGUF("In Process configure"); }

void LOG_DETAILS(const char *File, const char *Func, int Line,
                 const char *format, ...) {
    char *Buffer = calloc(LOG_BUFFER, sizeof(char));
    sprintf(Buffer, "%s:%d:%s() =>", File, Line, Func);
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