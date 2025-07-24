#ifndef TOOLS_DEF
#define TOOLS_DEF

#include "global.h"
#ifndef TB_DEF
#include "api.h"
#include "usart.h"
#endif

#define response(...)\
do {if(err == 0) 		 \
 sprintf(resp,__VA_ARGS__);} while(0)\

#define response_err(...)\
do {if(err != 0) 		 \
 sprintf(resp,__VA_ARGS__);} while(0)\

#define cmd(...) \
	cmd_compare((tcp_buffer+i),__VA_ARGS__) \

#define rd(...)\
	sscanf(&tcp_buffer[i],__VA_ARGS__)\

#define DEGREE_OF_FREEDOM 4

int  cmd_compare(char* buf, const char* cmd);
void debug_mess(char* mess);
void send_response ( int interface_in , char *resp );
void read_last_config_data(config_struct *dest_struct);
void default_conf(config_struct *dest_struct);
int  save_last_config_data(config_struct* source_struct, int conf_struct_size);
void reset_err(alarm_struct* alarm);
void err_cmd(char* resp,char* data_in,int id);
int check_type_id(char* data_in,char* type,int max_id);
int get_error_state(device_struct* mcs);

void indication_handler(QueueHandle_t* indication_queue,device_struct* mcs);

void handler_tools(device_struct* mcs);
void handler_alarm(device_struct* mcs);

void protection_err_clr(device_struct *mcs);
void protection_set_state(state_struct *state, int value);

void init_status(device_struct* mcs);

float abs_f(float c);
int clear_flash(device_struct* mcs);

#define get(name,type) type get_##name();
#define set(name,type) void set_##name(type _name);

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

#undef get
#undef set


#endif //TOOLS_DEF
