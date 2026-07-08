#include "timer.h"
#include "libopencm3/stm32/timer.h"
#include "libopencm3/stm32/rcc.h"


#define PRESCALER (84)
#define ARR_VALUE (1000)

static uint32_t tim_oc_mode_flag;

// PWM_freq = system_freq / (presacler + 1) * (arr - 1) 

void timer_setup(void){
    rcc_periph_clock_enable(RCC_TIM2);
    
    // High level Timer configuration
    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    
    // Setup PWM mode
    timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_PWM1);
    tim_oc_mode_flag = 0;

    //timer enable counter 
    timer_enable_counter(TIM2);
    timer_enable_oc_output(TIM2, TIM_OC1);

    // setup PWM Freq and resolution
    timer_set_prescaler(TIM2, PRESCALER - 1);
    timer_set_period(TIM2, ARR_VALUE - 1);
}

void change_pwm_mode(void){
    if(tim_oc_mode_flag == 0){
        timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_PWM2);
        tim_oc_mode_flag = 1;
    }
    else{
        timer_set_oc_mode(TIM2, TIM_OC1,TIM_OCM_PWM1);
        tim_oc_mode_flag = 0;
    }
}

void timer_pwm_set_duty_cycle(float duty_cycle){
    // duty_cycle = (ccr / arr) * 100

    const float raw_ccr_value = ((float)ARR_VALUE * duty_cycle) / 100;

    timer_set_oc_value(TIM2, TIM_OC1, (uint32_t)raw_ccr_value);
}