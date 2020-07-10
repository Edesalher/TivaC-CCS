//*******************************************LIBRERIAS DE C**********************************************//
#include <stdint.h>
#include <stdbool.h>
//*******************************************LIBRERIAS TIVA C********************************************//
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"       //Periféricos de entrada y salida
#include "inc/hw_gpio.h"          //Para desbloquear pines.
#include "inc/hw_memmap.h"        //Macros defining the memory map of the device.
#include "inc/hw_types.h"         //Common types and macros.
#include "inc/tm4c123gh6pm.h"     //TM4C123GH6PM Register Definitions.
#include "driverlib/pin_map.h"    //Mapping of peripherals to pins for all parts. Configurar pines de GPIO
#include "driverlib/interrupt.h"  //Prototypes for the NVIC Interrupt Controller Driver. Para interrupciones
#include "driverlib/uart.h"       //Para protocolo UART
#include "driverlib/pwm.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"

#define ClockRegistroF  4
#define DataRegistroC   4
#define ClockRegistroC  8
#define ClearRegistroF  8

bool HabilitarColumna = true;

int CARACTER[8][8] = {
                      {0,0,0,0,0,0,0,0},
                      {0,1,1,1,1,1,1,0},
                      {0,1,1,0,0,0,0,0},
                      {0,1,1,1,1,1,0,0},
                      {0,1,1,1,1,1,0,0},
                      {0,1,1,0,0,0,0,0},
                      {0,1,1,1,1,1,1,0},
                      {0,0,0,0,0,0,0,0}
};

void GPIO(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_2|GPIO_PIN_3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}

void Start_Matriz(void){
    int i = 0;
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ClearRegistroF);

    for(i = 0; i < 8; i++){
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, DataRegistroC);
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, DataRegistroC|ClockRegistroC);
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
    }
}

void Clock_Matriz(void){
    if(HabilitarColumna){
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ClockRegistroC);
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
        HabilitarColumna = false;
    }else{
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, DataRegistroC);
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, DataRegistroC|ClockRegistroC);
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
    }
}

void Print_Matriz(void){
    int f = 7, c = 0;

    for(c = 0; c < 8; c++){
        Clock_Matriz();
        for(f = 7; f > -1; f--){
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, (2*CARACTER[f][c])|ClearRegistroF);
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, (2*CARACTER[f][c])|ClockRegistroF|ClearRegistroF);
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ClearRegistroF);
        }
        SysCtlDelay((0.001*SysCtlClockGet())/3);
        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ClearRegistroF);
    }
    HabilitarColumna = true;
}

int main(void){
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_5);  //Reloj de Tiva C a 40MHz
    GPIO();
    Start_Matriz();

    while(true){
        Print_Matriz();
    }
}
