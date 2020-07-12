/*
 * global.h
 *
 *  Created on: Jul 11, 2020
 *      Author: edesalher
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#define ledrgb GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3

void initial_settings(void);
void enabling_PERIPH(void);
void GPIO_settings(void);
void TIMER_settings(void);
void interrupt_settings(void);

void state1_rest(void);

#endif /* GLOBAL_H_ */
