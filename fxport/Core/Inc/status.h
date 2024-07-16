/**
  ******************************************************************************
  * @file   status.h
  * @brief  status_t定義ヘッダファイル
  ******************************************************************************
  * @author A.Yamamoto, R&D Division, ART Finex Co.,Ltd.
  * @date   2023/06/08
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  * @details 詳細説明
  * C/C++で使用するAFX独自の型、定数、マクロ定義
  * ※機種依存コードは含めないこと
  *
  * @note 改訂履歴
  *	Revision	Date			Auther			Description
  *	1.0.000		2023/06/08		A.Yamamoto		新規作成
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _STATUS_H
#define _STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Typedef -------------------------------------------------------------------*/
typedef enum _STATUS_T
{
        STATUS_OK       = 0x00U,
        STATUS_ERROR    = 0x01U,
        STATUS_BUSY     = 0x02U,
        STATUS_TIMEOUT  = 0x03U,
        STATUS_HARDFAULT	= 0xFAU,

        STATUS_CLOCK_ERROR = 0x0101U,
        STATUS_CLOCK_HSION_TIMEOUT,
        STATUS_CLOCK_HSEON_TIMEOUT,
        STATUS_CLOCK_PLLON_TIMEOUT,
        STATUS_CLOCKSRC_HSI_TIMEOUT,
        STATUS_CLOCKSRC_HSE_TIMEOUT,
        STATUS_CLOCKSRC_PLL_TIMEOUT,

        STATUS_SCI_ERROR = 0x0201U,
        STATUS_SCI_EMPTY,
        STATUS_SCI_BUSY,
        STATUS_SCI_ILLIGAL_PARAM_DATAWIDTH,
        STATUS_SCI_ILLIGAL_PARAM_PARITY,
        STATUS_SCI_ILLIGAL_PARAM_STOPBITS,
        STATUS_SCI_ILLIGAL_PARAM_BAUDRATE,

        STATUS_FLASH_ERROR = 0x0301U,
        STATUS_FLASH_ERASE_TIMEOUT,

        STATUS_MNGPRM_ERROR = 0x0401U,
        //* ManageParam 格納用FLASHメモリにレコードが存在しない
        STATUS_MNGPRM_UNINITIALIZED,
        //* ManageParam 格納用FLASHメモリに未使用レコードが存在しない
        STATUS_MNGPRM_NOSPACE,
} status_t;

typedef enum _STATUS_T
{
        STATUS_OK       = 0x00U,
        STATUS_ERROR    = 0x01U,
        STATUS_BUSY     = 0x02U,
        STATUS_TIMEOUT  = 0x03U,
        STATUS_HARDFAULT	= 0xFAU,

        STATUS_CLOCK_ERROR = 0x0101U,
        STATUS_CLOCK_HSION_TIMEOUT,
        STATUS_CLOCK_HSEON_TIMEOUT,
        STATUS_CLOCK_PLLON_TIMEOUT,
        STATUS_CLOCKSRC_HSI_TIMEOUT,
        STATUS_CLOCKSRC_HSE_TIMEOUT,
        STATUS_CLOCKSRC_PLL_TIMEOUT,

        STATUS_SCI_ERROR = 0x0201U,
        STATUS_SCI_EMPTY,
        STATUS_SCI_BUSY,
        STATUS_SCI_ILLIGAL_PARAM_DATAWIDTH,
        STATUS_SCI_ILLIGAL_PARAM_PARITY,
        STATUS_SCI_ILLIGAL_PARAM_STOPBITS,
        STATUS_SCI_ILLIGAL_PARAM_BAUDRATE,

        STATUS_FLASH_ERROR = 0x0301U,
        STATUS_FLASH_ERASE_TIMEOUT,

        STATUS_MNGPRM_ERROR = 0x0401U,
        //* ManageParam 格納用FLASHメモリにレコードが存在しない
        STATUS_MNGPRM_UNINITIALIZED,
        //* ManageParam 格納用FLASHメモリに未使用レコードが存在しない
        STATUS_MNGPRM_NOSPACE,
} status_t2;


#ifdef __cplusplus
}
#endif

#endif /* _STATUS_H */
