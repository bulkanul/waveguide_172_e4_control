#include "hardware.h"

bool is_button_fl_pressed (void)
{
    return HAL_GPIO_ReadPin (Button_FL_GPIO_Port, Button_FL_Pin) == GPIO_PIN_SET;
}

bool is_button_bl_pressed (void)
{
    return HAL_GPIO_ReadPin (Button_BL_GPIO_Port, Button_BL_Pin) == GPIO_PIN_SET;
}

bool is_button_pl_pressed (void)
{
    return HAL_GPIO_ReadPin (Button_PL_GPIO_Port, Button_PL_Pin) == GPIO_PIN_SET;
}

bool is_door1_change_level (void)
{
    return HAL_GPIO_ReadPin (DOOR1_GPIO_Port, DOOR1_Pin) != GPIO_PIN_RESET;
}

bool is_door2_change_level (void)
{
    return HAL_GPIO_ReadPin (DOOR2_GPIO_Port, DOOR2_Pin) != GPIO_PIN_RESET;
}

bool is_door3_change_level (void)
{
    return HAL_GPIO_ReadPin (DOOR3_GPIO_Port, DOOR3_Pin) != GPIO_PIN_RESET;
}

bool is_alarm_key (void)
{
	return HAL_GPIO_ReadPin (KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET;
}

bool is_alarm_emergency (void)
{
	return HAL_GPIO_ReadPin (EMERGENCY_GPIO_Port, EMERGENCY_Pin) == GPIO_PIN_RESET;
}

void out1_on (void)
{
	HAL_GPIO_WritePin (OUT1_GPIO_Port, OUT1_Pin, GPIO_PIN_SET);
}

void out1_off (void)
{
	HAL_GPIO_WritePin (OUT1_GPIO_Port, OUT1_Pin, GPIO_PIN_RESET);
}

void out2_on (void)
{
	HAL_GPIO_WritePin (OUT2_GPIO_Port, OUT2_Pin, GPIO_PIN_SET);
}

void out2_off (void)
{
	HAL_GPIO_WritePin (OUT2_GPIO_Port, OUT2_Pin, GPIO_PIN_RESET);
}
