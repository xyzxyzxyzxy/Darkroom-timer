/*
 * Keypad.h
 *
 *  Created on: Sep 6, 2021
 *      Author: ga
 */

#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_

#define C1_PORT GPIOD
#define C1_PIN GPIO_PIN_11
#define C2_PORT GPIOD
#define C2_PIN GPIO_PIN_9
#define C3_PORT GPIOB
#define C3_PIN GPIO_PIN_15
#define C4_PORT GPIOB
#define C4_PIN GPIO_PIN_13
#define R1_PORT GPIOB
#define R1_PIN GPIO_PIN_11
#define R2_PORT GPIOE
#define R2_PIN GPIO_PIN_15
#define R3_PORT GPIOE
#define R3_PIN GPIO_PIN_13
#define R4_PORT GPIOE
#define R4_PIN GPIO_PIN_11

GPIO_TypeDef* Cx_PORT[4] = {C1_PORT, C2_PORT, C3_PORT, C4_PORT};
uint16_t Cx_PIN[4] = {C1_PIN, C2_PIN, C3_PIN, C4_PIN};
GPIO_TypeDef* Rx_PORT[4] = {R1_PORT, R2_PORT, R3_PORT, R4_PORT};
uint16_t Rx_PIN[4] = {R1_PIN, R2_PIN, R3_PIN, R4_PIN};

char keypad[5][5] = {{'\0', '\0', '\0', '\0', '\0'},
		{'\0', '1', '2', '3', 'A'},
		{'\0', '4', '5', '6', 'B'},
		{'\0', '7', '8', '9', 'C'},
		{'\0', '*', '0', '#', 'D'}};


#endif /* INC_KEYPAD_H_ */
