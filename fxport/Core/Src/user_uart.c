#include "user_uart.h"
#include "user_fifo.h"


typedef struct {
	UART_HandleTypeDef*	HalHandle;
	FIFO_HANDLE_T		TxFifo;
	FIFO_HANDLE_T		RxFifo;
	uint8_t				TxBuf[1];		//HAL APIs Transmit buf
	uint8_t				RxBuf[1];		//HAL APIs Receive buf
	uint8_t				TxProcessing;	//Transmit Processing Flag, 1=Processing
} USER_UART_HANDLE_T;


USER_UART_HANDLE_T UserUart_Handle[UART_PORT_NUM];

//create Tx/Rx FIFO buffer each UART channel...
uint8_t Uart3_TxFifoBuf[UART3_TX_FIFO_SIZE];
uint8_t Uart3_RxFifoBuf[UART3_RX_FIFO_SIZE];




USERUART_STS_T USERUART_FifoInit( USER_UART_HANDLE_T* uhuart )
{
	//create if() sentence each UART channel...
	if ( uhuart->HalHandle->Instance == USART1 )
	{
		FIFO_Init(&uhuart->TxFifo, Uart3_TxFifoBuf, UART3_TX_FIFO_SIZE);
		FIFO_Init(&uhuart->RxFifo, Uart3_RxFifoBuf, UART3_RX_FIFO_SIZE);
	}
	else
	{
		return USERUART_STS_EROOR;
	}

	return USERUART_STS_OK;
}



USER_UART_HANDLE_T* USERUART_GetUserHandle( UART_HandleTypeDef* huart )
{
	for ( uint8_t idx=0 ; idx<UART_PORT_NUM ; idx++ )
	{
		if ( UserUart_Handle[idx].HalHandle == huart )
		{
			return &UserUart_Handle[idx];
		}
	}

	return NULL;
}


USER_UART_HANDLE_T* USERUART_GetEmptyUserHandle( void )
{
	for ( uint8_t idx=0 ; idx<UART_PORT_NUM ; idx++ )
	{
		if ( UserUart_Handle[idx].HalHandle == NULL )
		{
			return &UserUart_Handle[idx];
		}
	}

	return NULL;
}



void USERUART_UnregisterAllHandle( void )
{
	for ( uint8_t i=0 ; i<UART_PORT_NUM ; i++ )
	{
		UserUart_Handle[i].HalHandle = NULL;
		UserUart_Handle[i].RxFifo.Buf = NULL;
		UserUart_Handle[i].RxFifo.BufSize = 0;
		UserUart_Handle[i].TxFifo.Buf = NULL;
		UserUart_Handle[i].TxFifo.BufSize = 0;
		UserUart_Handle[i].TxProcessing = 0;
	}

	return;
}


USERUART_STS_T USERUART_RegisterHandle( UART_HandleTypeDef* huart )
{
	USER_UART_HANDLE_T* userHuart;
	USERUART_STS_T result;

	if ( huart == NULL )
	{
		return USERUART_STS_HANDLE_NOT_EXIST;
	}

	//check handle already registered
	userHuart = USERUART_GetUserHandle(huart);
	if ( userHuart != NULL )
	{
		return USERUART_STS_HANDLE_ALREADY_REGISTERED;
	}

	//get empty register
	userHuart = USERUART_GetEmptyUserHandle();
	if ( userHuart == NULL )
	{
		return USERUART_STS_HANDLE_CANNOT_REGISTER;
	}

	//register UART HAL handle
	userHuart->HalHandle = huart;

	//Initialize and bind FIFO to handle
	result = USERUART_FifoInit(userHuart);
	if ( result != USERUART_STS_OK )
	{
		return USERUART_STS_EROOR;
	}

	//ready transmit
	userHuart->TxProcessing = 0;

	//ready receive
	HAL_UART_Receive_IT(huart, userHuart->RxBuf, 1);

	return USERUART_STS_OK;
}


USERUART_STS_T USERUART_UnregisterHandle( UART_HandleTypeDef* huart )
{
	USER_UART_HANDLE_T* userHuart;

	if ( huart == NULL )
	{
		return USERUART_STS_HANDLE_NOT_EXIST;
	}

	//check handle registered
	userHuart = USERUART_GetUserHandle(huart);
	if ( userHuart == NULL )
	{
		return USERUART_STS_HANDLE_NOT_REGISTERED;
	}

	//unregister UART handle and FIFO
	userHuart->HalHandle = NULL;
	userHuart->TxFifo.Buf = NULL;
	userHuart->TxFifo.BufSize = 0;
	userHuart->RxFifo.Buf = NULL;
	userHuart->RxFifo.BufSize = 0;

	return USERUART_STS_OK;
}


int32_t USERUART_Read( UART_HandleTypeDef* huart, uint8_t readBuf[], uint16_t readSize, uint32_t timeoutMs )
{
	USER_UART_HANDLE_T* userHuart = USERUART_GetUserHandle(huart);
	if ( userHuart == NULL )
	{
		return USERUART_STS_HANDLE_NOT_REGISTERED;
	}

	// set timeout timer counter
	uint32_t startTick = HAL_GetTick();
	if ( timeoutMs < HAL_MAX_DELAY )
	{
		//Add a freq to guarantee minimum wait
		timeoutMs += HAL_GetTickFreq();
	}

	uint16_t cnt = 0;
	do
	{
		int16_t fifoSts = FIFO_Pop(&userHuart->RxFifo, &readBuf[cnt]);
		if ( fifoSts == FIFO_STS_OK )
		{
			cnt++;
		}
		if ( cnt >= readSize )
		{
			break;
		}

	} while( HAL_GetTick() - startTick < timeoutMs );

	return cnt;
}


int32_t USERUART_Write( UART_HandleTypeDef* huart, uint8_t writeBuf[], uint16_t writeSize, uint32_t timeoutMs )
{
	USER_UART_HANDLE_T* userHuart = USERUART_GetUserHandle(huart);
	if ( userHuart == NULL )
	{
		return USERUART_STS_HANDLE_NOT_REGISTERED;
	}

	// set timeout timer counter
	uint32_t startTick = HAL_GetTick();
	if ( timeoutMs < HAL_MAX_DELAY )
	{
		//Add a freq to guarantee minimum wait
		timeoutMs += HAL_GetTickFreq();
	}

	uint16_t cnt = 0;
	do
	{
		int16_t fifoSts = FIFO_Push(&userHuart->TxFifo, writeBuf[cnt]);
		if ( fifoSts == FIFO_STS_OK )
		{
			cnt++;
		}

		if ( !userHuart->TxProcessing )
		{
			fifoSts = FIFO_Pop(&userHuart->TxFifo, userHuart->TxBuf);
			if ( fifoSts == FIFO_STS_OK )
			{
				userHuart->TxProcessing = 1;
				HAL_UART_Transmit_IT(huart, userHuart->TxBuf, 1);
			}
		}

		if ( cnt >= writeSize )
		{
			break;
		}
	} while( HAL_GetTick() - startTick < timeoutMs );

	return cnt;
}


int32_t USERUART_ReceiveCount( UART_HandleTypeDef* huart )
{
	USER_UART_HANDLE_T* userHuart = USERUART_GetUserHandle(huart);
	if ( userHuart == NULL )
	{
		return USERUART_STS_HANDLE_NOT_REGISTERED;
	}

	int32_t count = FIFO_Count(&userHuart->RxFifo);

	if ( count < 0 )
	{
		return USERUART_STS_EROOR;
	}

	return count;
}



void USERUART_TxCompleteCallback( UART_HandleTypeDef* huart )
{
	USER_UART_HANDLE_T* userHuart = USERUART_GetUserHandle(huart);
	if ( userHuart == NULL )
	{
		return;
	}

	//get next charcter
	if ( FIFO_Pop(&userHuart->TxFifo, userHuart->TxBuf) == FIFO_STS_OK )
	{
		HAL_UART_Transmit_IT(huart, userHuart->TxBuf, 1);
	}
	else
	{
		//finished or error, stop processing
		userHuart->TxProcessing = 0;
	}
}


void USERUART_RxCompleteCallback( UART_HandleTypeDef* huart )
{
	USER_UART_HANDLE_T* userHuart = USERUART_GetUserHandle(huart);
	if ( userHuart == NULL )
	{
		return;
	}

	//store received character
	FIFO_Push(&userHuart->RxFifo, userHuart->RxBuf[0]);

	//continue Receive
	HAL_UART_Receive_IT(huart, userHuart->RxBuf, 1);
}


void USERUART_ErrorCallback( UART_HandleTypeDef* huart )
{

}
