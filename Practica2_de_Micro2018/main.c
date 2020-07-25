#include <global.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"     //Definitions and configurations for GPIO peripheral.
#include "inc/hw_memmap.h"      //Macros defining the memory map of the device.
#include "driverlib/timer.h"
#include "uartstdio.h"

#define rest      0
#define fibonacci 1
#define storage   2
#define operation 3

int fsm_state = 0;


struct FSM {
    int previous_state;
    int actual_state;
    int next_state;
    int ledState;
    int ledPort;
};

typedef struct FSM FSMstate;

FSMstate states[4] = {
                   {operation, rest, fibonacci, ledState1, GPIO_PORTB_BASE},
                   {rest, fibonacci, storage, ledState2, GPIO_PORTB_BASE},
                   {fibonacci, storage, operation, ledState4, GPIO_PORTE_BASE},
                   {storage, operation, rest, ledState5, GPIO_PORTB_BASE},
};


void status_change(){
    int PORTFpin;
    //tHE PORTF pin that raised the interrupt is determined.
    PORTFpin = GPIOIntStatus(GPIO_PORTF_BASE, true);
    //The flag that was raised by the interruption of the button is lowered.
    GPIOIntClear(GPIO_PORTF_BASE, PORTFpin);

    GPIOPinWrite(states[fsm_state].ledPort, states[fsm_state].ledState, 0);
    if(PORTFpin == GPIO_INT_PIN_0){
        fsm_state = states[fsm_state].next_state;
    }else if(PORTFpin == GPIO_INT_PIN_4){
        fsm_state = states[fsm_state].previous_state;
    }
    GPIOPinWrite(states[fsm_state].ledPort, states[fsm_state].ledState, states[fsm_state].ledState);

    if(fsm_state == rest){
        //The timer starts counting.
        TimerEnable(TIMER0_BASE, TIMER_A);
    }else{
        //The timer stops counting.
        TimerDisable(TIMER0_BASE, TIMER_A);
        GPIOPinWrite(GPIO_PORTF_BASE, ledrgb, 0);
    }
    SysCtlDelay((0.3*SysCtlClockGet())/3);
}


void enter_button(){
    int PORTBpin;
    //The PORTB pin that raised the interrupt is determined.
    PORTBpin = GPIOIntStatus(GPIO_PORTB_BASE, true);
    //The flag that was raised by the interruption of the pin enter is lowered.
    GPIOIntClear(GPIO_PORTB_BASE, PORTBpin);

    if(fsm_state == rest){

    }else if(fsm_state == fibonacci){
        state2_fibonacci();
    }else if(fsm_state == storage){
        state4_storage();
    }else if(fsm_state == operation){
        state5_operation();
    }
    SysCtlDelay((0.3*SysCtlClockGet())/3);
}


void main(void){
    initial_settings();
    enabling_PERIPH();
    GPIO_settings();
    TIMER_settings();
    interrupt_settings();
    UART_settings();

    //The timer starts counting.
    TimerEnable(TIMER0_BASE, TIMER_A);
    //The ledState1 starts.
    GPIOPinWrite(states[fsm_state].ledPort, states[fsm_state].ledState, states[fsm_state].ledState);
    while(true){
        if(fsm_state == rest){
            state1_rest();
        }else{}
    }
}
