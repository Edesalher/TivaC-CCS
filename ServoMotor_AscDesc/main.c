//********************************LIBRERÍAS*****************************************//
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/tm4c123gh6pm.h"
//******************************LIBRERÍS TIVA C*************************************//
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
//*********************************DEFINICIONES*************************************//
#define Frequency 50
//**********************************VARIABLES***************************************//
short Switch=0;
volatile uint32_t LoadServo;
volatile uint32_t PWMClock;
volatile uint8_t Duty_Cycle = 25;
//********************************INTERRUPCIONES************************************//
void GPIO_Int(void){
    GPIOIntClear(GPIO_PORTF_BASE, GPIOIntStatus(GPIO_PORTF_BASE, true));
    Switch = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);  //Se detecta si se presionó push-button de PF0 ó PF4

    if(Switch == 16){ //Se presionó push-button de PF4
        Duty_Cycle = Duty_Cycle + 2;
        if(Duty_Cycle > 125){
            Duty_Cycle = 125;
        }
    }else if(Switch == 1){  //Se presionó push-button de PF0
        Duty_Cycle = Duty_Cycle - 2;
        if(Duty_Cycle < 25){
            Duty_Cycle = 25;
        }
    }
}
//********************************PROCEDIMIENTOS************************************//
void PWM_Config(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //Habilitar puerto B
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);  //Frecuencia que le llega al reloj del PWM
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);
    GPIOPinConfigure(GPIO_PB6_M0PWM0);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_7);
    GPIOPinConfigure(GPIO_PB7_M0PWM1);
}

void PWM_Init(void){
    PWMClock = SysCtlClockGet()/64;
    LoadServo = (PWMClock / Frequency) - 1;
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, LoadServo);
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
}

void Button_Config(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);  //Habilitar puerto F

    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;  //Desbloquear PF0
    GPIO_PORTF_CR_R = 0x0f;

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);  //PF0 y PF4 como entradas
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);  //PF0 y PF4 con resistencias PULL-DOWN
}

void Button_Int_Config(void){
    IntMasterEnable();  //Habilitar interrupciones globales
    IntEnable(INT_GPIOF);  //Habilitar las interrupciones del puerto F
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_4);  //Habilitar las interrupciones por PF0 y PF4
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, GPIO_LOW_LEVEL);  //Interrupción de PF0 o PF4 por nivel bajo
    IntPrioritySet(INT_GPIOF, 0);  //Prioridad de la interrupción puerto F
}

//************************************MAIN******************************************//
int main(void){
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5);  //Reloj de Tiva C a 80MHz
    PWM_Config();
    PWM_Init();
    Button_Config();

    while(1)  {
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, Duty_Cycle*LoadServo/1000); //Definir el ancho del pulso del PWM
    }
}
