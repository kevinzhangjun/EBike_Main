#ifndef __e_bike_app
#define __e_bike_app

#include "sdk_project_config.h"

#define No_Debug			1

#define Revolution			20
#define Inch_mm 			25.4
#define Mile_M				1600000
#define PI					3.14159
#define Time_Factor			3600*1000*
#define RPM_1_Period		6000
#define RPM_120_Period		50

#define CMD_Len	sizeof(float)*9+8
	




typedef enum
{
	MT_Init,
	MT_Idle,
	MT_Debug,
	MT_Start,
	MT_Run,
	MT_Stop,
	MT_Data_Monitor,
	MT_Check_Open,
	MT_Check_Short,
	Reserve
}Motor_ID;

typedef struct
{
	 uint8_t		Assistant_Mode;	
	 uint8_t		Asistance_Level;	// 1-9 default 9
	 uint8_t		Amplitude;				// 11-127
	 uint8_t		Brightness;	
	 uint8_t		Auto_Off;					//1-9min default 5min
	 uint8_t 		Speed_Limit;	 
	 uint8_t 		Error_Code;	
	 uint8_t 		Version;	
	 uint8_t 		Light_Switch;			//'0'=off, '1'=On
	 uint8_t 		Walk_mode;				//'0'=off, '1'=On
	
	 float			Total_Speed_Pulses;
	 float			Cur_Speed;
	 float			AVG_Speed;
	 float			Max_Speed;
	 float			Wheel_Size;
	 float			Trip_Distance;
	 float			Trip_ODO;
	 float			Total_Trip_ODO;

}Idt_EB_APP_T;

void e_bike_app_init(void);



void MT_RST_On(void);
void MT_RST_Off(void);
void MT_ESF_On(void);
void MT_ESF_Off(void);

void Get_Speed_Value(void);

void Motor_Handler(void);
void Monitor_Handler(void);

extern volatile uint8_t hallcode;
extern volatile uint8_t lasthallcode;

extern uint16_t volatile Speed_Cnt_500us;
extern uint16_t volatile Speed_Value;
extern float  Speed_Mil_Hr;

extern uint16_t volatile Candence_Out_500us;
extern uint16_t volatile Candence_Out_Value;
extern uint16_t volatile Candence_In_500us;
extern uint16_t volatile Candence_In_Value;

extern Motor_ID MT_State;



#endif
