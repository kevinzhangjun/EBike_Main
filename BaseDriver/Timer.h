#ifndef _TIM1_TIMER_H_
#define _TIM1_TIMER_H_

#include "sdk_project_config.h"


/* LPTMR channel used */
#define LPTMR_CHANNEL       0UL
/* Period in nanosecond unit */
#define LPTMR_PERIOD_500us        500000UL /* The period is 500us second */
#define LPTMR_IRQ timingLptmrInst0_callback



void Timer_Init(void);
void LPTMR_IRQ(void * userData);

void Time_1ms_Tasks(void);
void Time_2ms_Tasks(void);
void Time_4ms_Tasks(void);
void Time_8ms_Tasks(void);
void Time_64ms_Tasks(void);

extern volatile uint32_t 	LED_4ms;

#endif


