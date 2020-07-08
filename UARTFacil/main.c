/*************************************************** LIBRERIAS DE C ***********************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
//#include <string.h>
//#include <math.h>
/************************************************ LIBRERIAS PARA TIVAC ********************************************************/
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "uartstdio.h"
/**************************************************** DEFINICIONES ************************************************************/
#define ParametrosUART  UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE
#define PA0             GPIO_PIN_0
#define PA1             GPIO_PIN_1
/************************************************ VARIABLES GLOBALES **********************************************************/
/************************************************** CONFIGURACIONES ************************************************************/
void TIVACSetup(void){
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5); //Se establece el reloj de TivaC a 80MHz.
}

void UARTSetup(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);                                //Se habilita módulo UART0.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);                                //Se habilita el PUERTO A.
    GPIOPinTypeUART(GPIO_PORTA_BASE, PA0|PA1);                                  //PA0 y PA1 como tipo UART.
    GPIOPinConfigure(GPIO_PA0_U0RX);                                            //PA0 = RX.
    GPIOPinConfigure(GPIO_PA1_U0TX);                                            //PA1 = TX.
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, ParametrosUART);  //Baudrate=115200, cadena de 8 bits, 1 bit de parada, paridad par.
    UARTStdioConfig(0, 115200, SysCtlClockGet());
}
/************************************************** PROCEDIMIENTOS ************************************************************/
int ArraySize(char ARRAY[]){
    int longitudDelArreglo = 0;
    longitudDelArreglo = sizeof(ARRAY)/sizeof(ARRAY[0]);
    return longitudDelArreglo;
}

void PrintData(int dato){
    int i, longitudArreglo = 0;
    char DATO[] = "";

    ltoa(dato, DATO);
    longitudArreglo = ArraySize(DATO);
    for(i = 0; i < longitudArreglo; i++){
        UARTCharPut(UART0_BASE, DATO[i]);
    }
}
/******************************************************** MAIN ****************************************************************/
int main(void){
    TIVACSetup();
    UARTSetup();

    while(true){
        UARTwrite("Distancia: ", 11);
        PrintData(35);
        UARTwrite(" cm", 3);
        UARTwrite("\n\r", 2);
        SysCtlDelay(4000000/3);
    }
}
