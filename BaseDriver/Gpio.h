#ifndef _GPIO_PORT_H_
#define _GPIO_PORT_H_ 

#include "sdk_project_config.h"
#include "pins_driver.h"

/*****Uart �ӿ�����*****/
#define UART0_PORT  		(PORTB)
#define UART0_RX_ID   		(0u)
#define UART0_TX_ID   		(1u)
#define UART0_PORT_MUX   	(PORT_MUX_ALT2)


/*****led �ӿ�����*****/
#define LED_RED_GPIO  		(PTE)
#define LED_RED_PORT  		(PORTE)
#define LED_RED_ID   		(0u)
#define LED_RED_PIN   		(1<<LED_RED_ID)

/*****power control�ӿ�����*****/
#define BVCC_CTL_GPIO  		(PTD)
#define BVCC_CTL_PORT  		(PORTD)
#define BVCC_CTL_ID   		(15u)
#define BVCC_CTL_PIN   		(1<<BVCC_CTL_ID)

/*****lamp �ӿ�����*****/
#define LAMP_FRONT_GPIO  	(PTD)
#define LAMP_FRONT_PORT  	(PORTD)
#define LAMP_FRONT_ID   	(6u)
#define LAMP_FRONT_PIN   	(1<<LAMP_FRONT_ID)

#define LAMP_REAR_GPIO  	(PTD)
#define LAMP_REAR_PORT  	(PORTD)
#define LAMP_REAR_ID   		(5u)
#define LAMP_REAR_PIN   	(1<<LAMP_REAR_ID)


/*****Key�ӿ�����*****/
#define KEY_GPIO  			(PTC)
#define KEY_PORT  			(PORTC)
#define KEY_ID   			(8u)
#define KEY_GPIO_PIN   		(1<<KEY_ID)
#define Pwr_Off_Time		3000/64
#define Pwr_On_Time			1000/64
#define S2_Double_Cnt		50	//interal time < 400ms

/*****Brake�ӿ�����*****/
#define BRAKE_GPIO  		(PTE)
#define BRAKE_PORT  		(PORTE)
#define BRAKE_ID   			(3u)
#define BRAKE_GPIO_PIN   	(1<<BRAKE_ID)

/*****CADENCE_IN�ӿ�����*****/
#define CADENCE_IN_GPIO  			(PTD)
#define CADENCE_IN_PORT  			(PORTD)
#define CADENCE_IN_ID   			(16u)
#define CADENCE_IN_GPIO_PIN   		(1<<CADENCE_IN_ID)
#define CADENCE_IN_PORT_IRQn 		PORTD_IRQn
#define CADENCE_IN_DIR_THRESHOLD 	8

#define Revolution				20
#define CDN_IN_RPM_1_Period		60*1000*100/Revolution
#define CDN_IN_RPM_480_Period	1*125*100/Revolution
#define CDN_IN_RPM_Factor		60*1000*100/Revolution



typedef enum
{
	mode_init,
	mode1_Auto,
	mode2_Walk,
	mode3_Manual_Adjust,
	mode4_Manual_Run,
	mode5_Manual_Stop,
	end_mode
}Mode_ID;

typedef struct
{
  Mode_ID		Mode_State;	   //����1��״̬���
	bool      	PwrOff_State;
	bool		Brake1_State;
	bool 		Brake2_State;
	uint32_t  Cnt_64ms;      //���� 64ms
}Key_Info;

typedef enum
{
	CDN_IN_FORWARD,
	CDN_IN_BACKWARD
}CDN_IN_DIR_ID;
typedef struct
{
	CDN_IN_DIR_ID	  Cdn_In_Dir;
	uint8_t 		  Cdn_In_Dir_Flt_Backward;
	uint8_t 		  Cdn_In_Dir_Flt_Forward;
	uint32_t volatile Cdn_In_Dir_10us;
	uint32_t volatile Speed_Cnt_10us;
	uint32_t volatile Cdn_In_Cnt_10us;
	uint32_t volatile Speed_Value;
	uint32_t 		  Speed_Value_lst;
	uint32_t volatile Cdn_In_Value;
	uint32_t 		  Cdn_In_Value_lst;
	float  	 		  Speed_Per_Hr;
	float  	 		  Speed_RPM;
	float  	 		  Cdn_In_RPM;
}Speed_ST;

void GPIO_Init(void);

void LED_Red_On(void);
void LED_Red_Off(void);
void LED_Red_Toggle(void);

void Power_Off(void);
void Power_On(void);
void Power_Toggle(void);

void Key_Scan(void);
void Key_State_Result(void);
void Key_Routine(void);

void Lamp_Front_En(void);
void Lamp_Front_Dis(void);

void Lamp_Rear_En(void);
void Lamp_Rear_Dis(void);

void Brake_Routine(void);


void EXTI_Configuration(void);

extern uint16_t s2_8ms;
extern Key_Info key_info;

extern Speed_ST Speed_Info;

#endif
