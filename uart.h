/*
 * uart.h
 *
 * Copyright (c) 2015 David Muriuki
 * see the LICENCE file
 */

#ifndef UART_H_
#define UART_H_

void init_uart(UART_MemMapPtr uartch, int sysclk, int baud);
void uart_putchar (UART_MemMapPtr channel, uint8_t ch);
void uartsend(uint8_t ch);
void puts(uint8_t *s);
char uart_read(void);
uint16_t data_ready (UART_MemMapPtr channel);
uint8_t uart_getchar (UART_MemMapPtr channel);
uint16_t data_available(void);
#endif /* UART_H_ */
