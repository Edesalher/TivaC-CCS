/**************************************** LIBRERÍAS DE C *****************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
/************************************ LIBRERÍAS PARA LA TIVA C ***********************************************/
#include "inc/hw_memmap.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"      //Periféricos de entrada y salida
#include "driverlib/pin_map.h"   //Configurar pines de GPIO
#include "driverlib/interrupt.h" //Para interrupciones
#include "driverlib/timer.h"     //Para activar el timer
#include "driverlib/uart.h"      //Para protocolo UART
#include "uartstdio.h"
/**************************************************** DEFINICIONES ************************************************************/
#define ParametrosUART  UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE
#define ECHOPin         GPIO_PIN_6  //PB6
#define TRIGGERPin      GPIO_PIN_7  //PB7
#define RX              GPIO_PIN_0  //PA0
#define TX              GPIO_PIN_1  //PA1
/******************************************* VARIABLES ********************************************************/
long carga = 7999999;                        //Carga inicial de TIMER0A.
double tiempoA, tiempoB, tiempo, distancia;  //Almacenan el valor calculado para la distancia medida y tiempos medidos.
bool trigger = false, echo = true;           //Indican si Trigger y Echo estan en estado alto o bajo.
char DISTANCIA[3] = "";                      //Vectores que almcenan los texot a enviar por UART.
/***************************************** PROCEDIMIENTOS *****************************************************/
void GPIO_Config(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);                                              //Se habilita el puerto B.
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, ECHOPin);                                           //PB6 = ECHO
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, TRIGGERPin);                                       //PB7 = TRIGGER
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);  //PB6 con resistencia PULL-DOWN.
}

void TIMER_Config(void){
    //Configuración de TIMER0A para generar diparo en TRIGGER de 10us y para medir el tiempo de duración del pulso ECHO.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);       //Se habilita el TIMER0.
    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT_UP); //Se configura el modo del TIMER0 como disparo ascendente.
    TimerLoadSet(TIMER0_BASE, TIMER_A, carga);          //Se carga el TIMER0A (numero de cuentas que hará).
    TimerEnable(TIMER0_BASE, TIMER_A);                  //Se habilita TIMER0A.
}

void INTERRUPTIONS(void){
    IntMasterEnable();                                            //Se habilitan las interrupciones globales del periférico NVIC.
    IntEnable(INT_GPIOB);                                         //Se habilitan las interrupciones del puerto B.
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_6);               //Se habilita la interrupcion por PB6 = ECHO.
    GPIOIntTypeSet(GPIO_PORTB_BASE, ECHOPin, GPIO_BOTH_EDGES);    //ECHO interrumpe por flanco de subida y bajada.
    IntPrioritySet(INT_GPIOB, 0);                                 //Prioridad de interrupcion 0.

    IntEnable(INT_TIMER0A);                                       //Se habilitan las interrupciones del TIMER0A.
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);              //TIMER0A interrumpe al terminar de contar.
    IntPrioritySet(INT_TIMER0A, 1);                               //Prioridad de interrupcion 1.
}

void UART_Config(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);                               //Se habilita UART 0.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);                               //Se habilita el puerto A.
    GPIOPinTypeUART(GPIO_PORTA_BASE, RX|TX);                                   //Se configuran PA0 y PA1 como tipo UART.
    GPIOPinConfigure(GPIO_PA0_U0RX);                                           //PA0 = RX.
    GPIOPinConfigure(GPIO_PA1_U0TX);                                           //PA1 = TX.
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, ParametrosUART); //Baudrate=115200, cadena de 8 bits, 1 bit de parada, paridad par.
    UARTStdioConfig(0, 115200, SysCtlClockGet());
}

int ArraySize(char ARRAY[]){
    int longitudDelArreglo = 0;
    longitudDelArreglo = sizeof(ARRAY)/sizeof(ARRAY[0]);
    return longitudDelArreglo;
}
/************************************* RUTINAS DE INTERRUPCIÓN *************************************************/
void TRIGGER(void){
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);     //Se limpia la interrupción por fin de conteo de TIMER0A.
    TimerDisable(TIMER0_BASE, TIMER_A);                 //Se deshabilita TIMER0A.
    if(trigger){
        trigger = false;
        carga = 7999999;                                //Para que TIMER0A de un tiempo de 100ms.
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, 0);   //PB7(TRIGGER) = 0.
    }else{
        trigger = true;
        carga = 799;                                    //Para que TIMER0A de un tiempo de 10us (ancho de tiempo necesario para el pulso del trigger).
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, 128); //PB7(TRIGGER) = 1.
        TimerEnable(TIMER0_BASE, TIMER_A);              //Se habilita TIMER0A.
    }
}

void ECHO(void){
    GPIOIntClear(GPIO_PORTB_BASE, ECHOPin);            //Se limpia la interrupción por PB6(ECHO).
    if(echo){
        echo = false;
        TimerEnable(TIMER0_BASE, TIMER_A);             //Se habilita el TIMER0A porque PB6(ECHO) = 1.
        tiempoA = TimerValueGet(TIMER0_BASE, TIMER_A); //Se toma el valor del TIMER0A porque PB6(ECHO) = 1.
    }else{
        echo = true;
        tiempoB = TimerValueGet(TIMER0_BASE, TIMER_A); //Se toma el valor del TIMER0A porque PB6(ECHO) = 0.
        if(tiempoA > tiempoB){
            tiempo = (tiempoA - tiempoB)/80000000;     //Se calcula el valor del tiempo medido en segundos.
        }else{
            tiempo = (tiempoB - tiempoA)/80000000;     //Se calcula el valor del tiempo medido en segundos.
        }
        distancia = ((34300*tiempo)/2) - 2.7;          //Se calcula el valor de la distancia medida en centímetros (cm).
    }
}

/************************************************ MAIN ********************************************************/
int main(void) {
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5); //Reloj de Tiva C a 80MHz
    GPIO_Config();
    TIMER_Config();
    INTERRUPTIONS();
    UART_Config();
    GPIOPinWrite(GPIO_PORTB_BASE, TRIGGERPin, 0); //PB7(TRIGGER) = 0.
    int i;
    int longitudArreglo;

    while(true){
        UARTwrite("Distancia: ", 11);
        ltoa(distancia, DISTANCIA);
        longitudArreglo = ArraySize(DISTANCIA);
        for(i = 0; i < longitudArreglo; i++){
            UARTCharPut(UART0_BASE, DISTANCIA[i]);
        }
        UARTwrite(" cm", 3);
        UARTwrite("\n\r", 2);
    }
}
