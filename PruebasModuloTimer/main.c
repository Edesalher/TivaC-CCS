/*************************************************** LIBRERIAS DE C ***********************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
/************************************************ LIBRERIAS PARA TIVAC ********************************************************/
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_timer.h"
#include "driverlib/pin_map.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/timer.h"
/************************************************ VARIABLES GLOBALES **********************************************************/
double period;
int contador;
double valorTimer;
//double time = 0;
int PF4;
bool disable = false;
/************************************************** CONFIGURACIONES ************************************************************/
void TIVACSetup(void){
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5); //Se establece el reloj de TivaC a 80MHz.
}

void TIMERSetup(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);        //Se habilita el módulo TIMER0.
    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT_UP);  //Se configura el modo de conteo del TIMER0 como disparo ascendente.
    TimerLoadSet(TIMER0_BASE, TIMER_A, 80000000);          //Se carga el TIMER0A.
    TimerEnable(TIMER0_BASE, TIMER_A);                   //Se habilita el TIMER0.
}

void GPIOSetup(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);         //Activar puerto F
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}
/************************************************ RUTINAS DE INTERRUPCIÓN *****************************************************/

/************************************************** PROCEDIMIENTOS ************************************************************/

/******************************************************** MAIN ****************************************************************/
int main(void){
    TIVACSetup();
    TIMERSetup();
    GPIOSetup();

    while(true){
        //TimerDisable(TIMER0_BASE, TIMER_A);                   //Se habilita el TIMER0.
        period = TimerValueGet(TIMER0_BASE, TIMER_A);
        if(period == 0){
            contador++;
            if(contador > 20){
                contador = 0;
            }
        }
        PF4 = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
        if(PF4 == 0){
            if(disable){
                disable = false;
            }else{
                disable = true;
            }
        }
        if(disable){
            TimerDisable(TIMER0_BASE, TIMER_A);                   //Se habilita el TIMER0.
            valorTimer = TimerValueGet(TIMER0_BASE, TIMER_A);
            HWREG(TIMER0_BASE + TIMER_O_TAV) = 0;
            //TimerLoadSet(TIMER0_BASE, TIMER_A, valorTimer);          //Se carga el TIMER0A.
        }else{
            TimerEnable(TIMER0_BASE, TIMER_A);                   //Se habilita el TIMER0.
        }
    }
}
