#include "MKL25Z4.h"
#define BAUD_RATE 9600
#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 128
#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define MASK(x) (1 << (x))

enum Color {
    Red,
    Green,
    Blue,
		Black
};

volatile uint8_t data;
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

enum Color Color_Convert(uint8_t data)
{
	if(data == 0x31)
	{
		return Red;
	}
	if (data == 0x33)
	{
		return Green;
	}
	if (data == 0x35)
	{
		return Blue;
	}
	return Black;
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
	if(color == Black) {
		PTB->PDOR |= MASK(RED_LED);
		PTB->PDOR |= MASK(GREEN_LED);
		PTD->PDOR |= MASK(BLUE_LED);
	}
}

void initUART2(uint32_t baud_rate)
{
	uint32_t divisor, bus_clock;
	
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	PORTE->PCR[UART_TX_PORTE22] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_TX_PORTE22] |= PORT_PCR_MUX(4); // setting the pin 22 to UART transmit function
	
	PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4); // setting the pin 22 to UART transmit function
	
	UART2->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK)); // disable the transmit and receive functions first to configure other parts
	
	bus_clock = (DEFAULT_SYSTEM_CLOCK/2); // this is the UART module clock
	divisor = bus_clock / (baud_rate * 16); // this is the value to be stored in SBR[12:0], representative of the baud rate
	UART2->BDH = UART_BDH_SBR(divisor >> 8); // upper 5 bits of SBR[12:0] stored in BDH register
	UART2->BDL = UART_BDL_SBR(divisor); // lower 8 bits of SBR[12:0] stored in BDL register
	
	NVIC_ClearPendingIRQ(UART2_IRQn);
	NVIC_EnableIRQ(UART2_IRQn); // enable ISR for UART2 module
	
	UART2->C2 &= ~((UART_C2_TIE_MASK) | (UART_C2_RIE_MASK)); 
	UART2->C2 |= ((UART_C2_TIE_MASK) | (UART_C2_RIE_MASK)); // enable the transmit and receive interrupt
	
	
	UART2->C1 = 0; // no parity bit
	UART2->S2 = 0; // do not need to configure
	UART2->C3 = 0; // 9 bits data bus, do not need to configure
	
	UART2->C2 |= ((UART_C2_TE_MASK) | (UART_C2_RE_MASK)); // enable the transmit and receive functions now
}

void UART2_IRQHandler() 
{
	NVIC_ClearPendingIRQ(UART2_IRQn);
	
	if (UART2->S1 & UART_S1_TDRE_MASK) // check if we can transmit data
	{
		UART2->D = data;
	}
	if (UART2->S1 & UART_S1_RDRF_MASK) 
	{
		data = UART2->D;
	}
	else {
		
	}
}

static void delay(volatile uint32_t nof)
{
	while(nof!=0)
	{
		__asm("NOP");
		nof--;
	}
}

int main(void)
{
	SystemCoreClockUpdate();
	initUART2(BAUD_RATE);
	InitGPIO();
	
	while(1)
	{
		Led_Control(Color_Convert(data));
	}
}