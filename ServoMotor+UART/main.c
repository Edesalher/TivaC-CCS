//********************************Librerias*****************************************//
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
//******************************Librerias Tiva C************************************//
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/uart.h"
//*********************************Definiciones*************************************//
#define Frequency 50
//**********************************Variables***************************************//
volatile uint8_t dato;
uint32_t LoadServo;
uint32_t PWMClock;

uint8_t  Pulso = 2.5;
//***********************************Metodos****************************************//
void ConfigGeneral(void);
void UARTConfig(void);
void ConfigPWM(void);
void Accion(void);

//****************************Rutina Interrupción***********************************//
void UARTIntHandler(void){
    uint32_t Status;
    Status = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, Status);

    while(UARTCharsAvail(UART0_BASE)){
        dato = UARTCharGetNonBlocking(UART0_BASE);
        SysCtlDelay(SysCtlClockGet()/(1000*3));
    }

    UARTCharPut(UART0_BASE, dato);
    Accion();
}

//********************************Procedimientos************************************//
void ConfigGeneral(void){
    SysCtlClockSet(SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
}

void UARTConfig(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0|GPIO_PIN_1);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE);
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX|UART_INT_RT);
    IntMasterEnable();
}

void ConfigPWM(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPinConfigure(GPIO_PD0_M1PWM0 );

    PWMClock = SysCtlClockGet()/64;
    LoadServo = (PWMClock / Frequency) - 1;

    PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, LoadServo);
    PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);
    PWMGenEnable(PWM1_BASE, PWM_GEN_0);
}

void Accion(void){
    if(dato == 'Q'){
        Pulso = 2.5;
    }else if(dato == 'W'){
        Pulso = 5;
    }else if(dato == 'E'){
        Pulso = 7.5;
    }else if(dato == 'R'){
        Pulso = 10;
    }else if(dato == 'T'){
        Pulso = 12.5;
    }
}

//************************************Main******************************************//
void main(void){
    ConfigGeneral();
    UARTConfig();
    ConfigPWM();

    while(1)  {
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, (Pulso*LoadServo)/100);
    }
}
