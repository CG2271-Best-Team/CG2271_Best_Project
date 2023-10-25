#include "MKL25Z4.h"
#define PTA1_Pin 1
#define PTA2_Pin 2
#define PTC1_Pin 1
#define FREQUENCY 1250
#define LOAD_250ms 300000
#define MASK(x) (1 << (x))

extern int volatile LED_flag;
 
void Init_LED(void) 
{
	  // Enable clock gating to PORTA and PORTC
	  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTA_MASK;

		// Set all LED pins to GPIO
		PORTA->PCR[PTA1_Pin] &= ~PORT_PCR_MUX_MASK;
		PORTA->PCR[PTA1_Pin] |= PORT_PCR_MUX(1);
		PORTA->PCR[PTA2_Pin] &= ~PORT_PCR_MUX_MASK;
		PORTA->PCR[PTA2_Pin] |= PORT_PCR_MUX(1);
		PORTC->PCR[PTC1_Pin] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[PTC1_Pin] |= PORT_PCR_MUX(1);
		
		// Set all LED pins to be output
		PTA->PDDR |= MASK((PTA1_Pin) | (PTA2_Pin));
		PTC->PDDR |= MASK(PTC1_Pin);
}

void offRGB(void)
{
    PTA->PDOR |= (MASK(PTA1_Pin) | MASK(PTA2_Pin));
    PTC->PDOR |= MASK(PTC1_Pin);
}

void Red_LED_Blink()
{
	int time = (LED_flag) ? 500 : 250;
	PTC->PDOR |= MASK(PTC1_Pin);
	osDelay(time);
	PTC->PDOR &= ~MASK(PTC1_Pin);
	osDelay(time);
}
