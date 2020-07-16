#include <global.h>
#include <stdint.h>     //To use integer type values.
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"   //To configure the clock, enable peripherals and general control.
#include "driverlib/gpio.h"     //Definitions and configurations for GPIO peripheral.
#include "inc/hw_memmap.h"      //Macros defining the memory map of the device.
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"        //It is used to unlock PF0 pin.
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#define option_bits_7a10 GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
#define option_bit_11    GPIO_PIN_3
#define enter            GPIO_PIN_2
#define timerload        80000000   //80x10^6 because this value corresponds ton1 second.
#define buttons          GPIO_PIN_0|GPIO_PIN_4


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


//Configurations of pins and resistors for the inputs.
void GPIO_settings(){
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, data_bits_0a3);
    GPIOPadConfigSet(GPIO_PORTD_BASE, data_bits_0a3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, data_bits_4a6);
    GPIOPadConfigSet(GPIO_PORTA_BASE, data_bits_4a6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, option_bits_7a10);
    GPIOPadConfigSet(GPIO_PORTC_BASE, option_bits_7a10, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, option_bit_11|enter);
    GPIOPadConfigSet(GPIO_PORTB_BASE, option_bit_11, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOPadConfigSet(GPIO_PORTB_BASE, enter, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    //Unlocking PF0 pin.
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, buttons);
    GPIOPadConfigSet(GPIO_PORTF_BASE, buttons, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, ledrgb);
}


void TIMER_settings(){
    //The timer is set as periodic, so when the count ends, it starts again automatically.
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, timerload);
}


void interrupt_settings(){
    IntMasterEnable();

    IntEnable(INT_GPIOB);
    GPIOIntEnable(GPIO_PORTB_BASE, enter);
    //The enter push button will raise an interruption when a falling edge occurs when pressed.
    GPIOIntTypeSet(GPIO_PORTB_BASE, enter, GPIO_FALLING_EDGE);

    IntEnable(INT_GPIOF);
    GPIOIntEnable(GPIO_PORTF_BASE, buttons);
    //The push buttons will raise an interruption when a falling edge occurs when pressed.
    GPIOIntTypeSet(GPIO_PORTF_BASE, buttons, GPIO_FALLING_EDGE);
    //IntPrioritySet(INT_GPIOF, 0);

    IntEnable(INT_TIMER0A);
    //The timer will raise an interruption when it finishes counting.
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}
