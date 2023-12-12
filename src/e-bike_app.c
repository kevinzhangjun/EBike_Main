/******************************************************************
**	  E_Bike
**	  Uart Interrupt
**
**    Versiomn: V1.0
**	  Author  : Kevin Zhang
********************************************************************/

/* Includes ------------------------------------------------------------------*/

#include "e-bike_app.h"


Motor_ID MT_State;
Idt_EB_APP_T EB_APP_ST;


void e_bike_app_init(void)
{


}

void Get_Cdn_In_Value(void)
{

	if(Speed_Info.Cdn_In_Value != Speed_Info.Cdn_In_Value_lst && Speed_Info.Cdn_In_Value !=0)
	{
		Speed_Info.Cdn_In_Value_lst = Speed_Info.Cdn_In_Value;

		if(Speed_Info.Cdn_In_Value < CDN_IN_RPM_1_Period && Speed_Info.Cdn_In_Value > CDN_IN_RPM_480_Period)
		{

			Speed_Info.Cdn_In_RPM = (float)CDN_IN_RPM_Factor;
			Speed_Info.Cdn_In_RPM /= Speed_Info.Cdn_In_Value;
		}
		else
		{
			Speed_Info.Cdn_In_RPM = 0;
		}
	}
}
