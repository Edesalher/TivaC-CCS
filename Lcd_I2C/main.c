#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

// Number of I2C data packets to send.
#define NUM_I2C_DATA 3

// Address of the LCD 2x16
#define LCD_ADDRESS 0x27
//Value for B2 and B3 pins.
#define PB2 GPIO_PIN_2  // PB2 = SCL
#define PB3 GPIO_PIN_3  // PB3 = SDA


void i2c_setup(){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    // For this example I2C0 is used with PortB.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    // Configure the function of I2C0 for B2 and B3 pins.
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, PB2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, PB3);

    // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module. I2C data transfer rate is set to 100kbps.
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

    // Tell the master module what address it will place on the bus when
    // communicating with the slave. The receive parameter is set to false
    // which indicates the I2C Master is initiating a writes to the slave.
    I2CMasterSlaveAddrSet(I2C0_BASE, LCD_ADDRESS, false);
}


/**
 * main.c
 */
int main(void){
    uint32_t dataTx[NUM_I2C_DATA];
    //uint32_t data;
    uint32_t index;

    // Set the clocking to run directly from the external crystal/oscillator.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    dataTx[0] = 'I';
    dataTx[1] = 0;
    dataTx[2] = 1;

    for(index = 0; index < NUM_I2C_DATA; index++){
        // Place the data to be sent in the data register (FIFO).
        I2CMasterDataPut(I2C0_BASE, dataTx[index]);
        // Initiate send of data from the master.
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
        // Wait until master module is done transferring.
        while(I2CMasterBusy(I2C0_BASE)){
        }
    }

    while(true);
}
