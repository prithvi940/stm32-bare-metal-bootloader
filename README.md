# stm32-bare-metal-bootloader

## About the Project
This project is about the learning bare-metal programming from bottom up using the open source tools like libopencm3 and GCC. This repository includes the basic concepts implementation beginning with the Blinking of led then moves towards basic peripherals like systick, timers and interrupts for PWM control of LED and at last focuses on building of the bootloader to update the existing firmware on the board.

## Hardware
- STM32F446RE Nucleo board
- ST-LINK Debugger(On-Board)
- Micro-USB Cable

## Prerequisites 
- Installed and setup
  - GNU ARM Embedded Tools
  - GNU Make
- Configure the linkerscript according to the board being used



## Sources 
- [Youtube Playlist](https://youtube.com/playlist?list=PLP29wDx6QmW7HaCrRydOnxcy8QmW0SNdQ&si=7ZAxgRQhB5zHcD-X)
- [Installing ST-Link drivers and setup environment](https://www.youtube.com/watch?v=FNDp1G0bYoU&t=1084s)

## note
firmware updater implementation is done already in typescript but also separate fw-updater is in progress using python 
