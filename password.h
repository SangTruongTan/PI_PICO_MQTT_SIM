/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : password.h
 * @brief          : Header for password.c file.
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
#ifndef __PASSWORD_H_
#define __PASSWORD_H_

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


/* Private defines -----------------------------------------------------------*/
#define PASSWORD_DEBUG 1  // Comment if there is no debuging enable

/* Exported types ------------------------------------------------------------*/
typedef struct {
    char User[32];
    char Password[64];
} password_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
/* Support for writing and reading user and password in FLASH */
/**
 * @brief Write the Mqtt indentifier to the flash memory
 * @param Identifier The pointer of Identifier
 * @param User The string of User.
 * @param Password The string of Password.
 * @retval void
 */
void pico_write_identifier(password_t *Identifier, char *User, char *Password);

/**
 * @brief Read the Mqtt indentifier from the flash memory
 * @return true Read the identifier successfully.
 * @return false Unsuccessfully in reading.
 */
bool pico_read_identifier(void);

/**
 * @brief Write data to flash with static offset
 * @param StructData The pointer of struct data
 * @param Size The size of data
 */
void pico_write_data(void *StructData, uint16_t Size);

/**
 * @brief Read data from flash
 * @return void* Return the pointer of data
 */
void *pico_read_data(void);

#endif /* __PICO_LIBRARY_H_ */
