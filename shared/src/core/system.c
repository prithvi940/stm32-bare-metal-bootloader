#include "core/system.h"

static volatile uint64_t ticks = 0;

void systick_setup(void){
    systick_set_frequency(systick_freq, cpu_freq);
    systick_counter_enable();
    systick_interrupt_enable();
}

void sys_tick_handler(void){
    ticks++;
}
void rcc_setup(void){
    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]);
} 

uint64_t system_get_ticks(void){
    return ticks;
}

void system_delay(uint64_t milliseconds){
    uint64_t end_time = system_get_ticks() + milliseconds;
    while(system_get_ticks() < end_time){
        //spin
    }
}