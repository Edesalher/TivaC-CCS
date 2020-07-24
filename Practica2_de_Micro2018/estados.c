#include <global.h>
#include <stdint.h>     //To use integer type values.
#include <stdbool.h>
#include "driverlib/gpio.h"     //Definitions and configurations for GPIO peripheral.
#include "inc/hw_memmap.h"      //Macros defining the memory map of the device.
#include "driverlib/timer.h"
#include "uartstdio.h"

#define MatrixA 0
#define MatrixB 8

int colorValue = 0, n, sum, row, column;
int matrix_A[4][4];
int matrix_B[4][4];


void led_rgb(){
    //The flag that was raised by the interruption of the timer is lowered.
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    if(colorValue == 14){
        colorValue = 0;
    }else{
        colorValue += 2;
    }
}


bool even_number(int number){
    if(number % 2 == 0){
        return true;
    }else{
        return false;
    }
}


void print_matrix(int matrix[4][4]){
    int i, j;

    for(i = 0; i <= 3; i++){
        for(j = 0; j <= 3; j++){
            UARTprintf("%d ", matrix[i][j]);
        }
        UARTprintf("\n\r");
    }
    UARTprintf("\n\r");
}

/*********************************** STATES ************************************/

void state1_rest(){
    GPIOPinWrite(GPIO_PORTF_BASE, ledrgb, colorValue);
}


void state2_fibonacci(){
    int bits_0a3, bits_4a6, i, f0 = 0, f1 = 1, fn, fn_one, fn_two;

    //The data bits (0 a 6) are read to obtain the entered number.
    bits_0a3 = GPIOPinRead(GPIO_PORTD_BASE, data_bits_0a3);
    bits_4a6 = GPIOPinRead(GPIO_PORTA_BASE, data_bits_4a6);
    n = bits_0a3 + bits_4a6;
    fn_one = f1;
    fn_two = f0;

    sum = 0;
    for(i = 2; i <= n; i++){
        fn =  fn_one + fn_two;
        if(even_number(fn)){
            sum += fn;
        }else{
            sum = sum;
        }
        fn_two = fn_one;
        fn_one = fn;
    }
    UARTprintf("n = %d \n\r", n);
    UARTprintf("Sum: %d \n\r", sum);
    UARTprintf("\n\r");
}


void state4_storage(){
    int bits_0a3, bits_4a6, bits_7a8, bits_9a10, bit_11, value;

    //The data bits (0 a 6) are read to obtain the entered number.
    bits_0a3 = GPIOPinRead(GPIO_PORTD_BASE, data_bits_0a3);
    bits_4a6 = GPIOPinRead(GPIO_PORTA_BASE, data_bits_4a6);
    value = bits_0a3 + bits_4a6;

    //The value of bits 7, 8, 9 and 10 are read to know the row and column number.
    bits_9a10 = GPIOPinRead(GPIO_PORTC_BASE, option_bits_9a10);
    row = bits_9a10/64;
    bits_7a8 = GPIOPinRead(GPIO_PORTC_BASE, option_bits_7a8);
    column = bits_7a8/16;

    //The value of bit 11 is read to know which matrix was selected.
    bit_11 = GPIOPinRead(GPIO_PORTB_BASE, option_bit_11);
    if(bit_11 == MatrixA){
        matrix_A[row][column] = value;
        UARTprintf("Matrix A\n\r");
        print_matrix(matrix_A);
    }else if(bit_11 == MatrixB){
        matrix_B[row][column] = value;
        UARTprintf("Matrix B\n\r");
        print_matrix(matrix_B);
    }
}
