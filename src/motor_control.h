/*

* motor_control.h
 *
 *  Created on: Oct 13, 2023
 *      Author: PK6300
 */

#ifndef MOTOR_CONTROL_H_
#define MOTOR_CONTROL_H_

#include "S32K142.h"
#include "stdbool.h"

#define MC_DISABLED		0
#define MC_SPEED		1
#define MC_TORQUE		2

typedef struct Motor {
	// inputs to be collected prior to calling MC_do_control()
	int16_t I_A;  // phase A current reading
	int16_t I_B;  // phase B current reading
	int16_t I_C;  // currently unused
	int16_t I_A_offset;
	int16_t I_B_offset;
	int16_t I_C_offset;

	int16_t Iq_ref;
	int16_t Id_ref;
	int16_t Speed_ref;

	uint16_t state;

	int16_t Speed_P;
	int16_t Speed_I;

	int16_t P_gain;   // Proportional gain - current
	int16_t I_gain;   // Integral gail     - current
	int16_t CC_gain;  // Cross coupling gain (coupled integrators)

	int16_t d_theta;
	// internal data - some may be useful outside
	int16_t V_alpha;
	int16_t V_beta;
	uint16_t hall_input;
	uint16_t hall_count;

	int16_t theta;
	int16_t old_hall_theta;   // need to init to -1
	int32_t freq_filt;        // need to init to 0
	int32_t d_int;
	int32_t q_int;
	int32_t s_int;

	// outputs from MC_do_control() to be stuffed in hardware registers
	uint16_t pwm_a;  // duty cycle A
	uint16_t pwm_b;  // duty cycle B
	uint16_t pwm_c;  // duty cycle C

} Motor;

extern Motor M1;
extern Motor M2;

// application level motor funtions
void Init_Motor_Control(void);
void MC_Set_Speed(Motor* M, int16_t rpm);
void MC_Set_Torque(Motor* M, int16_t Nm_q8);

// use these to specify which motor
#define SHIFT_MOTOR &M1
#define ASSIST_MOTOR &M2

// strictly for display in the debugger
extern int32_t M1A;
extern int32_t M1B;

void MC_enable_PWM(void);
void MC_disable_PWM(void);
void MC_init_pwm(void);
void MC_do_current_control(Motor* M);
void MC_do_speed_control(Motor* M);


/* This filter takes 16 bit input, 16bit gain (Q14) and 32bit static variable for internal state */
/* the variable "in" is both the input and output - i.e. sample goes in and is updated to filtered value */
#define LPF16(in,gain,st) (in=(int16_t)((st = (st)-((int32_t)gain*((int16_t)((st)>>14)))+(int32_t)gain*in)>>14))

#define PWM_PERIOD 1200
#define MIN_PWM 400
#define MAX_PWM 1150
#define PWM_RANGE (MAX_PWM-MIN_PWM)
#define PWM_OFFSET ((MAX_PWM+MIN_PWM)/2)

#define MAX_CURRENT_CMD (1100)
#define RPM_TO_SPEED (65536 * 16384 / 200000)
#define TORQUE_TO_CURRENT (32 * 256)


#endif /* MOTOR_CONTROL_H_ */
