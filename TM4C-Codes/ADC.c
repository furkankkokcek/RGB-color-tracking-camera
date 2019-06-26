#include "ADC.h"
#include "SysCtrl.h"
#include "GPIO_ADC.h"
#include "HW_ADC.h"



//ADC Setupt function.
void ADC0_SS3_Init(U8_t channel,U8_t pin,TrigADC_m tri){
//Enable clocl for PORTD.
	clock_Enable_PORT(Clock_PortD);	
//GPIO init.	
	GPIO_PortD_ADC(pin);
//Enable ADC clock.
  Clock_Enable_ADC(ADC0);	
//!Register setup.
//disable ADC.
  GPIO_PortD_ADC_Dis();
//!Sample Sequencer (3) Configuration.
//disable sample sequencer.
	ADCACTSS&=~(1<<3);       
	switch(tri){
		case SW:    //Always continuously sample by SW.
		            ADCEMUX&=~(0xF000);break;      
		case GPIO:  //GPIO as switch button.
                ADCEMUX=(0x4000);break; 
    case TIMER: //ADC trigered by timer.
                ADCEMUX=(0x5000);break;
		default:
		        break;        		
	}//!End of switch.
//configure input pin by multiblxer	
  ADCSSMUX3=channel;            
//control register only one sample and raw interrupt.
  ADCSSCTL3|=6;	
//Enable sample sequencer.
	ADCACTSS|=(1<<3);
//Enable ADC in GPIO register.	
  GPIO_PortD_ADC_EN(pin); 
	
}//! End of ADC setup Function.

//------------------------------------------------------------------------------

U32_t ADC0_SS3_Read(void){
	U32_t digital=0;
//start conversion.
	ADCPSSI|=(1<<3);
//polling on Interrupt status flag.
	while(!(ADCRIS&(1<<3)));
//get digital value.
	digital=ADCSSFIFO3;
//clear Interrupt status flag.
	ADCISC=0x8;
//return value.
	return digital;
}//! End of reading digital function.

//-----------------------------------------------------------------------------

void ADC0SS1_Init(U8_t sample,TrigADC_m tri){
//Enable clocl for PORTD.
	clock_Enable_PORT(Clock_PortD);	
//Enable ADC clock.
    Clock_Enable_ADC(ADC0);	
//!Register setup.
//disable ADC.
    GPIO_PortD_ADC_Dis();
//!Sample Sequencer (1) Configuration.
//disable sample sequencer.
	ADCACTSS&=~(1<<1);
//trigger method.	
	switch(tri){
		case SW:    //Always continuously sample by SW.
		        ADCEMUX&=~(0x00f0);break;      
		case GPIO:  //GPIO as switch button.
                ADCEMUX=(0x0040);break; 
        case TIMER: //ADC trigered by timer.
                ADCEMUX=(0x0050);break;
		default:
		        break;        		
	}//!End of trigger switch.
//choose sample in SS1. number of sensor from 1 to 4.
    switch(sample){
		case 1://sample1
		       GPIO_PortD_ADC(0);
               ADCSSMUX1|=0x0007;//pd0.
               ADCSSCTL1|=(1<<2)|(1<<1);	   
               ADCACTSS|=(1<<1);//Enable sample sequencer.	
               GPIO_PortD_ADC_EN(0); //Enable ADC in GPIO register.	
			   break;
		case 2:
			   //sample 1 and 2.
			   GPIO_PortD_ADC(0);
			   GPIO_PortD_ADC(1);
			   ADCSSMUX1|=0x0067;//pd1,0.
			   ADCSSCTL1|=(1<<2)|(1<<6)|(1<<5);	   
	           ADCACTSS|=(1<<1);//Enable sample sequencer.	
               GPIO_PortD_ADC_EN(0); //Enable ADC in GPIO register.
			   GPIO_PortD_ADC_EN(1); //Enable ADC in GPIO register.
			   break;
		case 3:
		       //sample 1,2 and 3.
			   GPIO_PortD_ADC(0);
			   GPIO_PortD_ADC(1);
			   GPIO_PortD_ADC(2);
			   ADCSSMUX1|=0x0567;//pd1,0,2.
			   ADCSSCTL1|=(1<<2)|(1<<6)|(1<<10)|(1<<9);	   
	           ADCACTSS|=(1<<1);//Enable sample sequencer.	
               GPIO_PortD_ADC_EN(0); //Enable ADC in GPIO register.
			   GPIO_PortD_ADC_EN(1); //Enable ADC in GPIO register.
			   GPIO_PortD_ADC_EN(2); //Enable ADC in GPIO register.
			   break;
		case 4:
               //sample 1,2,3 and 4.
			   GPIO_PortD_ADC(0);
			   GPIO_PortD_ADC(1);
			   GPIO_PortD_ADC(2);
			   GPIO_PortD_ADC(3);
			   ADCSSMUX1|=0x4567;//pd1,0,2,3.
			   ADCSSCTL1|=(1<<2)|(1<<6)|(1<<10)|(1<<14)|(1<<13);	   
	           ADCACTSS|=(1<<1);//Enable sample sequencer.	
               GPIO_PortD_ADC_EN(0); //Enable ADC in GPIO register.
			   GPIO_PortD_ADC_EN(1); //Enable ADC in GPIO register.
			   GPIO_PortD_ADC_EN(2); //Enable ADC in GPIO register.
			   GPIO_PortD_ADC_EN(3); //Enable ADC in GPIO register.
			   break;		
		
		default:
		       break;	   		
		
	}//!End of sample switch.
	
}//! End of ADC init SS1.

//----------------------------------------------------------------------------------------

//ADC Reading:Digital value.
void ADC0_SS1_Read(U32_t digital[4]){
	U8_t index=0;
//start conversion.
	ADCPSSI|=(1<<1);
//polling on Interrupt status flag.
	while(!(ADCRIS&(1<<1)));
//get reading of samples	
	while(ADCOSTAT&(1<<1)){
		digital[index++]=ADCSSFIFO1;
	}//!End of reading loop.
	
//clear Interrupt status flag.
	ADCISC=0x2;

}//! End of ADC Read SS1 FIFO1.


