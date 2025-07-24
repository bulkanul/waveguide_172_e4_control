#include "../Inc/tools.h"
#include "../Inc/server.h"
#include "../Inc/commands_handler.h"
#include "../Inc/main.h"
#include "FreeRTOS.h"
#include "math.h"

#define THRESHOLD_FREQ 1000
#define DCDC1_DIODE_TRESHOLD_VALUE 0.6

static int interface_in   = INTERFACE_ETHERNET;
static int console_mode = DISABLE;

static uint8_t usr_ontask_terminator  = false;
static uint8_t usr_ontask_state       = false;
static uint8_t usr_offtask_state 	  = false;
static uint8_t usr_offtask_terminator = false;

uint16_t time_hot; 

#ifndef TB_DEF
extern GPIO_TypeDef* door_gpio[];
extern uint16_t door_pin[];
#endif

#define get(name,type) type get_##name(){ return name;}
#define set(name,type) void set_##name(type _name){  name = _name;}

get(usr_ontask_terminator,uint8_t)
get(usr_ontask_state,uint8_t)
get(usr_offtask_state,uint8_t)
get(usr_offtask_terminator,uint8_t)
set(usr_ontask_terminator,uint8_t)
set(usr_ontask_state,uint8_t)
set(usr_offtask_state,uint8_t)
set(usr_offtask_terminator,uint8_t)

get(console_mode,int)
set(console_mode,int)

get(interface_in,int)
set(interface_in,int)

int cmd_compare(char* buf, const char* cmd){
	int size = strlen(cmd);
	for(int i = 0; i < size; i ++)
	{
		if(*(buf + i) != *(cmd + i)) return 0;
	}
	return 1;
}

void debug_mess(char* mess) {
#ifdef TB_DEF
	char buf[500];
	sprintf(buf, "<%lu> : ", (long)osKernelSysTick());
	*(buf + strlen(buf)) = '\0';
	if (strlen(buf) + strlen(mess) < 500)
	{
		sprintf(buf + strlen(buf), "%s", mess);
	}
	*(buf + strlen(buf)) = '\0';

	taskENTER_CRITICAL();
	printf(buf);
	taskEXIT_CRITICAL();
#endif
}

void send_response ( int interface_in , char *resp ){
	if ( interface_in == INTERFACE_ETHERNET ){
#ifndef TB_DEF
	  if ( getRemoteClient() != NULL )
		  netconn_write( getRemoteClient() , resp , strlen ( resp ) , NETCONN_COPY ) ;
#else
			netconn_write(0, resp, strlen(resp), NETCONN_COPY);
#endif
	}
	else if ( interface_in == INTERFACE_USB_UART1 )
		HAL_UART_Transmit ( &huart1 , ( uint8_t* ) resp , strlen ( resp ) , 500 ) ;
}

void read_last_config_data(config_struct *dest_struct){
	int conf_struct_size = sizeof(config_struct);
	int err = -1;
	int repeats = 20 ;
	portENTER_CRITICAL();
	flash_read_config_data((uint32_t*)dest_struct,conf_struct_size,PARAMS_MEMORY_ADDR);
	if ( * ( ( uint32_t * ) dest_struct ) == 0xFFFFFFFF){
		default_conf ( dest_struct ) ;
		while ( err == -1 && repeats > 0 )
		{
			err = save_last_config_data ( dest_struct , sizeof ( config_struct ) ) ;
			repeats -- ;
		}
	}
	//TODO for user mode
	portEXIT_CRITICAL();
}

void default_conf(config_struct *dest){
	memset(dest,0,sizeof(config_struct));
	dest->ip[0] = 192;
	dest->ip[1] = 168;
	dest->ip[2] = 26;
	dest->ip[3] = 220;
	dest->mac[0] = 0;
	dest->mac[1] = 0x80;
	dest->mac[2] = 0xE1;
	dest->mac[3] = 0;
	dest->mac[4] = 0;
	dest->mac[5] = 0x0d;
}

int save_last_config_data(config_struct* source_struct, int conf_struct_size){
	int err = !SUCCESS;
	int repeats = 5;
	portENTER_CRITICAL();
	while(err == !SUCCESS && repeats > 0)
	{
		err = flash_save_config_data((uint32_t*)source_struct, conf_struct_size, PARAMS_MEMORY_ADDR);
		repeats --;
	}
	portEXIT_CRITICAL();
	if(repeats == 0) return -1;
	return 0;
}

void reset_err(alarm_struct* alarm){
	alarm->key = 0;
	alarm->emergency = 0;
	for(int i = 0; i < DOOR_COUNT; i ++)
		alarm->door[i] = 0;
}

#ifdef TB_DEF
int key = 0;
int emergency = 0;
int door_index = 0;
int door[DOOR_COUNT];
#endif

void handler_alarm(device_struct* mcs){
	alarm_struct *alarm = &mcs->alarms;
#ifndef TB_DEF
	alarm->key = !((KEY_GPIO_Port->IDR & KEY_Pin)!=0);
	alarm->emergency = !((EMERGENCY_GPIO_Port->IDR & EMERGENCY_Pin)!=0);
	for(int i = 0; i < DOOR_COUNT; i ++)
		alarm->door[i] = !((door_gpio[i]->IDR & door_pin[i])!=0);
#else
	alarm->key = key;
	alarm->emergency = emergency;
	for(int i = 0; i < DOOR_COUNT; i ++)
		alarm->door[i] = door[i];
#endif
}

int get_error_state(device_struct* mcs){
	int err = 0;
	alarm_struct *alarm = &mcs->alarms;
	err += alarm->key;
	err += alarm->emergency;
	for(int i = 0; i < DOOR_COUNT; i ++)
		err += alarm->door[i];
	return err;
}

void protection_err_clr(device_struct *mcs){
	reset_err(&mcs->alarms);
}

void protection_set_state(state_struct *state, int value){
	HAL_GPIO_WritePin(PROTECTION_ONOFF_GPIO_Port, PROTECTION_ONOFF_Pin, value == 0);
	state->protection_state = value;
}

void indication_handler(QueueHandle_t* indication_queue,device_struct* mcs){

	int yellow_led_counter = 0;
	int green_led_counter = 0;
	uint8_t tmp = 0;

	HAL_GPIO_WritePin(LED_POWER_GPIO_Port, LED_POWER_Pin, GPIO_PIN_SET);
	for(;;){
		HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, get_error_state(mcs));

		if( xQueueReceive(*indication_queue,&tmp,0) == pdPASS ){
			if(tmp == ETH_IND_REQ){
				yellow_led_counter = 0;
			}
			tmp = 0;
		}

		/// green led blink ----------------------------------------------------------------------------------
		if (green_led_counter == 10)
			HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, true);
		else if (green_led_counter == 200) {
			HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, false);
			green_led_counter = 0;
		}
		green_led_counter++;
		/// green led blink ----------------------------------------------------------------------------------


		if(yellow_led_counter < 5){
		  HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port,LED_YELLOW_Pin,GPIO_PIN_SET);
		  yellow_led_counter++;
		} else{
		  HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port,LED_YELLOW_Pin,GPIO_PIN_RESET);
		  yellow_led_counter++;
		}

		osDelay(20);
	}
}

#define TIME_HOT_THRESHOLD 1000

void init_status(device_struct* mcs){

}

float abs_f(float c){
	return (c >= 0)? c : c*(-1); 
}

void err_cmd(char* resp,char* data_in,int id){
    char cmd[40];
	char type[30];
		data_in[1] = 'r';
    sscanf(data_in,"%s %s",cmd,type);
    sprintf(resp,"%s %s %i ERR\r\n",cmd,type,id);
}

void handler_tools(device_struct* mcs){

}

int check_type_id(char* data_in,char* type,int max_id){
	int id = -1;
	int err = 0;
	char* text_type = strstr(data_in," ");
	if(text_type !=0)
	    err = cmd_compare(++text_type, type) == 0;
	else
	    err++;
	if(text_type != 0 && err == 0){
		char* text_id = strstr(text_type," ");
		if(text_id != 0){
			sscanf(++text_id,"%i",&id);
			if(id >= max_id)
				id = -1;
		} else
			err++;

	} else
		err++;
	return id;
}

int clear_flash(device_struct* mcs){
	int err  = 0;
#ifndef TB_DEF
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PageError = 0;
	HAL_FLASH_Unlock();
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.Sector = PARAMS_MEMORY_FLASH_SECTOR;
	EraseInitStruct.NbSectors = 1;
	EraseInitStruct.Banks = FLASH_BANK_1;
	EraseInitStruct.VoltageRange = VOLTAGE_RANGE_3;
	err += HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK;
	if (err == HAL_OK){

		default_conf(&mcs->config);
		err += save_last_config_data(&mcs->config, sizeof(config_struct));

	}
	HAL_FLASH_Lock();
#endif
	return err;
}

#undef get
#undef set
