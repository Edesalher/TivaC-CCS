#include <global.h>
#include <stdint.h>     //To use integer type values.
#include <stdbool.h>

#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"   //To configure the clock, enable peripherals and general control.
#include "driverlib/gpio.h"     //Definitions and configurations for GPIO peripheral.
#include "inc/hw_memmap.h"      //Macros defining the memory map of the device.
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#define data_bits_0a3    GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
#define data_bits_4a6    GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
#define option_bits_7a10 GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
#define option_bit_11    GPIO_PIN_3
#define timerload        80000000   //80x10^6 because this value corresponds ton1 second.


void initial_settings(){
    //Setting the clock frequency to 80MHz.
    SysCtlClockSet(SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5);
}


//Enabling Port A, B, C, D, F and TIMER 0.
void enabling_PERIPH(){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
}


void GPIO_settings(){
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, data_bits_0a3);
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, data_bits_4a6);
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, option_bits_7a10);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, option_bit_11);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, ledrgb);
}


void TIMER_settings(){
    //The timer is set as periodic, so when the count ends, it starts again automatically.
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, timerload);
    //The timers starts counting.
    TimerEnable(TIMER0_BASE, TIMER_A);
}


void interrupt_settings(){
    IntMasterEnable();
    IntEnable(INT_TIMER0A);
    //The timer will raise an interrupt when it finishes counting.
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}
