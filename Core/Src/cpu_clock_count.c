/*
 * cpu_clock_count.c
 *
 *  Created on: Aug 21, 2020
 *      Author: compAnton
 */

#include "cpu_clock_count.h"

inline void cpu_clk_rst(void)
{
	SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;// enabling DWT
	DWT_CYCCNT = 0;													// clearing dwt
	DWT_CONTROL|= DWT_CTRL_CYCCNTENA_Msk; 	// enabling counter
}

inline long cpu_clk_get(void)
{
	return DWT_CYCCNT;									// and here is count
}
///////////
