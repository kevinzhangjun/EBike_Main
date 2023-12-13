/*
 * peripheral_init.c
 *
 *  Created on: Sep 29, 2023
 *      Author: PK6300
 */

#include "S32K142.h"
#include "peripheral_init.h"
#include "adc.h"
#include "motor_control.h"

void peripheral_init(void) {
    PCC->PCCn[PCC_PORTA_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock to PORT A */
    PCC->PCCn[PCC_PORTB_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock to PORT B */
    PCC->PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock to PORT C */
    PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock to PORT D */
    PCC->PCCn[PCC_PORTE_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock to PORT E */
    PORTE->PCR[0] |= PORT_PCR_MUX(1);  // PORTE_PIN0 is GPIO
//    PORTE->PCR[1] |= PORT_PCR_MUX(1);  // PORTE_PIN1 is GPIO

    // HALL Sensor IO for M1 motor
    PORTD->PCR[0] |= PORT_PCR_MUX(1);  // PORTD_0 is GPIO - HALL M1 A1
    PORTE->PCR[11] |= PORT_PCR_MUX(1); // PORTE_11 is GPIO - HALL M1 B1
    PORTE->PCR[10] |= PORT_PCR_MUX(1); // PORTE_10 is GPIO - HALL M1 C1
    PTD->PDDR &= ~1;                   // I/O is input
    PTE->PDDR &= ~((1<<10) | (1<<11)); // I/O is input

    // HALL Sensor IO for M2 motor
    PORTA->PCR[11] |= PORT_PCR_MUX(1); // PORTA_11 is GPIO - HALL M1 C1
    PORTA->PCR[12] |= PORT_PCR_MUX(1); // PORTA_12 is GPIO - HALL M1 B1
    PORTA->PCR[13] |= PORT_PCR_MUX(1); // PORTA_13 is GPIO - HALL M1 A1
    PTA->PDDR &= ~((1<<11) | (1<<12) | (1<<13)); // inputs

//    PTE->PDDR |= 1;     // PE0 is output - LED

    PORTD->PCR[7] |= PORT_PCR_MUX(1);   // PTD7 is output for GD reset
    PTD->PDDR |= (1<<7);
    PTD->PCOR = 1<<7;    // clear ~GD reset

//    PTE->PCOR = 1;   // LED on
//    PTE->PSOR = 1;   // LED off

//    PORTB->PCR[1] |= PORT_PCR_MUX(1); // CAN/UART TX is GPIO
//    PTB->PDDR |= 2; // PB2 is output

//    SIM->CHIPCTL |= 0x00000004;   // route ADC0_SE8 to portB pin 13 - NEC board

    // Motor 1 gate drive outputs
    PORTE->PCR[2] |= PORT_PCR_MUX(4);  // PORTE_PIN2 is FTM3_Ch6  PWM_A_Hi
    PORTE->PCR[6] |= PORT_PCR_MUX(4);  // PORTE_PIN6 is FTM3_Ch7  PWM_A_Low
    PORTC->PCR[6] |= PORT_PCR_MUX(4);  // PORTC_PIN6 is FTM3_CH2  PWM_B_Hi
    PORTC->PCR[7] |= PORT_PCR_MUX(4);  // PORTC_PIN7 is FTM3_Ch3  PWM_B_Low
    PORTD->PCR[2] |= PORT_PCR_MUX(2);  // PORTD_PIN2 is FTM3_Ch4  PWM_C_Hi
    PORTD->PCR[3] |= PORT_PCR_MUX(2);  // PORTD_PIN3 is FTM3_Ch5  PWM_C_Low

    // Motor 2 gate drive outputs
    PORTE->PCR[8] |= PORT_PCR_MUX(2);  // PORTE_PIN8 is FTM0_Ch0  PWM_A_Hi
    PORTE->PCR[9] |= PORT_PCR_MUX(2);  // PORTE_PIN9 is FTM0_Ch1  PWM_A_Low
    PORTC->PCR[2] |= PORT_PCR_MUX(2);  // PORTC_PIN2 is FTM0_CH2  PWM_B_Hi
    PORTC->PCR[3] |= PORT_PCR_MUX(2);  // PORTC_PIN3 is FTM0_Ch3  PWM_B_Low
    PORTB->PCR[4] |= PORT_PCR_MUX(2);  // PORTB_PIN4 is FTM0_Ch4  PWM_C_Hi
    PORTB->PCR[5] |= PORT_PCR_MUX(2);  // PORTB_PIN5 is FTM0_Ch5  PWM_C_Low

    ADC_init(PCC_ADC0_INDEX, ADC0);
    ADC_init(PCC_ADC1_INDEX, ADC1);

    // FTM3 will lead FTM0 by a little bit
    Motor_Timer_Init(PCC_FTM3_INDEX, FTM3);
    Motor_Timer_Init(PCC_FTM0_INDEX, FTM0);

    // Set up our timer interrupt on FTM3
    S32_NVIC->ICPR[3] = 1 << (122 % 32); /* IRQ104-FTM3 Reload interrupt: clr any pending IRQ*/
    S32_NVIC->ISER[3] = 1 << (122 % 32); /* IRQ104-FTM3 Reload interrupt: enable IRQ */
    S32_NVIC->IP[138] = 0x0A; 			/* IRQ122-FTM3 Reload interrupt: priority 10 of 0-15*/

   	FTM3->SC = 0x00FC0068;  // enable Reload IRQ
}

void Motor_Timer_Init(int pcc_ftm_index, FTM_Type *ftm) {
 PCC->PCCn[pcc_ftm_index] &= ~PCC_PCCn_CGC_MASK; /* Ensure clk disabled for config */
 PCC->PCCn[pcc_ftm_index] |= PCC_PCCn_PCS(0b001)/* Clock Src=1, 8 MHz SOSCDIV1_CLK */
 | PCC_PCCn_CGC_MASK; 							/* Enable clock for FTM regs */

 /* for the NXP board the high sides are active low for our board, active high */
// ftm->POL = 0b00010101;  /* Polarity for high-side channels is active low */
 ftm->POL = 0b00000000;  /* Polarity for all channels is active high (default) */

// ftm->MODE |= FTM_MODE_WPDIS_MASK; /* Write protect to registers disabled (default) */
 ftm->MODE = FTM_MODE_WPDIS_MASK; /* Write protect to registers disabled (default) Interrupts disabled */

 ftm->FLTCTRL = 0;
 ftm->CONF = 0x00000040;  /* safe state for debug mode */

// ftm->SYNC = 0x0002;         /* update duty cycles on CNTMAX */
 ftm->SYNC = 0x0001;         /* update duty cycles on CNTMIN */
// ftm->MOD = 2000 -1 ;	/* FTM1 counter final value (used for PWM mode) */
 ftm->MOD = 1200 -1 ;	/* FTM1 counter final value (used for PWM mode) */
 	 	 	 	 	 	/* FTM1 Period = MOD-CNTIN+0x0001 ~= 62500 ctr clks */
 	 	 	 	 	 	/* 80MHz /2 /2000 = 20KHz */

 ftm->SC = 0x00FC0028;   /* Enable PWM channel 6 output*/
 	 	 	 	 	 	 /* Enable PWM channel 7 output*/
 	 	 	 	 	 	 /* Enable PWM channel 2 output*/
	 	 	 	 	 	 /* Enable PWM channel 3 output*/
	 	 	 	 	 	 /* Enable PWM channel 4 output*/
 	 	 	 	 	 	 /* Enable PWM channel 5 output*/
 	 	 	 	 	 	 /* TOIE (Timer Overflow Interrupt Ena) = 0 (disabled) */
	 	 	 	 	 	 /* RIE (Reload Interrupt Ena) = 0 (disabled) */
 	 	 	 	 	 	 /* CPWMS (Center aligned PWM Select) = 1 (up/down mode) */
 	 	 	 	 	 	 /* CLKS (Clock source) = 1 (FTM input clock) */
 	 	 	 	 	 	 /* PS (Prescaler factor) = 1. Prescaler = 0 */
 ftm->COMBINE = 0x32323200;/* FTM mode settings used: DECAPENx, MCOMBINEx, COMBINEx=0 */

#define DEADTIME_VAL (0x00008080 + 8)
 ftm->DEADTIME = DEADTIME_VAL;   // 400ns deadtime  80MHz/4 = 20MHz(50ns) and 8 cycles of that
 //ftm->DEADTIME = 0x00008096;   // 1000ns deadtime  80MHz/4 = 20MHz and 22 cycles of that

 ftm->MOD = 1200 -1 ;	/* FTM1 counter final value (used for PWM mode) */
 	 	 	 	 	 	/* FTM1 Period = MOD-CNTIN+0x0001 ~= 62500 ctr clks */
 	 	 	 	 	 	/* 80MHz /2 /2000 = 20KHz */

//#define CHAN_CNSC 0x00000028 /* FTM0 ch1: edge-aligned PWM, low true pulses */
	 	 	 	 	 	 	 /* CHIE (Chan Interrupt Ena) = 0 (default) */
	 	 	 	 	 	 	 /* MSB:MSA (chan Mode Select)=0b10, Edge Align PWM*/
	 	 	 	 	 	 	 /* ELSB:ELSA (chan Edge/Level Select)=0b10, low true */

 #define CHAN_CNSC 0x00000024 /* FTM0 ch1: edge-aligned PWM, low true pulses */
	 	 	 	 	 	 	 /* CHIE (Chan Interrupt Ena) = 0 (default) */
	 	 	 	 	 	 	 /* MSB:MSA (chan Mode Select)=0b10, Edge Align PWM*/
	 	 	 	 	 	 	 /* ELSB:ELSA (chan Edge/Level Select)=0b01, high true */


 ftm->CONTROLS[6].CnSC = CHAN_CNSC;
 ftm->CONTROLS[7].CnSC = CHAN_CNSC;
 ftm->CONTROLS[2].CnSC = CHAN_CNSC;
 ftm->CONTROLS[3].CnSC = CHAN_CNSC;
 ftm->CONTROLS[4].CnSC = CHAN_CNSC;
 ftm->CONTROLS[5].CnSC = CHAN_CNSC;

 ftm->CONTROLS[6].CnV = 600; /* FTM ch6 compare value (~50% duty cycle) */
 ftm->CONTROLS[2].CnV = 600; /* FTM ch2 compare value (~50% duty cycle) */
 ftm->CONTROLS[4].CnV = 600; /* FTM ch4 compare value (~50% duty cycle) */

}
