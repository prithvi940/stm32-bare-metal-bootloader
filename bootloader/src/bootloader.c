// the bootloader is only here to jump to main firmware function
//kept separate from main firmware 
// it is of 32 bytes here and has its own IVT
// for each IVT the first entry is of stack pointer which is already pointed automatically by the libopencm3

#include "common-defines.h"
#include "libopencm3/cm3/vector.h"
#include "libopencm3/stm32/memorymap.h"

#define BOOTLOADER_SIZE        (0x8000U)
#define MAIN_APP_START_ADDRESS (FLASH_BASE + BOOTLOADER_SIZE)

static void jump_to_main(void){
  // METHOD 1
  // typedef void (*void_fn)(void);

  // uint32_t* reset_vector_entry = (uint32_t*)(MAIN_APP_START_ADDRESS + 4U); // each entry in the interrupt vector table is of 4 bytes and reset_vector is the second entry in it
  // uint32_t* reset_vector = (uint32_t*)(*reset_vector_entry); // reset_vector_entry is address to the second entry in the interrupt vector table, here we first dereference it and then convert it to the pointer

  // void_fn jump_fn = (void_fn)reset_vector; 

  // jump_fn();

  //METHOD 2
  vector_table_t* main_vector_table = (vector_table_t*)MAIN_APP_START_ADDRESS; // created a pointer of type vector_table_t already in vector.h which points to the vector table of the firmware.c
  main_vector_table -> reset(); // go to vector table of the firmware, then find reset handler address stored there and call it  
}

int main(void){
  jump_to_main();

  // never return from this function
  return 0;
}