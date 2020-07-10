//*******************************************LIBRERIAS DE C**********************************************//
#include <stdint.h>
#include <stdbool.h>
//*******************************************LIBRERIAS TIVA C********************************************//
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"       //Periféricos de entrada y salida
#include "inc/hw_gpio.h"          //Para desbloquear pines.
#include "inc/hw_memmap.h"        //Macros defining the memory map of the device.
#include "inc/hw_types.h"         //Common types and macros.
#include "inc/tm4c123gh6pm.h"     //TM4C123GH6PM Register Definitions.
#include "driverlib/pin_map.h"    //Mapping of peripherals to pins for all parts. Configurar pines de GPIO
#include "driverlib/interrupt.h"  //Prototypes for the NVIC Interrupt Controller Driver. Para interrupciones
#include "driverlib/uart.h"       //Para protocolo UART

//*********************************************VARIABLES*************************************************//
volatile uint8_t dato;
//*******************************************CONFIGURACIÓN GPIO**********************************************//
void ConfigGPIO(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);  //Habilitar puerto F.
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);  //Pines PF1, PF2 y PF3 como salidas.
}
//*******************************************CONFIGURACIÓN UART**********************************************//
void ConfigUART(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600, UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1);
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
}
//*******************************************CONFIGURACIÓN NVIC**********************************************//
void ConfigNVIC(void){
    IntMasterEnable();  //Habilitar interrupciones globales del periférico NVIC.
    IntEnable(INT_UART1);
    UARTIntEnable(UART1_BASE, UART_INT_RX|UART_INT_RT);
}
//**************************************RUTINAS DE INTERRUPCIÓN******************************************//
void IntUART(void){
    uint32_t Status;
    Status = UARTIntStatus(UART1_BASE, true);
    UARTIntClear(UART1_BASE, Status);

    while(UARTCharsAvail(UART1_BASE)){
        dato = UARTCharGetNonBlocking(UART1_BASE);  //Se almacena el dato recibido por UART.
    }
    //UARTCharPut(UART1_BASE, dato);
}
//***********************************************MAIN*****************************************************/
int main(void){
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5); //Reloj de Tiva C a 80MHz
    ConfigGPIO();
    ConfigUART();
    ConfigNVIC();

    while(true){
        switch(dato){
        case 'A':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 2);
            break;
        case 'B':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 4);
            break;
        case 'Y':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 8);
            break;
        case 'X':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
            break;
        case '-':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
            break;
        default:
            break;
        }
    }
}
