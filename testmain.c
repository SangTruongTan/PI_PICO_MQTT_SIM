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
Identifier_t mIden;
/* Private function prototypes -----------------------------------------------*/
int main(void);
void get_line_input(char *Buffer);
void get_phone_message(char *Phone, char *Message);
void get_phone(char *Phone);
void get_message(char *Message);
bool send_sms(char *Phone, char *Text);
bool main_get_back(char *Text);
/* Private user code ---------------------------------------------------------*/
int main(void) {
    char *SourcePhone = "0123456789";
    int count = 1;
    // Initialize
    char *ActualSource = mConfigure.Sender;
    char *Message = mConfigure.SmsBuffer;
    mConfigure.get_back = main_get_back;
    mConfigure.send_sms = send_sms;
    mConfigure.identify = &mIden;
    configure_init(&mConfigure);
    if (strcmp(SourcePhone, "") != 0) {
        strcpy(mConfigure.Sender, SourcePhone);
        strcpy(mConfigure.identify->PhoneNumber[0], SourcePhone);
    }
    // printf("Size of Struct:%ld\r\n", sizeof(Identifier_t));
    // Test case
    //  strcpy(mConfigure.PhoneNumber[0], "1234");
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
        // if (strstr(Message, "value=") != NULL) {
        //     substr_t text = substr(Message, "value=", "\032");
        //     float val = atof(text.Target);
        //     printf("Value:%f\r\n", val);
        //     alert_status(val, 30.1, 60.2);
        //     free(text.Target);
        // } else {
        //     configure_main_process();
        // }
        configure_main_process();
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

bool send_sms(char *Phone, char *Text) {
    char *Buffer = calloc(LOG_BUFFER, sizeof(char));
    sprintf(Buffer, "\"%s\"==>%s", Text, Phone);
    LOG(Buffer);
    free(Buffer);
    return true;
}

bool main_get_back(char *Text) {
    LOG(Text);
    return true;
}
