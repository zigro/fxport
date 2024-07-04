#include <stddef.h>
#include "user_fifo.h"


//#define __USE_FIFO_BUSY_FLAG__


int16_t FIFO_Init( FIFO_HANDLE_T* hfifo, uint8_t buf[], uint16_t bufSize )
{
	if ( hfifo == NULL )
	{
		return FIFO_STS_INIT_PARM_ERR;
	}

	if ( (buf == NULL) || (bufSize < 2) )
	{
		return FIFO_STS_INIT_PARM_ERR;
	}

	hfifo->Buf = buf;
	hfifo->BufSize = bufSize;
	hfifo->Ridx = 0;
	hfifo->Widx = 0;
	hfifo->Flag = 0;

	return FIFO_STS_OK;
}

int16_t FIFO_Push( FIFO_HANDLE_T* hfifo, uint8_t dat )
{
	if ( hfifo == NULL )
	{
		return FIFO_STS_PARM_ERR;
	}

	//busy
#if defined(__USE_FIFO_BUSY_FLAG__)
	if ( hfifo->Flag&FIFO_FLAG_MASK_BUSY )
	{
		return FIFO_STS_BUSY;
	}

	//set busy flag
	hfifo->Flag |= FIFO_FLAG_MASK_BUSY;
#endif//__USE_FIFO_BUSY_FLAG__

	uint16_t widx = hfifo->Widx + 1;

	//overflow write index
	if ( widx >= hfifo->BufSize )
	{
		widx = 0;
	}

	//check FIFO full
	if ( widx == hfifo->Ridx )
	{
		//clear busy flag
#if defined(__USE_FIFO_BUSY_FLAG__)
		hfifo->Flag &= ~FIFO_FLAG_MASK_BUSY;
#endif//__USE_FIFO_BUSY_FLAG__

		return FIFO_STS_OVERFLOW;
	}

	//add data
	hfifo->Buf[hfifo->Widx] = dat;
	hfifo->Widx = widx;

	//clear busy flag
#if defined(__USE_FIFO_BUSY_FLAG__)
	hfifo->Flag &= ~FIFO_FLAG_MASK_BUSY;
#endif//__USE_FIFO_BUSY_FLAG__

	return FIFO_STS_OK;
}


int16_t FIFO_Push2( FIFO_HANDLE_T* hfifo, uint8_t dat[], uint16_t datLen )
{
	if ( (hfifo == NULL) || (dat == NULL) || (datLen == 0) )
	{
		return FIFO_STS_PARM_ERR;
	}

	//busy
#if defined(__USE_FIFO_BUSY_FLAG__)
	if ( hfifo->Flag&FIFO_FLAG_MASK_BUSY )
	{
		return FIFO_STS_BUSY;
	}

	//set busy flag
	hfifo->Flag |= FIFO_FLAG_MASK_BUSY;
#endif//__USE_FIFO_BUSY_FLAG__

	uint16_t datIdx;
	uint16_t widx;

	for ( datIdx=0 ; datIdx<datLen ; datIdx++ )
	{
		widx = hfifo->Widx + 1;

		//overflow write index
		if ( widx >= hfifo->BufSize )
		{
			widx = 0;
		}

		//check FIFO full
		if ( widx == hfifo->Ridx )
		{
			//clear busy flag
#if defined(__USE_FIFO_BUSY_FLAG__)
			hfifo->Flag &= ~FIFO_FLAG_MASK_BUSY;
#endif//__USE_FIFO_BUSY_FLAG__

			return FIFO_STS_OVERFLOW;
		}

		//push data
		hfifo->Buf[hfifo->Widx] = dat;
		hfifo->Widx = widx;
	}

	//clear busy flag
#if defined(__USE_FIFO_BUSY_FLAG__)
	hfifo->Flag &= ~FIFO_FLAG_MASK_BUSY;
#endif//__USE_FIFO_BUSY_FLAG__

	return FIFO_STS_OK;
}


int16_t FIFO_Pop( FIFO_HANDLE_T* hfifo, uint8_t* buf )
{
	if ( (hfifo == NULL) || (buf == NULL) )
	{
		return FIFO_STS_PARM_ERR;
	}

	//busy
#if defined(__USE_FIFO_BUSY_FLAG__)
	if ( hfifo->Flag&FIFO_FLAG_MASK_BUSY )
	{
		return FIFO_STS_BUSY;
	}

	//set busy flag
	hfifo->Flag |= FIFO_FLAG_MASK_BUSY;
#endif//__USE_FIFO_BUSY_FLAG__

	//check FIFO empty
	if ( hfifo->Ridx == hfifo->Widx )
	{
		//clear busy flag
#if defined(__USE_FIFO_BUSY_FLAG__)
		hfifo->Flag &= ~FIFO_FLAG_MASK_BUSY;
#endif//__USE_FIFO_BUSY_FLAG__

		return FIFO_STS_EMPTY;
	}

	//pop data
	*buf = hfifo->Buf[hfifo->Ridx];
	if ( ++hfifo->Ridx >= hfifo->BufSize )
	{
		hfifo->Ridx = 0;
	}

	//clear busy flag
#if defined(__USE_FIFO_BUSY_FLAG__)
	hfifo->Flag &= ~FIFO_FLAG_MASK_BUSY;
#endif//__USE_FIFO_BUSY_FLAG__

	return FIFO_STS_OK;
}


int16_t FIFO_Clear( FIFO_HANDLE_T* hfifo )
{
	if ( hfifo == NULL )
	{
		return FIFO_STS_PARM_ERR;
	}

	//busy
#if defined(__USE_FIFO_BUSY_FLAG__)
	if ( hfifo->Flag&FIFO_FLAG_MASK_BUSY )
	{
		return FIFO_STS_BUSY;
	}

	//set busy flag
	hfifo->Flag |= FIFO_FLAG_MASK_BUSY;
#endif//__USE_FIFO_BUSY_FLAG__

	hfifo->Ridx = 0;
	hfifo->Widx = 0;

	//clear busy flag
#if defined(__USE_FIFO_BUSY_FLAG__)
	hfifo->Flag &= ~FIFO_FLAG_MASK_BUSY;
#endif//__USE_FIFO_BUSY_FLAG__

	return FIFO_STS_OK;
}


int32_t FIFO_Count( FIFO_HANDLE_T* hfifo )
{
	if ( hfifo == NULL )
	{
		return FIFO_STS_PARM_ERR;
	}

	// write index >= read index
	if ( hfifo->Widx >= hfifo->Ridx )
	{
		return hfifo->Widx - hfifo->Ridx;
	}

	// write index < read index
	return hfifo->BufSize - hfifo->Ridx + hfifo->Widx;
}

