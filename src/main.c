/******************************************************************
**	  E_Bike
**	  Main
**
**    Versiomn: V1.0
**	  Author  : Kevin Zhang
********************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "sdk_project_config.h"
#include "bike_control.h"
#include "blackboard.h"

struct blackboard bb;

/*!
  \brief The main function for the project.
  \details The startup initialization sequence is the following:
 * - __start (startup asm routine)
 * - main()
*/

#define SYS_STATE_INIT        0
#define SYS_STATE_NORMAL      1
#define SYS_STATE_UART_TEST   2
#define SYS_STATE_ALPHA1_TEST 3

int main(void)
{

	/*delay 200ms, wait hardware stable*/
//	OSIF_TimeDelay(200);

    CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
                   g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

    GPIO_Init();
    Timer_Init();
    Init_Uart();
    peripheral_init();
    Init_Motor_Control();
    MC_disable_PWM();

    int sys_state = SYS_STATE_INIT;

    while(1)
    {
    	//base routine call
    	Key_Routine();
    	Brake_Routine();
    	Get_Speed_Value();
    	Get_Cdn_In_Value();
    	ADC0_GetResult();
    	HMI_Handler();

    	switch (sys_state)
    	{
    	case SYS_STATE_INIT:
    		// currently there is nothing to do here
			sys_state = SYS_STATE_NORMAL;
    		break;

    	case SYS_STATE_NORMAL:
			do_bike_control();
    		break;

    	case SYS_STATE_UART_TEST:
			break;

    	case SYS_STATE_ALPHA1_TEST:
			motor_test();
    		break;

    	default:
    		sys_state = SYS_STATE_INIT;
    	}
    }

}

