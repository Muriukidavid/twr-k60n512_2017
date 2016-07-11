#include "generics.h"

/*--------------------------------
	BASE POINTERS
 --------------------------------*/

//The CPUID Base Register 
//#define cpuidbase 0xE000ED00u

//NIVC base pointer
#define nvicbase 0xE000E100u

//watchdog base pointer
//#define watchdogbase 0x40052000u

//SIM - System Integration Module base pointer
#define simbase 0x40047000u

//MCG - Multipurpose Clock Generator base pointer
#define mcgbase 0x40064000u

//FMC - Flash Memory Controller base pointer
#define fmcbase 0x4001F000u

//LLWU - Low Leakage Wake-Up base pointer
#define llwubase 0x4007C000u

//uart5 base pointer
#define uart5base 0x400EB000u
//Ports base pointers for GPIO
//This is used for R/W, interrupts and so on...
#define porta 0x400FF000u
#define portb 0x400FF040u
#define portc 0x400FF080u
#define portd 0x400FF0C0u
#define porte 0x400FF100u

//Ports base pointers for port peripheral register structure
//This is used for pin control(multiplexing), interrupt status and Filter functionality 
#define pta 0x40049000u
#define ptb 0x4004A000u
#define ptc 0x4004B000u
#define ptd 0x4004C000u
#define pte 0x4004D000u
/*-------------------------------
	END OF BASE POINTERS
  -------------------------------*/



/*--------------------------------
	PERIPHERAL STRUCTURES
 --------------------------------*/

/*--------------------------------
	INTERNALS[GPIO, PORT, WDOG, NVIC, CPUID]
 --------------------------------*/
/** GPIO - Peripheral register structure */
typedef struct {
  volatile REG PDOR;   /**< Port Data Output Register, offset: 0x0 */
  volatile REG PSOR;   /**< Port Set Output Register, offset: 0x4 */
  volatile REG PCOR;   /**< Port Clear Output Register, offset: 0x8 */
  volatile REG PTOR;   /**< Port Toggle Output Register, offset: 0xC */
  volatile REG PDIR;   /**< Port Data Input Register, offset: 0x10 */
  volatile REG PDDR;   /**< Port Data Direction Register, offset: 0x14 */
} volatile *GPIO;

/** PORTs - Ports registers and Register structure*/
//Pin Control Register Structure
typedef struct {
volatile unsigned int
ps:1, //Pull Select, 0: internal pull-down resistor, 1: internal pull-up resistor :: Pull Enable(pe below) bit has to be set first
pe:1, //Pull Enable, 0: internal pull-up/down disabled, 1: internal pull-up/down enabled :: Provided pin is configured as a digital input
sre:1, //Slew Rate Enable, 0: fast 1: slow :: pins have to be digital outputs 
pad1:1, //unused
pfe:1, //Passive Filter Enable, 0: Passive Input Filter is disabled, 1: Passive Input Filter is enabled :: Provided pin is configured as a digital input
       //A low pass filter (10 MHz to 30 MHz bandwidth) is enabled on the digital input path, Disable the Passive Input Filter when supporting high speed interfaces (> 2 MHz) on the pin
ode:1, //Open Drain Enable, 0:disabled 1:enabled :: Provided pin is configured as a digital output
dse:1, //Drive Strength Enable, 0:Low drive strength 1:High drive strength :: Provided pin is configured as a digital output  
pad2:1, //unused
mux:3, //mux pins, 000:Pin Disabled (Analog). 001:Alt1(GPIO). 010:Alt2. 011:Alt3. 100:Alt4. 101:Alt5. 110:Alt6. 111:Alt7(chip specific/JTAG/NMI).
pad3:4, //unused
lock:1, //Lock Register, 0:Pin Control Register bits [15:0] are not locked, 1:Pin Control Register bits [15:0] are locked and cannot be updated until the next System Reset
irqc:4, //Interrupt configuration, 0000:Interrupt/DMA Request disabled. 0001:DMA Request on rising edge. 0010:DMA Request on falling edge. 0011:DMA Request on either edge. 0100:Reserved 1000:Interrupt when logic zero. 
	//1001:Interrupt on rising edge. 1010:Interrupt on falling edge. 1011:Interrupt on either edge. 1100:Interrupt when logic one. others:reserved
pad5:4,
isf:1, //Interrupt status flag
pad6:7;
} volatile pcr;

/** PORT - Peripheral register structure */
typedef struct {
  pcr PCR[32];	/**< Pin Control Register n, array offset: 0x0, array step: 0x4 */
  REG GPCLR;	/**< Global Pin Control Low Register, offset: 0x80 */
  REG GPCHR;	/**< Global Pin Control High Register, offset: 0x84 */
  uint8_t RESERVED_0[24];
  REG ISFR;	/**< Interrupt Status Flag Register, offset: 0xA0 */
  uint8_t RESERVED_1[28];
  REG DFER;	/**< Digital Filter Enable Register, offset: 0xC0 */
  REG DFCR;	/**< Digital Filter Clock Register, offset: 0xC4 */
  REG DFWR;	/**< Digital Filter Width Register, offset: 0xC8 */
} volatile *PORT;


/** WDOG - Peripheral register structure */
typedef struct {
  uint16_t STCTRLH;                                //< Watchdog Status and Control Register High, offset: 0x0 
  uint16_t STCTRLL;                                //< Watchdog Status and Control Register Low, offset: 0x2 
  uint16_t TOVALH;                                 //< Watchdog Time-out Value Register High, offset: 0x4 
  uint16_t TOVALL;                                 //< Watchdog Time-out Value Register Low, offset: 0x6 
  uint16_t WINH;                                   //< Watchdog Window Register High, offset: 0x8 
  uint16_t WINL;                                   //< Watchdog Window Register Low, offset: 0xA 
  uint16_t REFRESH;                                //< Watchdog Refresh Register, offset: 0xC 
  uint16_t UNLOCK;                                 //< Watchdog Unlock Register, offset: 0xE 
  uint16_t TMROUTH;                                //< Watchdog Timer Output Register High, offset: 0x10 
  uint16_t TMROUTL;                                //< Watchdog Timer Output Register Low, offset: 0x12 
  uint16_t RSTCNT;                                 //< Watchdog Reset Count Register, offset: 0x14 
  uint16_t PRESC;                                  //< Watchdog Prescaler Register, offset: 0x16 
} volatile *WDOG;



/** NVIC - Peripheral register structure */
typedef struct {
  uint32_t ISER[4];                                /**< Interrupt Set Enable Register n, array offset: 0x0, array step: 0x4 */
  uint8_t RESERVED_0[112];
  uint32_t ICER[4];                                /**< Interrupt Clear Enable Register n, array offset: 0x80, array step: 0x4 */
  uint8_t RESERVED_1[112];
  uint32_t ISPR[4];                                /**< Interrupt Set Pending Register n, array offset: 0x100, array step: 0x4 */
  uint8_t RESERVED_2[112];
  uint32_t ICPR[4];                                /**< Interrupt Clear Pending Register n, array offset: 0x180, array step: 0x4 */
  uint8_t RESERVED_3[112];
  uint32_t IABR[4];                                /**< Interrupt Active bit Register n, array offset: 0x200, array step: 0x4 */
  uint8_t RESERVED_4[240];
  uint8_t IP[104];                                 /**< Interrupt Priority Register n, array offset: 0x300, array step: 0x1 */
  uint8_t RESERVED_5[2712];
  uint32_t STIR[1];                                /**< Software Trigger Interrupt Register, array offset: 0xE00, array step: 0x4 */
} volatile *NVIC;


/** CPUID - The CPU Identification Number 
*	Implementer(8)|Variant(4)|Constant(4)[=0xF]|PartNo(12)|revision(4)
*/


/*--------------------------------
	OSCI
 --------------------------------*/

typedef struct{
	volatile unsigned char
	sc16p:1,	//Oscillator 16 pF Capacitor Load Configure
	sc8p:1,		//Oscillator 8 pF Capacitor Load Configure
	sc4p:1,
	sc2p:1,
	padding1:1,
	erefsten:1,	//External Reference Stop Enable
	padding2:1,
	erclken:1;
} volatile *OSCI_CR;




/*--------------------------------
	Peripheral instances
 --------------------------------*/
//instances of ports
PORT PA = (PORT)pta;
PORT PB = (PORT)ptb;
PORT PC = (PORT)ptc;
PORT PD = (PORT)ptd;
PORT PE = (PORT)pte;

//instances of GPIO for ports
GPIO GPIOA = (GPIO)porta;
GPIO GPIOB = (GPIO)portb;
GPIO GPIOC = (GPIO)portc;
GPIO GPIOD = (GPIO)portd;
GPIO GPIOE = (GPIO)porte;

//nivc instance
NVIC nvic = (NVIC)nvicbase;

//FMC instance
//FMC fmc = (FMC)fmcbase;

OSCI_CR osci = (OSCI_CR)0x40065000u;
