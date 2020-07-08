/*************************************************** LIBRERIAS DE C ***********************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
/************************************************ LIBRERIAS PARA TIVAC ********************************************************/
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "uartstdio.h"
/**************************************************** DEFINICIONES ************************************************************/
#define RX              GPIO_PIN_0  //PA0
#define TX              GPIO_PIN_1  //PA1
#define TRIGGER         GPIO_PIN_1  //PE1
#define ECHO            GPIO_PIN_0  //PB0
/************************************************ VARIABLES GLOBALES **********************************************************/
bool   echo = true, medidaHecha = false;
double pulseTime;
int    posicion;
/************************************************** CONFIGURACIONES ************************************************************/
void TIVACSetup(){
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5); //Se establece el reloj de TivaC a 80MHz.

    //PERIPH Setup
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);   //Se habilita PUERTO A.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);   //Se habilita PUERTO B.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);   //Se habilita PUERTO E.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);  //Se habilita TIMER0.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);   //Se habilita UART0.

    //GPIO Digital Setup
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, TRIGGER);  //TRIGGER es salida.
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, ECHO);      //ECHO es entrada.

    //ECHOInterruptSetup
    IntMasterEnable();                                       //Se habilitan interrupciones del periférico NVIC.
    IntEnable(INT_GPIOB);                                    //Se habilitan interrupciones por PUERTO B.
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0);          //Se habilita interrupciones por PB0 = ECHO.
    GPIOIntTypeSet(GPIO_PORTB_BASE, ECHO, GPIO_BOTH_EDGES);  //ECHO interrumpirá por flanco de subida y bajada.

    //TIMERSetup
    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT_UP);      //TIMER0 contará con 1 disparo ascendente.
    TimerLoadSet(TIMER0_BASE, TIMER_A, 2400000);             //Se carga al timer con 2.4M = 30ms.

    //UARTSetup
    GPIOPinTypeUART(GPIO_PORTA_BASE, RX|TX);                 //Pines PA0 y PA1 como tipo UART.
    GPIOPinConfigure(GPIO_PA0_U0RX);                         //PA0 = U0RX.
    GPIOPinConfigure(GPIO_PA1_U0TX);                         //PA1 = U0TX.
    UARTStdioConfig(0, 115200, SysCtlClockGet());            //Puerto 0, Baudarate=115200, Reloj de UART.
}
/************************************************ RUTINAS DE INTERRUPCIÓN *****************************************************/
void ECHOPulse(){
    GPIOIntClear(GPIO_PORTB_BASE, ECHO);
    pulseTime = 0;
    echo = !echo;
    if(echo){
        TimerEnable(TIMER0_BASE, TIMER_A); //Inicia conteo de TIMER0.
    }else{
        TimerDisable(TIMER0_BASE, TIMER_A); //Inicia conteo de TIMER0.
        pulseTime = TimerValueGet(TIMER0_BASE, TIMER_A)/80;
        HWREG(TIMER0_BASE + TIMER_O_TAV) = 0;  //Se resetea TIMER0.
        medidaHecha = true;
    }
}
/************************************************** PROCEDIMIENTOS ************************************************************/
double GetDistance(){
    //Pulso de disparo.
    GPIOPinWrite(GPIO_PORTE_BASE, TRIGGER, TRIGGER);  //TRIGGER = HIGH.
    SysCtlDelay((0.00001*SysCtlClockGet())/3);        //delay = 10us.
    GPIOPinWrite(GPIO_PORTE_BASE, TRIGGER, 0);        //TRIGGER = LOW.
    //Espera flanco de subida por el pin ECHO.
    while(!medidaHecha);
    medidaHecha = false;
    //El tiempo del pulso ECHO ya fué medido. Se retorno la distancia en cm.
    SysCtlDelay(1600000/3);                           //delay = 20ms.
    return pulseTime/58;
}
/******************************************************** MAIN ****************************************************************/
int main(void){
    TIVACSetup();

    while(true){
        posicion = GetDistance();
        UARTprintf("Distancia: %d cm \n\r", posicion);
    }
}
