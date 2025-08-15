#include "hardware.h"

#define BUTTONS_COUNT (3)

static GPIO_PinState stable_state[BUTTONS_COUNT];
static uint16_t pins[BUTTONS_COUNT] = {Button_FL_Pin, Button_BL_Pin, Button_PL_Pin};
static bool is_pressed_button[BUTTONS_COUNT] = {false, false, false};

void buttons_handler (void)
{
	for (size_t i = 0; i < BUTTONS_COUNT; i++) {
		GPIO_PinState new_state = HAL_GPIO_ReadPin(GPIOE, pins[i]);
		if (new_state != stable_state[i]) {
			stable_state[i] = new_state;
			is_pressed_button[i] = true;
		}
	}
}

bool is_button_fl_pressed (void)
{
    return is_pressed_button[0];
}

bool is_button_bl_pressed (void)
{
	return is_pressed_button[1];
}

bool is_button_pl_pressed (void)
{
	return is_pressed_button[2];
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

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	for (size_t i = 0; i < BUTTONS_COUNT; i++) {
		if (GPIO_Pin == pins[i]) {
			stable_state[i] = HAL_GPIO_ReadPin (GPIOE, GPIO_Pin);
		}
	}
}
