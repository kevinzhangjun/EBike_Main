#ifndef _GPIO_PORT_H_
#define _GPIO_PORT_H_ 

#include "sdk_project_config.h"
#include "pins_driver.h"

/*****Uart 接口声明*****/
#define UART0_PORT  		(PORTB)
#define UART0_RX_ID   		(0u)
#define UART0_TX_ID   		(1u)
#define UART0_PORT_MUX   	(PORT_MUX_ALT2)


/*****led 接口声明*****/
#define LED_RED_GPIO  		(PTE)
#define LED_RED_PORT  		(PORTE)
#define LED_RED_ID   		(0u)
#define LED_RED_PIN   		(1<<LED_RED_ID)

/*****power control接口声明*****/
#define BVCC_CTL_GPIO  		(PTD)
#define BVCC_CTL_PORT  		(PORTD)
#define BVCC_CTL_ID   		(15u)
#define BVCC_CTL_PIN   		(1<<BVCC_CTL_ID)

/*****lamp 接口声明*****/
#define LAMP_FRONT_GPIO  	(PTD)
#define LAMP_FRONT_PORT  	(PORTD)
#define LAMP_FRONT_ID   	(6u)
#define LAMP_FRONT_PIN   	(1<<LAMP_FRONT_ID)

#define LAMP_REAR_GPIO  	(PTD)
#define LAMP_REAR_PORT  	(PORTD)
#define LAMP_REAR_ID   		(5u)
#define LAMP_REAR_PIN   	(1<<LAMP_REAR_ID)


/*****Key接口声明*****/
#define KEY_GPIO  			(PTC)
#define KEY_PORT  			(PORTC)
#define KEY_ID   			(8u)
#define KEY_GPIO_PIN   		(1<<KEY_ID)
#define Pwr_Off_Time		3000/64
#define Pwr_On_Time			1000/64
#define S2_Double_Cnt		50	//interal time < 400ms

/*****Brake接口声明*****/
#define BRAKE_GPIO  		(PTE)
#define BRAKE_PORT  		(PORTE)
#define BRAKE_ID   			(3u)
#define BRAKE_GPIO_PIN   	(1<<BRAKE_ID)

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
  Mode_ID		Mode_State;	   //按键1的状态标记
	bool      	PwrOff_State;
	bool		Brake1_State;
	bool 		Brake2_State;
	uint32_t  Cnt_64ms;      //计数 64ms
}Key_Info;


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

extern uint16_t s2_8ms;
extern Key_Info key_info;

#endif
