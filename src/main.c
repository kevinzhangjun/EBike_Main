/******************************************************************
**	  E_Bike
**	  Main
**
**    Versiomn: V1.0
**	  Author  : Kevin Zhang
********************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "sdk_project_config.h"
#include "motor_control.h"
#include "peripheral_init.h"

volatile int exit_code = 0;

/*!
  \brief The main function for the project.
  \details The startup initialization sequence is the following:
 * - __start (startup asm routine)
 * - main()
*/
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

    while(1)
    {
    	//base routine call
    	Key_Routine();
    	Brake_Routine();
    }

    for(;;) {
    	if(exit_code != 0)
    	{
    		break;
    	}
    }
  return exit_code;

}

