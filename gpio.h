/*
 * gpio.h
 *
 * Copyright (c) 2015 David Muriuki
 * see the LICENCE file
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "MK60DZ10.h"
#include "Mk60.h"

extern void toggle_LEDS(void);
extern void toggle_LED1(void);
extern void toggle_LED2(void);
extern void toggle_LED3(void);
extern void toggle_LED4(void);
/*
	brief PORTA initialization
*/
void gpio_init(void)
{
	//PortA pins 10, 11, 28 and 29 as alt1=GPIO
	PORTA_PCR10 = PORT_PCR_MUX(0x01);
	PORTA_PCR11 = PORT_PCR_MUX(0x01);
	PORTA_PCR28 = PORT_PCR_MUX(0x01);
	PORTA_PCR29 = PORT_PCR_MUX(0x01);
	//PTE8 and PTE9 as alt3=UART5
	PORTE_PCR8 = PORT_PCR_MUX(0x3);
	PORTE_PCR9 = PORT_PCR_MUX(0x3);

	//GPIO port data direction Port A as output 
	//for LEDs (pin 11, 28, 29 and 10), 
	//Port E UART5(PTE8 TX)
	//GPIOA->PDDR.bit_reg.bit11 = out;
	GPIOA_PDDR |= GPIO_PDDR_PDD(11);
	//GPIOA->PDDR.bit_reg.bit28 = out;
	GPIOA_PDDR |= GPIO_PDDR_PDD(28);
	//GPIOA->PDDR.bit_reg.bit29 = out;
	GPIOA_PDDR |= GPIO_PDDR_PDD(29);
	//GPIOA->PDDR.bit_reg.bit10 = out;
	GPIOA_PDDR |= GPIO_PDDR_PDD(10); 
	GPIOE_PDDR = GPIO_PDDR_PDD(8);
	//No need to configure GPIO pins (UART5_RX) as an input, 
	//by default all pins are inputs

	//switch off all LEDs initially
	GPIOA_PDOR = (uint32_t)(1<<28|1<<29|1<<10|1<<11);
}

#endif /* GPIO_H_ */
