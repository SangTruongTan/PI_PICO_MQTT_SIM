/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : testmain.c
 * @brief          : For testing configuration library.
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
#include "stdlib.h"
#include "string.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE 128

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
Configuration_t mConfigure;
/* Private function prototypes -----------------------------------------------*/
int main(void);
void get_line_input(char *Buffer);
void get_phone_message(char *Phone, char *Message);
void get_phone(char *Phone);
void get_message(char *Message);
void send_sms(const char *Phone, const char *Text, ...);
/* Private user code ---------------------------------------------------------*/
int main(void) {
    char *SourcePhone = "0123456789";
    int count = 1;
    // Initialize
    char *ActualSource = mConfigure.Sender;
    char *Message = mConfigure.SmsBuffer;
    mConfigure.get_back = LOG;
    mConfigure.send_sms = send_sms;
    configure_init(&mConfigure);
    if (strcmp(SourcePhone, "") != 0) {
        strcpy(mConfigure.Sender, SourcePhone);
        strcpy(mConfigure.PhoneNumber[0], SourcePhone);
    }
    //Test case
    // strcpy(mConfigure.PhoneNumber[0], "1234");
    while (1) {
        printf("%d. ", count);
        if (strcmp(SourcePhone, "") == 0) {
            // Configure Source Number
            get_phone_message(ActualSource, Message);
        } else {
            // Don't need to confiure Source Number
            get_message(Message);
        }
        if (strcmp(Message, "exit") == 0 || strcmp(SourcePhone, "exit") == 0)
            return 0;
        printf("\n");
        if (strstr(Message, "value=") != NULL) {
            substr_t text = substr(Message, "value=", "\032");
            float val = atof(text.Target);
            printf("Value:%f\r\n", val);
            alert_status(val, 30.1, 60.2);
            free(text.Target);
        } else {
            configure_main_process();
        }
        count += 1;
    }
    return 0;
}

void get_line_input(char *Buffer) {
    fgets(Buffer, BUFFER_SIZE, stdin);
    strtok(Buffer, "\n");
}

void get_phone_message(char *Phone, char *Message) {
    get_phone(Phone);
    get_message(Message);
}

void get_phone(char *Phone) {
    printf("Your Phone:");
    get_line_input(Phone);
    // printf("The Phone Number is:%s, Length=%ld\n", Phone,
    //        strlen(Phone));
}

void get_message(char *Message) {
    printf("Your message:");
    get_line_input(Message);
    // printf("The message is:%s, Length=%ld\n", Message, strlen(Message));
}

void send_sms(const char *Phone, const char *Text, ...) {
    char *Buffer = calloc(LOG_BUFFER, sizeof(char));
    sprintf(Buffer, "\"%s\"==>%s", Text, Phone);
    LOG(Buffer);
    free(Buffer);
}
