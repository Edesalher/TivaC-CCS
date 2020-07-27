#include <global.h>
#include <stdint.h>     //To use integer type values.
#include <stdbool.h>
#include "driverlib/gpio.h"     //Definitions and configurations for GPIO peripheral.
#include "inc/hw_memmap.h"      //Macros defining the memory map of the device.
#include "driverlib/timer.h"
#include "uartstdio.h"

#define MatrixA 0
#define MatrixB 8

#define Matrix1A 0
#define Matrix1B 8
#define Matrix2A 0
#define Matrix2B 2
#define addition       0
#define substraction   1
#define multiplication 2

int colorValue = 0;
int matrix_A[4][4], matrix_B[4][4], result[4][4];

typedef int array_int[4];


void led_rgb(){
    //The flag that was raised by the interruption of the timer is lowered.
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //To change the color of the rgb led, the value of bits 1, 2 and 3 must grow with an increase of 2.
    if(colorValue == 14){
        colorValue = 0;
    }else{
        colorValue += 2;
    }
}


//To determine if a number is even or not.
bool even_number(int number){
    if(number % 2 == 0){
        return true;
    }else{
        return false;
    }
}


//It allows adding two 4x4 matrices.
void add(array_int *matrix1, array_int *matrix2){
    int m, n;

    for(m = 0; m <= 3; m++){
        for(n = 0; n <= 3; n++){
            result[m][n] = matrix1[m][n] + matrix2[m][n];
        }
    }
}


//It allows to subtract two 4x4 matrices.
void sub(array_int *matrix1, array_int *matrix2){
    int m, n;

    for(m = 0; m <= 3; m++){
        for(n = 0; n <= 3; n++){
            result[m][n] = matrix1[m][n] - matrix2[m][n];
        }
    }
}


//It allows multiplying two 4x4 matrices.
void multi(array_int *matrix1, array_int *matrix2){
    int m1, m2, n2, sum;

    for(m1 = 0; m1 <= 3; m1++){
        for(n2 = 0; n2 <= 3; n2++){
            sum = 0;
            for(m2 = 0; m2 <= 3; m2++){
                sum += matrix1[m1][m2]*matrix2[m2][n2];
            }
            result[m1][n2] = sum;
        }
    }
}

//Reset a matrix.
void clean_matrix(int matrix[4][4]){
    int i, j;

    for(i = 0; i <= 3; i++){
        for(j = 0; j <= 3; j++){
            matrix[i][j] = 0;
        }
    }
}


void print_matrix(int matrix[4][4]){
    int i, j;

    for(i = 0; i <= 3; i++){
        UARTprintf("   ");
        for(j = 0; j <= 3; j++){
            UARTprintf("%d  ", matrix[i][j]);
        }
        UARTprintf("\n\r");
    }
}

/*********************************** STATES ************************************/

void state1_rest(){
    GPIOPinWrite(GPIO_PORTF_BASE, ledrgb, colorValue);
}


void state2_fibonacci(){
    int bits_0a3, bits_4a6, i, f0 = 0, f1 = 1, fn, fn_one, fn_two, n, sum;

    //The data bits (0 a 6) are read to obtain the entered number.
    bits_0a3 = GPIOPinRead(GPIO_PORTD_BASE, data_bits_0a3);
    bits_4a6 = GPIOPinRead(GPIO_PORTA_BASE, data_bits_4a6);
    //The value of n where we want the fibonacci sequence to end.
    n = bits_0a3 + bits_4a6;
    fn_one = f1;
    fn_two = f0;

    sum = 0;
    /*
     * The fibonacci sequence is generated up to the value of "n". Every time
     * a fibonacci number is generated, it's checked to determine if it's even or not.
     * Even numbers are added.
     */
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
    //Print the data.
    UARTprintf("n = %d \n\r", n);
    UARTprintf("Sum: %d \n\r", sum);
    UARTprintf("********************\n\r");
}


void state4_storage(){
    int bits_0a3, bits_4a6, bits_7a8, bits_9a10, matrix, value, row, column;

    //The data bits (0 a 6) are read to obtain the value of the entered number.
    bits_0a3 = GPIOPinRead(GPIO_PORTD_BASE, data_bits_0a3);
    bits_4a6 = GPIOPinRead(GPIO_PORTA_BASE, data_bits_4a6);
    value = bits_0a3 + bits_4a6;

    //The value of bits 7, 8, 9 and 10 are read to know the row and column number.
    bits_9a10 = GPIOPinRead(GPIO_PORTC_BASE, option_bits_9a10);
    bits_7a8 = GPIOPinRead(GPIO_PORTC_BASE, option_bits_7a8);
    row = bits_9a10/64;
    column = bits_7a8/16;

    //The value of bit 11 is read to know which matrix was selected. A or B.
    matrix = GPIOPinRead(GPIO_PORTB_BASE, option_bit_11);

    if(matrix == MatrixA){
        //The value that was entered is stored in the corresponding position in the matrix.
        matrix_A[row][column] = value;
        UARTprintf(" --MATRIX A--\n\r");
        print_matrix(matrix_A);
        UARTprintf("********************\n\r");
    }else if(matrix == MatrixB){
        //The value that was entered is stored in the corresponding position in the matrix.
        matrix_B[row][column] = value;
        UARTprintf(" --MATRIX B--\n\r");
        print_matrix(matrix_B);
        UARTprintf("********************\n\r");
    }
}


void state5_operation(){
    int bits_7a8, bits_9a10, operation, bit_11, bit_8;
    char *operation_text;
    array_int *matrix1, *matrix2;

    clean_matrix(result);

    //The value of bit 11 is read to know which matrix1 was selected.
    bit_11 = GPIOPinRead(GPIO_PORTB_BASE, option_bit_11);
    if(bit_11 == Matrix1A){
        matrix1 = matrix_A;
    }else if(bit_11 == Matrix1B){
        matrix1 = matrix_B;
   }
    //The value of bit 8 is read to know which matrix2 was selected.
    bits_7a8 = GPIOPinRead(GPIO_PORTC_BASE, option_bits_7a8);
    bit_8 = bits_7a8/16;
    if(bit_8 == Matrix2A){
        matrix2 = matrix_A;
    }else if(bit_8 == Matrix2B){
        matrix2 = matrix_B;
    }
    //The value of bits 9 and 10 are read to know which operation the user wants to do.
    bits_9a10 = GPIOPinRead(GPIO_PORTC_BASE, option_bits_9a10);
    operation = bits_9a10/64;

    if(operation == addition){
        operation_text = "ADDITION";
        add(matrix1, matrix2);
    }else if(operation == substraction){
        operation_text = "SUBTRACTION";
        sub(matrix1, matrix2);
    }else if(operation == multiplication){
        operation_text = "MULTIPLICATION";
        multi(matrix1, matrix2);
    }
    //Print the resulting matrix.
    UARTprintf(" --RESULT OF THE %s--\n\r", operation_text);
    print_matrix(result);
    UARTprintf("*********************************\n\r");
}
