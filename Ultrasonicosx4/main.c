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
#define RX        GPIO_PIN_0  //PA0
#define TX        GPIO_PIN_1  //PA1

#define TRIGGER1  GPIO_PIN_1  //PE1
#define TRIGGER2  GPIO_PIN_2  //PE2
#define TRIGGER3  GPIO_PIN_3  //PE3
#define TRIGGER4  GPIO_PIN_4  //PE4

#define ECHO1     GPIO_PIN_0  //PB0
#define ECHO2     GPIO_PIN_1  //PB1
#define ECHO3     GPIO_PIN_4  //PB4
#define ECHO4     GPIO_PIN_5  //PB5
#define ECHOINT1  GPIO_INT_PIN_0
#define ECHOINT2  GPIO_INT_PIN_1
#define ECHOINT3  GPIO_INT_PIN_4
#define ECHOINT4  GPIO_INT_PIN_5

#define SoundVel  34000       //Velocidad del sonido = 34000 cm/s.
/************************************************ VARIABLES GLOBALES **********************************************************/
uint32_t ECHOPin;
bool     echo = true, medidaHecha = false;
double   time1, time2, time3, time4;
int      posicion1, posicion2, posicion3, posicion4;
/************************************************** CONFIGURACIONES ************************************************************/
void TIVACSetup(){
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5); //Se establece el reloj de TivaC a 80MHz.
}

void PERIPHSetup(){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);   //Se habilita PUERTO A.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);   //Se habilita PUERTO B.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);   //Se habilita PUERTO E.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);  //Se habilita TIMER0.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);   //Se habilita UART0.
}

void GPIODigitalSetup(){
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, TRIGGER1|TRIGGER2|TRIGGER3|TRIGGER4);  //TRIGGERs son salidas.
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, ECHO1|ECHO2|ECHO3|ECHO4);               //ECHOs son entradas.
}

void ECHOInterruptSetup(){
    IntMasterEnable();                                                          //Se habilitan interrupciones del periférico NVIC.
    IntEnable(INT_GPIOB);                                                       //Se habilitan interrupciones por PUERTO B.
    GPIOIntEnable(GPIO_PORTB_BASE, ECHOINT1|ECHOINT2|ECHOINT3|ECHOINT4);        //Se habilita interrupciones por ECHOs.
    GPIOIntTypeSet(GPIO_PORTB_BASE, ECHO1|ECHO2|ECHO3|ECHO4, GPIO_BOTH_EDGES);  //ECHOs interrumpirán por flanco de subida y bajada.
}

void TIMERSetup(){
    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT_UP);      //TIMER0 contará con 1 disparo ascendente.
    TimerLoadSet(TIMER0_BASE, TIMER_A, 2400000);             //Se carga al timer con 2.4M = 30ms.
}

void UARTSetup(){
    GPIOPinTypeUART(GPIO_PORTA_BASE, RX|TX);                 //Pines PA0 y PA1 como tipo UART.
    GPIOPinConfigure(GPIO_PA0_U0RX);                         //PA0 = U0RX.
    GPIOPinConfigure(GPIO_PA1_U0TX);                         //PA1 = U0TX.
    UARTStdioConfig(0, 115200, SysCtlClockGet());            //Puerto 0, Baudarate=115200, Reloj de UART.
}
/************************************************ RUTINAS DE INTERRUPCIÓN *****************************************************/
void ECHOPulse(){
    ECHOPin = GPIOIntStatus(GPIO_PORTB_BASE, true);    //Se determina qué ECHO interrumpió.
    GPIOIntClear(GPIO_PORTB_BASE, ECHOPin);            //Se limpia la interrupción de dicho ECHO.
    double conteo = 0;
    if(echo){
        TimerEnable(TIMER0_BASE, TIMER_A);             //Se da inicio al conteo del TIMER0A.
        echo = false;
    }else{
        TimerDisable(TIMER0_BASE, TIMER_A);            //Se detiene el conteo del TIMER0A.
        conteo = TimerValueGet(TIMER0_BASE, TIMER_A);  //Se toma el valor actual que tiene el TIMER0A.
        switch(ECHOPin){
        case ECHOINT1:
            time1 = conteo/80000000;                        //Se calcula el tiempo transcurrido en seg.
            break;
        case ECHOINT2:
            time2 = conteo/80000000;                        //Se calcula el tiempo transcurrido en seg.
            break;
        case ECHOINT3:
            time3 = conteo/80000000;                        //Se calcula el tiempo transcurrido en seg.
            break;
        case ECHOINT4:
            time4 = conteo/80000000;                        //Se calcula el tiempo transcurrido en seg.
            break;
        default:
            break;
        }
        HWREG(TIMER0_BASE + TIMER_O_TAV) = 0;          //Se resetea TIMER0A.
        echo = true;
        medidaHecha = true;
    }
}
/************************************************** PROCEDIMIENTOS ************************************************************/
double GetDistance(uint32_t TRIGGERPin){
    double d1 = 0, d2 = 0, d3 = 0, d4 = 0;
    double distance;
    //Pulso de disparo.
    GPIOPinWrite(GPIO_PORTE_BASE, TRIGGERPin, TRIGGERPin);  //TRIGGER = HIGH.
    SysCtlDelay(800/3);                                     //delay = 10us.
    GPIOPinWrite(GPIO_PORTE_BASE, TRIGGERPin, 0);           //TRIGGER = LOW.
    //Espera flanco de subida por el pin ECHO.
    while(!medidaHecha);
    medidaHecha = false;
    //Demora de seguridad.
    SysCtlDelay(1000000/3);                                 //Delay = 12.5ms.
    //El tiempo del pulso ECHO ya fué medido. Se determina la distancia.
    switch(ECHOPin){
    case ECHOINT1:
        d1 = (SoundVel*time1)/2;
        distance = d1;
        break;
    case ECHOINT2:
        d2 = (SoundVel*time2)/2;
        distance = d2;
        break;
    case ECHOINT3:
        d3 = (SoundVel*time3)/2;
        distance = d3;
        break;
    case ECHOINT4:
        d4 = (SoundVel*time4)/2;
        distance = d4;
        break;
    default:
        break;
    }
    return distance;
}

void Ultrasonics(int numSensor){
    switch(numSensor){
    case 0:
        posicion1 = GetDistance(TRIGGER1); //Se obtiene distancia de sensor 1.
        break;
    case 1:
        posicion2 = GetDistance(TRIGGER2); //Se obtiene distancia de sensor 2.
        break;
    case 2:
        posicion3 = GetDistance(TRIGGER3); //Se obtiene distancia de sensor 3.
        break;
    case 3:
        posicion4 = GetDistance(TRIGGER4); //Se obtiene distancia de sensor 4.
        break;
    default:
        break;
    }
}
/******************************************************** MAIN ****************************************************************/
int main(void){
    TIVACSetup();
    PERIPHSetup();
    GPIODigitalSetup();
    ECHOInterruptSetup();
    TIMERSetup();
    UARTSetup();
    GPIOPinWrite(GPIO_PORTE_BASE, TRIGGER1|TRIGGER2|TRIGGER3|TRIGGER4, 0);  //TRIGGERs = LOW.
    SysCtlDelay(400/3);                                                     //delay = 5us.
    int sensor = 0;

    while(true){
        if(sensor > 3){
            sensor = 0;
        }
        Ultrasonics(sensor);
        UARTprintf("Distancias: %d, %d, %d, %d\n\r", posicion1, posicion2, posicion3, posicion4);
        sensor++;
    }
}
