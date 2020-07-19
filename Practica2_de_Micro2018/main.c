#include <global.h>
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/gpio.h"     //Definitions and configurations for GPIO peripheral.
#include "inc/hw_memmap.h"      //Macros defining the memory map of the device.
#include "driverlib/timer.h"

#define rest      0
#define fibonacci 1
#define storage   2

int fsm_state = 0;


struct FSM {
    int previous_state;
    int actual_state;
    int next_state;
};

typedef struct FSM FSMstate;

FSMstate states[3] = {
                   {storage, rest, fibonacci},
                   {rest, fibonacci, storage},
                   {fibonacci, storage, rest},
};


void status_change(){
    int PORTFpin;
    //tHE PORTF pin that raised the interrupt is determined.
    PORTFpin = GPIOIntStatus(GPIO_PORTF_BASE, true);
    //The flag that was raised by the interruption of the button is lowered.
    GPIOIntClear(GPIO_PORTF_BASE, PORTFpin);

    if(PORTFpin == GPIO_INT_PIN_0){
        fsm_state = states[fsm_state].next_state;
    }else if(PORTFpin == GPIO_INT_PIN_4){
        fsm_state = states[fsm_state].previous_state;
    }

    if(fsm_state == rest){
        //The timer starts counting.
        TimerEnable(TIMER0_BASE, TIMER_A);
    }else{
        //The timer stops counting.
        TimerDisable(TIMER0_BASE, TIMER_A);
        GPIOPinWrite(GPIO_PORTF_BASE, ledrgb, 0);
    }
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
    }
}


void main(void){
    initial_settings();
    enabling_PERIPH();
    GPIO_settings();
    TIMER_settings();
    interrupt_settings();

    //The timer starts counting.
    TimerEnable(TIMER0_BASE, TIMER_A);
    while(true){
        if(fsm_state == rest){
            state1_rest();
        }else{}
    }
}
