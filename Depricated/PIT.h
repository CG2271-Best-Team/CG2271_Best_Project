#include "MKL25Z4.h"

#define PTC1_Pin 1 // all port c
#define Green_Pin_1 3
#define Green_Pin_2 4
#define Green_Pin_3 5
#define Green_Pin_4 6
#define Green_Pin_5 7
#define Green_Pin_6 8
#define Green_Pin_7 9
#define Green_Pin_8 10
#define Green_Pin_9 11
#define Green_Pin_10 12 // port c

#define RED_LED 18 
#define FREQUENCY 1250
#define LOAD_250ms 3000000
#define LOAD_500ms 6000000
#define MASK(x) (1 << (x))
 
extern int volatile LED_flag;
int volatile RED_LED_FLAG = 1;
int volatile Green_LED_index = 3;
int volatile test_flag = 0;

void Init_PIT(void) 
{
	  // Enable clock gating to PIT module and PORTA and PORTC
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK; 
	  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTA_MASK; 

		// Set RED LED pins
		PORTC->PCR[PTC1_Pin] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[PTC1_Pin] |= PORT_PCR_MUX(1);
		PORTC->PCR[Green_Pin_1] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[Green_Pin_1] |= PORT_PCR_MUX(1);
		PORTC->PCR[Green_Pin_2] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[Green_Pin_2] |= PORT_PCR_MUX(1);
		PORTC->PCR[Green_Pin_3] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[Green_Pin_3] |= PORT_PCR_MUX(1);
		PORTC->PCR[Green_Pin_4] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[Green_Pin_4] |= PORT_PCR_MUX(1);
		PORTC->PCR[Green_Pin_5] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[Green_Pin_5] |= PORT_PCR_MUX(1);
		PORTC->PCR[Green_Pin_6] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[Green_Pin_6] |= PORT_PCR_MUX(1);
		PORTC->PCR[Green_Pin_7] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[Green_Pin_7] |= PORT_PCR_MUX(1);
		PORTC->PCR[Green_Pin_8] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[Green_Pin_8] |= PORT_PCR_MUX(1);
		PORTC->PCR[Green_Pin_9] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[Green_Pin_9] |= PORT_PCR_MUX(1);
		PORTC->PCR[Green_Pin_10] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[Green_Pin_10] |= PORT_PCR_MUX(1);


		// Set all LED pins to be output
		PTC->PDDR |= (MASK(PTC1_Pin ) | MASK(Green_Pin_1) | MASK(Green_Pin_2) | MASK(Green_Pin_3) | MASK(Green_Pin_4) | MASK(Green_Pin_5) | MASK(Green_Pin_6) | MASK(Green_Pin_7) | MASK(Green_Pin_8) | MASK(Green_Pin_9) | MASK(Green_Pin_10));
	
		//Configure PIT module for a 2Hz (0.5s) interrupt for 250ms blinking
		
	  // Enable PIT module
    PIT->MCR = 0x00; 
		
    // Calculate LDVAL value for 2Hz (24MHz / 2 / 2) = 6000000, which is the initial value for LED blinking in stationary mode
		PIT->CHANNEL[0].LDVAL = LOAD_250ms;
		
    // Enable PIT module 0 channel 0 interrupts generation
    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK; 
		
    // Enable PIT timer
    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK; 
		
		// Enable PIT ISR
		NVIC_ClearPendingIRQ(PIT_IRQn); 
    NVIC_EnableIRQ(PIT_IRQn); 
}

void PIT_IRQHandler() 
{
	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;	
	
	if (RED_LED_FLAG == 1)
	{
			RED_LED_FLAG = 0;
	}
	else 
	{
			RED_LED_FLAG = 1;
	}
	
	// Update the PIT value everytime after the execution of the PIT ISR
	PIT->CHANNEL[0].LDVAL = LED_flag ? LOAD_500ms : LOAD_250ms; // CAUTION!	
	
	
	// Update green led index 
	Green_LED_index = (Green_LED_index == 12) ? 3 : Green_LED_index + 1;
	
	NVIC_ClearPendingIRQ(PIT_IRQn);
}

void Red_LED_Control()
{
	if(RED_LED_FLAG == 1)
		{
			test_flag = !test_flag;
			PTC->PDOR |= MASK(PTC1_Pin);
		}
	else 
		{
			test_flag = !test_flag;
			PTC->PDOR &= ~MASK(PTC1_Pin);
		}
}

void Green_LED_Control() 
{
	if (LED_flag) { // moving - running mode
		PTC->PDOR &= ~(MASK(Green_Pin_1) | MASK(Green_Pin_2) | MASK(Green_Pin_3) | MASK(Green_Pin_4) | MASK(Green_Pin_5) | MASK(Green_Pin_6) | MASK(Green_Pin_7) | MASK(Green_Pin_8) | MASK(Green_Pin_9) | MASK(Green_Pin_10));
	
		PTC->PDOR |= MASK(Green_LED_index);
	} else { // stationary - all on
		PTC->PDOR |= (MASK(Green_Pin_1) | MASK(Green_Pin_2) | MASK(Green_Pin_3) | MASK(Green_Pin_4) | MASK(Green_Pin_5) | MASK(Green_Pin_6) | MASK(Green_Pin_7) | MASK(Green_Pin_8) | MASK(Green_Pin_9) | MASK(Green_Pin_10));
	}
}
