#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdbool.h>
#include "main.h"
#include "global.h"

enum buttons_e {
    BUTTON_FL = 0,
    BUTTON_BL,
    BUTTON_PL
};

enum leds_e {
    LED_POWER = 0,
    LED_EMISSION,
    LED_ERROR
};

bool is_button_fl_pressed (void);
bool is_button_bl_pressed (void);
bool is_button_pl_pressed (void);
bool is_door1_change_level (void);
bool is_door2_change_level (void);
bool is_door3_change_level (void);
bool is_alarm_key (void);
bool is_alarm_emergency (void);
void out1_on (void);
void out1_off (void);
void out2_on (void);
void out2_off (void);
void buttons_init (void);
void buttons_handler (device_struct*);

#endif // HARDWARE_H
