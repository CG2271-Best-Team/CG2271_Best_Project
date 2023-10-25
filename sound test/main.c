#include "MKL25Z4.h"

#define Right_Motor_Pin_F 0 //port b pin 0 TPM1 C0
#define Right_Motor_Pin_B 1 //port b pin 1 TPM1 C1
#define Left_Motor_Pin_F 29 //port e pin 29 TMP0 C2
#define Left_Motor_Pin_B 30 //port e pin 30 TMP0 C3
#define BUZZER_PIN 2 //port a pin 2 TMP2 C1

#define FREQUENCY 1250
#define processor_freq 48000000
#define MAX_SPEED 1250   // same as max frequency

#define NOTE_C4 261.63
#define NOTE_D4 293.66
#define NOTE_E4 329.63
#define NOTE_F4 349.23
#define NOTE_G4 392.00
#define NOTE_A4 440.00
#define NOTE_B4 493.88
#define NOTE_C5 523.25
#define NOTE_D5 587.33
#define NOTE_E5 659.25
#define NOTE_F5 698.46
#define NOTE_G5 783.99

//extern enum Direction volatile Motor_flag;
static volatile int tone;
static volatile int song_note;
static volatile int check;

/* Delay Function */
static void delay(volatile uint32_t nof) {
	while(nof!=0) {
	__asm("NOP");
	nof--;
	}
}

void Init_PWM(void) 
{
	// Motor setup
	
	// enable clock gating to GPIO on port B and port E
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	// configure port B pin 0 and 1 and port e pin 29 and pin 30 to PWN output
	PORTB->PCR[Right_Motor_Pin_F] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[Right_Motor_Pin_F] |= PORT_PCR_MUX(3);
	PORTE->PCR[Left_Motor_Pin_F] &= ~PORT_PCR_MUX_MASK;
  PORTE->PCR[Left_Motor_Pin_F] |= PORT_PCR_MUX(3);
	PORTB->PCR[Right_Motor_Pin_B] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[Right_Motor_Pin_B] |= PORT_PCR_MUX(3);
	PORTE->PCR[Left_Motor_Pin_B] &= ~PORT_PCR_MUX_MASK;
  PORTE->PCR[Left_Motor_Pin_B] |= PORT_PCR_MUX(3);
	
	// Buzzer set up
	
	// enable clock gating to GPIO on port A and buzzer
	//SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	PORTA->PCR[BUZZER_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[BUZZER_PIN] |= PORT_PCR_MUX(3);
	
	// Enable clock gating to timer peripheral
	SIM_SCGC6 |= SIM_SCGC6_TPM0_MASK;
	SIM_SCGC6 |= SIM_SCGC6_TPM1_MASK;
	SIM_SCGC6 |= SIM_SCGC6_TPM2_MASK;
	
	// select internal clock sources
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	// Frequency of PWN = Board frequency / Prescaler value * (MOD + 1)
	TPM0_MOD = FREQUENCY;
	TPM1_MOD = FREQUENCY;
	TPM2_MOD = FREQUENCY;
	
	// Initialize MOD values for motor
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1->SC |= TPM_SC_CMOD(1) | TPM_SC_PS(7);
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
	TPM0->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM0->SC |= TPM_SC_CMOD(1) | TPM_SC_PS(7);
	TPM0->SC &= ~(TPM_SC_CPWMS_MASK);
	
	// Initialize MOD values for buzzer
	TPM2->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM2->SC |= TPM_SC_CMOD(1) | TPM_SC_PS(7);
	TPM2->SC &= ~(TPM_SC_CPWMS_MASK);
	
	// Set up 6 channels to generate appropriate PWN signals for three sets of motors
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSA_MASK) | (TPM_CnSC_MSB_MASK));
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSA_MASK) | (TPM_CnSC_MSB_MASK));
	TPM0_C2SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSA_MASK) | (TPM_CnSC_MSB_MASK));
	TPM0_C3SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSA_MASK) | (TPM_CnSC_MSB_MASK));
	TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSA_MASK) | (TPM_CnSC_MSB_MASK));
	
	TPM1_C0SC |= TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1);
	TPM1_C1SC |= TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1);
	TPM0_C2SC |= TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1);
	TPM0_C3SC |= TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1);
	TPM2_C1SC |= TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1);
	
	TPM1_C0V = (FREQUENCY / 2); // 50 percent speed forward, right motors, green
	TPM0_C2V = (FREQUENCY / 2); // 50 percent speed forward, left motors, 
	
	TPM1_C1V = 0; 
	TPM0_C3V = 0; 
	
	TPM2_C1V = (FREQUENCY / 2); // initial buzzer is silent
}

void playNote(float note, int duration) {
    if (note != 0) {
			tone = processor_freq / (note * 128); // Calculate the mod value
			TPM2_MOD = tone;
			TPM2_C1V = tone / 3; // 50 percent speed
    } 
		else {
			TPM2_C1V = 0;
		}
		delay(duration);
}

volatile int called = 1;

void playHappyBirthday() {
    float notes[] = {50, 9000, NOTE_A4, NOTE_G4, NOTE_C5, NOTE_B4, NOTE_G4, 0,
                     NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_D5, NOTE_C5, 0,
                     NOTE_G4, NOTE_G4, NOTE_G5, NOTE_E5, NOTE_C5, NOTE_B4, NOTE_A4, 0,
                     NOTE_F5, NOTE_F5, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_C5};

    int durations[] = {400, 400, 800, 400, 400, 800, 400, 800,
                       400, 400, 800, 400, 400, 800, 800,
                       400, 400, 800, 400, 400, 400, 800, 800,
                       400, 400, 800, 400, 400, 800};

    for (int i = 0; i < 29; i++) { // There are 29 notes
			called++;
        playNote(notes[i], durations[i]/100);
    }
}

int main()
{
	SystemCoreClockUpdate();
	Init_PWM();
	playHappyBirthday();
}
