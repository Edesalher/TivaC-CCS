/*--------------------------------------------LIBRERÍAS DE C-------------------------------------------------*/
#include<stdint.h>
#include<stdbool.h>
#include <string.h>
/*-----------------------------------------LIBRERÍAS PARA LA TIVA C------------------------------------------*/
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
/*------------------------------------------------VARIABLES--------------------------------------------------*/
char mensaje[40];
/*----------------------------------------------PROCEDIMIENTOS-----------------------------------------------*/
void ConfigurarUART(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0|GPIO_PIN_1);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 38400, (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 38400, (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));
    UARTEnable(UART0_BASE);
    UARTEnable(UART1_BASE);
}

void UARTsprintf_vector(uint32_t ui32Base, const char*ucData_in, uint8_t length){
    uint8_t count_str = 0;
    while(count_str <= length){
        UARTCharPut(ui32Base, ucData_in[count_str]);
        count_str += 1;
    }
}

void UARTsprintf(uint32_t ui32Base, const char* ucData_in){
    uint8_t count_str = 0;
    while(count_str <= strlen(ucData_in)){
        UARTCharPut(ui32Base, ucData_in[count_str]);
        count_str += 1;
    }
}

void UARTsscanf(uint32_t ui32Base, char* ucBuffer){
    uint8_t count_str = 0;
    while(1){
        if(!UARTCharsAvail(ui32Base)){
            ucBuffer[count_str] = UARTCharGet(ui32Base);
            if((ucBuffer[count_str] =='\n')&&(ucBuffer[count_str - 1]=='\r')){
                break;
            }
            count_str += 1;
        }
    }
}
/*------------------------------------------------------MAIN------------------------------------------------------*/
int main(void){
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5);
    ConfigurarUART();

    memset(mensaje, 0, sizeof(mensaje));
    UARTsprintf(UART1_BASE, "AT\n\r"); //Comprobar si nuestro bluetooth responde a los comandos AT
    UARTsscanf(UART1_BASE, mensaje);
    UARTsprintf_vector(UART0_BASE, mensaje, sizeof(mensaje));

    UARTsprintf(UART1_BASE, "AT+NAME=GRUPO17\n\r"); //Cambiar nombre del m�dulo bluetooth a "17"
    UARTsscanf(UART1_BASE, mensaje);
    UARTsprintf_vector(UART0_BASE, mensaje, sizeof(mensaje));

    UARTsprintf(UART1_BASE, "AT+BAUD=921600,0,0\n\r"); //Velocidad = 115200 baudios, Cant. de bits de parada = 1 bit, Sin paridad
    UARTsscanf(UART1_BASE, mensaje);
    UARTsprintf_vector(UART0_BASE, mensaje, sizeof(mensaje));

    UARTsprintf(UART1_BASE, "AT+PSWD=SaleEnSemestre\n\r"); //Cambiar c�digo de vinculaci�n: PIN = SaleMicro
    UARTsscanf(UART1_BASE, mensaje);
    UARTsprintf_vector(UART0_BASE, mensaje, sizeof(mensaje));

    UARTsprintf(UART1_BASE, "AT+ROLE=1\n\r");//Configurar Role de bluetooth: ROLE = 0 -> ESCLAVO
    UARTsscanf(UART1_BASE, mensaje);
    UARTsprintf_vector(UART0_BASE, mensaje, sizeof(mensaje));

    UARTsprintf(UART1_BASE, "AT+RESET\n\r"); //Resetear nuestro m�dulo; despu�s de hacer esto salimos del MODO AT
    UARTsscanf(UART1_BASE, mensaje);
    UARTsprintf_vector(UART0_BASE, mensaje, sizeof(mensaje));

    while(1);
}
