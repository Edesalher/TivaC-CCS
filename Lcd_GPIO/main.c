/*
 * main.c
 *  Author:
 *      Ahmet Eren ODACI
 *
 *  Credit:
 *      Lokman G�KDERE
 *      http://www.mcu-turkey.com/stellaris-launchpad-16x2-lcd/
 */

#include <stdbool.h>
#include <stdint.h>
#include <lcd.h>

#include "driverlib/sysctl.h"

int main(void) {

        //SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
        SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5); //Reloj de Tiva C a 80MHz

        LCD_init();

        while (1) {
            LCD_Print("Hello World!", "...................."); //Print 2 lines

            SysCtlDelay(80000000/3); //Delay
            LCD_Clear();

            LCD_PrintLn(0, "GITHUB:");
            SysCtlDelay(8000000/3); //Delay
            LCD_PrintLn(1, "ahmeterenodaci");

            SysCtlDelay(80000000/3); //Delay
            LCD_Clear();

            LCD_PrintJustify(0, "Hello", "World!");

            SysCtlDelay(80000000/3); //Delay
            LCD_Clear();

            LCD_PrintLn(0, "Hello");
            LCD_PrintLn(1, "     World!");

            SysCtlDelay(80000000/3); //Delay
            LCD_Clear();
        }

}
