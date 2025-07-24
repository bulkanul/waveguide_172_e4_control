#include "../Inc/commands_handler.h"
#include "../Inc/gitcommit.h"

#ifndef TB_DEF
GPIO_TypeDef *relay_gpio[] = {RELAY1_GPIO_Port,RELAY2_GPIO_Port,RELAY3_GPIO_Port,RELAY4_GPIO_Port};
uint16_t relay_pin[] = {RELAY1_Pin,RELAY2_Pin,RELAY3_Pin,RELAY4_Pin};
GPIO_TypeDef *lamp_gpio[] = {LAMP1_GPIO_Port,LAMP2_GPIO_Port,LAMP3_GPIO_Port};
uint16_t lamp_pin[] = {LAMP1_Pin,LAMP2_Pin,LAMP3_Pin};
// for doors order is reversed because of on board silk paint
GPIO_TypeDef *door_gpio[] = {DOOR9_GPIO_Port,DOOR8_GPIO_Port,DOOR7_GPIO_Port,DOOR6_GPIO_Port,DOOR5_GPIO_Port,DOOR4_GPIO_Port,DOOR3_GPIO_Port,DOOR2_GPIO_Port,DOOR1_GPIO_Port};
uint16_t door_pin[] = {DOOR9_Pin,DOOR8_Pin,DOOR7_Pin,DOOR6_Pin,DOOR5_Pin,DOOR4_Pin,DOOR3_Pin,DOOR2_Pin,DOOR1_Pin};
#endif

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
			response("Console mode ON\r\n");
		else
			response("Console mode OFF\r\n");
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
		if (cmd("lgstatus comm")){
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
			sprintf(resp + strlen(resp), "\r");
		}
		else if(cmd("lsrelay comm"))
		{
			rd("lsrelay comm %i %i %i\r", &id, &index, &i_val);
			err += (index < 0 || index >= RELAY_COUNT);
			err += (i_val < 0 || i_val > 1);
			if(!err){
				HAL_GPIO_WritePin(relay_gpio[index],relay_pin[index],i_val);
				state->relay[index] = i_val;
			}
			response("lrrelay comm %i %i %i\r", id, index, i_val);
		}
		else if(cmd("lslamp comm"))
		{
			rd("lslamp comm %i %i %i\r", &id, &index, &i_val);
			err += (index < 0 || index >= LAMP_COUNT);
			err += (i_val < 0 || i_val > 1);
			if(!err)			{
				HAL_GPIO_WritePin(lamp_gpio[index],lamp_pin[index],i_val);
				state->lamp[index] = i_val;
			}
			response("lrlamp comm %i %i %i\r", id, index, i_val);
		}
		else if(cmd("lslaser comm"))
		{
			rd("lslaser comm %i %i\r", &id, &i_val);
			err += (i_val < 0 || i_val > 1);
			if(!err) {
				HAL_GPIO_WritePin(TRIGGER_SCAN_ACT_GPIO_Port,TRIGGER_SCAN_ACT_Pin,i_val);
				state->laser_controller = i_val;
			}
			response("lrlaser comm %i %i\r", id, i_val);
		}
		else if(cmd("lslasstat comm"))
		{
			int value[3] = {0,0,0};
			rd("lslasstat comm %i %i %i %i\r", &id, &value[0], &value[1], &value[2]);
			err += (value[0] < 0 || value[0] > 1);
			err += (value[1] < 0 || value[1] > 1);
			err += (value[2] < 0 || value[2] > 1);
			if(!err) {
//				HAL_GPIO_WritePin(LED_POWER_GPIO_Port,LED_POWER_Pin,value[0]);
				HAL_GPIO_WritePin(LED_EMISSION_GPIO_Port,LED_EMISSION_Pin,value[1]);
//				HAL_GPIO_WritePin(lamp_gpio[0],lamp_pin[0],value[1]);
//				state->lamp[0] = value[1];
//				HAL_GPIO_WritePin(lamp_gpio[1],lamp_pin[1],value[1]);
//				state->lamp[1] = value[1];
				HAL_GPIO_WritePin(LED_ERROR_GPIO_Port,LED_ERROR_Pin,value[2] + get_error_state(mcs));
//				state->laser_controller = i_val;
			}
			response("lrlasstat comm %i %i %i %i\r", id, value[0], value[1], value[2]);
		}
		else if(cmd("lsprot comm"))
		{
			rd("lsprot comm %i %i\r", &id, &i_val);
			err += (i_val < 0 || i_val > 1);
			if(!err)
				protection_set_state(state, i_val);
			response("lrprot comm %i %i\r", id, i_val);
		}
		if (err != 0)
			err_cmd(resp, tcp_buffer, id);
	}
}
