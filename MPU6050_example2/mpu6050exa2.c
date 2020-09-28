#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h> ////

#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"

#include "uartstdio.h"
#include "sensorlib/hw_mpu6050.h"
#include "sensorlib/i2cm_drv.h"
#include "sensorlib/mpu6050.h"

#define mpu6050_address 0x68
#define PB2 GPIO_PIN_2
#define PB3 GPIO_PIN_3
#define PA0 GPIO_PIN_0
#define PA1 GPIO_PIN_1

volatile bool process_done;
tI2CMInstance I2Cinst;


void init_console(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    GPIOPinTypeUART(GPIO_PORTA_BASE, PA0 | PA1);

    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(0, 115200, 16000000);
}


void init_I2C (){
    //enable I2C module 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    ////reset module
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
    //enable GPIO peripheral that contains I2C 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Configure the pin muxing for I2C0 functions on port B2 and B3.
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, PB2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, PB3);

    //// Enable and initialize the I2C0 master module.  Use the system clock for
    //// the I2C0 module.
    //// I2C data transfer rate set to 400kbps.
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);

    //// Clear I2C FIFOs
    HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;

    I2CMInit(&I2Cinst, I2C0_BASE, INT_I2C0, 0xff, 0xff, SysCtlClockGet());
    ////IntMasterEnable();
}


void delayMS(int ms) {
    //ROM_SysCtlDelay( (ROM_SysCtlClockGet()/(3*1000))*ms ) ;  // more accurate
    SysCtlDelay( (SysCtlClockGet()/(3*1000))*ms ) ;  // less accurate
}


void mpu6050_callback (void *pvCallbackData, uint_fast8_t status){
    if (status != I2CM_STATUS_SUCCESS){
        UARTprintf("ERROR\n");
    }
    process_done = true;
}


//// The interrupt handler for the I2C module.
////
void I2CMSimpleIntHandler (void){
    ////
    //// Call the I2C master driver interrupt handler.
    ////
    I2CMIntHandler(&I2Cinst);
}


void mpu6050_example (void){
    float accelerometer_data[3], gyroscope_data[3];
    tMPU6050 mpu6050;
    //float ax = 0, ay = 0, az = 0;
    float gx = 0, gy = 0, gz = 0;

    process_done = false;
    MPU6050Init(&mpu6050, &I2Cinst, mpu6050_address, mpu6050_callback, &mpu6050);
    while (!process_done);
    /*while (!MPU6050Init(&mpu6050, &I2Cinst, mpu6050_address, NULL, NULL)){
        UARTprintf("The MPU6050 has not started.\n");
    }*/
    UARTprintf("The MPU6050 has been started\n.");

    //// Configure the MPU6050 for +/- 4 g accelerometer range.
    process_done = false;
    MPU6050ReadModifyWrite(&mpu6050, MPU6050_O_ACCEL_CONFIG,
                           ~MPU6050_ACCEL_CONFIG_AFS_SEL_M,
                           MPU6050_ACCEL_CONFIG_AFS_SEL_4G,
                           mpu6050_callback, &mpu6050
                           );
    while (!process_done);
    ////
    process_done = false;
    MPU6050ReadModifyWrite(&mpu6050, MPU6050_O_PWR_MGMT_1, 0x00, 0b00000010 & MPU6050_PWR_MGMT_1_DEVICE_RESET, mpu6050_callback, &mpu6050);
    while (!process_done)
    {
    }
    ////
    process_done = false;
    MPU6050ReadModifyWrite(&mpu6050, MPU6050_O_PWR_MGMT_2, 0x00, 0x00, mpu6050_callback, &mpu6050);
    while (!process_done)
    {
    }

    while (1){
        process_done = false;
        MPU6050DataRead(&mpu6050, mpu6050_callback, &mpu6050);
        while (!process_done);
        /*while (!MPU6050DataRead(&mpu6050, NULL, NULL)){
            UARTprintf("The reading did not start.\n");
        }
        UARTprintf("Reading started successfully.\n");*/

        MPU6050DataAccelGetFloat(&mpu6050,
                                 &accelerometer_data[0],
                                 &accelerometer_data[1],
                                 &accelerometer_data[2]);
        MPU6050DataGyroGetFloat(&mpu6050,
                                &gyroscope_data[0],
                                &gyroscope_data[1],
                                &gyroscope_data[2]);

        //ax = accelerometer_data[0];
        //ay = accelerometer_data[1];
        //az = accelerometer_data[2];

        //gx = gyroscope_data[0];
        //gy = gyroscope_data[1];
        gz = gyroscope_data[2];

        gx = (atan2(accelerometer_data[0], sqrt (accelerometer_data[1] * accelerometer_data[1] + accelerometer_data[2] * accelerometer_data[2]))*180.0)/3.14;

        gy = (atan2(accelerometer_data[1], sqrt (accelerometer_data[0] * accelerometer_data[0] + accelerometer_data[2] * accelerometer_data[2]))*180.0)/3.14;

        //gz = (atan2(accelerometer_data[2], sqrt (accelerometer_data[0] * accelerometer_data[0] + accelerometer_data[1] * accelerometer_data[1]))*180.0)/3.14;

        //UARTprintf("Accel. X: %d | Accel. Y: %d | Accel. Z: %d\n", (int)ax, (int)ay, (int)az);
        UARTprintf("Ang. X: %d | Ang. Y: %d | Ang. Z: %d\n", (int)gx, (int)gy, (int)gz);

        delayMS(100);
    }
}


/***************************************** MAIN ************************************************/
void main (void){
    //Setting the clock frequency to 80MHz.
    //SysCtlClockSet(SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_USE_PLL | SYSCTL_SYSDIV_2_5);
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_XTAL_16MHZ);

    init_I2C();
    init_console();
    mpu6050_example();
}
