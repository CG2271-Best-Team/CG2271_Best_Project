#include "MKL25Z4.h"
#include "Directions.h"
#include "cmsis_os2.h"

#define BAUD_RATE 9600
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 128
#define LOAD_500ms 6000000
#define LOAD_250ms 3000000
#define MASK(x) (1 << (x))

volatile uint8_t data;
unsigned int counter = 0;

int volatile LED_flag = 1; // 1 for moving, 0 for stationary
int volatile Buzzer_flag = 0; // 0 for course run, 1 for termination tune
Direction volatile Motor_flag = Stationary;

extern osThreadId_t data_thread_id;



void Init_UART2(uint32_t baud_rate)
{
	uint32_t divisor, bus_clock;
	
	// Enable clock gating to UART module
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	// Set up port E pin 23 to UART receive function
	PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4);  
	
	// Disable the receive function first to configure other parts
	UART2->C2 &= ~(UART_C2_RE_MASK); 
	
	// Set up the SBR register value to ensure a baudrate of 9600 bps
	bus_clock = (DEFAULT_SYSTEM_CLOCK/2); // CHECK THIS CLOCK!!!
	divisor = bus_clock / (baud_rate * 16); 
	UART2->BDH = UART_BDH_SBR(divisor >> 8);
	UART2->BDL = UART_BDL_SBR(divisor); 
	
	// Set up interrupt for UART2 module
	NVIC_ClearPendingIRQ(UART2_IRQn);
	NVIC_EnableIRQ(UART2_IRQn);
	
	UART2->C2 &= ~(UART_C2_RIE_MASK); 
	UART2->C2 |= (UART_C2_RIE_MASK); 
	
	// Set up other peripherals
	UART2->C1 = 0; // no parity bit
	UART2->S2 = 0; // do not need to configure
	UART2->C3 = 0; // 9 bits data bus, do not need to configure
	
	// Enable the receive function after setting up the rest
	UART2->C2 |= UART_C2_RE_MASK; 
}

Direction Direction_convert(uint8_t data)
{
	if(data == 0x37 || data == 0x43)
	{
		return Stationary;
	}
	if(data == 0x29)
	{
		return Forward;
	}
	if(data == 0x31)
	{
		return Backward;
	}
	if (data == 0x33)
	{
		return Left;
	}
	if (data == 0x35)
	{
		return Right;
	}
	if (data == 0x39) 
	{
		return SpotRight;
	}
	if (data == 0x41)
	{
		return SpotLeft;
	}
}

int LED_convert (uint8_t data)
{
	if(data == 0x37)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


int Buzzer_convert (uint8_t data)
{
	if(data == 0x43)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void UART2_IRQHandler() // the UART IRQ sets the thread flag for data decoding thread to read in and decode the data
{
	NVIC_ClearPendingIRQ(UART2_IRQn);
	
	if (UART2->S1 & UART_S1_RDRF_MASK) 
	{
		data = UART2->D; // first capture the incoming data
		osThreadFlagsSet(data_thread_id, 0x00000001); // then set the data processing flag to decode it
	}
}

void Data_decode()
{
	Buzzer_flag = Buzzer_convert(data);
	Motor_flag = Direction_convert(data);
	LED_flag = LED_convert(data);
	
}

/* In PWM for now
void Motor_Control(void) {
	if (Direction_convert(data) == Forward)
	{
		
	}
	else if (Direction_convert(data) == Backward)
	{
		
	}
	else if (Direction_convert(data) == Left)
	{
		
	}
	else if (Direction_convert(data) == Right)
	{
		
	}
}
*/