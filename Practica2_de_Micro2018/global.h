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
#define option_bits_7a8  GPIO_PIN_4|GPIO_PIN_5
#define option_bits_9a10 GPIO_PIN_6|GPIO_PIN_7
#define option_bit_11    GPIO_PIN_3
#define ledrgb           GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
#define ledState1        GPIO_PIN_0  //PB0
#define ledState2        GPIO_PIN_1  //PB1
#define ledState3        GPIO_PIN_4  //PE4
#define ledState4        GPIO_PIN_5  //PE5
#define ledState5        GPIO_PIN_4  //PB4

void initial_settings(void);
void enabling_PERIPH(void);
void GPIO_settings(void);
void TIMER_settings(void);
void interrupt_settings(void);
void UART_settings(void);

void state1_rest(void);
void state2_fibonacci(void);
void state4_storage(void);

#endif /* GLOBAL_H_ */
