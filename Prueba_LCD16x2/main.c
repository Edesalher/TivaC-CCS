//********************************Librerias*****************************************//
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include <stdlib.h>
#include <string.h>
#include "HASLCD_JR.h"

//long contador = 10;
//char * c = "TM4C1234GH6PM";

//int a=54;
int conteo = 0;
char cuenta[] = "";
//int columna;
//int cantidad_elementos;
//int tamaño_del_array;
//int tamaño_del_tipo_de_dato;
//char buffer[10];

int main()
{
     SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ| SYSCTL_OSC_MAIN);

     has_lcd_4bitsetup();
     has_lcd_erase();

     while(1)
     {
         conteo++;
         if(conteo > 20){
             conteo = 0;
             has_lcd_erase();
         }
         //has_lcd_4bitsetup();
         //has_lcd_write(1,1,"LCD EN CCS");
         has_lcd_write(1,1,"NIVEL:");
         //SysCtlDelay(10000000);
         ltoa(conteo,cuenta);
         //has_lcd_write(1,8,buffer);
         has_lcd_write(1,8,cuenta);
//         tamaño_del_array = sizeof(cuenta);
//         tamaño_del_tipo_de_dato = sizeof(char);
//         cantidad_elementos = (tamaño_del_array / tamaño_del_tipo_de_dato);
//         if(cantidad_elementos >= 2){
//             columna = 11;
//         }else{
//             columna = 10;
//         }
//         has_lcd_write(1,columna,"mm");
         has_lcd_write(1,11,"mm");
         SysCtlDelay((SysCtlClockGet()*0.5)/3);
         //has_lcd_erase();
         //ltoa(a,buffer);   // here 2 means binary
     }
}
