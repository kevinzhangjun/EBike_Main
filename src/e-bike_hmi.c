/******************************************************************
**	  E_Bike
**	  hmi
**
**    Versiomn: V1.0
**	  Author  : Kevin Zhang
********************************************************************/

/* Includes ------------------------------------------------------------------*/

#include "e-bike_hmi.h"


#if(TEST_MT)


const char Rev_Head[TEST_ITEMS][HEAD_LEN]=
{
	"CMSARPM=","CMSATQE=","CMSGRPM=","CMSGARA="
};

const uint8_t Type_Len[TEST_ITEMS]=
{
	12U,11U,13U,11U
};

PFCN_VOID_STATES StateTable[TEST_ITEMS]=
{
	State_AMT_RPM,State_AMT_CstTQE,State_GMT_RPM,State_GMT_Ratio
};

void State_AMT_RPM(void)
{

}

void State_AMT_CstTQE(void)
{

}

void State_GMT_RPM(void)
{

}

void State_GMT_Ratio(void)
{

}

void RS232_Test(uint8_t len)
{
	uint8_t i,j;

	for(i=0; i<TEST_ITEMS; i++)
	{

		for(j=0; j<HEAD_LEN; j++)
		{
			if(Uart0.tr_buf[j] != Rev_Head[i][j])
			{
				break;
			}
		}

		if(j == HEAD_LEN)
		{
			if(Type_Len[i] == len)
			{
				StateTable[i]();
			}

			break;
		}
	}
}

#endif



void Send_Float_Buf(uint8_t *idx, float data)
{
	uint8_t *point, i;
	uint8_t index_data;

	index_data = *idx;
	point = (uint8_t *)&data;

	for(i=0; i<4; i++)
	{
		Uart0.tx_buf[index_data++] = *(point++);
	}

	*idx = index_data;
}

void Send_Byte_Buf(uint8_t *idx, uint8_t data)
{
	uint8_t *point;
	uint8_t index_data;

	index_data = *idx;
	point = (uint8_t *)&data;

	Uart0.tx_buf[index_data++] = *(point++);

	*idx = index_data;
}

void Send_Data_HMI(void)
{

}

void Rev_Cmd(uint8_t len)
{

#if(TEST_MT)


	RS232_Test(len);

#else


#endif

	Uart0.tr_idx = 0;
}

void HMI_Handler(void)
{

	if(Uart0.tr_2ms > 3 && Uart0.tr_idx !=0)
	{
		Rev_Cmd(Uart0.tr_idx);
	}
	else if(Uart0.tr_2ms > 500)
	{
		Uart0.tr_2ms = 0;
		LPUART_DRV_ReceiveData(INST_LPUART_LPUART_0, Uart0.tr_buf, 1U);
	}

	Send_Data_HMI();
}
