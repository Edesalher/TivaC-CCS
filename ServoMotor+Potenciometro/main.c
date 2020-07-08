/**********************************LIBRERIAS DE C************************************/
#include <stdint.h>
#include <stdbool.h>
/***********************************LIBRERIAS****************************************/
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
//*********************************DEFINICIONES*************************************//
#define Frequency 50 //Se define la freq = 50Hz a la que trabaja el servo SG90.

//**********************************VARIABLES***************************************//
int Prom; //Variable que guarda el promedio de las muestras tomadas por el ADC.
float DutyCycle; //Variable que guarda el valor de ciclo de trabajo calculado para la señal PWM.
uint32_t sample[8]; //Variable que guarda las 8 muestra tomadas por el ADC.

uint32_t LoadServo; //Variable que guarda el valor de carga calculado para periodo de la señal PWM a usar.
uint32_t PWMClock;  //Variable que guarda el valor de freq. calculada para el módulo PWM.

//********************************PROCEDIMIENTOS************************************//
void ConfigGPIO(void){
    //PWM
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //Se habilita puerto D.
    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0); //Se establece pin PD0 con PWM.
    GPIOPinConfigure(GPIO_PD0_M1PWM0);  //Se establece la salida PWM correspondiente a pin PD0.
    //ADC
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0); //Se habilita módulo ADC 0.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //Se habilita puerto B.
    GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_5); //Se establece pin PB5 como ADC.
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_ANALOG); //Se establece pin PB5 como analógico.
}

void ADC_Setup(void) {
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0); //Se configura el ADC0, se establece secuenciador a usar y tipo de disparo.
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH11); //Se establece la toma de 1er muestra --> Se lee canal 11 = PB5.
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH11); //Se establece la toma de 2da muestra --> Se lee canal 11 = PB5.
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH11); //Se establece la toma de 3er muestra --> Se lee canal 11 = PB5.
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH11); //Se establece la toma de 4ta muestra --> Se lee canal 11 = PB5.
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH11); //Se establece la toma de 5ta muestra --> Se lee canal 11 = PB5.
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH11); //Se establece la toma de 6ta muestra --> Se lee canal 11 = PB5.
    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH11); //Se establece la toma de 7ma muestra --> Se lee canal 11 = PB5.
    ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH11|ADC_CTL_IE|ADC_CTL_END); //Se establece la toma de 8va y última muestra, y se interrumpe.
    ADCSequenceEnable(ADC0_BASE, 0); //Se habilita la secuencia del ADC0.
}

void PWM_Config(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); //Se habilita módulo PWM 1.
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64); //Se establece la freq. a la que trabajará el PWM 1.

    PWMClock = SysCtlClockGet()/64;  //Se calcula la freq. a la que trabajará el PWM 1.
    LoadServo = (PWMClock / Frequency) - 1; //Se calcula la carga para el PWM1 = Periódo se la señal PWM a usar.

    PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);  //Se configura el PWM 1, se establece generador a usar y tipo de señal.
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, LoadServo); //Se establece la carga para generador 0 del PWM 1.
    PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true); //Se habilita salida 0 del PWM 1 = PD0.
    PWMGenEnable(PWM1_BASE, PWM_GEN_0); //Se habilita(Activa) generador 0 del PWM 1.
}

int main(void) {
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5); //Se establece freq. de TivaC a 80MHz.
    //Se hace llamada a métodos de configuraciones.
    ConfigGPIO();
    ADC_Setup();
    PWM_Config();

    while(1) {
        ADCIntClear(ADC0_BASE, 0); //Se limpia interrupción dada secuenciador 0 del ADC 0.
        ADCProcessorTrigger(ADC0_BASE, 0); //Se dispara el secuenciador 0 del ADC 0.
            while(!ADCIntStatus(ADC0_BASE, 0, false)) { //Se verifica si hay nueva lectura en ADC 0. Si no la hay, se permanece en el while.
            }
            ADCSequenceDataGet(ADC0_BASE, 0, sample); //Se guardan las 8 muestras tomadas por el ADC 0 en vector sample.
            Prom = (sample[0] + sample[1] + sample[2] + sample[3] + sample[4] + sample[5] + sample[6] + sample[7])/8; //Se calcula el promedio de las 8 muestras.
            DutyCycle = ((Prom*125)/4096); //Se calcula el ciclo de trabajo para la señal PWM a usar.
            //SysCtlDelay((0.1*SysCtlClockGet())/3); //Se hace una pausa de 0.1 seg.

       PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, (DutyCycle*LoadServo)/1000);  //Se establece el ancho de pulso de la señal PWM a usar.
    }
}
