/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#define FDCAN

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#ifndef FDCAN
  #define CAN_TYPE CAN_HandleTypeDef
#else
  #define CAN_TYPE FDCAN_HandleTypeDef
#endif


/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DOOR9_Pin GPIO_PIN_0
#define DOOR9_GPIO_Port GPIOF
#define DOOR8_Pin GPIO_PIN_1
#define DOOR8_GPIO_Port GPIOF
#define DOOR7_Pin GPIO_PIN_2
#define DOOR7_GPIO_Port GPIOF
#define DOOR6_Pin GPIO_PIN_3
#define DOOR6_GPIO_Port GPIOF
#define DOOR5_Pin GPIO_PIN_4
#define DOOR5_GPIO_Port GPIOF
#define DOOR4_Pin GPIO_PIN_5
#define DOOR4_GPIO_Port GPIOF
#define DOOR3_Pin GPIO_PIN_6
#define DOOR3_GPIO_Port GPIOF
#define DOOR2_Pin GPIO_PIN_7
#define DOOR2_GPIO_Port GPIOF
#define DOOR1_Pin GPIO_PIN_8
#define DOOR1_GPIO_Port GPIOF
#define EMERGENCY_Pin GPIO_PIN_9
#define EMERGENCY_GPIO_Port GPIOF
#define KEY_Pin GPIO_PIN_10
#define KEY_GPIO_Port GPIOF
#define RELAY1_Pin GPIO_PIN_8
#define RELAY1_GPIO_Port GPIOE
#define RELAY2_Pin GPIO_PIN_9
#define RELAY2_GPIO_Port GPIOE
#define RELAY3_Pin GPIO_PIN_10
#define RELAY3_GPIO_Port GPIOE
#define RELAY4_Pin GPIO_PIN_11
#define RELAY4_GPIO_Port GPIOE
#define LAMP1_Pin GPIO_PIN_12
#define LAMP1_GPIO_Port GPIOE
#define LAMP2_Pin GPIO_PIN_13
#define LAMP2_GPIO_Port GPIOE
#define LAMP3_Pin GPIO_PIN_14
#define LAMP3_GPIO_Port GPIOE
#define TRIGGER_SCAN_ACT_Pin GPIO_PIN_12
#define TRIGGER_SCAN_ACT_GPIO_Port GPIOB
#define LED_POWER_Pin GPIO_PIN_9
#define LED_POWER_GPIO_Port GPIOD
#define LED_EMISSION_Pin GPIO_PIN_10
#define LED_EMISSION_GPIO_Port GPIOD
#define LED_ERROR_Pin GPIO_PIN_11
#define LED_ERROR_GPIO_Port GPIOD
#define LED_YELLOW_Pin GPIO_PIN_14
#define LED_YELLOW_GPIO_Port GPIOD
#define LED_GREEN_Pin GPIO_PIN_15
#define LED_GREEN_GPIO_Port GPIOD
#define PROTECTION_ONOFF_Pin GPIO_PIN_3
#define PROTECTION_ONOFF_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
