/**********************************LIBRERIAS DE C************************************/
#include <stdint.h>
#include <stdbool.h>
/***********************************LIBRERIAS****************************************/
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"
//**********************************VARIABLES***************************************//
float Promedio;
float Voltaje;
int Numeros;
uint32_t sample[8];
//***********************************MÉTODOS****************************************//
void ADC_Setup(void) {
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH7);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH7);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH7);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH7);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH7);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH7);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH7);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH7|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 0);
}

int main(void) {
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_4);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_ANALOG); //Se establece pin PD0 como analógico.
    ADC_Setup();

    while(1) {
           ADCIntClear(ADC0_BASE, 0);
           ADCProcessorTrigger(ADC0_BASE, 0);
               while(!ADCIntStatus(ADC0_BASE, 0, false)) {
               }
               ADCSequenceDataGet(ADC0_BASE, 0, sample);
               Promedio = (sample[0] + sample[1] + sample[2] + sample[3] + sample[4] + sample[5] + sample[6] + sample[7])/8;
               Voltaje = (Promedio*3.3)/4096; //Ecuación que determina el valor de voltaje que se lee en el Pot. Voltaje de 0V a 3.3V
               Numeros = (Promedio*7)/4040; //Ecuación que detetermina números del 0 al 7 según se mueve el Pot.
               //SysCtlDelay(10000000);
       }
}
