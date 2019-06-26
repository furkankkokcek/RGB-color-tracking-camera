#ifndef __GPIO_H__
#define __GPIO_H__

#include "data_types.h"



// initialization PORTD.
void GPIO_PortD_ADC(U8_t pin);

// Enable ADC. 
void GPIO_PortD_ADC_EN(U8_t pin);


// Disable ADC. 
void GPIO_PortD_ADC_Dis(void);
	
#endif //__GPIO_H__


