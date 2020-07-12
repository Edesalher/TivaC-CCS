#include <global.h>
#include <stdint.h>     //To use integer type values.
#include <stdbool.h>

#include "driverlib/gpio.h"     //Definitions and configurations for GPIO peripheral.
#include "inc/hw_memmap.h"      //Macros defining the memory map of the device.
#include "driverlib/timer.h"

int colorValue = 0;


void led_rgb(){
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    if(colorValue == 14){
        colorValue = 0;
    }else{
        colorValue += 2;
    }
}


void state1_rest(){
    GPIOPinWrite(GPIO_PORTF_BASE, ledrgb, colorValue);
}



