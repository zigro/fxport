#ifndef __USER_FIFO_H__
#define __USER_FIFO_H__


#include <stdint.h>


#define FIFO_STS_OK				 0
#define FIFO_STS_BUSY			-1
#define FIFO_STS_EMPTY			-2
#define FIFO_STS_OVERFLOW		-3		//FIFO full or overrun
#define FIFO_STS_PARM_ERR		-4
#define FIFO_STS_INIT_PARM_ERR	-5


#define FIFO_FLAG_MASK_BUSY		0x01


typedef struct {
	uint8_t*	Buf;		//FIFO buffer
	uint16_t	BufSize;	//FIFO buffer size
	uint16_t	Ridx;		//read index
	uint16_t	Widx;		//write index
	uint8_t		Flag;		//status flag	b0:busy, b1-b7:(RFU)
} FIFO_HANDLE_T;


int16_t FIFO_Init( FIFO_HANDLE_T* hfifo, uint8_t buf[], uint16_t bufSize );
int16_t FIFO_Push( FIFO_HANDLE_T* hfifo, uint8_t dat );
int16_t FIFO_Push2( FIFO_HANDLE_T* hfifo, uint8_t dat[], uint16_t datLen );
int16_t FIFO_Pop( FIFO_HANDLE_T* hfifo, uint8_t* buf );
int16_t FIFO_Clear( FIFO_HANDLE_T* hfifo );
int32_t FIFO_Count( FIFO_HANDLE_T* hfifo );


#endif	//endof : #ifndef __USER_FIFO_H__
