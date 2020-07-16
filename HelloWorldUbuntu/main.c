#include <stdint.h>     //To use integer type values.
#include <stdbool.h>    //To use boolean type values.

#include "driverlib/sysctl.h"   //To configure the clock, enable peripherals and general control.
#include "driverlib/gpio.h"     //Definitions and configurations for GPIO peripheral.
#include "inc/hw_memmap.h"      //Macros defining the memory map of the device.


int main(void){
    //Setting the clock frequency to 80MHz.
    SysCtlClockSet(SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5);
    //Enabling port F.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    //PF1, PF2 and PF3 as GPIO outputs.
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    while(true){
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 8); //Green LED on
        SysCtlDelay((0.5*SysCtlClockGet())/3);        //2 seconds delay.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0); //Green LED off.
        SysCtlDelay((0.5*SysCtlClockGet())/3);        //2 seconds delay.
    }
}
