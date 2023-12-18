/*

 * motor_control.c
 *
 *  Created on: Oct 13, 2023
 *      Author: PK6300
 */

#include "motor_control.h"
#include "S32K142.h"

#define ONE_Q14     16384
#define ONE_OVER_ROOT3  18918   // 1/sqrt(3) Q14
#define ONE_THIRD 10923   // 1/3 q15
#define TWO_THIRDS 21845  // 2/3 q15
#define SQRT3_OVER2 28378  // sqrt(3)/2  Q15
#define SQRT3_OVER3 18918  // sqrt(3)/3  Q15
//#define SPEED_FILT_GAIN 819 // 0.05 Q14
//#define SPEED_FILT_GAIN 1638 // 0.1 Q14
#define SPEED_FILT_GAIN 3276 // 0.2 Q14
#define iABS(N) ((N<0)?(-N):(N))
#define icos(x) (isin(x+16384))

Motor M1;
Motor M2;

int32_t M1A;
int32_t M1B;

static int16_t hall_to_theta[8] = { 0,  27307, 5461, 16384, -16384, -27307, -5461, 0 };

// Max current on NXP is 120A so lets use 2048 counts per 128A = about 16 counts per Amp.
// Software include a x4 prior to Clarke and Park, so 64 counts per amp in our command.
// A reference of 512 would be about 4A peak.

void Init_Motor_Control() {
	M1.I_gain = 312;
	M1.P_gain = 24000;              // ~4-5ms current response at 300Hz open loop (position)
	M1.CC_gain = 100;

	M1.Speed_P = 2500;
	M1.Speed_I = 10;

//	M1.I_gain = 156;				// About 10ms response
//	M1.P_gain = 12000;
//	M1.CC_gain = 50;
    // avg ADC offset is 2108
	M1.I_A_offset = 2108;
	M1.I_B_offset = 2108;
	M1.I_C_offset = 2108;

	M1.Id_ref = 0;
	M1.Iq_ref = 0;  // ~ 64 per A - on NXP
	M1.Speed_ref = 0;

	M1.hall_count = 0;
	M1.state = 0;
	M1.theta = 0;
	M1.d_theta = 0;

	M1.d_int = 0;
	M1.q_int = 0;
	M1.s_int = 0;

	M1.V_alpha = 0;
	M1.V_beta = 0;

	M1.freq_filt = 0;

	M1.pwm_a = PWM_OFFSET;
	M1.pwm_b = PWM_OFFSET;
	M1.pwm_c = PWM_OFFSET;

	M1.enabled = false;

	M2 = M1;
	M1.old_hall_theta = (int16_t)(hall_to_theta[M1.hall_input]);
	M2.old_hall_theta = (int16_t)(hall_to_theta[M2.hall_input]);
}

// to be called every 2ms or so
void MC_do_speed_control(Motor* M)
{
#define SP_SHIFT 10
	int16_t speed_err = M->Speed_ref - (M->d_theta);
	// Integrator with limits
	M->s_int += (int32_t)speed_err * M->Speed_I;
	if (M->s_int > (MAX_CURRENT_CMD << SP_SHIFT))
		M->s_int = (MAX_CURRENT_CMD << SP_SHIFT);
	if (M->s_int < -(MAX_CURRENT_CMD << SP_SHIFT))
		M->s_int = -(MAX_CURRENT_CMD << SP_SHIFT);

	// add the proportional term and limit
	int32_t curr = M->s_int + (int32_t)speed_err * M->Speed_P;
	if (curr > (MAX_CURRENT_CMD << SP_SHIFT))
		curr = (MAX_CURRENT_CMD << SP_SHIFT);
	if (curr < -(MAX_CURRENT_CMD << SP_SHIFT))
		curr = -(MAX_CURRENT_CMD << SP_SHIFT);

	int16_t output = curr >> SP_SHIFT;


	// Anti regen - enable this if you don't have a bidirectional power supply
	if ((M->s_int ^ M->d_theta) < 0)
		M->s_int = 0;
	if ((output ^ (M->d_theta)) < 0)
		output = 0;

	// set the current reference
	M->Iq_ref = output;
}

void MC_Set_Speed(Motor* M, int16_t rpm)
{
	rpm = ((rpm + 200)/500) * 500;
	M->Speed_ref = ((int32_t)rpm * RPM_TO_SPEED) >> 14;
}

#define POINT225 14746  /* 0.225 q16 */
/* takes a Q16 position in revolutions. returns sin(x) Q14 */
int16_t isin(int16_t x)
{
    int16_t a,y;
    int32_t p;
    /* first approximation */
    p = ((int32_t)x * x);
    if (x<0) p = ((int32_t)x << 15) + p;
    else p = ((int32_t)x << 15) - p;
    a = (int16_t)((p+8192) >> 14); //rounding here helps
    /* refine that approximation */
    y = (int16_t)(((int32_t)a * iABS(a)) >> 14) - a;
    y = ((int16_t)(((int32_t)y * POINT225) >> 16)) + a;
    return y;
}

/* recip_sqrt - computes 1/sqrt(x). x is q14 output is q14 */
int16_t recip_sqrt(uint16_t x)
// uses iteration: y' = y(1.5-x*y*y/2)
{
    uint16_t y,p1,p2;
    /* rather than an initial guess we use a linear approximation */
    /* y = 1.062281 - 0.15509x is good when 2 iterations are used */
    /* 4 digits accuracy from 1.1 to 3.3 */

    y = 17404 - (uint16_t)(((uint32_t)x * 10164)>>16);
    // iteration 2
    p1 = ((uint32_t)y * y)>>14;
    p2 = ((uint32_t)y * x)>>15;
    y += y>>1;
    y -= ((uint32_t)p1 * p2)>>14;

    /* y = 1.221752 - 0.26618x is good when 1 iteration is used */
    /* 4 digits accuracy from 1.3 to 1.8  (perfect at 1.32 & 1.75 */
//    y = 20017 - (uint16_t)(((uint32_t)x * 17444)>>16);
    // iteration 3
    p1 = ((uint32_t)y * y)>>14;
    p2 = ((uint32_t)y * x)>>15;
    y += y>>1;
    y -= ((uint32_t)p1 * p2 + 8192)>>14;
    return y;
}

#define MAG_LIMIT 18918      // 2/sqrt(3) q14
#define MAG32 ((MAG_LIMIT*2) << 12)
void limit_magnitude(int32_t *x, int32_t *y)
{
	while((*x > MAG32) || (*x < -MAG32) || (*y > MAG32) || (*y < -MAG32)) {
		*x = *x >> 1;
		*y = *y >> 1;
	}
	// compute magnitude squared q28
	uint32_t mag2 = (*x >> 12) * (*x >> 12) + (*y >> 12) * (*y >> 12);
	if (mag2 > (MAG_LIMIT * MAG_LIMIT))
	{
		int16_t correction = recip_sqrt(mag2 >> 14);
		correction = ((int32_t)correction * MAG_LIMIT) >> 14;    // we don't want to normalize the vector to 1.0
		*x = ((*x >> 12) * correction) >> 2;
		*y = ((*y >> 12) * correction) >> 2;
	}

}

//#define POSITION_OFFSET (10128+5929) //(-5461)
#define POSITION_OFFSET (5461)

#define MAX_HALL_COUNT 2000
#define HALL_WINDOW 5461   // 30 degrees +/-
//#define HALL_WINDOW 6371 // 35 degrees

void MC_do_position(Motor* M)
{
	int16_t hall_theta = (int16_t)(hall_to_theta[M->hall_input]);
	if (hall_theta == 0)
		hall_theta = M->old_hall_theta;

	int16_t hall_delta = hall_theta - M->old_hall_theta;
	M->hall_count++;

	if (hall_delta != 0) {
		if (iABS(M->d_theta) < 48) {  // low speed about 15 Hz
			M->theta = M->old_hall_theta + (hall_delta/2);
		}
		int16_t speed = hall_delta / M->hall_count;
		LPF16(speed, SPEED_FILT_GAIN, M->freq_filt);
		M->d_theta = speed;
		M->hall_count = 0;
	}
	M->old_hall_theta = hall_theta;

	if(M->hall_count > MAX_HALL_COUNT)
	{
		M->hall_count = MAX_HALL_COUNT;
		M->d_theta = 0;
		M->freq_filt = 0;
	}

	if (iABS(M->d_theta) < 48) 	// this is about 15Hz electrical
	{
		if (M->hall_count > 40) // leave it between hall angles for a bit after an edge
		{
			M->theta = hall_theta;  // then use the new angle
		}
	}
	else
	{   // at higher speed lets extrapolate
		int16_t theta = M->theta + M->d_theta;
		if (M->d_theta > 0) {
			if ((theta - hall_theta) > HALL_WINDOW) theta -= (M->d_theta) >> 1; // slow down we overran
			if ((hall_theta - theta) > HALL_WINDOW) theta += (M->d_theta) >> 1; // catch up
		} else {
			if ((theta - hall_theta) > HALL_WINDOW) theta += (M->d_theta) >> 1; // catch up
			if ((hall_theta - theta) > HALL_WINDOW) theta -= (M->d_theta) >> 1; // slow down we overran
		}
		M->theta = theta;
	}

}

void MC_do_current_control(Motor* M)
{
	MC_do_position(M);
	int16_t offset = POSITION_OFFSET;
	int16_t sin = isin(M->theta + offset);
	int16_t cos = icos(M->theta + offset);

	int16_t I_a = (M->I_A - M->I_A_offset)*4;   // approximate range = 2048*4 = 8192 so about +/-1  q13  let's call this current q13
	int16_t I_b = (M->I_B - M->I_B_offset)*4;   // and we will never command more than +/-1 or maybe 0.8 which is about 6400 decimal
	int16_t I_c = -I_a - I_b;

	I_a = -I_a;
	I_b = -I_b;
	I_c = -I_c;

	int16_t I_alpha = ((int32_t)I_a * TWO_THIRDS - (int32_t)I_b * ONE_THIRD - (int32_t)I_c * ONE_THIRD) >> 15;
	int16_t I_beta = ((int32_t)I_b * SQRT3_OVER3 - (int32_t)I_c * SQRT3_OVER3) >> 15;

	int16_t I_d = (((int32_t)I_alpha * cos) + ((int32_t)I_beta * sin)) >> 14;
	int16_t I_q = (((int32_t)I_beta * cos) - ((int32_t)I_alpha * sin)) >> 14;

	int16_t Id_err = M->Id_ref - I_d;
	int16_t Iq_err = M->Iq_ref - I_q;

	// Integrator gains
	M->d_int += (int32_t)Id_err * M->I_gain;
	M->q_int += (int32_t)Iq_err * M->I_gain;
	// Cross couple the integrators
	int16_t ccd = ((int32_t)(M->CC_gain) * M->d_theta) >> 10;
	M->d_int -= (int32_t)Iq_err * ccd;
	M->q_int += (int32_t)Id_err * ccd;
	limit_magnitude(&(M->d_int), &(M->q_int));

	int32_t vd_out = ((int32_t)Id_err * M->P_gain) + M->d_int;
	int32_t vq_out = ((int32_t)Iq_err * M->P_gain) + M->q_int;

	limit_magnitude(&vd_out, &vq_out);

	// at this point we have 16bit vd,vq which are Q14
	int16_t vd = vd_out >> 12;
	int16_t vq = vq_out >> 12;

	/******************************************************
	 * Voltage output section. Convert vd,vq to pwm a,b,c *
	 *****************************************************/
	int16_t a,b,c, temp;
	//  We will need Valpha, Vbeta for sensorless operation to track BEMF later
	M->V_alpha = (((int32_t)vd * cos) - ((int32_t)vq * sin)) >> 14;
	M->V_beta = (((int32_t)vd * sin) + ((int32_t)vq * cos)) >> 14;
	a = M->V_alpha;
    b = ((int32_t)(M->V_beta) * SQRT3_OVER2) >> 15;
    b = b - (a >> 1);
    c = -a - b;

    // We do a fast version of SVM with this. temp=2*a*b*c where "2" depend on the magnitude limit above
    temp = (int16_t)(((int32_t)a * b) >> 15);
    temp = (int16_t)(((int32_t)c * temp) >> 14);
    a -= temp;
    b -= temp;
    c -= temp;

    // clip the phase voltages to +/-1
    // if vd,vq is correctly limited, this may be redundant
    if (a > ONE_Q14)
        a = ONE_Q14;
    else if (a < -ONE_Q14)
        a = -ONE_Q14;

    if (b > ONE_Q14)
        b = ONE_Q14;
    else if (b < -ONE_Q14)
        b = -ONE_Q14;

    if (c > ONE_Q14)
        c = ONE_Q14;
    else if (c < -ONE_Q14)
    	c = -ONE_Q14;

    // Save our output duty cycles - values for hardware
    M->pwm_a = PWM_OFFSET - (a * (PWM_RANGE/2) >> 14);
    M->pwm_b = PWM_OFFSET - (b * (PWM_RANGE/2) >> 14);
    M->pwm_c = PWM_OFFSET - (c * (PWM_RANGE/2) >> 14);
}

void pause(void) {
	volatile int x;
	for (x=0; x<8; x++);
}

void MC_enable_PWM(void)
{
	M1.d_int = 0;
	M1.q_int = 0;
	M1.s_int = 0;
	M2.d_int = 0;
	M2.q_int = 0;
	M2.s_int = 0;

	M1.hall_count = 0;
	M1.freq_filt = 0;
	M2.hall_count = 0;
	M2.freq_filt = 0;

	M1.enabled = true;
    M2.enabled = true;

    PTD->PSOR = 1<<7;   // ~Reset GD high

}
void MC_disable_PWM(void)
{
    PTD->PCOR = 1<<7;    // clear ~GD reset
    M1.enabled = false;
    M2.enabled = false;

    FTM3->CONTROLS[6].CnV = PWM_OFFSET; /* FTM ch6 compare value (~50% duty cycle) */
    FTM3->CONTROLS[2].CnV = PWM_OFFSET; /* FTM ch2 compare value (~50% duty cycle) */
    FTM3->CONTROLS[4].CnV = PWM_OFFSET; /* FTM ch4 compare value (~50% duty cycle) */

    FTM0->CONTROLS[6].CnV = PWM_OFFSET; /* FTM ch6 compare value (~50% duty cycle) */
    FTM0->CONTROLS[2].CnV = PWM_OFFSET; /* FTM ch2 compare value (~50% duty cycle) */
    FTM0->CONTROLS[4].CnV = PWM_OFFSET; /* FTM ch4 compare value (~50% duty cycle) */
}


