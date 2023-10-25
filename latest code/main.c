/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"
#include "UART.h"
#include "PWN.h"
#include "PIT.h"

/* Delay Function 
static void delay(volatile uint32_t nof) {
	while(nof!=0) {
	__asm("NOP");
	nof--;
	}
}  */

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/

void Red_LED_thread () 
{
  // ...
  for (;;) {
		Red_LED_Control();
	}
}

void Green_LED_thread () 
{
	for (;;) {
		Green_LED_Control();
	}
}

 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	Init_PWN();
	Init_PIT();
	Init_UART2(BAUD_RATE); // Baudrate = 9600s
  // ...
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(Red_LED_thread, NULL, NULL);    // Create application main thread
	osThreadNew(Green_LED_thread, NULL, NULL);
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
