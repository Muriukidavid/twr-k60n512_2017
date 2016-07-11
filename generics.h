/*
 * Copyright (c) 2015 David Muriuki
 * see the LICENCE file
*/
#ifndef generics_h
#define generics_h
#define out 1
#define in  0
#define on 1
#define off 0
//Shut compiler errors :)
#ifdef __UINT8_TYPE__
typedef __UINT8_TYPE__ uint8_t;
#endif
#ifdef __UINT16_TYPE__
typedef __UINT16_TYPE__ uint16_t;
#endif
#ifdef __UINT32_TYPE__
typedef __UINT32_TYPE__ uint32_t;
#endif

//generic bit register
typedef struct {
volatile unsigned int 
bit0:1,
bit1:1,
bit2:1,
bit3:1,
bit4:1,
bit5:1,
bit6:1,
bit7:1,
bit8:1,
bit9:1,
bit10:1,
bit11:1,
bit12:1,
bit13:1,
bit14:1,
bit15:1,
bit16:1,
bit17:1,
bit18:1,
bit19:1,
bit20:1,
bit21:1,
bit22:1,
bit23:1,
bit24:1,
bit25:1,
bit26:1,
bit27:1,
bit28:1,
bit29:1,
bit30:1,
bit31:1;
} bitreg;

//generic uint32 register with single bit access
typedef union {
bitreg bit_reg;
uint32_t word_reg;
} REG;
#endif
