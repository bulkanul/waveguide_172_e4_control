#include "../Inc/commands_handler.h"
#include "../Inc/gitcommit.h"
#include "hardware.h"

#ifndef TB_DEF
GPIO_TypeDef *relay_gpio[RELAY_COUNT] = { RELAY1_GPIO_Port, RELAY2_GPIO_Port, RELAY3_GPIO_Port, RELAY4_GPIO_Port, RELAY5_GPIO_Port };
uint16_t relay_pin[RELAY_COUNT] = { RELAY1_Pin, RELAY2_Pin, RELAY3_Pin, RELAY4_Pin, RELAY5_Pin};

GPIO_TypeDef *lamp_gpio[LAMP_COUNT] = { Lamp_FL_GPIO_Port, Lamp_BL_GPIO_Port, Lamp_PL_GPIO_Port };
uint16_t lamp_pin[LAMP_COUNT] = { Lamp_FL_Pin, Lamp_BL_Pin, Lamp_PL_Pin };
#endif

extern osMutexId CanMutexHandle;

static void common_command(device_struct *mcs, char *resp, char *debug_buffer, char *tcp_buffer, int i);

void CommandsHandler(device_struct *mcs, char *resp, char *debug_buffer, char *tcp_buffer, int i)
{
	sprintf(debug_buffer, "incoming cmd :> %s\r\n", tcp_buffer + i);
	debug_mess(debug_buffer);

	common_command(mcs, resp, debug_buffer, tcp_buffer, i);
	if(resp[0] == 'l')
		send_response(mcs->current_interface, resp);
}

void common_command(device_struct *mcs, char *resp, char *debug_buffer, char *tcp_buffer, int i)
{
	int err = 0;
	int id;
	config_struct* conf = &mcs->config;
	state_struct* state = &mcs->state;
	alarm_struct* alarm = &mcs->alarms;
	if (cmd("console")){
		set_console_mode(!get_console_mode());
		if (get_console_mode())
			response("lConsole mode ON\r\n");
		else
			response("lConsole mode OFF\r\n");
	}
	else if (cmd("lgvers"))
		response("lrvers %s", PROTOCOL_VERSION);
	else if(cmd("lghash"))
		response("lrhash %s", GIT_INFO);
	else if (cmd("lsip")){
		rd("lsip %d.%d.%d.%d", (int*)&conf->ip[0],(int*)&conf->ip[1],(int*)&conf->ip[2],(int*)&conf->ip[3]);
		err += save_last_config_data(&mcs->config, sizeof(config_struct));
		response("lrip  %d.%d.%d.%d Save OK\r\n", (int)conf->ip[0],(int)conf->ip[1],(int)conf->ip[2],(int)conf->ip[3]);
		response_err("lrip  %d.%d.%d.%d Save ERROR\r\n", (int)conf->ip[0],(int)conf->ip[1],(int)conf->ip[2],(int)conf->ip[3]);
	}
	else if (cmd("lgip"))
		response("lrip  %d.%d.%d.%d\r\n", (int)conf->ip[0],(int)conf->ip[1],(int)conf->ip[2],(int)conf->ip[3]);
//--------SET CURRENT MAC ADDRESS  REQUEST---------------
	else if(cmd("lsmac")){
		rd("lsmac %x %x %x %x %x %x" ,
			(unsigned int*)&conf->mac[0],
			(unsigned int*)&conf->mac[1],
			(unsigned int*)&conf->mac[2],
			(unsigned int*)&conf->mac[3],
			(unsigned int*)&conf->mac[4],
			(unsigned int*)&conf->mac[5]
			) ;
		err += save_last_config_data ( &mcs->config , sizeof ( config_struct ) );
		response("lrmac %x %x %x %x %x %x Save OK\r\n ",
			 (unsigned int)conf->mac[0],
			 (unsigned int)conf->mac[1],
			 (unsigned int)conf->mac[2],
			 (unsigned int)conf->mac[3],
			 (unsigned int)conf->mac[4],
			 (unsigned int)conf->mac[5]
		) ;
		response_err("lrmac %x %x %x %x %x %x ERROR OK\r\n",
			 (unsigned int)conf->mac[0],
			 (unsigned int)conf->mac[1],
			 (unsigned int)conf->mac[2],
			 (unsigned int)conf->mac[3],
			 (unsigned int)conf->mac[4],
			 (unsigned int)conf->mac[5]
		);
	}
	else if(cmd("lgmac")){
		response("lrmac %x %x %x %x %x %x\r\n ",
			 (unsigned int)conf->mac[0],
			 (unsigned int)conf->mac[1],
			 (unsigned int)conf->mac[2],
			 (unsigned int)conf->mac[3],
			 (unsigned int)conf->mac[4],
			 (unsigned int)conf->mac[5]
		);
	}
	else if (cmd("lsfactrst")){
		err +=  clear_flash(mcs);
		response("lrfactrst Save OK\r\n");
		response_err("lrfactrst Save ERROR\r\n");
	}
	id = check_type_id(tcp_buffer, "comm", 1);
	if (id != -1) {
		int i_val;
		int index;
		if (cmd("lgstatus comm")) {
			sprintf(resp, "lrstatus comm %i", id);
			sprintf(resp + strlen(resp), " %i",alarm->key);
			sprintf(resp + strlen(resp), " %i",alarm->emergency);
			for(int i = 0; i < DOOR_COUNT; i ++)
				sprintf(resp + strlen(resp), " %i",alarm->door[i]);
			sprintf(resp + strlen(resp), " %i",state->protection_state);
			for(int i = 0; i < RELAY_COUNT; i ++)
				sprintf(resp + strlen(resp), " %i",state->relay[i]);
			for(int i = 0; i < LAMP_COUNT; i ++)
				sprintf(resp + strlen(resp), " %i",state->lamp[i]);
			sprintf(resp + strlen(resp), " %i",state->laser_controller);
			for(int i = 0; i < BUTTON_COUNT; i ++)
				sprintf(resp + strlen(resp), " %i",state->buttons[i]);
			for(int i = 0; i < OUT_COUNT; i ++)
				sprintf(resp + strlen(resp), " %i",state->out[i]);
			sprintf(resp + strlen(resp), "\r");
		}
		else if(cmd("lsrelay comm"))
		{
			rd("lsrelay comm %i %i %i\r", &id, &index, &i_val);

			err += (index < 0 || index >= RELAY_COUNT);
			err += (i_val != 0 && i_val != 1);

			if (err == 0) {
				HAL_GPIO_WritePin (relay_gpio[index], relay_pin[index], i_val == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
				state->relay[index] = i_val;
			}

			response("lrrelay comm %i %i %i\r", id, index, i_val);
		}
		else if(cmd("lslamp comm"))
		{
			rd("lslamp comm %i %i %i\r", &id, &index, &i_val);

			err += (index < 0 || index >= LAMP_COUNT);
			err += (i_val != 0 && i_val != 1);

			if (err == 0) {
				xSemaphoreTake(CanMutexHandle,portMAX_DELAY);
				HAL_GPIO_WritePin (lamp_gpio[index], lamp_pin[index], i_val == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
				state->lamp[index] = i_val;
				xSemaphoreGive(CanMutexHandle);
			}

			response("lrlamp comm %i %i %i\r", id, index, i_val);
		}
		else if(cmd("lslasstat comm"))
		{
			int leds_state[3] = { 0,0,0 };
			rd("lslasstat comm %i %i %i %i\r", &id, &leds_state[LED_POWER], &leds_state[LED_EMISSION], &leds_state[LED_ERROR]);

			err += (leds_state[LED_POWER] != 0 && leds_state[LED_POWER] != 1);
			err += (leds_state[LED_EMISSION] != 0 && leds_state[LED_EMISSION] != 1);
			err += (leds_state[LED_ERROR] != 0 && leds_state[LED_ERROR] != 1);

			if (err == 0) {
				HAL_GPIO_WritePin (LED_POWER_GPIO_Port, LED_POWER_Pin, leds_state[LED_POWER] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
				HAL_GPIO_WritePin (LED_EMISSION_GPIO_Port, LED_EMISSION_Pin, leds_state[LED_EMISSION] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
				HAL_GPIO_WritePin (LED_ERROR_GPIO_Port, LED_ERROR_Pin, (leds_state[LED_ERROR] + get_error_state (mcs)) == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
			}

			response("lrlasstat comm %i %i %i %i\r", id, leds_state[LED_POWER], leds_state[LED_EMISSION], leds_state[LED_ERROR]);
		}
		else if(cmd("lsprot comm"))
		{
			rd("lsprot comm %i %i\r", &id, &i_val);

			err += (i_val != 0 && i_val != 1);

			if (err == 0)
				protection_set_state(state, i_val);

			response("lrprot comm %i %i\r", id, i_val);
		}
		else if(cmd("lsout comm"))
		{
			int out1_flag, out2_flag;
			rd("lsout comm %i %i %i\r", &id, &out1_flag, &out2_flag);

			err += (out1_flag != 0 && out1_flag != 1);
			err += (out2_flag != 0 && out2_flag != 1);

			if (err == 0) {
				out1_flag == 0 ? out1_off () : out1_on ();
				out2_flag == 0 ? out2_off () : out2_on ();

				state->out[0] = out1_flag;
				state->out[1] = out2_flag;
			}

			response("lrout comm %i %i %i\r", id, out1_flag, out2_flag);
		}
		if (err != 0)
			err_cmd(resp, tcp_buffer, id);
	}
}
