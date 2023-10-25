#include "MKL25Z4.h"                    // Device header
#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define MASK(x) (1 << (x))

enum Color {
    Red,
    Green,
    Blue
};

unsigned int counter = 0;

void InitGPIO(void)
{
	// Enable Clock to PORTB and PORTD
	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));
	
	// Configure MUX settings to make all 3 pins GPIO
	PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);
	PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
	PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);
	
	// Set Data Direction Registers for PortB and PortD
	PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
	PTD->PDDR |= MASK(BLUE_LED);
}

void Led_Control(enum Color color)
{
	
	if(color == Red) {
		PTB->PDOR &= ~MASK(RED_LED);
		PTB->PDOR |= MASK(GREEN_LED);
		PTD->PDOR |= MASK(BLUE_LED);
	}
	if(color == Green) {
		PTB->PDOR |= MASK(RED_LED);
		PTB->PDOR &= ~MASK(GREEN_LED);
		PTD->PDOR |= MASK(BLUE_LED);
	}
	if(color == Blue) {
		PTB->PDOR |= MASK(RED_LED);
		PTB->PDOR |= MASK(GREEN_LED);
		PTD->PDOR &= ~MASK(BLUE_LED);
	}
}

int main(void)
{
	SystemCoreClockUpdate();
	InitGPIO();
	enum Color currentColor = Red;
	
	while(1)
	{
		if (counter == 500000) 
		{
			switch (currentColor)
			{
							case Red:
									currentColor = Green;
									break;
							case Green:
									currentColor = Blue;
									break;
							case Blue:
									currentColor = Red;
									break;
							default:
									break;
				}
			counter = 0;
  	}
		Led_Control(currentColor);
		counter++;
	}
}
