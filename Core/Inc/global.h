#ifndef GLOBAL_DEF
#define GLOBAL_DEF
#include "setting_project.h"
#include "stdio.h"
#include "string.h"
#include "main.h"
#ifndef TB_DEF
#include "params_func.h"
#endif

//#define MODE_CALLBACK_PHOTO

#ifdef DEBUG_ENABLE 
#if DEBUG_MESSAGE_FORMAT == 0
#define debug_message(flag,buf,...) \
		do{if(flag){sprintf(buf,__VA_ARGS__); \
		debug_mess(buf);} }while(0)
#elif DEBUG_MESSAGE_FORMAT == 1
		#define debug_message(flag,buf,...) \
		do{if(flag){printf(__VA_ARGS__);} }while(0)
	#endif 
#else
	#define debug_message(buf,...)
#endif

typedef struct {
	int 	key;
	int		emergency;
	int		door[DOOR_COUNT];
} alarm_struct;

typedef struct {
	int		emission;
	int		laser_controller;
	int		relay[RELAY_COUNT];
	int		lamp[LAMP_COUNT];
	int		protection_state;
	int 	buttons[BUTTON_COUNT];
	int 	out[OUT_COUNT];
} state_struct;

typedef struct {
	u32 	ip[4];
	u32 	mac[6];
} config_struct;

typedef struct {
	int current_interface;
	alarm_struct alarms;
	config_struct config;
	state_struct state;
} device_struct;

extern device_struct mcs_storage;
#endif
