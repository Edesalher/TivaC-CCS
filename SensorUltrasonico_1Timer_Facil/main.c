/*************************************************** LIBRERIAS DE C ***********************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
/************************************************ LIBRERIAS PARA TIVAC ********************************************************/
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
/*************************************************** DEFINICIONES *************************************************************/

/************************************************ VARIABLES GLOBALES **********************************************************/
bool echo = true;
double time = 0;
double timerValue = 0;
int posicionDePelotita;
/************************************************** PROCEDIMIENTOS ************************************************************/
void GPIOConfig(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);                                              //Se habilita PUERTO B.
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0);                                       //PB0 = TRIGGER.
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_1);                                        //PB1 = ECHO.
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);  //PB1 con resistencia PULL-DOWN.
}

void TIMERConfig(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);        //Se habilita el TIMER0.
    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT_UP);  //Se configura el modo de conteo del TIMER0 como un disparo ascendente.
}

void INTERRUPConfig(void){
    IntMasterEnable();                                             //Se habilitan las interrupciones globales del periférico NVIC.
    IntEnable(INT_GPIOB);                                          //Se habilitan las interrupciones del puerto B.
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_1);                //Se habilita la interrupcion por PB1 = ECHO.
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_BOTH_EDGES);  //PB1 interrumpe por flanco de subida y bajada.
}

void TriggerPulse(void){
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 1);  //PB0 = HIGH.
    SysCtlDelay(800/3);                            //delay = 10us.
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 0);  //PB0 = LOW.
}
/************************************************ RUTINAS DE INTERRUPCIÓN *****************************************************/
void ECHO(void){
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_1);        //Se limpia interrupción por PB1.
    if(echo){
        TimerLoadSet(TIMER0_BASE, TIMER_A, 2000000);  //Se carga el TIMER0A (numero de cuentas que hará).
        TimerEnable(TIMER0_BASE, TIMER_A);
        echo = false;
    }else{
        TimerDisable(TIMER0_BASE, TIMER_A);
            timerValue = TimerValueGet(TIMER0_BASE, TIMER_A);
            time = timerValue/80000000;
        TimerEnable(TIMER0_BASE, TIMER_A);
        while(timerValue != 0){
            timerValue = TimerValueGet(TIMER0_BASE, TIMER_A);
        }
        TimerDisable(TIMER0_BASE, TIMER_A);
        echo = true;
    }
}
/******************************************************** MAIN ****************************************************************/
int main(void){
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5); //Se establece reloj de TivaC a 80MHz.
    GPIOConfig();
    TIMERConfig();
    INTERRUPConfig();

    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 0);  //TRIGGER = LOW.
    SysCtlDelay(400/3);                            //delay = 5us.

    while(true){
        TriggerPulse();
        SysCtlDelay(4000000/3);                    //delay = 50ms. Tiempo de espera antes de tomar otra medida.
        posicionDePelotita = ((time/2)*34000);     //Se calcula la posición del objeto.
    }
}
