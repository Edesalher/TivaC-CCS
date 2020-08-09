#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "uartstdio.h"
#include "sensorlib/hw_mpu6050.h"
#include "sensorlib/i2cm_drv.h"
#include "sensorlib/mpu6050.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include <stdarg.h>

#define SLAVE_ADDRESS 0x68
//Value for A0 and A1 pins.
#define PA0 GPIO_PIN_0
#define PA1 GPIO_PIN_1
//Value for B2 and B3 pins.
#define PB2 GPIO_PIN_2
#define PB3 GPIO_PIN_3

tI2CMInstance sI2CInst;


// This function sets up UART0 to be used for a console to display information
// as the example is running.
void init_console(void){
    // Enable GPIO port A which is used for UART0 pins.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // Configure the function of UART0 for A0 and A1 pins.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    // Enable UART0 so that we can configure the clock.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    // Select the alternate (UART) function for these pins.
    GPIOPinTypeUART(GPIO_PORTA_BASE, PA0 | PA1);
    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, 16000000);
}


char buf[100];
char* ftoa(float f)
{
int pos=0,ix,dp,num;
if (f<0)
{
buf[pos++]='-';
f = -f;
}
dp=0;
while (f>=10.0)
{
f=f/10.0;
dp++;
}
for (ix=1;ix<8;ix++)
{
num = (int)f;
f=f-num;
if (num>9)
buf[pos++]='#';
else
buf[pos++]='0'+num;
if (dp==0) buf[pos++]='.';
f=f*10.0;
dp--;
}
return buf;
}



// The interrupt handler for the I2C module.
void I2CMSimpleIntHandler (void) {
    // Call the I2C master drive interrupt handler.
    I2CMIntHandler(&sI2CInst);
}


// A boolean that is set when a MPU6050 command has completed.
volatile bool g_bMPU6050Done;

// The function that is provided by this example as a callback when MPU6050
// transaction have completed.
void MPU6050Callback(void *pvCallbackData, uint_fast8_t ui8Status) {
    // See if an error ocurred.
    if (ui8Status != I2CM_STATUS_SUCCESS) {
        // An error ocurred, so handle it here if required.
        UARTprintf("Error\n");
    }
    // Indicate that the MPU6050 transaction has completed.
    g_bMPU6050Done = true;
}


void init_I2C() {
    // The I2C0 peripheral must be enabled before use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    // For this example I2C0 is used with PortB.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    // Configure the function of I2C0 for B2 and B3 pins.
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, PB2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, PB3);

    I2CIntRegister(I2C0_BASE, &I2CMSimpleIntHandler);
    IntMasterEnable();

    // Enable and initialize the I2C0 master module.
    // For this example we will use a data rate of 400kbps.
    I2CMInit(&sI2CInst, I2C0_BASE, INT_I2C0, 0xff, 0xff, SysCtlClockGet());
//    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);
}

// THe MPU6050 example.

void MPU6050Example() {
    float fAccel[3], fGyro[3];
    tMPU6050 sMPU6050;

    // Initialize the MPU6050. This code assumes that the I2C master instance has
    // already been initialized.
    UARTprintf("MPU6050 initialization\n");
    // Write two bytes of data to the I2C device at address 0x3C.
    g_bMPU6050Done = false;
    MPU6050Init(&sMPU6050, &sI2CInst, 0x68, MPU6050Callback, 0);
    while (!g_bMPU6050Done);

    // Configure the MPU6050 for +/- 4 g accelerometer range.
    UARTprintf("Configure the MPU6050 for +/- 4 g accelerometer range\n");
    // Read four bytes of data from the I2C device at address 0x3C.
    g_bMPU6050Done = false;
    MPU6050ReadModifyWrite(&sMPU6050, MPU6050_O_ACCEL_CONFIG,
                           ~MPU6050_ACCEL_CONFIG_AFS_SEL_M,
                           MPU6050_ACCEL_CONFIG_AFS_SEL_4G, MPU6050Callback,
                           0);
    while (!g_bMPU6050Done);

    // Loop forever reading data from the MPU6050. Typically, this process
    // would be done in the background, but for the purpose of this example,
    // it is shown in an infinite loop.
    while (true) {
        //Request another reading from the MPU6050.
        g_bMPU6050Done = false;
        MPU6050DataRead(&sMPU6050, MPU6050Callback, 0);
        while (!g_bMPU6050Done);

        // Get the new accelerometer and gyroscope readings.
        MPU6050DataAccelGetFloat(&sMPU6050, &fAccel[0], &fAccel[1], &fAccel[2]);
        MPU6050DataGyroGetFloat(&sMPU6050, &fGyro[0], &fGyro[1], &fGyro[2]);

        // Display the data that the slave has received.
        UARTprintf("a[x, y, z] = %d %d %d || g[x, y, z] = %d %d %d\n", ftoa(fAccel[0]), ftoa(fAccel[1]), ftoa(fAccel[2]), ftoa(fGyro[0]), ftoa(fGyro[1]), ftoa(fGyro[2]));
        SysCtlDelay((0.5*SysCtlClockGet())/3);
    }
}

void main(void) {
    //Setting the clock frequency to 80MHz.
    SysCtlClockSet(SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_USE_PLL | SYSCTL_SYSDIV_2_5);
    // Set up the serial console to use for displaying messages.
    init_console();
    init_I2C();
    MPU6050Example();
}
