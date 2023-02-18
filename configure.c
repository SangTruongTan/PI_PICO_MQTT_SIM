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
#define LOG_OUTPUT(X, Y) \
    uart_puts(uart0, Y); \
    uart_puts(uart0, "\r\n")
#else
#define LOG_OUTPUT(X, ...) printf(X, __VA_ARGS__);
#endif

#define CONFIGURE_MASTER_PHONE_SYNTAX_HEAD "master=\""
#define CONFIGURE_MASTER_PHONE_SYNTAX \
    "*Syntax: <master=\"xxxxxxxxxx\";PIN=\"yyyyyy\">"
#define CONFIGURE_DEFAULT_PIN "PIN=\"082308\""
#define CONFIGURE_LIST                                        \
    "1. Add phone number:<addphone=\"xxxxxxxxxx\";pos=y>\r\n" \
    "2. Delete phone:<deletephone=y>\r\n"                     \
    "3. List phone:<listphone=?>\r\n"
#define CONFIGURE_ADD_PHONE_SYNTAX \
    "Check syntax again:<addphone=\"xxxxxxxxxx\";pos=y>"
#define CONFIGURE_DELETE_PHONE_SYNTAX "Check syntax again:<deltephone=y>"
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

void process_configure_sms(void) {
    LOGUF("In Process configure");
    if (strstr(pConfigure->SmsBuffer, "configure=?")) {
        pConfigure->get_back(CONFIGURE_LIST);
    } else if (strstr(pConfigure->SmsBuffer, "listphone=?")) {
        list_phone();
    } else if (strstr(pConfigure->SmsBuffer, "addphone=\"")) {
        if (add_phone() == false) {
            pConfigure->get_back(CONFIGURE_ADD_PHONE_SYNTAX);
        }
    } else if (strstr(pConfigure->SmsBuffer, "deletephone=") != NULL) {
        if (delete_phone() == false) {
            pConfigure->get_back(CONFIGURE_DELETE_PHONE_SYNTAX);
        }
    } else {
        pConfigure->get_back(CONFIGURE_LIST);
    }
}

void list_phone(void) {
    char *Buffer = calloc(LOG_BUFFER, sizeof(char));
    char *BufferLine = calloc(32, sizeof(char));
    if (Buffer != NULL && BufferLine != NULL) {
        LOGUF("In list phone");
        for (int i = 1; i < PHONE_LIST; i++) {
            sprintf(BufferLine, "%d. %s\r\n", i, pConfigure->PhoneNumber[i]);
            strcat(Buffer, BufferLine);
        }
        pConfigure->get_back(Buffer);
    } else {
        LOGUF("Not enough space");
    }
    free(Buffer);
    free(BufferLine);
}

bool add_phone(void) {
    bool retval = false;
    char *Phone = calloc(PHONE_LENGTH + 1, sizeof(char));
    char *Buffer = calloc(LOG_BUFFER, sizeof(char));
    int iPos;
    int pos1;
    int pos2;
    int pos3;
    char *cPos;
    if (Phone == NULL || Buffer == NULL) {
        LOGUF("Not enough storage");
        return false;
    }
    cPos = strchr(pConfigure->SmsBuffer, '\"');
    pos1 = cPos - pConfigure->SmsBuffer;
    LOGF("Pos1 =%d", pos1);
    cPos = strchr(pConfigure->SmsBuffer + pos1 + 1, '\"');
    if (cPos != NULL) {
        pos2 = cPos - pConfigure->SmsBuffer;
        if (pos2 - pos1 - 1 == PHONE_LENGTH) {
            LOGF("Pos1 =%d", pos2);
            memcpy(Phone, pConfigure->SmsBuffer + pos1 + 1, pos2 - pos1 - 1);
            cPos = strstr(pConfigure->SmsBuffer, "pos=");
            if (cPos != NULL) {
                pos3 = cPos - pConfigure->SmsBuffer + 4;
                LOGF("Pos3 =%d", pos3);
                if (pConfigure->SmsBuffer[pos3] >= '0' &&
                    pConfigure->SmsBuffer[pos3] <= '9') {
                    iPos = pConfigure->SmsBuffer[pos3] - '0';
                    sprintf(Buffer, "Add phone successfully:Phone=%s,Pos=%d",
                            Phone, iPos);
                    LOGUF(Buffer);
                    pConfigure->get_back(Buffer);
                    strcpy(pConfigure->PhoneNumber[iPos], Phone);
                    retval = true;
                }
            }
        }
    }
    free(Phone);
    free(Buffer);
    return retval;
}

bool delete_phone(void) {
    bool retval = false;
    char *cPos = strchr(pConfigure->SmsBuffer, '=');
    int iPos = cPos - pConfigure->SmsBuffer + 1;
    char *Buffer = calloc(32, sizeof(char));
    if (Buffer == NULL) {
        LOGUF("Not enough storage");
        return false;
    }
    if (pConfigure->SmsBuffer[iPos] >= '0' &&
        pConfigure->SmsBuffer[iPos] <= '9') {
        int iPhonePos = pConfigure->SmsBuffer[iPos] - '0';
        memset(pConfigure->PhoneNumber[iPhonePos], '\0', PHONE_LENGTH + 1);
        sprintf(Buffer, "Delete Phone %d successfully", iPhonePos);
        LOGUF(Buffer);
        pConfigure->get_back(Buffer);
        retval = true;
    } else {
        retval = false;
    }
    free(Buffer);
    return retval;
}

void LOG_DETAILS(const char *File, const char *Func, int Line,
                 const char *format, ...) {
    char *Buffer = calloc(LOG_BUFFER, sizeof(char));
    sprintf(Buffer, "%s:%d:%s() =>", File, Line, Func);
    LOG_OUTPUT("%s", Buffer);
    va_list vl;
    va_start(vl, format);
    vsprintf(Buffer, format, vl);
    LOG_OUTPUT("%s\r\n", Buffer);
    va_end(vl);
    free(Buffer);
}

void LOG(const char *format, ...) {
    char *Buffer = calloc(LOG_BUFFER, sizeof(char));
    va_list vl;
    va_start(vl, format);
    vsprintf(Buffer, format, vl);
    LOG_OUTPUT("%s\r\n", Buffer);
    va_end(vl);
    free(Buffer);
}