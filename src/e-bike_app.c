/******************************************************************
**	  E_Bike
**	  Uart Interrupt
**
**    Versiomn: V1.0
**	  Author  : Kevin Zhang
********************************************************************/

/* Includes ------------------------------------------------------------------*/

#include "e-bike_app.h"


volatile uint8_t hallcode;
volatile uint8_t lasthallcode=0;
Motor_ID MT_State;

uint16_t volatile Speed_Cnt_500us;
uint16_t volatile Speed_Value;
uint16_t Speed_Value_lst;
float  	 Speed_Mil_Hr;

uint16_t volatile Candence_Out_500us;
uint16_t volatile Candence_Out_Value;

uint16_t volatile Candence_In_500us;
uint16_t volatile Candence_In_Value;

Idt_EB_APP_T EB_APP_ST;


void e_bike_app_init(void)
{


}


