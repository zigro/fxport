/**
  ******************************************************************************
  * @file   stdint.h
  * @brief  AFX C/C++用 標準定義ヘッダファイル
  ******************************************************************************
  * @author A.Yamamoto, R&D Division, ART Finex Co.,Ltd.
  * @date   2023/06/01
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
  *	1.0.000		2023/06/01		A.Yamamoto		新規作成
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _AFX_WRAP_STDINT_H
#include_next <stdint.h>
#define _AFX_WRAP_STDINT_H

/* Includes ------------------------------------------------------------------*/
//#include <stddef.h>
//#include <stdint.h>
//#include <sys/_stdint.h>
//#include <assert.h>
//#include "cmsis_compiler.h"
//#include "test.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Typedef -------------------------------------------------------------------*/
#ifndef _BYTE_DECLARED
    //! byte型定義 unsigned 8bit
    typedef uint8_t			byte;
    #define _BYTE_DECLARED
#endif

#ifndef _WORD_DECLARED
//  typedef unsigned short  word;
    //! word型定義 unsigned 16bit
    typedef uint16_t		word;
    #define _WORD_DECLARED
#endif

#ifndef _WORD16_DECLARED
    //! 16bit word型定義
    typedef uint16_t		word16;
    #define _WORD16_DECLARED
#endif

#ifndef _DWORD_DECLARED
//  typedef unsigned short  word;
    //! word型定義 unsigned 16bit
    typedef uint32_t		dword;
    #define _DWORD_DECLARED
#endif

    #ifndef _WORD32_DECLARED
    //! 32bit word型定義
    typedef uint32_t		word32;
    #define _WORD32_DECLARED
#endif

#ifndef _WORD64_DECLARED
    //! 64bit word型定義
    typedef uint64_t		word64;
    #define _WORD64_DECLARED
#endif

#ifndef __SIZE_T_DECLARED
    //! 16bit size型定義
    typedef uint16_t		_size_t;
    #define __SIZE_T_DECLARED
#endif

#ifndef _BOOL_DECLARED
    //! bool型定義
    typedef	_Bool			bool;
    #define _BOOL_DECLARED
#endif

/* Macro ---------------------------------------------------------------------*/
#ifndef __weak__
    #define __weak__        __attribute__((weak))
#endif

#ifndef __WEAK_ALIAS
    #define __WEAK_ALIAS(f)        __attribute__((weak,alias(f)))
#endif

#ifndef __WEAK_DEFAULT
    extern void __Default_Handler(void);
    #define __WEAK_DEFAULT        __attribute__((weak,alias("__Default_Handler")))
#endif

#ifndef NULL
	#define	NULL			((void*)0)
#endif

#ifndef TRUE
	#define	TRUE			((bool)(0==0))
#endif

#ifndef FALSE
	#define	FALSE			((bool)(0!=0))
#endif


#ifdef __cplusplus
}
#endif

#endif /* _AFX_WRAP_STDINT_H */
