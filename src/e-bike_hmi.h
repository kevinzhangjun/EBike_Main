#ifndef __e_bike_hmi
#define __e_bike_hmi

#include "sdk_project_config.h"

#define TEST_MT			1


#if(TEST_MT)
#define HEAD_LEN		8
#define TEST_ITEMS		4

typedef void (*PFCN_VOID_STATES)();


#endif


void HMI_Handler(void);

void State_AMT_RPM(void);
void State_AMT_CstTQE(void);
void State_GMT_RPM(void);
void State_GMT_Ratio(void);


#endif
