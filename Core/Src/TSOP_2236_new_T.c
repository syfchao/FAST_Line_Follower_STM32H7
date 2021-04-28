/*
 * IR_TSOP2236_byTEOR.c
 *
 *  Created on: 6 wrz 2020
 *      Author: Teodor
 *
 *
 *
 *       You have to configure in CubeMX EXTI line where is TSOP2236 output, falling edge and with pull-up resistor.
 *       Later check the defines in header file
 *
 *       Next in file stm32fxxx_it.h (header)  below the line:  "USER CODE BEGIN ET"
 *       Add it:
 *        extern void IR_IT_Dec();
		  extern uint32_t t2;
 *
 *      Next In file stm32fxxx_it.c in generated interuppt function as you can see example below,
 *
 *      	you have add function IR_IT_Dec(); calling as show below
 *
 *
 *      void EXTI15_10_IRQHandler(void)  //< your EXTI Line handler
{
   USER CODE BEGIN EXTI15_10_IRQn 0

	IR_IT_Dec();   //Only Add this function calling here!!

	 // __HAL_GPIO_EXTI_CLEAR_IT(PINIR_Pin); //Optionally
   USER CODE END EXTI15_10_IRQn 0
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
   USER CODE BEGIN EXTI15_10_IRQn 1

   USER CODE END EXTI15_10_IRQn 1
}

//Is need the 100us resolution timer so configure it in CubeMX and follow steps below:

Next in file stm32fxxx_it.c in your timer handler add the variable to increment
void TIM2_IRQHandler(void)
{
   USER CODE BEGIN TIM2_IRQn 0
   t2++;                       //<<Add this unsigned 32bit variable to increment after every Interrupt. Set increment evry 100us
   USER CODE END TIM2_IRQn 0
  HAL_TIM_IRQHandler(&htim2);
   USER CODE BEGIN TIM2_IRQn 1

   USER CODE END TIM2_IRQn 1
}
in the same file initialize the variale uint32_t t2; sure
 */


#include "stm32h7xx_hal.h"
#include "stm32h7xx_it.h"
#include <stdbool.h>
#include "main.h"
#include <tim.h>
#include <Robot_control.h>
#include <EEPROM.h>

uint32_t irdata[33];   //Used to record the time between two falling edges
bool receiveComplete; //Receive Complete Flag Bits
uint8_t idx;          //Number received for indexing
bool startflag;       //Indicates start of reception
uint32_t pr_IR=0;
uint32_t BLINK_IR=0;
int flaga_RT=0;
int IR_READY;


void EEPROM_IR_STATE_READ();
void OBSLUGAPILOTA();


void IR_INIT()
{
	 HAL_TIM_Base_Start_IT(&htim2); //Turn on Timer
	 EEPROM_IR_STATE_READ();            //--<< Set it in Bluetooth APP
	// IR_READY=1;
}

//My delay 100us function
void delay_100us(uint32_t delay_100us)
{
	uint32_t dtime=t2;

	while(t2 <   (dtime+delay_100us)  )
   	   {
   	   }
}

void IR_IT_Dec()
{

	if( ( (pr_IR+160 )  >  t2  &&  (pr_IR+100 ) <   t2 )  ) //Receive Sync Header
	{
		idx=0;
	//	goto DONE;
	}

	   irdata[idx]= t2-pr_IR;
   	   idx++;

   	   if(idx==33)
   	   {
   		 receiveComplete = true;
   		idx=0;
   	   }
//DONE:
	pr_IR=t2;
	flaga_RT=1;

}

uint32_t remote_code[32];
uint32_t receiver_codet=0;

void teo_dec()
{
	for(int i=0; i<33; i++)
	{
		if(irdata[i] >=8 && irdata[i] < 15)   //Represents 0
		{
			receiver_codet &= ~(1UL << (32-i));
		}
		else if(irdata[i] >=16 && irdata[i]<25) //Represents 1
		{
			receiver_codet |= (1UL << (32-i));
		}
	}
}

void IR_READER()
{
	if(IR_READY)
	{
	if(pr_IR && flaga_RT==1 )
	{
		  HAL_GPIO_TogglePin(IR_LED_GPIO_Port, IR_LED_Pin);

		  flaga_RT=0;
	}

	if(receiveComplete)
	{
		receiveComplete=false;
		teo_dec();
		OBSLUGAPILOTA();
	}

	}

}

int znakwylaczdiode=0;


void OBSLUGAPILOTA()
{

if(receiver_codet==2774180190) //DVD - ROBOT START
{
	URUCHAMIANIE_ROBOTA=1;
}

if(receiver_codet==2774151375) //TV - ROBOT STOP
{
	URUCHAMIANIE_ROBOTA=0;
}
if(receiver_codet==2774176110) // DVR
{


}

if(receiver_codet==2774151885) //  "CD"
{

}
if(receiver_codet==2774143470) // CD-R
{


}
if(receiver_codet==2774159790) //FM
{


}
if(receiver_codet==2774192430) //am  //IR OFF!!! @@@@@@@@@@@@@
{
	IR_READY=0;
}

if(receiver_codet==2774182740) //AUTO SURR
{

}
if(receiver_codet==2774139900) //STErEO/F.S.SURR
{

}

if(receiver_codet==2774143215) // "standard"
{

}
if(receiver_codet==2774190900) //adv.surr
{

}


}



void EEPROM_IR_STATE_READ()
{
	EEPROM_READ_INT(450, &IR_READY);
}
