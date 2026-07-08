#include "core/ring_buffer.h"

void ring_buffer_setup(ring_buffer_t* rb, uint8_t* buffer, uint32_t size){
    rb -> buffer = buffer;
    rb -> read_index = 0;
    rb -> write_index = 0 ;
    rb -> mask = size - 1; 
}

bool ring_buffer_empty(ring_buffer_t* rb){
    return rb -> read_index == rb -> write_index;
}

bool ring_buffer_read(ring_buffer_t* rb, uint8_t* byte){
    int32_t local_read_index = rb -> read_index;
    int32_t local_write_index = rb -> write_index;

    if(local_read_index == local_write_index){
        return false;
    } 

    *byte = rb -> buffer[local_read_index];
    local_read_index = (local_read_index + 1) & rb -> mask;

    /*
    here, bit masking is done to loop around the read index back to start if it reaches the end
    - the mask here is BUFFER_SIZE - 1
    - the looping around can be done by normal modulo divison or if-else loop but these options are slow
    - the masking working example 
        - assume buffer size if of 8
        - if write_index == 7 then operation is done as 
            - write_index = (write_index + 1) & (BUFFER_SIZE - 1)
            -   (0b0111 + 1) & (0b1000 - 1)
                = (0b1000) & (0b0111)
                = 0, hence write index loops back to 0
        - if write_index == 6 then operation is done as
            -   (0b0110 + 1) & (0b1000 - 1)
                = (0b0111) & (0b0111)
                = (0b0111)
                = (7), hence at the next index   
    */

    rb -> read_index = local_read_index;

    return true;
}

bool ring_buffer_write(ring_buffer_t* rb, uint8_t byte){
    uint32_t local_read_index = rb -> read_index;
    uint32_t local_write_index = rb -> write_index;

    local_write_index = (local_write_index + 1) & rb -> mask;

    if(local_read_index == local_write_index){
        return false;
    }

    rb -> buffer[local_write_index] = byte;
    rb -> write_index = local_write_index;

    return true;

}
