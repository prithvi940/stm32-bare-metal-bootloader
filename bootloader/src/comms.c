#include "comms.h"
#include "core/uart.h"
#include "core/crc8.h"

typedef enum comms_state_t{
    COMMSSTATE_LENGTH,
    COMMSSTATE_DATA,
    COMMSSTATE_CRC
}comms_state_t;

#define PACKET_BUFFER_LENGTH (8)

static comms_state_t state = COMMSSTATE_LENGTH;
static uint8_t data_byte_count = 0;
static comms_packet_t temp_packet = {.length = 0, .data = {0}, .crc = 0}; 
static comms_packet_t retx_packet = {.length = 0, .data = {0}, .crc = 0};
static comms_packet_t ack_packet = {.length = 0, .data = {0}, .crc = 0};
static comms_packet_t last_transmitted_packet = {.length = 0, .data = {0}, .crc = 0};

// Packet buffer components
static comms_packet_t packet_buffer[PACKET_BUFFER_LENGTH];
static uint32_t packet_read_index = 0;
static uint32_t packet_write_index = 0;
static uint32_t next_write_index = 0;
static uint32_t packet_buffer_mask = PACKET_BUFFER_LENGTH - 1;


bool comms_is_single_byte_packet(const comms_packet_t* packet, uint8_t data){
    if(packet->length != 1)
        return false;

    if(packet->data[0] != data)
        return false;
    
    for(uint8_t i = 0; i < PACKET_DATA_BYTES; i++){
        if(packet->data[i] != 0xff)
            return false;
    }
    return true;
}

void comms_packet_copy(const comms_packet_t* source, comms_packet_t* dest){
    dest->length = source->length;
    for(uint8_t i = 0; i < PACKET_DATA_BYTES; i++){
        dest->data[i] = source->data[i];
    }
    dest->crc = source->crc;
}

void comms_setup(void){
    retx_packet.length = 1;
    retx_packet.data[0] = PACKET_RETX_DATA0;
    for(uint8_t i = 1; i < PACKET_LENGTH_BYTES; i++){
        retx_packet.data[i] = 0xff;
    }
    retx_packet.crc = comms_calculate_crc(&retx_packet);

    ack_packet.length = 1;
    ack_packet.data[0] = PACKET_ACK_DATA0;
    for(uint8_t i = 1; i < PACKET_LENGTH_BYTES; i++){
        ack_packet.data[i] = 0xff;
    }
    ack_packet.crc = comms_calculate_crc(&ack_packet);
}

bool comms_packet_available(void){
    return (packet_read_index != packet_write_index);
}

void comms_update(void){
        while(uart_data_available()){
        switch(state){
            case COMMSSTATE_LENGTH:{
                temp_packet.length = uart_read_byte();
                state = COMMSSTATE_DATA;
            } break;

            case COMMSSTATE_DATA:{
                temp_packet.data[data_byte_count++] = uart_read_byte(); 
                if(data_byte_count >= PACKET_DATA_BYTES){
                    data_byte_count = 0;
                    state = COMMSSTATE_CRC;
                }
            } break;

            case COMMSSTATE_CRC:{
                temp_packet.crc = uart_read_byte();
                uint8_t computed_crc = comms_calculate_crc(&temp_packet); 
                // we using the temp_packet first byte's address as address for the data, starts from the LEN 
                if(computed_crc != temp_packet.crc){
                    comms_write(&temp_packet);
                    state = COMMSSTATE_LENGTH;
                    break;
                }

                if(comms_is_single_byte_packet(&temp_packet, PACKET_RETX_DATA0)){
                    comms_write(&last_transmitted_packet);
                    state = COMMSSTATE_LENGTH;
                    break;
                }

                if(comms_is_single_byte_packet(&temp_packet, PACKET_ACK_DATA0)){
                    state = COMMSSTATE_LENGTH;
                    break;
                }

                next_write_index = (packet_write_index + 1) & (packet_buffer_mask);
                
                comms_packet_copy(&temp_packet, &packet_buffer[packet_write_index]);
                packet_write_index = next_write_index;
                
                comms_write(&ack_packet);
                state = COMMSSTATE_LENGTH;

                /* 
                so the second "if" is the sender's side "if" which receives the packet from the receiver of the firmware and checks if it is a retranmission packet

                                        First if                        Second if
                CRC                     Failed                          Passed
                Who messed up           Sender's packet corrupted       Other side didn't receive YOUR last packet
                What you write          Retransmit request to sender    Your own last transmitted packet
                comms_write argument    &temp_packet                    &last_transmitted_packet

                Same state machine, two different roles, handled in the same place. That's the symmetric protocol design
                */

            } break;

            default:{
                state = COMMSSTATE_LENGTH;
            }

        }
    }
} // unit of process that will be called in firmware.c to do the work

void comms_write(comms_packet_t* packet){
    comms_packet_copy(packet, &last_transmitted_packet);
    uart_write((uint8_t*)packet, PACKET_BYTES);
}

void comms_read(comms_packet_t* packet){
    comms_packet_copy(&packet_buffer[packet_read_index], packet);
    packet_read_index = (packet_read_index + 1) & (packet_buffer_mask);
}

uint8_t comms_calculate_crc(comms_packet_t* packet){
    return crc8((uint8_t*)packet, (PACKET_LENGTH_BYTES + PACKET_DATA_BYTES));
}