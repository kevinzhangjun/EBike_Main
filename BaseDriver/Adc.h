#ifndef _ADC_H_
#define _ADC_H_ 

#include "sdk_project_config.h"


/* Uses ADC PAL config 0 - configured to measure ADC0 InputChannel 28, connected to the potentiometer. */
#define ADC0_INSTANCE    0UL


//#define ADC_CHN         ADC_INPUTCHAN_EXT14
#define ADC_VREFH       5.06f
#define ADC_VREFL       0.0f
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
	Shift_Ch,
	Throttle_Ch,
	MOS1_Temp_Ch,
	MOS2_Temp_Ch,
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
	 bool			ADC_4ms_Flg;
 	 float			V36_Value;
 	 SHIFT_ID_T		Shift_Value;
 	 float			Throttle_Value;
	 float			MOS1_Temp_Value;
	 float			MOS2_Temp_Value;
	 uint16_t		ADC_MAX;
	 float 			avgVolts[ADC_end];

}Idt_ADC_T;


void Init_ADC(void);
void ADC_Start_Group(uint8_t selectedGroupIndex);

extern Idt_ADC_T ADC_ST;


#endif
