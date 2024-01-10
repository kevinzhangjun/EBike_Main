/******************************************************************
**	  E_Bike
**	  application
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

void Brake_Routine(void)
{

	if(key_info.s2_Falling & BRAKE_GPIO_PIN)
	{
		key_info.s2_Falling &= (~BRAKE_GPIO_PIN);

		if(MT_State == MT_Run)
		{
			MT_State = MT_Stop;
			if(key_info.Mode_State	== mode4_Manual_Run)
			{
				key_info.Mode_State	= mode5_Manual_Stop;
			}

		}
	}
	else
	{
		//nothing to do
	}
}

void Get_Speed_Value(void)
{
	float temp, temp1;

	if(Speed_Info.Speed_Value != Speed_Info.Speed_Value_lst && Speed_Info.Speed_Value !=0)
	{
		Speed_Info.Speed_Value_lst = Speed_Info.Speed_Value;

		if(Speed_Info.Speed_Value < RPM_1_Period && Speed_Info.Speed_Value > RPM_480_Period)
		{
			temp = (float)Time_Factor;			//i hr 10us unit
			temp /= Speed_Info.Speed_Value;
			temp /= 1;					//1 rev time

//			temp1 = Config_Data[Whirl_Size]+2*Config_Data[Tire_Thick]/10;
			temp1 = Whirl_Size + Tire_Thick*2;
			temp1 *=PI;
			temp1 *= Inch_mm;

			temp1 *= temp;
			temp1 /= KM_M;

			Speed_Info.Speed_Per_Hr = temp1;

			Speed_Info.Speed_RPM = (float)SPEED_RPM_Factor;
			Speed_Info.Speed_RPM /= Speed_Info.Speed_Value;
		}
		else
		{
			Speed_Info.Speed_Per_Hr = 0;
		}
	}
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

void ADC0_GetResult(void)
{
	uint8_t idx;

	if(ADC_ST.ADC_4ms_Flg)
	{
		ADC_ST.ADC_4ms_Flg = false;

		for(idx = 0; idx <ADC_end; idx++)
		{
			/* Convert avg to volts */
			ADC_ST.avgVolts[idx] = ((float) ADC_results[idx] / ADC_MAX) * (ADC_VREFH - ADC_VREFL);
		}

		ADC_ST.V36_Value = ADC_ST.avgVolts[Volt_36V_Ch] * Bat_Volt_Factor;

		if(ADC_ST.avgVolts[Shift_Ch ] > Shift_V2)
		{
			ADC_ST.Shift_Value = Shift_Idle;
		}
		else if(ADC_ST.avgVolts[Shift_Ch ] > Shift_V1)
		{
			ADC_ST.Shift_Value = Shift_Up;
		}
		else
		{
			ADC_ST.Shift_Value = Shift_Down;
		}

		ADC_ST.Throttle_Value = ADC_ST.avgVolts[Throttle_Ch];
		ADC_ST.MOS1_Temp_Value = ADC_ST.avgVolts[MOS1_Temp_Ch];
		ADC_ST.MOS2_Temp_Value = ADC_ST.avgVolts[MOS2_Temp_Ch];

		if(ADC_ST.MOS1_Temp_Value < Temp_150_Valve || ADC_ST.MOS2_Temp_Value < Temp_150_Valve )
		{
			if(MT_State == MT_Run)
			{
				MT_State = MT_Stop;
				key_info.Mode_State	= mode5_Manual_Stop;
			}
		}
	}
}



