/**********************************LIBRERIAS DE C************************************/
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
/***********************************LIBRERIAS****************************************/
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/pwm.h"
//*********************************DEFINICIONES*************************************//
#define Frequency 50 //Se define la freq = 50Hz a la que trabaja el servo Futaba S3003.

//**********************************VARIABLES***************************************//
float DutyCycle;    //Guarda el valor del ciclo de trabajo calculado para la señal PWM.

uint32_t LoadServo; //Guarda el valor de carga calculado = al periodo de la señal PWM a usar.
uint32_t PWMClock;  //Guarda el valor de freq. a usar para el módulo PWM.

//********************************PROCEDIMIENTOS************************************//
void ConfigGPIO(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //Se habilita puerto D.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //Se habilita puerto F.
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0); //Se establece pin PD0 con PWM.
    GPIOPinConfigure(GPIO_PD0_M1PWM0);           //Se establece la salida PWM correspondiente a pin PD0.
}

void PWM_Config(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); //Se habilita módulo PWM 1.
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);        //Se establece la freq a la que trabajará el PWM 1.

    PWMClock = SysCtlClockGet()/64;             //Se calcula la freq a la que trabajará el PWM 1.
    LoadServo = (PWMClock / Frequency) - 1;     //Se calcula la carga para el PWM1.

    PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);  //Se configura el PWM 1, se establece generador a usar y tipo de señal.
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, LoadServo);          //Se establece la carga para generador 0 del PWM 1.
    PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);            //Se habilita salida 0 del PWM 1 = PD0.
    PWMGenEnable(PWM1_BASE, PWM_GEN_0);                        //Se habilita generador 0 del PWM 1.
}

int main(void) {
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5); //Se establece freq. de TivaC a 80MHz.
    ConfigGPIO();
    PWM_Config();
    float pulseWidth, angle = 1764;  //81
    int i;

    while(1) {
        if(i < angle){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
            for(i = 1486; i < angle; i++){
//               pulseWidth = ((125/9)*i) + 375;
//               PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, pulseWidth);  //Se establece el ancho de pulso de la señal PWM a usar.
               PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, i);
               SysCtlDelay((0.008*SysCtlClockGet())/3);
//               SysCtlDelay(1000/3);
           }
        }else{
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, pulseWidth);  //Se establece el ancho de pulso de la señal PWM a usar.
        }
    }
}
