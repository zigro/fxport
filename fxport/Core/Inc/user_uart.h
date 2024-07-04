#ifndef __USER_UART_H__
#define __USER_UART_H__

#include <stdint.h>
//#include "stm32h5xx_hal_uart.h"
//#include "stm32h5xx_hal.h"
#include "mcu.h"


#define UART_PORT_NUM		1	// number of UART port

#define UART3_TX_FIFO_SIZE	(1472 + 1)	//Ether - UDP max payload?
#define UART3_RX_FIFO_SIZE	(1472 + 1)


typedef enum {
	 USERUART_STS_OK =  0

	,USERUART_STS_HANDLE_NOT_EXIST = -1
	,USERUART_STS_HANDLE_CANNOT_REGISTER = -2
	,USERUART_STS_HANDLE_NOT_REGISTERED = -3
	,USERUART_STS_HANDLE_ALREADY_REGISTERED = -4
	,USERUART_STS_EROOR = -100
} USERUART_STS_T;


void USERUART_UnregisterAllHandle( void );
USERUART_STS_T USERUART_RegisterHandle( UART_HandleTypeDef* huart );
USERUART_STS_T USERUART_UnregisterHandle( UART_HandleTypeDef* huart );

int32_t USERUART_Read( UART_HandleTypeDef* huart, uint8_t readBuf[], uint16_t readSize, uint32_t timeoutMs );
int32_t USERUART_Write( UART_HandleTypeDef* huart, uint8_t writeBuf[], uint16_t writeSize, uint32_t timeoutMs );

int32_t USERUART_ReceiveCount( UART_HandleTypeDef* huart );



//callback functions
void USERUART_TxCompleteCallback( UART_HandleTypeDef *huart );
void USERUART_RxCompleteCallback( UART_HandleTypeDef *huart );
void USERUART_ErrorCallback( UART_HandleTypeDef *huart );



#endif	// endof : {#ifndef __USER_UART_H__}
