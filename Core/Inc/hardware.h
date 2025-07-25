#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdbool.h>
#include "main.h"

enum {
    BUTTON_FL = 0,
    BUTTON_BL,
    BUTTON_PL
};

bool is_button_fl_pressed (void);
bool is_button_bl_pressed (void);
bool is_button_pl_pressed (void);
bool is_door1_change_level (void);
bool is_door2_change_level (void);
bool is_door3_change_level (void);
bool is_alarm_key (void);
bool is_alarm_emergency (void);

#endif // HARDWARE_H
