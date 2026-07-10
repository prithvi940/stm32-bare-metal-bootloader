#ifndef INC_COMMS_H
#define INC_COMMS_H

#include "common-defines.h"

#define PACKET_LENGTH_BYTES (1)
#define PACKET_DATA_BYTES   (16)
#define PACKET_CRC_BYTES    (1)
#define PACKET_BYTES        (PACKET_LENGTH_BYTES + PACKET_DATA_BYTES + PACKET_CRC_BYTES)
#define PACKET_ACK_DATA0    (0x15)
#define PACKET_RETX_DATA0   (0x19)

typedef struct comms_packet_t{
    uint8_t length;
    uint8_t data[PACKET_DATA_BYTES];
    uint8_t crc;  
}comms_packet_t;
/*
the struct when created with different sizes of data variables the compiler will pad the lower size data 
variables to make it same size as largest data type in a struct  
*/
bool comms_is_single_byte_packet(const comms_packet_t* packet, uint8_t data);
void comms_packet_copy(const comms_packet_t* source, comms_packet_t* dest);
void comms_setup(void);
bool comms_packet_available(void);
void comms_update(void); // unit of process that will be called in firmware.c to do the work
void comms_write(comms_packet_t* packet);
void comms_read(comms_packet_t* packet);
uint8_t comms_calculate_crc(comms_packet_t* packet);

#endif
