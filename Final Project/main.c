/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"
#include "UART.h"
#include "PWM.h"
#include "PIT.h"
#include "Directions.h"

osThreadId_t data_thread_id;

//int volatile
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/

void Red_LED_thread () 
{
  // ...
  for (;;) 
	{
		Red_LED_Control();
	}
}

void Green_LED_thread () 
{
	for (;;) 
	{
		Green_LED_Control();
	}
}

void Buzzer_thread() 
{
	for (;;) 
	{
		Buzzer_control();
	}
} 

void Motor_thread()
{
	for (;;)
	{
		Motor_Control();
	}
}

void Data_thread()
{
	for (;;)
	{
		uint32_t flags = osThreadFlagsWait(0x00000001, osFlagsWaitAll, osWaitForever); // wait for the LSB to be set to start decoding data
		if (flags & 0x00000001)
			Data_decode();
	}
}

int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	Init_PWM();
	Init_PIT();
	Init_UART2(BAUD_RATE); // Baudrate = 9600s
  // ...
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
	
	data_thread_id = osThreadNew(Data_thread, NULL, NULL); // Brain thread for decoding UART data
  osThreadNew(Red_LED_thread, NULL, NULL);    // Create application main thread
	osThreadNew(Green_LED_thread, NULL, NULL);
	osThreadNew(Buzzer_thread, NULL, NULL);
	osThreadNew(Motor_thread, NULL, NULL);
	
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
