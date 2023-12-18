/******************************************************************
**	  E_Bike
**	  Main
**
**    Versiomn: V1.0
**	  Author  : Kevin Zhang
********************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "sdk_project_config.h"
#include "adc.h"

volatile int exit_code = 0;


void motor_test() {
	int16_t pot1 = ADC_filtered[5];
	int16_t pot2 = ADC_filtered[6];

	if (pot1 > 300) {
		MC_Set_Speed( SHIFT_MOTOR, pot1 );
//	MC_Set_Speed( ASSIST_MOTOR, pot2*3/4 );
	} else {
		MC_disable_PWM();
	}
}

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

    while(exit_code == 0)
    {
    	//base routine call
    	Key_Routine();
    	Brake_Routine();
    	Get_Cdn_In_Value();
    	motor_test();
    }

  return exit_code;

}

