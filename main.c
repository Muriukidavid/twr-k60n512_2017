/*
 * File: main.c
 *
 * Copyright (c) 2015 David Muriuki
 * see the LICENCE file
 */

#include "main.h"

int main(void){
	uint8_t blink=0, data;
	//initialize system
	SystemInit();
	//initialize ports
	gpio_init();
	//initialize a serial port
	init_uart(UART5_BASE_PTR,periph_clk_khz,115200);

	//Loop forever
	while(1)
	{
		if(blink)
			toggle_LEDS();
		if(data_available()){
			data = uart_read();
			uartsend(data);
		}
		delay();
	}
}

/*
	brief  Silly delay
*/
void delay(void)
{
  volatile unsigned int i,j;

  for(i=0; i<1000; i++)
  {
	for(j=0; j<3000; j++)
      __asm__("nop");
  }
}
