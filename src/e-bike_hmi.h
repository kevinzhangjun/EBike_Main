#ifndef __e_bike_hmi
#define __e_bike_hmi

#include "sdk_project_config.h"

#define TEST_MT			1


#if(TEST_MT)
#define HEAD_LEN		8
#define TORQUE_FACTOR	40
#define TORQUE_MAX		120
#define MIN_GEAR_RATIO	0.3
#define MAX_GEAR_RATIO	3
#define MIN_GEAR_RATIO_LIM	0.81
#define MAX_GEAR_RATIO_LIM	1.2

typedef void (*PFCN_VOID_STATES)();

typedef enum
{
	Idle,
	Assit_RMP,
	Assit_TQE,
	Gear_RPM,
	Gear_Ratio,
	End_Tst
}MT_TEST_ID;

typedef enum
{
	Rev_CMD,
	Res_CMD,
	End_CMD
}MT_CMD_ID;

typedef struct
{

	float  	 		  AssitMT_RPM_In;
	float  	 		  AssitMT_RPM_Out;
	float			  Const_Torque_In;
	float  	 		  Iq_Out;
	float  	 		  Id_Out;
	float  	 		  GearMT_RPM_In;
	float  	 		  GearMT_RPM_Out;
	float  	 		  GearMT_Ratio_In;
	float  	 		  CacdenceIn_Out;
	bool			  status;
}RS232_ST;

#endif


void HMI_Handler(void);

#if(TEST_MT)
void State_AMT_RPM_CMD(void);
void State_AMT_ConstTQE_CMD(void);
void State_GMT_RPM_CMD(void);
void State_GMT_Ratio_CMD(void);

void State_AMT_RPM_RES(void);
void State_AMT_ConstTQE_RES(void);
void State_GMT_RPM_RES(void);
void State_GMT_Ratio_RES(void);

#endif

#endif
