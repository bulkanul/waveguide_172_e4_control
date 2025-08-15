#include "hardware.h"

#include "FreeRTOS.h"
#include "task.h"

#include "setting_project.h"

static uint16_t pins[BUTTON_COUNT] = {Button_FL_Pin, Button_BL_Pin, Button_PL_Pin};
static uint16_t lamps[LAMP_COUNT] = {Lamp_FL_Pin, Lamp_BL_Pin, Lamp_PL_Pin};

static GPIO_PinState raw_state[BUTTON_COUNT];
static GPIO_PinState debounced_state[BUTTON_COUNT];
static GPIO_PinState last_debounced_state[BUTTON_COUNT] = {GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET};
static TickType_t debounce_start_tick = 0;
static int btn_pending = 0;

void buttons_handler (void)
{
	const uint32_t DELAY_IN_MS = 50;
	TickType_t current_time = xTaskGetTickCount();
	TickType_t delay = pdMS_TO_TICKS(DELAY_IN_MS);

	if(btn_pending)
	{
		if ((current_time - debounce_start_tick) >= delay) {
			for (size_t i = 0; i < BUTTON_COUNT; i++) {
				GPIO_PinState new_state = HAL_GPIO_ReadPin(GPIOE, pins[i]);
				if (new_state == raw_state[i])
				{
					debounced_state[i] = new_state;
					if(debounced_state[i] == GPIO_PIN_SET)
					{
						if(debounced_state[i] != last_debounced_state[i])
							HAL_GPIO_TogglePin(GPIOC, lamps[i]);
					}
					last_debounced_state[i] = debounced_state[i];
				}
			}
			btn_pending = 0;
		}
	}
}

bool is_button_fl_pressed (void)
{
    return last_debounced_state[0];
}

bool is_button_bl_pressed (void)
{
	return last_debounced_state[1];
}

bool is_button_pl_pressed (void)
{
	return last_debounced_state[2];
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
	for (size_t i = 0; i < BUTTON_COUNT; i++) {
		if (GPIO_Pin == pins[i]) {
			raw_state[i] = HAL_GPIO_ReadPin(GPIOE, pins[i]);
			debounce_start_tick = xTaskGetTickCount();   // старт «окна» антидребезга
			btn_pending = 1;
		}
	}
}
