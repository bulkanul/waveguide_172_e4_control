/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
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
#define RELAY5_Pin GPIO_PIN_12
#define RELAY5_GPIO_Port GPIOE
#define Button_PL_Pin GPIO_PIN_13
#define Button_PL_GPIO_Port GPIOE
#define Button_PL_EXTI_IRQn EXTI15_10_IRQn
#define Button_BL_Pin GPIO_PIN_14
#define Button_BL_GPIO_Port GPIOE
#define Button_BL_EXTI_IRQn EXTI15_10_IRQn
#define Button_FL_Pin GPIO_PIN_15
#define Button_FL_GPIO_Port GPIOE
#define Button_FL_EXTI_IRQn EXTI15_10_IRQn
#define TRIGGER_SCAN_ACT_Pin GPIO_PIN_12
#define TRIGGER_SCAN_ACT_GPIO_Port GPIOB
#define LED_POWER_Pin GPIO_PIN_9
#define LED_POWER_GPIO_Port GPIOD
#define LED_EMISSION_Pin GPIO_PIN_10
#define LED_EMISSION_GPIO_Port GPIOD
#define LED_ERROR_Pin GPIO_PIN_11
#define LED_ERROR_GPIO_Port GPIOD
#define OUT1_Pin GPIO_PIN_12
#define OUT1_GPIO_Port GPIOD
#define OUT2_Pin GPIO_PIN_13
#define OUT2_GPIO_Port GPIOD
#define LED_YELLOW_Pin GPIO_PIN_14
#define LED_YELLOW_GPIO_Port GPIOD
#define LED_GREEN_Pin GPIO_PIN_15
#define LED_GREEN_GPIO_Port GPIOD
#define Lamp_PL_Pin GPIO_PIN_7
#define Lamp_PL_GPIO_Port GPIOC
#define Lamp_BL_Pin GPIO_PIN_8
#define Lamp_BL_GPIO_Port GPIOC
#define Lamp_FL_Pin GPIO_PIN_9
#define Lamp_FL_GPIO_Port GPIOC
#define PROTECTION_ONOFF_Pin GPIO_PIN_3
#define PROTECTION_ONOFF_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
