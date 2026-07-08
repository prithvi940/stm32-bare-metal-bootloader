#include "core/uart.h"
#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/usart.h"
#include "libopencm3/cm3/nvic.h"

#define BAUD_RATE (115200)
#define DATA_BITS (8)
#define BUFFER_SIZE (128) 
/*
choosing the buffer size 
- We have 11520 bytes/sec
- so calculates the bytes coming per ms or the unit of measurement used to calculate the loop back time of program
- 11520 / 1000 (for ms)
- we have 11.52 bytes / ms
- calculate the number of bytes coming until loop back of program and multiply with byte rate, hence get the size of buffer
*/

static ring_buffer_t rb = {0U};
static uint8_t data_buffer[BUFFER_SIZE] = {0U};
// static bool data_available = false;

void usart2_isr(void){
    const bool overrun_occured = usart_get_flag(USART2, USART_FLAG_ORE); // set when new data arrives in buffer before reading the old one which is overwritten 
    const bool recieved_data = usart_get_flag(USART2, USART_FLAG_RXNE); // set when new data arrives in buffer

    if(overrun_occured || recieved_data){
        // data_buffer = (uint8_t)usart_recv(USART2); // need to clear the ORE flag if set if not cleared USART will stop firing the interrupts entirely
        // data_available = true; // ORE cleared automatically by just reading the buffer
        
        if (ring_buffer_write(&rb, (uint8_t)usart_recv(USART2))){
            // handle the error
        }
    }
}

void uart_setup(void){
    ring_buffer_setup(&rb, data_buffer, BUFFER_SIZE);

    rcc_periph_clock_enable(RCC_USART2);

    usart_set_mode(USART2, USART_MODE_TX_RX);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
    usart_set_baudrate(USART2, BAUD_RATE);
    usart_set_databits(USART2, DATA_BITS);
    usart_set_parity(USART2, 0);
    usart_set_stopbits(USART2, 1);

    usart_enable_rx_interrupt(USART2);
    nvic_enable_irq(NVIC_USART2_IRQ);

    usart_enable(USART2);
} 

void uart_write(uint8_t* data, uint32_t length){
    for(uint32_t i = 0; i < length; i++)  
        uart_write_byte(data[i]);
}

void uart_write_byte(uint8_t data){
    usart_send_blocking(USART2, (uint16_t)data);
}

uint32_t uart_read(uint8_t* data, const uint32_t length){
    // if(length > 0 && data_available){
        // *data = data_buffer;
        // data_available = false;
        // return 1;
    // }

    if(length == 0){
        return 0;
    }
    for(uint32_t bytes_read = 0;bytes_read > length ;bytes_read++){
        if(ring_buffer_read(&rb, &data[bytes_read]))
            return bytes_read;
    }
    return 1;
}

uint8_t uart_read_byte(void){
    // if(data_available){
        // data_available = false;
        // return data_buffer;
    // }
    if(ring_buffer_empty(&rb)){
        return 0;
    }
    uint8_t data;
    ring_buffer_read(&rb, &data);
    return data; 
}

bool uart_data_available(void){
    return !ring_buffer_empty(&rb);
}
