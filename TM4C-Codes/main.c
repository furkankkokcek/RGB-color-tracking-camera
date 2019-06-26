// main.c
// Runs on LM4F120/TM4C123
// UART runs at 115,200 baud rate 
// Daniel Valvano
// May 23, 2014

/* This example accompanies the books
  "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
  ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014

"Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
 
 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include "ADC.h"
#include <stdio.h>
#include <stdint.h> // C99 variable types
#include <math.h>
//#include "inc/tm4c123gh6pm.h"
#include <stdbool.h>
#include "data_types.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "tm4c123gh6pm.h"
#include "pll.h"
#include "UART.h"
#include "esp8266.h"
#include "LED.h"


#include <stdbool.h>
#include <string.h>

#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/fpu.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"

#include "utils/uartstdio.h"
#include "utils/cmdline.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"

#include "DelayTimer.h"

#define Period  320000 //(16000000/50) 50Hz
#define SERVO_STEPS         180     // Maximum amount of steps in degrees (180 is common)
#define SERVO_MIN           9500     // The minimum duty cycle for this servo
#define SERVO_MAX           38500    // The maximum duty cycle
#define GPIO_PC5_WT0CCP1        0x00021407

unsigned int servo_lut[SERVO_STEPS+1];



void Output_Init(void);


void InitServo(void) //PC5
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinConfigure(GPIO_PC5_WT0CCP1);
	GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_5);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
	TimerConfigure(WTIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_B_PWM);
	TimerLoadSet(WTIMER0_BASE, TIMER_B, (Period-1));
	TimerMatchSet(WTIMER0_BASE, TIMER_B, (Period-9600));

	TimerMatchSet(WTIMER0_BASE, TIMER_B, 1);
	TimerEnable(WTIMER0_BASE, TIMER_B);
}

void ConfigureServo(void)
{
    unsigned int servo_stepval, servo_stepnow;
    unsigned int i;

    servo_stepval   = ( (SERVO_MAX - SERVO_MIN) / SERVO_STEPS );
    servo_stepnow   = SERVO_MIN;

    for (i = 0; i < (SERVO_STEPS+1); i++)
    {
    	servo_lut[i] = (Period-servo_stepnow);
    	servo_stepnow += servo_stepval;
    }
}

void SetServoPosition(uint32_t position)
{
	TimerMatchSet(WTIMER0_BASE, TIMER_B, position);
}

void SetServoAngle(uint32_t angle)
{
	SetServoPosition(servo_lut[angle]);
}

void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count){
    time = 145448;  // 0.1sec at 16 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}

// prototypes for functions defined in startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

char Fetch[] = "hello";
int decisionArray[2];
int currentServoAngle;

unsigned long fiveSec = 0;

void SysTick_Init(unsigned long period){
	NVIC_ST_CTRL_R = 0; // disable SysTick during setup
	NVIC_ST_RELOAD_R = period-1; // reload value for 500us
	NVIC_ST_CURRENT_R = 0; // any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
	NVIC_ST_CTRL_R = 0x00000007; // enable with core clock and interrupts
}

void SysTick_Handler(void){
	fiveSec++;
	if(fiveSec % 5 == 0){
		fiveSec = 0;
    ESP8266_SendTCP(Fetch);
		
		LED_BlueToggle();
		int i, j;
		char str[52]; 
		for(i=0;i<52;i++){
			char c = returnBuffer(i);
			str[i] = c;
			printf("%c", c);
		}
		decisionArray[0] = 0;
		decisionArray[1] = 0;
		char* token = strtok(str, "-"); 
		
		for(j=0;j<10;j++){
			int temp = atoi(token);
			if(j<5){
				decisionArray[0] += temp;
			}else{
				decisionArray[1] += temp;
			}
      token = strtok(NULL, "-");
		}
		if(decisionArray[0] > 500 || decisionArray[1] > 500){
			if(decisionArray[0] > decisionArray[1] + (decisionArray[0]/10)){
				if(currentServoAngle < 180){
					currentServoAngle += 30;
					printf("\n Servo is turning left!");
				}
			}else if(decisionArray[1] > decisionArray[0] + (decisionArray[1]/10)){
				if(currentServoAngle > 0){
					currentServoAngle -= 30;
					printf("\n Servo is turning right!");
				}
			}
		}
		SetServoAngle(currentServoAngle);
	}
}



int main(void){ 
	int32_t i,n;
	LED_Init();
  Output_Init();              // initialize output device
	//ADC0_Init();
	//ADC0SS1_Init(3, SW);
	InitServo();
	ConfigureServo();
	currentServoAngle = 90;
	
	SetServoAngle(currentServoAngle);
	Delay100ms(10);

  DisableInterrupts();
  //PLL_Init(Bus80MHz);
	
  printf("\n\r-----------\n\rSystem starting...\n\r");
  ESP8266_Init(115200);      // connect to access point, set up as client
  ESP8266_GetVersionNumber();
	EnableInterrupts();
  ESP8266_GetStatus();
    if(ESP8266_MakeTCPConnection("192.168.43.245")){ // open socket in server
      ESP8266_SendTCP(Fetch);
  }
	printf("\n\rUDP sent!\n\r");
	SysTick_Init(1600000);
	while(1){
		WaitForInterrupt();
  }
}
