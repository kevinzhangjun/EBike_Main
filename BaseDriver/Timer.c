/******************************************************************
**	  E_Bike
**	  Timer
**
**    Versiomn: V1.0
**	  Author  : Kevin Zhang
********************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "Timer.h"

volatile uint32_t timer_10us;
volatile uint8_t  timer_500us;
volatile uint32_t LED_4ms;


  
void Time_1ms_Tasks(void);
void Time_2ms_Tasks(void);
void Time_4ms_Tasks(void);
void Time_8ms_Tasks(void);
void Time_64ms_Tasks(void);

/********************************************************************************************
*Function Name: void TimCounterInit(void)
*
*Input parameter: None
*Output parameter: None
*
*TIMCLK = 72 MHz, Prescaler = TIM_PSC, TIM counter clock = TIMCLK/TIM_COUNTER MHz
*******************************************************************************************/

void Timer_Init(void)
{
    /* Write your local variable definition here */
    uint64_t lpitResolution;
    uint64_t lptmrResolution;
	status_t status;


    /* Initialize TIMING over LPIT */
    status = TIMING_Init(&timingLpitInst0, &timingLpit_InitConfig0);
    DEV_ASSERT(status == STATUS_SUCCESS);
    /* Initialize TIMING over LPTMR */
    status = TIMING_Init(&timingLptmrInst0, &timingLptmr_InitConfig0);
    DEV_ASSERT(status == STATUS_SUCCESS);

    /* Get tick resolution in nanosecond unit for TIMING over LPIT */
    status = TIMING_GetResolution(&timingLpitInst0, TIMER_RESOLUTION_TYPE_NANOSECOND, &lpitResolution);
    DEV_ASSERT(status == STATUS_SUCCESS);
    /* Get tick resolution in nanosecond unit for TIMING over LPTMR */
    status = TIMING_GetResolution(&timingLptmrInst0, TIMER_RESOLUTION_TYPE_NANOSECOND, &lptmrResolution);
    DEV_ASSERT(status == STATUS_SUCCESS);


    INT_SYS_SetPriority(LPIT0_Ch0_IRQn,1);
    /* Start LPIT channel 0 counting with the period is 1 second,
       the period in tick = the period in nanosecond / LPIT tick resolution in nanosecond */
    TIMING_StartChannel(&timingLpitInst0, LPIT_CHANNEL, LPITMR_PERIOD_10us/lpitResolution);

    INT_SYS_SetPriority(LPTMR0_IRQn,3);
    /* Start LPTMR channel 0 counting with the period is 1 second,
       the period in tick = the period in nanosecond / LPTMR tick resolution nanosecond*/
    TIMING_StartChannel(&timingLptmrInst0, LPTMR_CHANNEL, LPTMR_PERIOD_500us/lptmrResolution);



}


/********************************************************************************************
*Function Name: void LPIT_IRQ(void)
*
*Input parameter: None
*Output parameter: None
*
*TIM Handler
*******************************************************************************************/
void LPIT_IRQ(void * userData)
{
    (void)userData;

    timer_10us++;
    Speed_Info.Cdn_In_Cnt_10us++;
    if((PINS_DRV_ReadPins(CADENCE_IN_GPIO) & CADENCE_IN_GPIO_PIN) == 0)
    {
    	Speed_Info.Cdn_In_Dir_10us++;
    }

}

/********************************************************************************************
*Function Name: void LPTMR_IRQ(void)
*
*Input parameter: None
*Output parameter: None
*
*TIM Handler
*******************************************************************************************/
void LPTMR_IRQ(void * userData)
{
    (void)userData;

	timer_500us++;
			
    if((timer_500us & 0x01) == 0)
    {
		Time_1ms_Tasks();

		if((timer_500us & 0x03) == 0)
		{
			Time_2ms_Tasks();

			if((timer_500us & 0x07) == 0)
			{
				Time_4ms_Tasks();

				if((timer_500us & 0x0F) == 0)
				{
					Time_8ms_Tasks();

					if((timer_500us & 0x7F) == 0)
					{
						Time_64ms_Tasks();
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
	else
	{
		//nothing to do
	}
}

void Time_1ms_Tasks(void)
{

}

void Time_2ms_Tasks(void)
{
	tr0_2ms++;
	tx0_2ms++;
}

void Time_4ms_Tasks(void)
{

	LED_4ms++;

	if(key_info.Mode_State == mode3_Manual_Adjust || key_info.Mode_State == mode5_Manual_Stop)
	{
		if(LED_4ms >= 80)
		{
			LED_4ms = 0;

			LED_Red_Toggle();

		}
	}
	else if(key_info.Mode_State == mode4_Manual_Run)
	{
		if(LED_4ms >= 40)
		{
			LED_4ms = 0;

			LED_Red_Toggle();

		}
	}
	else if(LED_4ms >= 125 && Speed_Info.Cdn_In_Dir == CDN_IN_FORWARD)
	{
		LED_4ms = 0;
    	/* Toggle LED_RED */
		LED_Red_Toggle();
	}
	else if(LED_4ms >= 50 && Speed_Info.Cdn_In_Dir == CDN_IN_BACKWARD)
	{
		LED_4ms = 0;
    	/* Toggle LED_RED */
		LED_Red_Toggle();
	}
	else
	{
			//nothing to do
	}
}

void Time_8ms_Tasks(void)
{ 
	Key_Scan();
	s2_8ms++;

}

void Time_64ms_Tasks(void)
{
	Key_State_Result();
}

