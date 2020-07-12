#include <global.h>
#include <stdint.h>
#include <stdbool.h>


int main(void){
    initial_settings();
    enabling_PERIPH();
    GPIO_settings();
    TIMER_settings();
    interrupt_settings();

    while(true){
        state1_rest();
    }
}
