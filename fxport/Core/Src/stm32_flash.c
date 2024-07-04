/*
 * stm32_flash.c
 *
 *  Created on: 2024/07/04
 *      Author: a-yamamoto
 */

//---- Includes ------------------------------------------------------------------------------//
#include <assert.h>
#include <stdint.h>
//#include "stm32.h"
//#include "stm32_hal_def.h"
//#include "stm32_hal_conf.h"
//#include "stm32_hal_flash.h"
#include "mcu.h"
#include "mcu_flash.h"
//#include "mcu_clock.h"

//---- Definitions ---------------------------------------------------------------------------//
//const uint8_t const _DATA_FLASH[FLASH_PAGE_SIZE]  __attribute__ ((section (".data_flash_section")));

//---- Variable Definitions ------------------------------------------------------------------//
#if 0
//	DATA用FLASHメモリー
flash_record_t flash_records[FLASH_RECORD_COUNT];

//	メモリー配置はリンカスクリプトで行う
//	flash_recordsはSTM32G474RETX_FLASH.ldで領域定義
//	STM32G474RETX_FLASH.ld
//
//	MEMORY
//	{
//	  RAM    (xrw)    : ORIGIN = 0x20000000,   LENGTH = 18K
//	//FLASH    (rx)    : ORIGIN = 0x8000000,   LENGTH = 64K
//	  VEEPROM  (r)    : ORIGIN = 0x08000000,   LENGTH = 2K
//	  FLASH    (rx)   : ORIGIN = 0x08000800,   LENGTH = 62K
//	}
//
//  .flash_data:
//  {
//    flash_records = .;
//    . = . + LENGTH(FLASH_DATA);
//  } >FLASH_DATA


//---- Functions ----------------------------------------------------------------------------//

/*---------------------------------------------------------------------------------------------
//	void* Flash_LastRecord(void)
//
//	flash_records[]から最後に書き込まれたレコードを検索する
//	書き込まれたレコードが存在しない場合NULLを返す;
---------------------------------------------------------------------------------------------*/
void* Flash_LastRecord(void)
{
	flash_record_t* lastRecord = NULL;
	for(int n=0; n<FLASH_RECORD_COUNT; ++n)
	{
		switch(flash_records[n].flash_sign)
		{
		  case FLASH_SIGN_RECORD:
			lastRecord = &flash_records[n];
			break;
		  case FLASH_SIGN_WRITEABLE:
			return lastRecord;
		}
	}
	return lastRecord;
}

/*---------------------------------------------------------------------------------------------
//	void* Flash_NextRecord(void)
//
//	flash_records[]から次に書込み可能なレコードを検索する
---------------------------------------------------------------------------------------------*/
void* Flash_NextRecord(void)
{
	for(int n=0; n<FLASH_RECORD_COUNT; ++n)
	{
		switch(flash_records[n].flash_sign)
		{
		  case FLASH_SIGN_RECORD:
			break;
		  case FLASH_SIGN_WRITEABLE:
			return &flash_records[n];
		}
	}
	return &flash_records[0];
}

/*---------------------------------------------------------------------------------------------
//	HAL_StatusTypeDef Flash_Clear()
//
//	flash_records[]を消去する
---------------------------------------------------------------------------------------------*/
/*
static HAL_StatusTypeDef Flash_ClearPages()
{
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef EraseInitStruct;
    if (READ_BIT(FLASH->OPTR, FLASH_OPTR_DBANK) == 0U)
    {   //	0：128 ビットデータ読出し幅でのシングルバンクモード
        EraseInitStruct.TypeErase 	= FLASH_TYPEERASE_PAGES;
        EraseInitStruct.Banks		= FLASH_BANK_1;
        EraseInitStruct.Page		= 32;
        EraseInitStruct.NbPages		= 32;	// 4KB * 32 = 128KB
    }
    else
    {	//	1：64 ビットデータでのデュアルバンクモード
        EraseInitStruct.TypeErase 	= FLASH_TYPEERASE_PAGES;
        EraseInitStruct.Banks		= FLASH_BANK_1;
        EraseInitStruct.Page		= 64;
        EraseInitStruct.NbPages		= 64;	// 2KB * 64 = 128KB
    }

    // Eraseに失敗したsector番号がerror_sectorに入る
    // 正常にEraseができたときは0xFFFFFFFFが入る
    uint32_t error_sector;
    HAL_StatusTypeDef result = HAL_FLASHEx_Erase(&EraseInitStruct, &error_sector);

    HAL_FLASH_Lock();

    return result;
}
*/
/*---------------------------------------------------------------------------------------------
//	HAL_StatusTypeDef Flash_Store(const void* record, size_t size)
//
//	flash_records[]に新規レコードを書き込む
---------------------------------------------------------------------------------------------*/
HAL_StatusTypeDef Flash_Store(const void* record, size_t size)
{
	HAL_StatusTypeDef result;
	if (record == NULL || size > FLASH_RECORD_SIZE)
		return HAL_ERROR;

	flash_record_t*
	next_record = Flash_NextRecord();
	if (next_record->flash_sign != FLASH_SIGN_WRITEABLE){
		result = Flash_ErasePage(next_record);
		if (result != HAL_OK)
			return result;
	}
	result = Flash_WriteRecord(next_record, record, size);
	if (result != HAL_OK){
	//	Flash_WriteRecord(next_record, FLASH_SIGN_BADRECORD, NULL, 0);
		return result;
	}
	next_record++;
	if (next_record < &flash_records[FLASH_RECORD_COUNT] && next_record->flash_sign != FLASH_SIGN_WRITEABLE)
	{	//	直後のレコードが書き込み可能でなければERASEする
		result = Flash_ErasePage(next_record);
		if (result != HAL_OK)
			return result;
	}
	return result;
}
#endif

#ifdef	USE_ORIGINAL_HALL_DRIVER
/*---------------------------------------------------------------------------------------------
//	HAL_StatusTypeDef Flash_ErasePage(void* flashptr)
//
//	flash_records[]をページ単位で消去する
//	flashptrがページの先頭でない場合はHAL_ERRORを返す
---------------------------------------------------------------------------------------------*/
static HAL_StatusTypeDef Flash_ErasePage(void* flashptr)
{
//  #define PAGESIZE_SINGLEBANK		4096L	// BUG?
    #define PAGESIZE_SINGLEBANK		2048L
	#define PAGESIZE_DUALBANK		2048L
    uint32_t pagesize = (READ_BIT(FLASH->OPTR, FLASH_OPTR_DBANK) == 0U)?
					PAGESIZE_SINGLEBANK: PAGESIZE_DUALBANK;

    // アドレスがPage境界にない
    if ((uint32_t)flashptr % pagesize != 0L)
    	return HAL_ERROR;

    HAL_FLASH_Unlock();


    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase 	= FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks		= FLASH_BANK_1;
    EraseInitStruct.Page		= ((uint32_t)flashptr - FLASH_MEMORY_TOP) / pagesize;
    EraseInitStruct.NbPages		= 1;

    // Eraseに失敗したpage番号がpage_errorに入る
    // 正常にEraseができたときは0xFFFFFFFFが入る
    uint32_t page_error;
    HAL_StatusTypeDef result = HAL_FLASHEx_Erase(&EraseInitStruct, &page_error);

    HAL_FLASH_Lock();

    return result;
}



#else
/**
  * @brief  Wait for a FLASH operation to complete.
  * @param  Timeout maximum flash operation timeout
  * @retval HAL_StatusTypeDef HAL Status
  */

uint32_t stm32_FlashErrorCode = 0;

#define	FLASH_WaitForLastOperation(Timeout)	stm32_FLASH_WaitForLastOperation(Timeout)

static uint32_t stm32_FLASH_SR_BSY(void)
{
#ifndef FLASH_SR_BSY
	#if defined(FLASH_DBANK_SUPPORT)
		#define FLASH_SR_BSY  (FLASH_SR_BSY1 | FLASH_SR_BSY2)
	#else
		#define FLASH_SR_BSY  (FLASH_SR_BSY1)
	#endif /* FLASH_DBANK_SUPPORT */
#endif
    return (FLASH->SR & FLASH_SR_BSY);
}

static uint32_t stm32_FLASH_SR_CFGBSY(void)
{
    return (FLASH->SR & FLASH_SR_CFGBSY);
}

static status_t stm32_FLASH_WaitForLastOperation(uint32_t Timeout)
{
    /* Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
       Even if the FLASH operation fails, the BUSY flag will be reset and an error
       flag will be set */

    /* Wait if any operation is ongoing */
    if (WaitWhile(stm32_FLASH_SR_BSY ,Timeout) != STATUS_OK){
      return STATUS_TIMEOUT;
    }

    /* check flash errors */
    uint32_t error = (FLASH->SR & FLASH_SR_ERRORS);
    /* Clear SR register */
    FLASH->SR = FLASH_SR_CLEAR;

    if (error != 0x00U)
    {
        /*Save the error code*/
        stm32_FlashErrorCode = error;
        return STATUS_FLASH_ERROR;
    }
    return WaitWhile(stm32_FLASH_SR_CFGBSY ,Timeout);
}

/*---------------------------------------------------------------------------------------------
//	HAL_StatusTypeDef Flash_ErasePage(void* flashptr)
//
//	flash_records[]をページ単位で消去する
//	flashptrがページの先頭でない場合はHAL_ERRORを返す
---------------------------------------------------------------------------------------------*/
__STATIC_INLINE status_t stm32_FlashErasePage(uint32_t nFlashPage)
{
//  /* Process Locked */
//  __HAL_LOCK(&pFlash);

	/* Wait for last operation to be completed */
	if (FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE) != STATUS_OK)
		return STATUS_FLASH_ERROR;
	/* Start erase page */
	//FLASH_PageErase(FLASH_BANK_1, nFlashPage);
	_FLASH_PageErase(nFlashPage);
	if (FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE) != STATUS_OK)
		return STATUS_FLASH_ERASE_TIMEOUT;
	/* If operation is completed or interrupted, disable the Page Erase Bit */
	CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
//  /* Process Unlocked */
//  __HAL_UNLOCK(&pFlash);
	return STATUS_OK;
}


status_t stm32_FlashErase(const void* flashptr)
{
	_assert_param(((uint32_t)flashptr & (FLASH_PAGE_SIZE -1)) == 0LU);

    HAL_FLASH_Unlock();
    uint32_t nFlashPage	= FLASH_ADDRESS_TO_PAGE(flashptr);
    uint32_t status = stm32_FlashErasePage(nFlashPage);
    HAL_FLASH_Lock();

    return status;
}

/**
  * @brief  Program double-word (64-bit) at a specified address.
  * @param  Address Specifies the address to be programmed.
  * @param  Data Specifies the data to be programmed.
  * @retval None
  */
static void FLASH_Program_DoubleWord(uint32_t Address, uint64_t Data)
{
  /* Set PG bit */
  SET_BIT(FLASH->CR, FLASH_CR_PG);

  /* Program first word */
  *(uint32_t *)Address = (uint32_t)Data;

  /* Barrier to ensure programming is performed in 2 steps, in right order
    (independently of compiler optimization behavior) */
  __ISB();

  /* Program second word */
  *(uint32_t *)(Address + 4U) = (uint32_t)(Data >> 32U);
}

/*---------------------------------------------------------------------------------------------
//	void* Flash_NextRecord(void)
//
//	指定されたFLASHメモリーにデータを書き込む
---------------------------------------------------------------------------------------------*/
#define	HAL_FLASH_Program(TypeProgram, Address, Data)	stm32_HAL_FLASH_Program(Address, Data)

static status_t stm32_HAL_FLASH_Program(uint32_t Address, uint64_t Data)
{
	status_t status;
	stm32_FlashErrorCode = HAL_FLASH_ERROR_NONE;
	/* Wait for last operation to be completed */
	status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
	if (status == STATUS_OK)
	{
		/* Check the parameters */
		_assert_param(IS_FLASH_PROGRAM_ADDRESS(Address));

		/* Program double-word (64-bit) at a specified address */
		FLASH_Program_DoubleWord(Address, Data);
		/* Wait for last operation to be completed */
		status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

		/* If the program operation is completed, disable the PG or FSTPG Bit */
		CLEAR_BIT(FLASH->CR, FLASH_TYPEPROGRAM_DOUBLEWORD);
	}
	/* return status */
	return status;
}
#endif

status_t stm32_FlashWrite(void const* flashptr, void const* buffer, size_t buffer_length)
{
    status_t status = STATUS_OK;

    if (flashptr == NULL || buffer == NULL)
		return STATUS_FLASH_ERROR;

    HAL_FLASH_Unlock();
	do {
		// block_countを64bit単位に切り上げる
		const size_t block_count = (buffer_length + sizeof(uint64_t) - 1) >> 3;
	    uint64_t *dstptr = (uint64_t*)flashptr;
	    uint64_t *srcptr = (uint64_t*)buffer;

		// データを64bit単位で書き込む
		for (size_t block = 0; block < block_count; block++)
		{
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)dstptr++, *srcptr++) != STATUS_OK)
			{
				status = STATUS_FLASH_ERROR;
				break;
			}
		}
	} while(0);
    HAL_FLASH_Lock();
    return status;
}
