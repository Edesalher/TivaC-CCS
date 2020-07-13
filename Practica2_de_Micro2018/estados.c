#include <global.h>
#include <stdint.h>     //To use integer type values.
#include <stdbool.h>

#include "driverlib/gpio.h"     //Definitions and configurations for GPIO peripheral.
#include "inc/hw_memmap.h"      //Macros defining the memory map of the device.
#include "driverlib/timer.h"

int colorValue = 0, sum;


void led_rgb(){
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    if(colorValue == 14){
        colorValue = 0;
    }else{
        colorValue += 2;
    }
}


bool even_number(int number){
    if(number % 2 == 0){
        return true;
    }else{
        return false;
    }
}


/************************* STATES **************************/

void state1_rest(){
    GPIOPinWrite(GPIO_PORTF_BASE, ledrgb, colorValue);
}


void state2_fibonacci(){
    int i, n = 8, f0 = 0, f1 = 1, fn, fn_one, fn_two;
    fn_one = f1;
    fn_two = f0;

    sum = 0;
    for(i = 2; i <= n; i++){
        fn =  fn_one + fn_two;
        if(even_number(fn)){
            sum += fn;
        }else{
            sum = sum;
        }
        fn_two = fn_one;
        fn_one = fn;
    }
}
