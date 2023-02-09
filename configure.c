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

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
Configuration_t *pConfigure;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
void configure_init(Configuration_t *Configure) {
    pConfigure = Configure;
}
