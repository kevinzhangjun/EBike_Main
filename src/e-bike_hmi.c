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

const char Rev_Head[End_Tst-1][HEAD_LEN]=
{
	"CMSARPM=","CMSATQE=","CMSGRPM=","CMSGARA="
};

const char Data_Head[4][1]=
{
	"-","+","0","9"
};

const uint8_t Type_Len[End_Tst-1]=
{
	12U,11U,13U,11U
};

const char Res_TQE_Head[]=
{
	"CMRATQE-IQC=xx/IDC=xx/MTARPM=xxxx"
};

const uint8_t Res_TQE_Pos[3]=
{
	12U,19U,29U
};

const uint8_t Res_Len[End_Tst-1]=
{
	12U,33U,13U,19U
};

PFCN_VOID_STATES StateTable[End_CMD][End_Tst-1]=
{
	{State_AMT_RPM_CMD,State_AMT_ConstTQE_CMD,State_GMT_RPM_CMD,State_GMT_Ratio_CMD},
	{State_AMT_RPM_RES,State_AMT_ConstTQE_RES,State_GMT_RPM_RES,State_GMT_Ratio_RES}
};

RS232_ST RS232_MT_Test;
MT_TEST_ID MT_Test_Mode;
MT_CMD_ID  CMD_Type;
#endif

void Start_AMT_RPM(int16_t	Set_RPM)
{

}

void Start_AMT_ConstTQE(uint8_t Set_Const_TQE)
{

}

void Start_GMT_RPM(int16_t	Set_RPM)
{

}

void Start_GMT_Ratio(float	Set_Gear_Ratio)
{

}

#if(TEST_MT)

void Set_AMT_RPM_Res(void)
{
	//test response
	RS232_MT_Test.AssitMT_RPM_Out = RS232_MT_Test.AssitMT_RPM_In;

	//real data replace test response
}

void Set_AMT_ConstTQE_Res(void)
{
	//test response
	RS232_MT_Test.Iq_Out = 20;
	RS232_MT_Test.Id_Out = 3;
	RS232_MT_Test.AssitMT_RPM_Out = 1500;

	//real data replace test response
}

void Set_GMT_RPM_Res(void)
{
	//test response
	RS232_MT_Test.GearMT_RPM_Out = RS232_MT_Test.GearMT_RPM_In;

	//real data replace test response
}

void Set_GMT_Ratio_Res(void)
{
	//test response
	RS232_MT_Test.CacdenceIn_Out = 1100;
	RS232_MT_Test.GearMT_RPM_Out = -3100;

	//real data replace test response
}

bool Rec_Float_Buf(uint8_t *src, float *data, uint8_t len, bool point)
{
	uint8_t i;
	float tmp_data=0, Flg=1;
	bool status = true;

	if(point)
	{

		if(*src >= Data_Head[2][0] && *src <= Data_Head[3][0])
		{
			tmp_data = (*src++ - Data_Head[2][0]);
			tmp_data *=10;
			src++;
			tmp_data += (*src - Data_Head[2][0]);
			tmp_data /=10;
			if((tmp_data < MIN_GEAR_RATIO || tmp_data > MAX_GEAR_RATIO) ||
			   (tmp_data > MIN_GEAR_RATIO_LIM && tmp_data < MAX_GEAR_RATIO_LIM))
			{
				status = false;
			}
		}
		else
		{
			status = false;
		}
	}
	else
	{
		if(*src == Data_Head[0][0])
		{
			Flg = -1;
			src++;
		}
		else if(*src == Data_Head[1][0])
		{
			src++;
		}

		for(i=0; i<len; i++)
		{

			if(*src >= Data_Head[2][0] && *src <= Data_Head[3][0])
			{
				tmp_data *=10;
				tmp_data += (*src++ - Data_Head[2][0]);
			}
			else
			{
				status = false;
				break;
			}
		}
	}

	if(status)
	{
		tmp_data *= Flg;
		*data = tmp_data;
	}

	return status;
}

void Send_Float_Buf(uint8_t *des, uint8_t len, float data, bool point)
{
	uint8_t i;
	uint16_t mode_len=1;
	float   data_temp, temp;

	if(point)
	{
		data_temp = data;
		*des++ = (uint8_t)data_temp + Data_Head[2][0];
		*des++ = '.';
		data_temp = data*10 - (float)((uint8_t)data_temp)*10;
		*des++ = (uint8_t)data_temp + Data_Head[2][0];
	}
	else
	{
		for(i=0; i<len-1; i++)
		{
			mode_len *= 10;
		}

		if(data < 0)
		{
			data_temp = -1*data;
		}
		else
		{
			data_temp = data;
		}

		for(i=0; i<len; i++)
		{
			temp =data_temp/mode_len;
			*des++ = (uint8_t)temp + Data_Head[2][0];

			data_temp = data_temp - (float)(uint8_t)temp*mode_len;
			mode_len /=10;
		}
	}

}

void State_AMT_RPM_CMD(void)
{
	if(Rec_Float_Buf(&Uart0.tr_buf[HEAD_LEN],&RS232_MT_Test.AssitMT_RPM_In, 4, false))
	{
		//Add Assistant MT RPM Running function
		Start_AMT_RPM((int16_t)RS232_MT_Test.AssitMT_RPM_In);
		RS232_MT_Test.status = true;
	}
	else
	{
		RS232_MT_Test.status = false;
	}
}

void State_AMT_RPM_RES(void)
{
	Set_AMT_RPM_Res();
	Send_Float_Buf(&Uart0.tx_buf[HEAD_LEN], 4, RS232_MT_Test.AssitMT_RPM_Out, false);
}

void State_AMT_ConstTQE_CMD(void)
{
	if(Rec_Float_Buf(&Uart0.tr_buf[HEAD_LEN],&RS232_MT_Test.Const_Torque_In, 3, false))
	{
		//Add cost torque MT Running function
		if(RS232_MT_Test.Const_Torque_In <= TORQUE_MAX && RS232_MT_Test.Const_Torque_In !=0)
		{
			Start_AMT_ConstTQE((uint8_t)RS232_MT_Test.Const_Torque_In/TORQUE_FACTOR);
			RS232_MT_Test.status = true;
		}
		else
		{
			RS232_MT_Test.status = false;
		}
	}
	else
	{
		RS232_MT_Test.status = false;
	}
}

void State_AMT_ConstTQE_RES(void)
{
	Set_AMT_ConstTQE_Res();
	Send_Float_Buf(&Uart0.tx_buf[Res_TQE_Pos[0]], 2, RS232_MT_Test.Iq_Out, false);
	Send_Float_Buf(&Uart0.tx_buf[Res_TQE_Pos[1]], 2, RS232_MT_Test.Id_Out, false);
	Send_Float_Buf(&Uart0.tx_buf[Res_TQE_Pos[2]], 4, RS232_MT_Test.AssitMT_RPM_Out, false);
}

void State_GMT_RPM_CMD(void)
{
	if(Rec_Float_Buf(&Uart0.tr_buf[HEAD_LEN],&RS232_MT_Test.GearMT_RPM_In, 4, false))
	{
		//Add Gear MT RPM Running function
		Start_GMT_RPM((int16_t)RS232_MT_Test.GearMT_RPM_In);
		RS232_MT_Test.status = true;
	}
	else
	{
		RS232_MT_Test.status = false;
	}
}

void State_GMT_RPM_RES(void)
{
	Set_GMT_RPM_Res();

	if(RS232_MT_Test.GearMT_RPM_Out >= 0)
	{
		Uart0.tx_buf[HEAD_LEN] = '+';
	}
	else
	{
		Uart0.tx_buf[HEAD_LEN] = '-';
	}

	Send_Float_Buf(&Uart0.tx_buf[HEAD_LEN+1], 4, RS232_MT_Test.GearMT_RPM_Out, false);
}

void State_GMT_Ratio_CMD(void)
{
	if(Rec_Float_Buf(&Uart0.tr_buf[HEAD_LEN],&RS232_MT_Test.GearMT_Ratio_In, 1, true))
	{
		//Add Gear MT Ratio Running function
		Start_GMT_Ratio(RS232_MT_Test.GearMT_Ratio_In);
		RS232_MT_Test.status = true;
	}
	else
	{
		RS232_MT_Test.status = false;
	}
}

void State_GMT_Ratio_RES(void)
{
	Set_GMT_Ratio_Res();

	Uart0.tx_buf[HEAD_LEN] = 'C';
	Uart0.tx_buf[HEAD_LEN+5] = 'M';

	if(RS232_MT_Test.GearMT_RPM_Out >= 0)
	{
		Uart0.tx_buf[HEAD_LEN+6] = '+';
	}
	else
	{
		Uart0.tx_buf[HEAD_LEN+6] = '-';
	}

	Send_Float_Buf(&Uart0.tx_buf[HEAD_LEN+1], 4, RS232_MT_Test.CacdenceIn_Out, false);
	Send_Float_Buf(&Uart0.tx_buf[HEAD_LEN+7], 4, RS232_MT_Test.GearMT_RPM_Out, false);
}

void RS232_Test_Cmd(uint8_t len)
{
	uint8_t i,j;

	for(i=0; i<End_Tst-1; i++)
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
				StateTable[Rev_CMD][i]();
				if(RS232_MT_Test.status)
				{
					MT_Test_Mode = i+1;
				}
				else
				{
					MT_Test_Mode = Idle;
				}
			}

			break;
		}
	}
}

void RS232_Test_Res_Head(MT_TEST_ID Test_Type)
{
	uint8_t i;

	for(i=0; i<HEAD_LEN; i++)
	{
		Uart0.tx_buf[i] = Rev_Head[Test_Type-1][i];
	}

	Uart0.tx_buf[2] = 'R';

	Uart0.tx_len = Res_Len[MT_Test_Mode-1];

	if(Test_Type == Assit_TQE)
	{
		for(i=0; i<Res_Len[Assit_TQE-1]; i++)
		{
			Uart0.tx_buf[i] = Res_TQE_Head[i];
		}
	}
}

void RS232_Test_Res(void)
{
	if(MT_Test_Mode != Idle)
	{
		RS232_Test_Res_Head(MT_Test_Mode);
		StateTable[Res_CMD][MT_Test_Mode-1]();
	}

}

#endif


void Send_Data_HMI(void)
{
	if(Uart0.tx_2ms > 500)
	{
		Uart0.tx_2ms = 0;


#if(TEST_MT)

		RS232_Test_Res();
#else
		Uart0.tx_len = 0;
#endif

		if(Uart0.tx_len)
		{
			start_tx(Uart0.tx_len);
		}
	}
}

void Rev_Cmd(uint8_t len)
{

#if(TEST_MT)

	RS232_Test_Cmd(len);

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
