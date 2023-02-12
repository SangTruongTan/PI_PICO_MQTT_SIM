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
#define PHONE_LENGTH 10

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
Configuration_t mConfigure;
/* Private function prototypes -----------------------------------------------*/
int main(void);
void get_line_input(char *Buffer);
void get_phone_message(char *Phone, char *Message);
void get_phone(char *Phone);
void get_message(char *Message);
/* Private user code ---------------------------------------------------------*/
int main(void) {
    char *SourcePhone = "";
    char *ActualSource = calloc(PHONE_LENGTH, sizeof(char));
    char *Message = calloc(BUFFER_SIZE, sizeof(char));
    int count = 1;
    while (1) {
        printf("%d. ", count);
        if (strcmp(SourcePhone, "") == 0) {
            // Configure Source Number
            get_phone_message(ActualSource, Message);
        } else {
            // Don't need to confiure Source Number
            get_message(Message);
        }
        printf("\n");
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
    printf("The Phone Number is:%s, Length=%d\n", Phone,
           strlen(Phone));
}

void get_message(char *Message) {
    printf("Your message:");
    get_line_input(Message);
    printf("The message is:%s, Length=%d\n", Message, strlen(Message));
}
