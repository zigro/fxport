/**
  ******************************************************************************
  * @file   mcu_flash.h
  * @brief  Header file of MCU flash definition.
  *         STM32C0x/G0x Microprocessor Enviroment Definitions
  ******************************************************************************
  * @author A.Yamamoto, R&D Division, ART Finex Co.,Ltd.
  * @date   2023/06/13
  ******************************************************************************
  * @attention
  * Copyright (c) 2010-2023, ART Finex Co.,Ltd.
  * All rights reserved.
  *
  * 本プログラムは株式会社アートファイネックスにより開発されたものであり
  * 著作権の全ては株式会社アートファイネックスに帰属する
  * 文書による許可なく、当該コードの全てあるいは一部を第３者に開示すること
  * または、当該コードの全てあるいは一部を改変、流用する事を禁ず
  *
  ******************************************************************************
  * @details 詳細説明
  * @note 改訂履歴
  *	Revision	Date			Auther			Description
  *	1.0.000		2023/06/13		A.Yamamoto		新規作成
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _MCU_FLASH_H
#define _MCU_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Private includes ----------------------------------------------------------*/
#include "mcu.h"
#include "status.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
#ifndef FLASH_PAGE_SIZE
#define	FLASH_PAGE_SIZE				2048
#endif
#define FLASH_PAGE_SHIFT				11		// 2048 = 2^11
#define FLASH_ADDRESS_TO_PAGE(p)		(((uint32_t)p - FLASH_BASE) >> FLASH_PAGE_SHIFT)
#define FLASH_TIMEOUT_VALUE     	1000U          /*!< FLASH Execution Timeout, 1 s */

/* Exported variable ------------------------------------------------------------*/
//extern const uint8_t const _DATA_FLASH[FLASH_PAGE_SIZE];//  __attribute__ ((section (".data_flash_section"))) ;
extern const void* __code_flash_end;
extern const void* __data_flash_start;
#define __DATA_FLASH		__data_flash_start
#define __UNUSED_FLASH		((void*)(((uint32_t)&__code_flash_end + (FLASH_PAGE_SIZE-1)) & ~(FLASH_PAGE_SIZE-1)))

/** @brief 指定されたFLASHメモリが未使用であるかの判定 */
__STATIC_INLINE bool stm32_FlashMemoryIsUnused(void* memory)
{
    return (*(uint64_t*)memory == 0xffffffffffffffffUL);
}
#define Flash_MemoryIsUnused(m)  stm32_FlashMemoryIsUnused(m)

/** @brief 指定されたFLASHメモリが消去済みであるかの判定 */
__STATIC_INLINE bool stm32_FlashMemoryIsErased(void* memory)
{
    return (*(uint64_t*)memory == 0x0000000000000000UL);
}
#define Flash_MemoryIsErased(m)  stm32_FlashMemoryIsErased(m)

//──────────────────────────────────────//
// Macro
//──────────────────────────────────────//
// #define		Flash_Status()						(fst & 0x30)
// #define		Flash_IsBusy()						(fst7 == 0)


//──────────────────────────────────────//
// Define
//──────────────────────────────────────//

// フラッシュメモリアドレス定義
#if 0
#define BLOCKA_ADDR			((unsigned char *)0x3000)	//BLOCK A first address
#define BLOCKA_END			((unsigned char *)0x33ff)	//BLOCK A last address
#define BLOCKB_ADDR			((unsigned char *)0x3400)	//BLOCK B first address
#define BLOCKB_END			((unsigned char *)0x37ff)	//BLOCK B last address
#define BLOCKC_ADDR			((unsigned char *)0x3800)	//BLOCK C first address
#define BLOCKC_END			((unsigned char *)0x3Bff)	//BLOCK C last address
#define BLOCKD_ADDR			((unsigned char *)0x3C00)	//BLOCK D first address
#define BLOCKD_END			((unsigned char *)0x3fff)	//BLOCK D last address
#else
#define BLOCKA_ADDR			(&__DATA_FLASH)	//BLOCK A first address
#endif

//フラッシュメモリアクセス　エラーコード
#define	FLASH_NOERROR		0x00						//エラーなし
#define FLASH_WRITE_ERR		0x10						//書込みエラー
#define	FLASH_ERASE_ERR		0x20						//消去エラー
#define FLASH_COMMAND_ERR	0x30						//コマンドエラー
#define	FLASH_NODATA		0x04						//フラッシュデータなし

//フラッシュメモリ領域定義
//#warning "#pragma	ADDRESS				_FLASH_BLOCKA		3000H		// BLOCKA"
//#pragma	ADDRESS				_FLASH_BLOCKA		3000H		// BLOCKA
//#pragma	ADDRESS				_FLASH_BLOCKB		3400H		// BLOCKB
//#pragma	ADDRESS				_FLASH_BLOCKC		3800H		// BLOCKC
//#pragma	ADDRESS				_FLASH_BLOCKD		3C00H		// BLOCKD

#define	NODATA				((unsigned long)0xffffffff)


//──────────────────────────────────────//
// フラッシュメモリアドレス定義
//──────────────────────────────────────//
//typedef unsigned char FlashBlock[1024];

//──────────────────────────────────────//
// 関数定義
//──────────────────────────────────────//

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■//
#if 1	//No memory for LOADER
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■//


// void 			_Flash_Initialize(void);
// #define			Flash_Initialize()			_Flash_Initialize()
#define	Flash_Initialize()

// unsigned char	_Flash_Erase(unsigned char *);
// #define	Flash_Erase(a1)		_Flash_Erase(a1)
status_t stm32_FlashErase(const void* flashptr);
#define	Flash_Erase(a1)		stm32_FlashErase(a1)

// unsigned char	_Flash_Write(unsigned char *, unsigned char *, unsigned);
// #define	Flash_Write(a1, a2, a3)	_Flash_Write(a1, a2, a3)
status_t stm32_FlashWrite(void const* flashptr, void const* buffer, size_t buffer_length);
#define	Flash_Write(a1, a2, a3)	stm32_FlashWrite(a1, a2, a3)



//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■//
#else	//Located memory for LOADER
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■//


void 			Flash_Initialize(void);
void 			_Flash_Initialize(void);

#pragma	PARAMETER	_Flash_Erase(             A0)
unsigned char		_Flash_Erase(unsigned char *);
#pragma	PARAMETER	Flash_Erase(             A0)
unsigned char		Flash_Erase(unsigned char *);

#pragma	PARAMETER	_Flash_Write(             A0,              A1,       R1)
unsigned char		_Flash_Write(unsigned char *, unsigned char *, unsigned);
#pragma	PARAMETER	Flash_Write(             A0,              A1,       R1)
unsigned char		Flash_Write(unsigned char *, unsigned char *, unsigned);


//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■//
#endif
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■//


#endif	//_R8C_FLASH_H_
