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
int Prom;
float Volt;
//short Volt;
uint32_t sample[8];
//***********************************MÉTODOS****************************************//
void ADC_Setup(void) {
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH11);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH11);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH11);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH11);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH11);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH11);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH11);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH11|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 0);
}

int main(void) {
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_4);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_5);
    ADC_Setup();

    while(1) {
           ADCIntClear(ADC0_BASE, 0);
           ADCProcessorTrigger(ADC0_BASE, 0);
               while(!ADCIntStatus(ADC0_BASE, 0, false)) {
               }
               ADCSequenceDataGet(ADC0_BASE, 0, sample);
               Prom = (sample[0] + sample[1] + sample[2] + sample[3] + sample[4] + sample[5] + sample[6] + sample[7])/8;
               Volt = ((Prom*3.3)/4096);
               SysCtlDelay(10000000);
       }
}
