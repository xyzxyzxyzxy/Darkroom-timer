/*
 * 7seg.h
 *
 *  Created on: Jun 30, 2021
 *      Author: ga
 */

#ifndef SRC_7SEG_H_
#define SRC_7SEG_H_

#include "stm32f4xx_hal.h"

#define D_PORT GPIOB
#define D1_PIN GPIO_PIN_7
#define D2_PIN GPIO_PIN_6
#define D3_PIN GPIO_PIN_5
#define D4_PIN GPIO_PIN_4

#define DIODE_PORT GPIOD
#define DIODE_A_PIN GPIO_PIN_7
#define DIODE_F_PIN GPIO_PIN_6
#define DIODE_B_PIN GPIO_PIN_5
#define DIODE_G_PIN GPIO_PIN_4
#define DIODE_C_PIN GPIO_PIN_3
#define DIODE_DP_PIN GPIO_PIN_2
#define DIODE_D_PIN GPIO_PIN_1
#define DIODE_E_PIN GPIO_PIN_0

void write_D1(void);

void write_D2(void);

void write_D3(void);

void write_D4(void);

void print_0(); // writing 0

void print_1(); // writing 1

void print_2(); // writing 2

void print_3(); // writing 3

void print_4(); // writing 4

void print_5(); // writing 5

void print_6(); // writing 6

void print_7(); // writing 7

void print_8(); // writing 8

void print_9(); // writing 9

void print_A(); // writing A

void print_b(); // writing b

void print_C(); // writing C

void print_d(); // writing d

void print_A();

void print_b();

void print_C();

void print_d();

void print_decimal(); // writing the decimal point

void print_OFF(); // turning all 7 segments plus the decimal point off

#endif /* SRC_7SEG_H_ */
