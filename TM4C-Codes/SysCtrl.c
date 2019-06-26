#include "SysCtrl.h"


/* Grant coprocessor access*/

void SystemInit(void){
/* This is required since TM4C123G has a floating point coprocessor */
SCB_CPAC |= 0x00F00000;
	
}


// Enable clock of the demanded port and offline others to save power.

//GPIO CLOCK
void clock_Enable_PORT(U8_t port){
	// make a delay to manipulate instruction well.
	U32_t delay=0;
	//Access clock register.
	GPIO_RCGC_PORT|=port;
	delay=GPIO_RCGC_PORT;
		
}//End of clock function



//UART CLOCK
void Clock_Enable_UART(U8_t uart_n){
	// make a delay to manipulate instruction well.
	U32_t delay=0;
	//Access clock register.	
	UART_RCGC|=uart_n;
	delay=UART_RCGC;

}//End of clock function

//Timer clock
void Clock_Enable_Timer(U8_t timer_n){
	// make a delay to manipulate instruction well.
	U32_t delay=0;
	//Access clock register.	
	Timer_clock|=timer_n;
	delay=Timer_clock;

}//End of clock function	
		
	
//ADC clock
void Clock_Enable_ADC(U8_t ADC_n){
	// make a delay to manipulate instruction well.
	U32_t delay=0;
	//Access clock register.	
	ADC_clock|=ADC_n;
	delay=ADC_clock;

}//End of clock function


