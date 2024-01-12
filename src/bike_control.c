/*
 * bike_control.c
 *
 *  Created on: Jan 8, 2024
 *      Author: PK6300
 */

#include <stdbool.h>
#include "motor_control.h"
#include "adc.h"

/*******************************************
* functions contributing to motor commands *
*******************************************/
#define KSHIFT (-48.0f)

float fixed_gear_shift_motor_speed(void)
{ // calculate the shift motor speed given cadence and gear ratio
	float ratio = 0.5f; // for now we use a single fixed ratio
	float w_in = 0.0f;  // we need to read cadence here

	float w_shift = KSHIFT * w_in * ( ratio + ((1.0f / KSHIFT) - 1.0f) );
	return w_shift;
}

float cvt_shift_motor_speed(void)
{ // calculate shift motor speed for the desired cadence
	float w_set = 0.0f;    // we need to read desired cadence here
	float w_out = 0.0f;    // this needs to be gearbox output speed derived from bike speed

	float ratio = w_out / w_set;  // this assumes we never allow a desired cadence of 0.0f

	float w_shift = KSHIFT * w_set * ( ratio + ((1.0f / KSHIFT) - 1.0f) );
	return w_shift;
}

float power_conservation_torque(void)
{ // calculate the assist torque to compensate the shift motor for net zero power
	return 0.0f;
}

float pedal_assist_torque(void)
{
	return 0.0f;
}

float walk_button_torque(void)
{
	return 0.0f;
}

void do_bike_control(void)
{
	// determine which functionality should be enabled at this time:
	// or maybe the logic is in the individual functions.
	bool cvt_enabled = false;

	// combine all enabled functionality into a pair of motor commands:
	float shift_speed = 0.0f;    // shift motor speed
	float assist_torque = 0.0f;  // assist motor torque

	shift_speed = fixed_gear_shift_motor_speed();

	if (cvt_enabled) {
		float cvt_speed = cvt_shift_motor_speed();
		if (cvt_speed < shift_speed)
		{
			shift_speed = cvt_speed;
		}
	}

	assist_torque = power_conservation_torque();
	assist_torque += pedal_assist_torque();
	assist_torque += walk_button_torque();

	// we never want to apply reverse assist torque (with the clutch)
	if (assist_torque < 0.0f){
		assist_torque = 0.0f;
	}

	MC_Set_Speed( SHIFT_MOTOR, (int16_t)shift_speed);
	MC_Set_Torque( ASSIST_MOTOR, (int16_t)(assist_torque * 256) );
}

// This is just for testing the alpha level PCB
void motor_test() {
	int16_t rpm = ADC_filtered[5];
//	int16_t pot2 = ADC_filtered[6];

	rpm = ((rpm + 200)/500) * 500;

	if (rpm > 300) {
		MC_Set_Speed( SHIFT_MOTOR, rpm );
//	MC_Set_Speed( ASSIST_MOTOR, pot2*3/4 );
	} else {
		MC_disable_PWM();
	}
}

