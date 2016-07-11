/*
 * File: system_k60.c
 *
 * Copyright (c) 2013, 0xc0170, edited by karibe
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 *
 */
#include "MK60DZ10.h"

int periph_clk_khz;

//Initialize system clocks core_clk_mhz = 96;
void SystemInit(void)
{
  uint32_t temp_pfapr;
  char i;

  //clock the ports
  SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK);

  //uart5 enabled
  SIM_SCGC1 |= SIM_SCGC1_UART5_MASK;

  //uart0  ,Comparator and Carrier Modulator Transmitter enable
  SIM_SCGC4 |= SIM_SCGC4_UART0_MASK | SIM_SCGC4_CMP_MASK | SIM_SCGC4_CMT_MASK;

  //uart0 RX source is CMP0
  SIM_SOPT5 |= SIM_SOPT5_UART0RXSRC(0x01);
  //IR diode drive strength 2pads
  SIM_SOPT2 |= SIM_SOPT2_CMTUARTPAD_MASK;

  //defaults
  MCG_C2 = 0;

  //Enable LLU module
  SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;

  // Release digital pins to normal mode
  LLWU_CS |= LLWU_CS_ACKISO_MASK;

  //Select external reference clock, 50Mhz/256 = 6.25Mhz clock for FLL
  MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);

  //wait for the current source for the FLL reference clock to be external
  while (MCG_S & MCG_S_IREFST_MASK);

  //wait for clock status to rise
  while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2);

  //11000b divide external clock by 25: 50Mhz/25 = 2Mhz for PLL
  MCG_C5 = MCG_C5_PRDIV(0x18);

  //clear C6
  MCG_C6 = 0x0;

  //save FMC_PFAPR for unchanged fields
  temp_pfapr = FMC_PFAPR;

  FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
            | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
            | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;


  SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(1)
              | SIM_CLKDIV1_OUTDIV3(1) | SIM_CLKDIV1_OUTDIV4(3);

  for (i = 0 ; i < 4 ; i++);

  //restore FMC_PFAPR
  FMC_PFAPR = temp_pfapr;

  //PLL selected, 11000b multiply PLL by 48 -> 2Mhz * 48 = core 96Mhz
  MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(24);

  //wait for PLL clock source to show up
  while (!(MCG_S & MCG_S_PLLST_MASK));

  //wait for PLL to lock
  while (!(MCG_S & MCG_S_LOCK_MASK));

  //Output of FLL or PLL is selected as source for MCGOUTCLK
  MCG_C1 &= ~MCG_C1_CLKS_MASK;

  //wait for output of the PLL to show up as selected clock
  while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3);

  //Calculate the peripheral clock in KHz
  periph_clk_khz = 96000 / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> 24) + 1);
}
