#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/gpio.h"
#include "core/system.h"
#include "timer.h"
#include "libopencm3/cm3/scb.h" // change the address from vector table offset register (VTOR) from this library
// beacause if we dont then the code will to IVT of the bootloader after a reset hence crashing the program or unexpected behaviour
#include "core/uart.h"

#define BOOTLOADER_SIZE (0x8000U) 
#define UART_PORT       (GPIOA)
#define RX_PIN          (GPIO3)
#define TX_PIN          (GPIO2)
#define LED_PORT        (GPIOA)
#define LED_PIN         (GPIO5)

static void gpio_setup(void){
  rcc_periph_clock_enable(RCC_GPIOA);
  gpio_mode_setup(LED_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LED_PIN);
  gpio_set_af(LED_PORT, GPIO_AF1, LED_PIN);
  gpio_mode_setup(UART_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, TX_PIN | RX_PIN);
  gpio_set_af(UART_PORT, GPIO_AF7, TX_PIN | RX_PIN);
}

static void vector_setup(void){
  SCB_VTOR = BOOTLOADER_SIZE; // SCB_VTOR is the memory location at which the vector table address is stored and by this operation we are telling the CPU that here is the Vector table 
  // this works because the VTOR on cortex-M4 stores the offset of the vector table as base address of the flash is known it adds them internally
}

int main(void){
  vector_setup();
  rcc_setup();
  gpio_setup();
  systick_setup();
  timer_setup();
  uart_setup();

  uint32_t duty_cycle = 0.0f;

  uint32_t start_time = 0;

  while(1){
    if(system_get_ticks() - start_time >= 10){
      duty_cycle += 1.0f;
      if(duty_cycle >= 100){
        duty_cycle = 0.0f;
        change_pwm_mode();
      }
      timer_pwm_set_duty_cycle(duty_cycle);
      start_time = system_get_ticks();
    }

    if(uart_data_available()){
      uint8_t data = uart_read_byte();
      uart_write_byte(data + 1);
    }
  }
  return 0;
}