#ifndef __e_bike_app
#define __e_bike_app

#include "sdk_project_config.h"

#define No_Debug			1


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
void Brake_Routine(void);
void Get_Speed_Value(void);
void Get_Cdn_In_Value(void);
void ADC0_GetResult(void);






extern Motor_ID MT_State;



#endif
