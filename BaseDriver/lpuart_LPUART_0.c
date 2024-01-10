/******************************************************************
**	  E_Bike
**	  Uart Interrupt
**
**    Versiomn: V1.0
**	  Author  : Kevin Zhang
********************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "lpuart_LPUART_0.h"

const lpuart_user_config_t lpUartInitConfig1 = {
  .transferType = LPUART_USING_INTERRUPTS,
  .baudRate = 9600UL,
  .parityMode = LPUART_PARITY_DISABLED,
  .stopBitCount = LPUART_TWO_STOP_BIT,
  .bitCountPerChar = LPUART_8_BITS_PER_CHAR,
  .rxDMAChannel = 0UL,
  .txDMAChannel = 0UL
};

lpuart_state_t lpUartState1;
UART_T Uart0;

/* UART rx Uart0_RX_IRQ for continuous reception, byte by byte */
void Uart0_RX_IRQ(void *driverState, uart_event_t event, void *userData)
{
    /* Unused parameters */
    (void)driverState;
    (void)userData;

    /* Check the event type */
    if (event == UART_EVENT_RX_FULL)
    {
        /* The reception stops when newline is received or the tr0_buf is full */
        if ((Uart0.tr_buf[Uart0.tr_idx] != '\n') && (Uart0.tr_idx != (BUFFER_SIZE - 2U)))
        {
            /* Update the tr0_buf index and the rx tr0_buf */
        	Uart0.tr_idx++;
            LPUART_DRV_SetRxBuffer(INST_LPUART_LPUART_0, &Uart0.tr_buf[Uart0.tr_idx], 1U);
            Uart0.tr_2ms = 0;
        }
    }
}

/* UART tx Uart0_TX_IRQ for continuous reception, byte by byte */
void Uart0_TX_IRQ(void *driverState, uart_event_t event, void *userData)
{
    /* Unused parameters */
    (void)driverState;
    (void)userData;

    if (event == UART_EVENT_END_TRANSFER)
    {
    	Uart0.tx_len = 0;
    }
}

void Init_Uart(void)
{

	/* Initialize LPUART instance */
    LPUART_DRV_Init(INST_LPUART_LPUART_0, &lpUartState1, &lpUartInitConfig1);
    /* Install the callback for rx events */
    LPUART_DRV_InstallRxCallback(INST_LPUART_LPUART_0, Uart0_RX_IRQ, NULL);
    LPUART_DRV_InstallTxCallback(INST_LPUART_LPUART_0, Uart0_TX_IRQ, NULL);
    Uart0.tr_idx = 0;

    LPUART_DRV_ReceiveData(INST_LPUART_LPUART_0, Uart0.tr_buf, 1U);
}

void start_tx(uint8_t len)
{
	Uart0.tx_len = len;
	LPUART_DRV_SendData(INST_LPUART_LPUART_0, (uint8_t *)Uart0.tx_buf, Uart0.tx_len);
}


