/*************************************************** LIBRERIAS DE C ***********************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
/************************************************ LIBRERIAS PARA TIVAC ********************************************************/
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "uartstdio.h"
/**************************************************** DEFINICIONES ************************************************************/
#define ParametrosUART  UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE
#define RX              GPIO_PIN_0  //PA0
#define TX              GPIO_PIN_1  //PA1
#define TRIGGER         GPIO_PIN_0  //PB0
#define ECHO            GPIO_PIN_1  //PB1
#define SoundVel        34000       //Velocidad del sonido = 34000 cm/s.
/************************************************ VARIABLES GLOBALES **********************************************************/
bool echo = true, medidaHecha = false;
double time;
int posicion;
/************************************************** CONFIGURACIONES ************************************************************/
void TIVACSetup(void){
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5); //Se establece el reloj de TivaC a 80MHz.
}

void GPIOSetup(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);                                        //Se habilita PUERTO B.
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, TRIGGER);                                    //TRIGGER es salida.
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, ECHO);                                        //ECHO es entrada.
    GPIOPadConfigSet(GPIO_PORTB_BASE, ECHO, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);  //ECHO con resistencia PULL-DOWN.
}

void ECHOInterruptSetup(void){
    IntMasterEnable();                                       //Se habilitan interrupciones del periférico NVIC.
    IntEnable(INT_GPIOB);                                    //Se habilitan interrupciones por PUERTO B.
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_1);          //Se habilita interrupciones por PB1 = ECHO.
    GPIOIntTypeSet(GPIO_PORTB_BASE, ECHO, GPIO_BOTH_EDGES);  //ECHO interrumpirá por flanco de subida y bajada.
}

void TIMERSetup(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);        //Se habilita TIMER0.
    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT_UP);  //TIMER0 contará con 1 disparo ascendente.
}

void UARTSetup(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);                                //Se habilita UART 0.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);                                //Se habilita el puerto A.
    GPIOPinTypeUART(GPIO_PORTA_BASE, RX|TX);                                    //Pines PA0 y PA1 como tipo UART.
    GPIOPinConfigure(GPIO_PA0_U0RX);                                            //PA0 = U0RX.
    GPIOPinConfigure(GPIO_PA1_U0TX);                                            //PA1 = U0TX.
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, ParametrosUART);  //Baudrate=115200, cadena de 8 bits, 1 bit de parada, paridad par.
    UARTStdioConfig(0, 115200, SysCtlClockGet());                               //Puerto 0, Baudarate=115200, Reloj de UART.
}
/************************************************ RUTINAS DE INTERRUPCIÓN *****************************************************/
void ECHOPulse(void){
    GPIOIntClear(GPIO_PORTB_BASE, ECHO);  //Se limpia la interrupción de ECHO.
    double conteo = 0;
    if(echo){
        TimerLoadSet(TIMER0_BASE, TIMER_A, 2400000);   //Se carga al timer con 2.4M = 30ms.
        TimerEnable(TIMER0_BASE, TIMER_A);             //Se da inicio al conteo del TIMER0A.
        echo = false;
    }else{
        TimerDisable(TIMER0_BASE, TIMER_A);            //Se detiene el conteo del TIMER0A.
        conteo = TimerValueGet(TIMER0_BASE, TIMER_A);  //Se toma el valor actual que tiene el TIMER0A.
        time = conteo/80000000;                        //Se calcula el tiempo transcurrido en seg.
        TimerLoadSet(TIMER0_BASE, TIMER_A, conteo);    //Se cambia el límite de conteo al TIMER0A.
        TimerEnable(TIMER0_BASE, TIMER_A);             //Se continúa el conteo de TIMER0A para que se resetee.
        echo = true;
        medidaHecha = true;
    }
}
/************************************************** PROCEDIMIENTOS ************************************************************/
double GetPosition(void){
    double distance = 0;
    //Pulso de disparo.
    GPIOPinWrite(GPIO_PORTB_BASE, TRIGGER, 1);        //TRIGGER = HIGH.
    SysCtlDelay(800/3);                               //delay = 10us.
    GPIOPinWrite(GPIO_PORTB_BASE, TRIGGER, 0);        //TRIGGER = LOW.
    //Espera flanco de subida por el pin ECHO.
    while(!medidaHecha);
    medidaHecha = false;
    //El tiempo del pulso ECHO ya fué medido. Se determina la distancia.
    distance = (SoundVel*time)/2;
    //Demora de seguridad y retorna con el valor de centimetros calculado.
    SysCtlDelay(4000000/3);                           //delay = 50ms.
    return distance;
}

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
    GPIOSetup();
    ECHOInterruptSetup();
    TIMERSetup();
    UARTSetup();
    GPIOPinWrite(GPIO_PORTB_BASE, TRIGGER, 0);  //TRIGGER = LOW.
    SysCtlDelay(400/3);                         //delay = 5us.

    while(true){
        posicion = GetPosition();
        UARTwrite("Distancia: ", 11);
        PrintData(posicion);
        UARTwrite(" cm", 3);
        UARTwrite("\n\r", 2);
    }
}
