/*************************************************** LIBRERIAS DE C ***********************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
/************************************************ LIBRERIAS PARA TIVAC ********************************************************/
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
//#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "uartstdio.h"
/**************************************************** DEFINICIONES ************************************************************/
#define RX              GPIO_PIN_0  //PA0
#define TX              GPIO_PIN_1  //PA1
#define TRIGGER         GPIO_PIN_1  //PE1
#define ECHO            GPIO_PIN_0  //PB0
/************************************************** CONFIGURACIONES ************************************************************/
void TIVACSetup(void){
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5); //Se establece el reloj de TivaC a 80MHz.

    //GPIO Setup
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);                                              //Se habilita PUERTO B.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);                                              //Se habilita PUERTO E.
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, TRIGGER);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, ECHO);
    GPIOPadConfigSet(GPIO_PORTB_BASE, ECHO, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);  //ECHO con resistencia PULL-DOWN.

    //UART Setup
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);   //Se habilita UART 0.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);   //Se habilita el puerto A.
    GPIOPinTypeUART(GPIO_PORTA_BASE, RX|TX);       //Se configuran PA0 y PA1 como tipo UART.
    GPIOPinConfigure(GPIO_PA0_U0RX);               //Se sonfigura PA0 = RX.
    GPIOPinConfigure(GPIO_PA1_U0TX);               //Se configura PA1 = TX.
    UARTStdioConfig(0, 115200, SysCtlClockGet());  //Puerto 0, Baudarate=115200, Reloj de UART.
}
/************************************************** PROCEDIMIENTOS ************************************************************/
int GetDistance(void){
    int ECHOpin, distance = 0;
    //Pulso de disparo
    GPIOPinWrite(GPIO_PORTE_BASE, TRIGGER, TRIGGER);  //TRIGGER = HIGH.
    SysCtlDelay(4000/3);                              //delay = 50us.
    GPIOPinWrite(GPIO_PORTE_BASE, TRIGGER, 0);        //TRIGGER = LOW.
    //Espera flanco de subida por el pin ECHO.
    ECHOpin = GPIOPinRead(GPIO_PORTB_BASE, ECHO);
    while(ECHOpin == 0){
        ECHOpin = GPIOPinRead(GPIO_PORTB_BASE, ECHO);
    }
    //Comienza a contar centimetros hasta que pin ECHO sea cero.
    ECHOpin = GPIOPinRead(GPIO_PORTB_BASE, ECHO);
    while(ECHOpin == 1){
        distance++;
        SysCtlDelay(4640/3);                          //delay = 58us. Tiempo en que el sonido recorre 2cm.
        ECHOpin = GPIOPinRead(GPIO_PORTB_BASE, ECHO);
    }
    //Demora de seguridad y retorna con el valor de centimetros contados.
    SysCtlDelay(4000000/3);                           //delay = 50ms.
    return distance;
}
/******************************************************** MAIN ****************************************************************/
int main(void){
    TIVACSetup();
    int posicion;

    GPIOPinWrite(GPIO_PORTE_BASE, TRIGGER, 0);  //TRIGGER = LOW.
    SysCtlDelay(400/3);                         //delay = 5us.
    while(true){
        posicion = GetDistance();
        UARTprintf("Distancia: %d\n\r", posicion);
    }
}
