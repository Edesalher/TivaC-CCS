#include <global.h>
#include <stdint.h>
#include <stdbool.h>


void main(void){
    initial_settings();
    enabling_PERIPH();
    GPIO_settings();
    TIMER_settings();
    interrupt_settings();

    state2_fibonacci();

    while(true){
        //state1_rest();
    }
}
