#ifndef _TIM1_TIMER_H_
#define _TIM1_TIMER_H_

#include "sdk_project_config.h"




/* LPIT channel used */
#define LPIT_CHANNEL        0UL
/* LPTMR channel used */
#define LPTMR_CHANNEL       0UL

/* Period in nanosecond unit */
#define LPITMR_PERIOD_10us        10000UL /* The period is 10us second */
#define LPTMR_PERIOD_500us        500000UL /* The period is 500us second */

#define LPIT_IRQ timingLpitInst0_callback
#define LPTMR_IRQ timingLptmrInst0_callback



void Timer_Init(void);
void LPIT_IRQ(void * userData);
void LPTMR_IRQ(void * userData);

extern volatile uint32_t 	LED_4ms;

#endif


