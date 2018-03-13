/*
 * Copyright (c) 2014, ARM Limited and Contributors. All rights reserved.
 * Copyright (c) 2015-2016, Renesas Electronics Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef RCAR_DEF_H__
#define RCAR_DEF_H__

/* Product register	*/
#define	RCAR_PRR			(0xFFF00044U)	/* Product register */

/*******************************************************************************
 *  RCAR product and cut information
 ******************************************************************************/
#define RCAR_PRODUCT_MASK		(0x00007F00U)
#define RCAR_CUT_MASK			(0x000000FFU)
#define RCAR_PRODUCT_H3			(0x00004F00U)
#define RCAR_PRODUCT_M3			(0x00005200U)
#define RCAR_PRODUCT_V3M		(0x00005400U)
#define RCAR_PRODUCT_V3H		(0x00005600U)
#define RCAR_CUT_ES10			(0x00000000U)
#define RCAR_CUT_ES11			(0x00000001U)
#define RCAR_MAJOR_MASK			(0x000000F0U)
#define RCAR_MINOR_MASK			(0x0000000FU)
#define RCAR_PRODUCT_SHIFT		(8U)
#define RCAR_MAJOR_SHIFT		(4U)
#define RCAR_MINOR_SHIFT		(0U)
#define RCAR_MAJOR_OFFSET		(1U)
#define RCAR_PRODUCT_H3_CUT10		(RCAR_PRODUCT_H3 | 0x00U) /* H3 WS1.0 */
#define RCAR_PRODUCT_H3_CUT11		(RCAR_PRODUCT_H3 | 0x01U) /* H3 WS1.1 */
#define RCAR_PRODUCT_M3_CUT10		(RCAR_PRODUCT_M3 | 0x00U) /* M3 */

/*******************************************************************************
 *  RCAR MD pin information
 ******************************************************************************/
#define MODEMR_BOOT_CPU_MASK		(0x000000C0U)
#define MODEMR_BOOT_CPU_CR7		(0x000000C0U)
#define MODEMR_BOOT_CPU_CA57		(0x00000000U)
#define MODEMR_BOOT_CPU_CA53		(0x00000040U)

/******************************************************************************/
/* Memory mapped Generic timer interfaces  */
/******************************************************************************/
#define ARM_SYS_CNTCTL_BASE		RCAR_CNTC_BASE

/*******************************************************************************
 *  Boot CPU
 ******************************************************************************/
#define	RCAR_BOOT_CA5X			(0U)	/* Master boot CPU is CA57/53 */
#define	RCAR_BOOT_CR7			(1U)	/* Master boot CPU is CR7 */

/*******************************************************************************
 *  Shared Data
 ******************************************************************************/

#endif /* RCAR_DEF_H__ */
