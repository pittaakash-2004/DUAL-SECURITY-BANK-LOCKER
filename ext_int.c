                 /* external_interrupt_test.c */
#include <lpc21xx.h>
#include "pin_function_defines.h"
#include "defines.h"
#include "ext_int.h"
#include "lcd.h"
#include"lcd_defines.h"
#include"kpm.h"
#include "spi_eeprom.h"
#include "delay.h"
#include "uart0.h"
#include "R305.h" 
#include "spi.h"
#include<string.h>
unsigned char a;
// Function to configure and enable External Interrupt 2 (EINT2)
void Enable_EINT2(void)
{
	// Configure P0.7 as EINT2 function
	// (Equivalent to CFGPIN(PINSEL0,3,FUNC4))
	PINSEL0 |= 0x0000C000;

	// Enable EINT2 interrupt in the VIC (Interrupt number 16)
	SSETBIT(VICIntEnable,16);

	// Assign EINT2 to VIC Vector Slot 1 and enable the slot
	VICVectCntl1 = 0x20 | 16;

	// Set the ISR (Interrupt Service Routine) address for EINT2
	VICVectAddr1 = (unsigned)eint2_isr;

	// Configure EINT2 as edge-triggered interrupt
	SETBIT(EXTMODE,2);

	// Configure EINT2 to trigger on the falling edge
	CLRBIT(EXTPOLAR,2);
}


// Interrupt Service Routine (ISR) for External Interrupt 2
void eint2_isr(void) __irq
{
	// Set a flag indicating that EINT2 interrupt has occurred
	a = 1;

	// Clear the EINT2 interrupt flag
	SCLRBIT(EXTINT,2);

	// Inform the VIC that interrupt servicing is complete
	VICVectAddr = 0;
}
