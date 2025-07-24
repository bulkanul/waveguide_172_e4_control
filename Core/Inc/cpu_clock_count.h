/*
 * cpu_clock_count.h
 *
 *  Created on: Aug 21, 2020
 *      Author: compAnton
 */

#ifndef INC_CPU_CLOCK_COUNT_H_
#define INC_CPU_CLOCK_COUNT_H_

#include "main.h"

#define DWT_CYCCNT    *(volatile unsigned long *)0xE0001004
#define DWT_CONTROL   *(volatile unsigned long *)0xE0001000
#define SCB_DEMCR     *(volatile unsigned long *)0xE000EDFC


void cpu_clk_rst(void);
long cpu_clk_get(void);

#endif /* INC_CPU_CLOCK_COUNT_H_ */
