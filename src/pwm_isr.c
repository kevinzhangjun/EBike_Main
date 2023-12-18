/*
 * pwm_isr.c
 *
 *  Created on: Oct 13, 2023
 *      Author: PK6300
 */

#include "S32K142.h"
#include "peripheral_init.h"
#include "adc.h"
#include "motor_control.h"

#define TIMING_PIN_HIGH  { PTD->PSOR = (1<<6); }
#define TIMING_PIN_LOW  { PTD->PCOR = (1<<6); }

// These are lists of ADC channels we need to read besides motor stuff.
int ADC0_channels[9] = { 0, 1, 2, 3, 6, 12, 7, 13, 14 };
//int ADC1_channels[8] = { 10, 0, 1, 2, 3, 4, 5, 6 };
int adc_reader = 0;

void FTM3_Ovf_Reload_IRQHandler(void) {
	TIMING_PIN_HIGH
	// Start conversion of phase A current on both motors
	convertAdcChan(ADC0, 9); // convert M1 I_A
	convertAdcChan(ADC1, 8); // convert M2 I_A

	// Clear our timer interrupt flag
	FTM3->SC &= 0x00FF0068;
	S32_NVIC->ICPR[3] = 1 << (122 & 31); /* IRQ104-FTM3 Reload interrupt: clr any pending IRQ*/

	// There might be time to do a little in here
	while(adc_complete(ADC0)==0){} /* Wait for conversion complete flag for A-phase currents */
	TIMING_PIN_LOW

	// Store the reading of phase A currents
	M1.I_A = read_adc_ch(ADC0, 0);
	M2.I_A = read_adc_ch(ADC1, 0);

	// Start conversion of phase B current on both motors
	convertAdcChan(ADC0, 8); // convert M1 I_B
	convertAdcChan(ADC1, 7); // convert M2 I_B

	// There might be time to do a little in here
	while(adc_complete(ADC0)==0){} /* Wait for conversion complete flag for B-phase currents */
	TIMING_PIN_HIGH
	// Store the reading of phase B currents
	M1.I_B = read_adc_ch(ADC0, 0);
	M2.I_B = read_adc_ch(ADC1, 0);

	// for display in the debugger
	M1A = M1.I_A;
	M1B = M1.I_B;

	// read HALL sensors for Motor 1
	M1.hall_input = (((PTD->PDIR) & 1) << 2)
				  | (((PTE->PDIR >> 11) & 1) << 1)
				  | (((PTE->PDIR >> 10) & 1) );

	// read HALL sensors for Motor 2  - Nice IO selection...
	M2.hall_input = ((PTA->PDIR >> 11) & 7);

	// We need to read a couple other channels every cycle
	convertAdcChan(ADC0, ADC0_channels[adc_reader]); // convert some channel on ADC0
//	convertAdcChan(ADC1, ADC1_channels[adc_reader]); // convert some channel on ADC1

	static uint16_t x = 0;
	x++;
	if (x == 20) {
		x = 0;
		if(M1.state == MC_SPEED)
			MC_do_speed_control(&M1);
		if(M2.state == MC_SPEED)
			MC_do_speed_control(&M2);
	}

	// here we do current control (inner loop - command comes from speed control)
	if (M1.state != MC_DISABLED) {
		MC_do_current_control(&M1);
		MC_do_current_control(&M2);
	} else {
		Init_Motor_Control();
	}

    // Write our duty cycles to the hardware registers
    FTM3->CONTROLS[6].CnV = M1.pwm_a;
    FTM3->CONTROLS[2].CnV = M1.pwm_b;
    FTM3->CONTROLS[4].CnV = M1.pwm_c;

    FTM0->CONTROLS[6].CnV = M2.pwm_a;
    FTM0->CONTROLS[2].CnV = M2.pwm_b;
    FTM0->CONTROLS[4].CnV = M2.pwm_c;

    //    FTM0->CONTROLS[6].CnV = 400;
    //    FTM0->CONTROLS[2].CnV = 400 + M1.I_A - M1.I_A_offset;
    //    FTM0->CONTROLS[4].CnV = 400 + M1.I_B - M1.I_B_offset;

	while(adc_complete(ADC0)==0){} /* Wait for conversion complete flag for other channels */
	// Store the results of the other 2 conversions
	int16_t adc_val0 = read_adc_ch(ADC0,0);
	ADC_results[adc_reader] = adc_val0;
//	ADC_results[adc_reader + 8] = read_adc_ch(ADC1, 0);
	ADC_filtered[adc_reader] = LPF16(adc_val0, 4000, ADC_filter_state[adc_reader]);
	adc_reader = (adc_reader + 1);
	if (adc_reader == NUM_ADC) adc_reader = 0;

	int16_t led_speed = M1.d_theta >> 5;
	if (M1.state == MC_DISABLED)
		led_speed = 3;
	// soft blink the LED
    { // do this every 50us
        static int16_t x;
        static int16_t a;
        int16_t y = 0x4000 - (((int32_t)x * x) >> 16);
        a = (a & 0x0fff) + (((int32_t)y * y) >> 16);
        PTE->PDOR = (a >> 12) ^ 1;   // LED output
//        x += (uint16_t)(M1.old_hall_theta) / 0x1555; // soft blink speed
        x += led_speed;
//        x += 3; // soft blink speed
    }
    TIMING_PIN_LOW
}
