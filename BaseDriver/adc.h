/*
 * adc.h
 *
 *  Created on: Oct 11, 2023
 *      Author: PK6300
 */

#ifndef ADC_H_
#define ADC_H_

#include "sdk_project_config.h"

#define NUM_ADC 9
#define convertAdcChan(adc, adcChan) { adc->SC1[0] &= ~ADC_SC1_ADCH_MASK; adc->SC1[0] = ADC_SC1_ADCH(adcChan); }
#define adc_complete(adc) ((adc->SC1[0] & ADC_SC1_COCO_MASK)>>ADC_SC1_COCO_SHIFT)
#define read_adc_ch(adc, chan) (adc->R[chan])

//#define ADC_CHN         ADC_INPUTCHAN_EXT14
#define ADC_VREFH       5.00f
#define ADC_VREFL       0.0f
#define	ADC_MAX			(1<<12)
#define Bat_Volt_Factor	13
#define Shift_V1		1
#define Shift_V2		4

#define Temp_PullRes	5100
#define Temp_150		372
#define Temp_150_Valve	Temp_150*ADC_VREFH/(Temp_150+Temp_PullRes)

#define Temp_140		470
#define Temp_130		602
#define Temp_120		779
#define Temp_110		1021
#define Temp_100		1357

typedef enum
{
	Volt_36V_Ch,
	M2_Y_Ch,
	Shift_Ch,
	Throttle_Ch,
	M1_Y_Ch,
	MT1_Temp_Ch,
	MT2_Temp_Ch,
	MOS2_Temp_Ch,
	MOS1_Temp_Ch,
	ADC_end
}CH_ID_T;

typedef enum
{
	Shift_Idle,
	Shift_Up,
	Shift_Down,
	SHIFT_end
}SHIFT_ID_T;

typedef struct
{
	 float			V36_Value;
	 float			M2_Y_Volt;
	 SHIFT_ID_T		Shift_Value;
	 float			Throttle_Value;
	 float			M1_Y_Volt;
	 float			MT1_Temp_Value;
	 float			MT2_Temp_Value;
	 float			MOS2_Temp_Value;
	 float			MOS1_Temp_Value;
	 bool			ADC_4ms_Flg;
	 float 			avgVolts[ADC_end];

}Idt_ADC_T;


void ADC_init(int pcc_adc_index, ADC_Type *adc);

extern uint16_t ADC_results[NUM_ADC];
extern uint16_t ADC_filtered[NUM_ADC];
extern int32_t ADC_filter_state[NUM_ADC];

extern Idt_ADC_T ADC_ST;
#endif /* ADC_H_ */
