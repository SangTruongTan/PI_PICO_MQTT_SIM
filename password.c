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
#include "password.h"

/* Private includes ----------------------------------------------------------*/
#include <math.h>
#include <string.h>

#include "hardware/flash.h"
#include "hardware/uart.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#ifdef PASSWORD_DEBUG
#define LOG(X) uart_puts(uart0, X)
#else
#define LOG(X)
#endif

#define FLASH_OFFSET (PICO_FLASH_SIZE_BYTES - 2 * FLASH_SECTOR_SIZE)

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
password_t *pPass;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
void pico_write_identifier(password_t *Identifier, char *User, char *Password) {
    pPass = Identifier;
    char *Buffer = calloc(FLASH_PAGE_SIZE / sizeof(char), sizeof(char));
    if (Buffer != NULL) {
        sprintf(Buffer, "%s,%s\r\0", User, Password);
        LOG("Data write to flash:");
        LOG(Buffer);
        LOG("\r\n");
        LOG("===>Erase the flash sector\r\n");
        flash_range_erase(FLASH_OFFSET, FLASH_SECTOR_SIZE);
        LOG("===>Write data to flash page\r\n");
        flash_range_program(FLASH_OFFSET, (uint8_t *)Buffer, FLASH_PAGE_SIZE);
    }
    free(Buffer);
}

bool pico_read_identifier(void) {
    bool retval = false;
    char *Buffer = calloc(FLASH_PAGE_SIZE / sizeof(char), sizeof(char));
    if (Buffer != NULL) {
        char *tptr = (char *)(XIP_BASE + FLASH_OFFSET);
        strcpy(Buffer, tptr);
        if (strstr(Buffer, ",") && strstr(Buffer, "\r")) {
            char *ptr = strtok(Buffer, ",");
            strcpy(pPass->User, ptr);
            ptr = strtok(Buffer, "\r");
            strcpy(pPass->Password, ptr);
            retval = true;
        }
    }
    return retval;
}

void pico_write_data(void *StructData, uint16_t Size) {
    LOG("Remove data with number of sector:");
    int NumberOfSectors = ceil((float)Size / FLASH_SECTOR_SIZE);
    LOG(NumberOfSectors);
    LOG("\r\n");
    for (int i = 0; i < NumberOfSectors; i++) {
        LOG("===>Erase the flash sector\r\n");
        flash_range_erase(FLASH_OFFSET, FLASH_SECTOR_SIZE);
        LOG("===>Write data to flash page\r\n");
    }
    int NumberOfPages = ceil((float)Size / FLASH_PAGE_SIZE);
    LOG("Number of Pages to write:");
    LOG(NumberOfPages);
    LOG("\r\n");
    for (int i = 0; i < NumberOfPages; i++) {
        LOG("===>Write data to flash page\r\n");
        flash_range_program(FLASH_OFFSET + i * FLASH_PAGE_SIZE,
                            (uint8_t *)StructData + i * FLASH_PAGE_SIZE,
                            FLASH_PAGE_SIZE);
    }
}

void *pico_read_data(void) {
    char *tptr = (char *)(XIP_BASE + FLASH_OFFSET);
    return tptr;
}
