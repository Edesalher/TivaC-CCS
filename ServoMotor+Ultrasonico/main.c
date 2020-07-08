/*************************************************** LIBRERIAS DE C ***********************************************************/
#include <stdint.h>
#include <stdbool.h>
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
#include "driverlib/pwm.h"
/*************************************************** DEFINICIONES *************************************************************/
#define TRIGGER         GPIO_PIN_0  //PB0
#define ECHO            GPIO_PIN_1  //PB1
#define PWMOUT          GPIO_PIN_0  //PD0
#define SoundVel        34000       //Velocidad del sonido = 34000 cm/s.
#define FreqServo       50          //Freq. a la que trabaja el servo SG90.
/************************************************ VARIABLES GLOBALES **********************************************************/
bool   echo = true, medidaHecha = false;
double time;
double posicion;
/************************************************** CONFIGURACIONES ************************************************************/
void TIVACSetup(){
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5); //Se establece el reloj de TivaC a 80MHz.
}

void PERIPHSetup(){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);   //Se habilita PUERTO A.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);   //Se habilita PUERTO B.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);   //Se habilita PUERTO D.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);  //Se habilita TIMER0.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);    //Se habilita módulo PWM1.
}

void GPIODigitalSetup(){
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, TRIGGER);  //TRIGGER es salida.
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, ECHO);      //ECHO es entrada.
}

void ECHOInterruptSetup(){
    IntMasterEnable();                                         //Se habilitan interrupciones del periférico NVIC.
    IntEnable(INT_GPIOB);                                      //Se habilitan interrupciones por PUERTO B.
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_1);            //Se habilita interrupciones por PB1 = ECHO.
    GPIOIntTypeSet(GPIO_PORTB_BASE, ECHO, GPIO_BOTH_EDGES);    //ECHO interrumpirá por flanco de subida y bajada.
}

void TIMERSetup(){
    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT_UP);        //TIMER0 contará con 1 disparo ascendente.
    TimerLoadSet(TIMER0_BASE, TIMER_A, 2400000);               //Se carga al timer con 2.4M = 30ms.
}

void PWMSetup(){
    uint32_t PWMClock;                                         //Guarda el valor de la freq. a usar para el módulo PWM.
    uint32_t PeriodoPWMServo;                                  //Guarda el valor de la carga calculada = periodo de la señal PWM a usar.
    GPIOPinTypePWM(GPIO_PORTD_BASE, PWMOUT);                   //Pin PD0 como tipo PWM.
    GPIOPinConfigure(GPIO_PD0_M1PWM0);                         //PD0 = MIPWM0.
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);                       //Se establece la freq a la que trabajará el PWM1.
    PWMClock = SysCtlClockGet()/64;
    PeriodoPWMServo = (PWMClock/FreqServo) - 1;
    PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);  //Se determinar usar GNERADOR 0 y tipo de señal.
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, PeriodoPWMServo);    //Carga para GENE0 del PWM1.
    PWMGenEnable(PWM1_BASE, PWM_GEN_0);                        //Se habilita GEN0.
    PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);            //Se habilita PWM1 - OUT0.

}
/************************************************ RUTINAS DE INTERRUPCIÓN *****************************************************/
void ECHOPulse(){
    GPIOIntClear(GPIO_PORTB_BASE, ECHO);               //Se limpia la interrupción de ECHO.
    double conteo = 0;
    if(echo){
        TimerEnable(TIMER0_BASE, TIMER_A);             //Se da inicio al conteo del TIMER0A.
        echo = false;
    }else{
        TimerDisable(TIMER0_BASE, TIMER_A);            //Se detiene el conteo del TIMER0A.
        conteo = TimerValueGet(TIMER0_BASE, TIMER_A);  //Se toma el valor actual que tiene el TIMER0A.
        time = conteo/80000000;                        //Se calcula el tiempo transcurrido en seg.
        HWREG(TIMER0_BASE + TIMER_O_TAV) = 0;          //Se resetea TIMER0A.
        echo = true;
        medidaHecha = true;
    }
}
/************************************************** PROCEDIMIENTOS ************************************************************/
double GetDistance(){
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

void WriteAngleServo(double control){
    double pulseWidth;
    double angulo;
    angulo = ((180/29)*control) - (540/29);
    pulseWidth = (172/12)*(angulo + (180/7));
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, pulseWidth);  //Se establece el ancho de pulso de la señal PWM a usar.
}
/******************************************************** MAIN ****************************************************************/
int main(void){
    TIVACSetup();
    PERIPHSetup();
    GPIODigitalSetup();
    ECHOInterruptSetup();
    TIMERSetup();
    PWMSetup();
    GPIOPinWrite(GPIO_PORTB_BASE, TRIGGER, 0);  //TRIGGER = LOW.
    SysCtlDelay(400/3);                         //delay = 5us.

    while(true){
        posicion = GetDistance();   //Se obtiene la posición actual del objeto.
        WriteAngleServo(posicion);  //Se establece el ángulo del servo.
    }
}
