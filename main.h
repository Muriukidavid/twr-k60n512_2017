/*
 * main.h
 *
 * Copyright (c) 2015 David Muriuki
 * see the LICENCE file
 */

#ifndef MAIN_H_
#define MAIN_H_
#include"MK60DZ10.h"
#include"gpio.h"
#include"uart.h"
extern int periph_clk_khz;
void delay(void);
extern void SystemInit(void);
#endif /* MAIN_H_ */
