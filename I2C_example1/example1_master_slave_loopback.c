//*****************************************************************************

// master_slave_loopback.c - Example demonstrating a simple I2C message transmission and reception.

//*****************************************************************************

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

//*****************************************************************************

//! This example shows how to configure the I2C0 module for loopback mode.
//! This includes setting up the master and slave module.  Loopback mode
//! internally connects the master and slave data and clock lines together.
//! The address of the slave module is set in order to read data from the
//! master.  Then the data is checked to make sure the received data matches
//! the data that was transmitted.  This example uses a polling method for
//! sending and receiving data.
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - I2C0 peripheral
//! - GPIO Port B peripheral (for I2C0 pins)
//! - I2C0SCL - PB2
//! - I2C0SDA - PB3
//!
//! The following UART signals are configured only for displaying console
//! messages for this example.  These are not required for operation of I2C.
//! - UART0 peripheral
//! - GPIO Port A peripheral (for UART0 pins)
//! - UART0RX - PA0
//! - UART0TX - PA1
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - None.

//*****************************************************************************

// Number of I2C data packets to send.
#define NUM_I2C_DATA 3
// Set the address for slave module. This is a 7-bit address sent in the
// following format:
//                      [A6:A5:A4:A3:A2:A1:A0:RS]
//
// A zero in the "RS" position of the first byte means that the master
// transmits (sends) data to the selected slave, and a one in this position
// means that the master receives data from the slave.
#define SLAVE_ADDRESS 0x3C
//Value for A0 and A1 pins.
#define PA0 GPIO_PIN_0
#define PA1 GPIO_PIN_1
//Value for B2 and B3 pins.
#define PB2 GPIO_PIN_2
#define PB3 GPIO_PIN_3

//*****************************************************************************

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

//*****************************************************************************

// Configure the I2C0 master and slave and connect them using loopback mode.
int main(void){
    uint32_t pui32DataTx[NUM_I2C_DATA];
    uint32_t pui32DataRx[NUM_I2C_DATA];
    uint32_t ui32Index;

    // Set the clocking to run directly from the external crystal/oscillator.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

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

    // Enable loopback mode.  Loopback mode is a built in feature that is
    // useful for debugging I2C operations.  It internally connects the I2C
    // master and slave terminals, which effectively let's you send data as
    // a master and receive data as a slave.
    I2CLoopbackEnable(I2C0_BASE);

    // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.  For this example we will use a data rate of 100kbps.
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

    // Enable the I2C0 slave module. This module is enabled only for testing
    // purposes.  It does not need to be enabled for proper operation of the
    // I2Cx master module.
    I2CSlaveEnable(I2C0_BASE);

    // Set the slave address to SLAVE_ADDRESS.  In loopback mode, it's an
    // arbitrary 7-bit number.
    I2CSlaveInit(I2C0_BASE, SLAVE_ADDRESS);

    // Tell the master module what address it will place on the bus when
    // communicating with the slave.  Set the address to SLAVE_ADDRESS
    // (as set in the slave module).  The receive parameter is set to false
    // which indicates the I2C Master is initiating a writes to the slave.  If
    // true, that would indicate that the I2C Master is initiating reads from
    // the slave.
    I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, false);

    // Set up the serial console to use for displaying messages.
    init_console();

    // Display the example setup on the console.
    UARTprintf("I2C Loopback Example ->");
    UARTprintf("\n   Module = I2C0");
    UARTprintf("\n   Mode = Single Send/Receive");
    UARTprintf("\n   Rate = 100kbps\n\n");
    // Initialize the data to send.
    pui32DataTx[0] = 'I';
    pui32DataTx[1] = '2';
    pui32DataTx[2] = 'C';
    // Initialize the receive buffer.
    for(ui32Index = 0; ui32Index < NUM_I2C_DATA; ui32Index++){
        pui32DataRx[ui32Index] = 0;
    }
    //*****************************************************************************
    // Indicate the direction of the data.
    UARTprintf("Transferring from: Master -> Slave\n");
    // Send 3 pieces of I2C data from the master to the slave.
    for(ui32Index = 0; ui32Index < NUM_I2C_DATA; ui32Index++){
        // Display the data that the I2C0 master is transferring.
        UARTprintf("  Sending: '%c'  . . .  ", pui32DataTx[ui32Index]);
        // Place the data to be sent in the data register.
        I2CMasterDataPut(I2C0_BASE, pui32DataTx[ui32Index]);
        // Initiate send of data from the master.  Since the loopback
        // mode is enabled, the master and slave units are connected
        // allowing us to receive the same data that we sent out.
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
        // Wait until the slave has received and acknowledged the data.
        while(!(I2CSlaveStatus(I2C0_BASE) & I2C_SLAVE_ACT_RREQ)){
        }
        // Read the data from the slave.
        pui32DataRx[ui32Index] = I2CSlaveDataGet(I2C0_BASE);
        // Wait until master module is done transferring.
        while(I2CMasterBusy(I2C0_BASE)){
        }
        // Display the data that the slave has received.
        UARTprintf("Received: '%c'\n", pui32DataRx[ui32Index]);
    }

    // Reset receive buffer.
    for(ui32Index = 0; ui32Index < NUM_I2C_DATA; ui32Index++){
        pui32DataRx[ui32Index] = 0;
    }
    //*****************************************************************************
    // Indicate the direction of the data.
    UARTprintf("\n\nTranferring from: Slave -> Master\n");
    // Modify the data direction to true, so that seeing the address will
    // indicate that the I2C Master is initiating a read from the slave.
    I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, true);
    // Do a dummy receive to make sure you don't get junk on the first receive.
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
    // Dummy acknowledge and wait for the receive request from the master.
    // This is done to clear any flags that should not be set.
    while(!(I2CSlaveStatus(I2C0_BASE) & I2C_SLAVE_ACT_TREQ)){
    }

    for(ui32Index = 0; ui32Index < NUM_I2C_DATA; ui32Index++){
        // Display the data that I2C0 slave module is transferring.
        UARTprintf("  Sending: '%c'  . . .  ", pui32DataTx[ui32Index]);
        // Place the data to be sent in the data register.
        I2CSlaveDataPut(I2C0_BASE, pui32DataTx[ui32Index]);
        // Tell the master to read data.
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        // Wait until the slave is done sending data.
        while(!(I2CSlaveStatus(I2C0_BASE) & I2C_SLAVE_ACT_TREQ)){
        }
        // Read the data from the master.
        pui32DataRx[ui32Index] = I2CMasterDataGet(I2C0_BASE);
        // Display the data that the slave has received.
        UARTprintf("Received: '%c'\n", pui32DataRx[ui32Index]);
    }

    // Tell the user that the test is done.
    UARTprintf("\nDone.\n\n");
    // Return no errors
    return(0);
}
