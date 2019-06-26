#include "GPIO_ADC.h"
#include "SysCtrl.h"
#include "HW_gpio.h"

void GPIO_PortD_ADC(U8_t pin){//!Start the setup function.

	//Open lock
	GPIO_PORTD_lock=0x4c4f434b;
	GPIO_PORTD_CR|=0x0000000f;
		switch(pin){
			case 0://pin d0
			GPIO_PORTD_AFSEL|=0x01;//Select alternating Function for PORTD pins as analog.
			GPIO_PORTD_DEN&=~(1<<PD0);//Disable digital function for portd pins
			break;
			case 1://pin d1
			GPIO_PORTD_AFSEL|=0x02;//Select alternating Function for PORTD pins as analog.
			GPIO_PORTD_DEN&=~(1<<PD1);//Disable digital function for portd pins
			break;
			case 2://pin d2
			GPIO_PORTD_AFSEL|=0x04;//Select alternating Function for PORTD pins as analog.
			GPIO_PORTD_DEN&=~(1<<PD2);//Disable digital function for portd pins
			break;
			case 3://pin d3
			GPIO_PORTD_AFSEL|=0x08;//Select alternating Function for PORTD pins as analog.
			GPIO_PORTD_DEN&=~(1<<PD3);//Disable digital function for portd pins
			break;
			default:
			break;
		}	
}//!End of function of initialization.


// Disable ADC. 
void GPIO_PortD_ADC_Dis(void){//!Start the Enable function.
	//Turn OFF Analog function for portd pins
	GPIO_PORTD_AMSEL=0x00;
}//!End of function of ADC disable.


// Enable ADC. 
void GPIO_PortD_ADC_EN(U8_t pin){//!Start the Disable function.
	//Turn ON Analog function for portd pins
	switch(pin){
		case 0:
		GPIO_PORTD_AMSEL|=0x01;break;
		case 1:
		GPIO_PORTD_AMSEL|=0x02;break;
		case 2:
		GPIO_PORTD_AMSEL|=0x04;break;
		case 3:
		GPIO_PORTD_AMSEL|=0x08;break;
	}
}//!End of function of ADC enable.


