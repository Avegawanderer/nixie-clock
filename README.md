# nixie-clock
IN-14 nixie clock firmware project based on STM32 MCU

GDB JLINK startup:
set remote hardware-breakpoint-limit 6
set remote hardware-watchpoint-limit 4
monitor semihosting enable
monitor semihosting ioclient 2
monitor reset halt
monitor flash device = STM32F103C8
monitor flash download = 1
load 
monitor reset
break main