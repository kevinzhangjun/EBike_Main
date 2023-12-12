/******************************************************************
**	  E_Bike
**	  Gpio
**
**    Versiomn: V1.0
**	  Author  : Kevin Zhang
********************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "Gpio.h"

Key_Info key_info;
uint16_t s2_scan;
uint16_t s2_scan_ll=KEY_GPIO_PIN;
uint16_t s2_ap;
uint16_t s2_ap_ll;
uint16_t s2_Rising;
uint16_t s2_Falling;
uint16_t s2_8ms;

Speed_ST Speed_Info;

void GPIO_Init(void)
{
	PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0, g_pin_mux_InitConfigArr0);
	Key_Scan();
	key_info.Mode_State = mode_init;
	Lamp_Front_Dis();
	Lamp_Rear_Dis();
	EXTI_Configuration();

}

/* LED App */
void LED_Red_On(void)
{

	PINS_DRV_ClearPins(LED_RED_GPIO, LED_RED_PIN);
}

void LED_Red_Off(void)
{

	PINS_DRV_SetPins(LED_RED_GPIO, LED_RED_PIN);
}

void LED_Red_Toggle(void)
{

	PINS_DRV_TogglePins(LED_RED_GPIO, LED_RED_PIN);
}

/* Power App */
void Power_Off(void)
{

	PINS_DRV_ClearPins(BVCC_CTL_GPIO, BVCC_CTL_PIN);
}

void Power_On(void)
{

	PINS_DRV_SetPins(BVCC_CTL_GPIO, BVCC_CTL_PIN);
}

void Power_Toggle(void)
{

	PINS_DRV_TogglePins(BVCC_CTL_GPIO, BVCC_CTL_PIN);
}

/* LAMP App */
void Lamp_Front_En(void)
{
	PINS_DRV_SetPins(LAMP_FRONT_GPIO, LAMP_FRONT_PIN);
}

void Lamp_Front_Dis(void)
{
	PINS_DRV_ClearPins(LAMP_FRONT_GPIO, LAMP_FRONT_PIN);
}

void Lamp_Rear_En(void)
{
	PINS_DRV_SetPins(LAMP_REAR_GPIO, LAMP_REAR_PIN);
}

void Lamp_Rear_Dis(void)
{
	PINS_DRV_ClearPins(LAMP_REAR_GPIO, LAMP_REAR_PIN);
}

/* KEY App */
void Key_Scan(void)
{
	s2_scan = (PINS_DRV_ReadPins(KEY_GPIO) & KEY_GPIO_PIN)|(PINS_DRV_ReadPins(BRAKE_GPIO) & BRAKE_GPIO_PIN);
	s2_ap |=s2_scan ^ s2_scan_ll;
	s2_scan_ll = s2_scan;
}

void Key_State_Result(void)
{
	s2_Falling |= (~s2_scan) & s2_ap_ll & (~s2_ap);
	s2_Rising |= s2_scan & s2_ap_ll & (~s2_ap);

	s2_ap_ll = s2_ap;
	s2_ap = 0;

	key_info.Cnt_64ms++;
}

void Key_Routine(void)
{

	if(s2_Falling & KEY_GPIO_PIN)
	{
		s2_Falling &= (~KEY_GPIO_PIN);

		if(s2_8ms < S2_Double_Cnt && key_info.Mode_State == mode1_Auto)
		{
			key_info.Mode_State = mode3_Manual_Adjust;

		}
		else if(key_info.Mode_State > mode_init)
		{
			key_info.Mode_State++;
			key_info.Cnt_64ms =0;
			s2_8ms =0;
			if(key_info.Mode_State == mode3_Manual_Adjust || key_info.Mode_State == end_mode)
			{
				key_info.Mode_State = mode1_Auto;
				MT_State = MT_Idle;
			}
			else
			{
				//nothing to do
			}
		}

	}
	else if(s2_Rising & KEY_GPIO_PIN)
	{
		s2_Rising &= (~KEY_GPIO_PIN);

		key_info.Cnt_64ms =0;
		if(key_info.Mode_State	== mode4_Manual_Run)
		{
			if(MT_State == MT_Run)
			{
				MT_State = MT_Stop;
				key_info.Mode_State	= mode5_Manual_Stop;
			}
		}

		else if(key_info.Mode_State == mode3_Manual_Adjust)
		{
			MT_State = MT_Debug;
		}
		else
		{
			//nothing to do
		}
	}
	else if((s2_scan & KEY_GPIO_PIN) == 0)
	{
		if(key_info.Mode_State < mode3_Manual_Adjust)
		{
			if(key_info.Cnt_64ms > Pwr_Off_Time)
			{
#if(No_Debug)
				while(1)
				{
					Power_Off();
				};
#endif
			}
			else if(key_info.Cnt_64ms > Pwr_On_Time)
			{

				Power_On();
				key_info.Mode_State = mode1_Auto;
			}
		}
		else if(MT_State == MT_Debug)
		{
			MT_State = MT_Start;

//test
			  MT_State = MT_Run;
		}
		else
		{
			//nothing to do
		}
	}
	else if((s2_scan & KEY_GPIO_PIN) != 0)
	{
		if(key_info.Mode_State < mode3_Manual_Adjust)
		{
			key_info.Cnt_64ms =0;
		}
		else
		{
			//nothing to do
		}
	}
	else
	{
		//nothing to do
	}
}

void Brake_Routine(void)
{

	if(s2_Falling & BRAKE_GPIO_PIN)
	{
		s2_Falling &= (~BRAKE_GPIO_PIN);

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

/*******************************************************************************
*
* Function: 	void SPEED_ISR(void)
*
*******************************************************************************/
void CADENCE_IN_ISR( void )
{
	uint32_t intStatus;
	/* check interrupt flags PTD16 and 15 */
	intStatus = PINS_DRV_GetPortIntFlag( CADENCE_IN_PORT );

	if(intStatus & CADENCE_IN_GPIO_PIN )
	{

		PINS_DRV_ClearPinIntFlagCmd( CADENCE_IN_PORT, CADENCE_IN_ID);

		if(Speed_Info.Cdn_In_Cnt_10us !=0)
		{
			Speed_Info.Cdn_In_Value = Speed_Info.Cdn_In_Cnt_10us;

			Speed_Info.Cdn_In_Cnt_10us = 0;

			if(Speed_Info.Cdn_In_Dir_10us < CADENCE_IN_DIR_THRESHOLD)
			{
				Speed_Info.Cdn_In_Dir = CDN_IN_BACKWARD;
			}
			else
			{
				Speed_Info.Cdn_In_Dir = CDN_IN_FORWARD;
			}

			Speed_Info.Cdn_In_Dir_10us = 0;
		}
		else
		{

		}
	}
	else
	{

	}
}

/********************************************************************************************
*Function Name: void EXTI_Configuration(void)
*
*Input parameter: None
*Output parameter: None
*
*
*******************************************************************************************/

void EXTI_Configuration(void)
{
	PINS_DRV_SetPinIntSel(CADENCE_IN_PORT, CADENCE_IN_ID, PORT_INT_FALLING_EDGE);
	INT_SYS_SetPriority(CADENCE_IN_PORT_IRQn,1);
    INT_SYS_InstallHandler(CADENCE_IN_PORT_IRQn, &CADENCE_IN_ISR, NULL);
    INT_SYS_EnableIRQ(CADENCE_IN_PORT_IRQn);
}

