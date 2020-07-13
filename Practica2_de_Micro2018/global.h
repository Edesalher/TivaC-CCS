/*
 * global.h
 *
 *  Created on: Jul 11, 2020
 *      Author: edesalher
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#define data_bits_0a3    GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
#define data_bits_4a6    GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
#define ledrgb           GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3

void initial_settings(void);
void enabling_PERIPH(void);
void GPIO_settings(void);
void TIMER_settings(void);
void interrupt_settings(void);

void state1_rest(void);
void state2_fibonacci(void);

#endif /* GLOBAL_H_ */