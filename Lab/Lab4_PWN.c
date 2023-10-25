#include "MKL25Z4.h"
#define CLOCK_SETUP 1
#define PTB0_Pin 0
#define PTB1_Pin 1

enum Note {
    NOTE_C = 262,
    NOTE_D = 294,
    NOTE_E = 330,
    NOTE_F = 349,
    NOTE_G = 392,
    NOTE_A = 440,
    NOTE_B = 494
};

int calculateModValue(enum Note note) {
    // Assuming SystemCoreClock is the clock frequency of your microcontroller
    // You may need to adjust this based on your actual clock frequency
    int prescaler = 128; // Assuming no prescaling

    // Calculate MOD value based on the formula
	  int modValue = SystemCoreClock / (prescaler * ((int)note - 1));

    return modValue;
}

void initPWN(void) 
{
	// enable clock gating to GPIO on port B
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;

	// configure port B pin 0 and 1 to PWN output
	PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);
	PORTB->PCR[PTB1_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[PTB1_Pin] |= PORT_PCR_MUX(3);
	
	// enable clock gating to timer peripheral on port B
	SIM_SCGC6 |= SIM_SCGC6_TPM1_MASK;

	// select internal clock sources
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	// Frequency of PWN = Board frequency / Prescaler value * (MOD + 1)
	TPM1->MOD = calculateModValue(NOTE_E);
	
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1->SC |= TPM_SC_CMOD(1) | TPM_SC_PS(7);
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
	
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSA_MASK) | (TPM_CnSC_MSB_MASK));
	TPM1_C0SC |= TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1);
}

int main(void) 
{
	initPWN();
	TPM1_C0V = calculateModValue(NOTE_E);
}