/*
 * uart.c
 *
 * Copyright (c) 2015 David Muriuki
 * see the LICENCE file
 */
//#include"uart.h"
#include "MK60DZ10.h"
extern int periph_clk_khz;
void init_uart(UART_MemMapPtr uartch, int sysclk, int baud){
	uint16_t ubd, temp, brfa;
	/* Make sure that the transmitter and receiver are disabled while we
	* change settings.
	*/
	UART_C2_REG(uartch) &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );
	/* Configure the UART for 8-bit mode, no parity */
	/* We need all default settings, so entire register is cleared */
	UART_C1_REG(uartch) = 0;
	/* Calculate baud settings */
	ubd = (uint16_t)((sysclk*1000)/(baud * 16));
	/* Save off the current value of the UARTx_BDH except for the SBR */
	temp = UART_BDH_REG(uartch) & ~(UART_BDH_SBR(0x1F));
	UART_BDH_REG(uartch) = temp | UART_BDH_SBR(((ubd & 0x1F00) >> 8));
	UART_BDL_REG(uartch) = (uint8_t)(ubd & UART_BDL_SBR_MASK);
	/* Determine if a fractional divider is needed to get closer to the baud rate */
	brfa = (((sysclk*32000)/(baud * 16)) - (ubd * 32));
	/* Save off the current value of the UARTx_C4 register except for the BRFA */
	temp = UART_C4_REG(uartch) & ~(UART_C4_BRFA(0x1F));
	UART_C4_REG(uartch) = temp | UART_C4_BRFA(brfa);
	/* Enable receiver and transmitter */
	UART_C2_REG(uartch) |= (UART_C2_TE_MASK | UART_C2_RE_MASK );
	//UART_S1_REG(uartch) &= ~UART_S1_TDRE_MASK;
}

void uart_putchar (UART_MemMapPtr channel, uint8_t ch)
{
	/* Wait until space is available in the FIFO */
	while(!(UART_S1_REG(channel) & UART_S1_TDRE_MASK));

	/* Send the character */
	UART_D_REG(channel) = (uint8_t)ch;
}

uint8_t uart_getchar (UART_MemMapPtr channel)
{
	/* Wait until character has been received */
	while (!(UART_S1_REG(channel) & UART_S1_RDRF_MASK));

	/* Return the 8-bit data from the receiver */
	return (uint8_t)UART_D_REG(channel);
}

uint16_t data_ready (UART_MemMapPtr channel)
{
	return (((uint16_t)(UART_S1_REG(channel) & UART_S1_RDRF_MASK)) == UART_S1_RDRF_MASK);
}


char uart_read(void){
	return uart_getchar (UART5_BASE_PTR);
}

//send a char
void uartsend(uint8_t ch){
	uart_putchar (UART5_BASE_PTR,ch);
}

//send a string
void puts(uint8_t *s){
	while(*s!='\0'){
		uartsend(*s++);
	}
}

//abstract data_ready function for main, doesnt need to know what chunnel is being used
uint16_t data_available(void)
{
	return data_ready(UART5_BASE_PTR);
}
