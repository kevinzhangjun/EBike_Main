/*
 * blackboard.h
 *
 *  Created on: Jan 9, 2024
 *      Author: PK6300
 */

#ifndef BLACKBOARD_H_
#define BLACKBOARD_H_
#include <stdbool.h>

/**************************************
* Defines the items on our blackboard *
**************************************/

struct blackboard {
	float v_batt;
	float cadence_rpm;
	float wheel_rpm;
	int shift_motor_rpm;
	int shift_motor_Nm_q8;  // Torque is Nm * 256
	int assist_motor_rpm;
	int assist_motor_Nm_q8; // Torque is Nm * 256
	int throttle;
	bool brake_pressed;
	bool manual_shifting;
	bool key_on;
	int temp_shift_motor;
	int temp_assist_motor;

};

extern struct blackboard bb;

#endif /* BLACKBOARD_H_ */
