#ifndef INC_SYSTEM_H
#define INC_SYSTEM_H 

#include "common-defines.h"
#include "libopencm3/cm3/vector.h"
#include "libopencm3/cm3/systick.h"
#include "libopencm3/stm32/f4/rcc.h"

#define systick_freq 1000
#define cpu_freq 84000000

void systick_setup(void);
// systick handler function is already has a definition in vector.h we are just overwriting it here
uint64_t system_get_ticks(void);
void rcc_setup(void); 

#endif