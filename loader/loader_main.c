/*
 * Copyright (c) 2015-2016, Renesas Electronics Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *   - Neither the name of Renesas nor the names of its contributors may be
 *     used to endorse or promote products derived from this software without
 *     specific prior written permission.
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

#include <stdint.h>
#include <string.h>
#include "rcar_def.h"
#include "reg_rcar_gen3.h"
#include "bit.h"
#include "debug.h"
#include "mmio.h"
#include "bl2_cpg_init.h"
#include "bl2_cpg_register.h"
#include "pfc_init.h"
#include "scif.h"
#include "rcar_version.h"
#if RCAR_LSI == RCAR_V3H
#include "../ddr/V3H/boot_init_dram.h"
#elif RCAR_LSI == RCAR_V3M
#include "../ddr/V3M/boot_init_dram.h"
#endif
#include "../qos/qos_init.h"

extern char _binary_cr7_tcm_loader_bin_start[];
extern char _binary_cr7_tcm_loader_bin_end[];
extern char _binary_cr7_tcm_loader_bin_size[];

/************************************************************************************************/
/*	Definitions 										*/
/************************************************************************************************/
/* Product Register */
#define PRR			(0xFFF00044U)
#define PRR_PRODUCT_MASK	(0x00007F00U)
#define PRR_CUT_MASK		(0x000000FFU)
#define PRR_MAJOR_MASK		(0x000000F0U)
#define PRR_MINOR_MASK		(0x0000000FU)
#define PRR_PRODUCT_SHIFT	(8U)
#define PRR_MAJOR_SHIFT		(4U)
#define PRR_MINOR_SHIFT		(0U)
#define PRR_MAJOR_OFFSET	(1U)
#define PRR_PRODUCT_H3		(0x00004F00U)		/* R-Car H3 */
#define PRR_PRODUCT_M3		(0x00005200U)		/* R-Car M3 */
#define PRR_PRODUCT_V3M		(0x00005400U)		/* R-Car V3M */
#define PRR_PRODUCT_V3H		(0x00005600U)		/* R-Car V3H */
#define PRR_PRODUCT_H3_ES_1_0	(0x00004F00U)		/* R-Car H3 ES1.0 */

/* MaskROM API */
typedef uint32_t(*ROM_GETLCS_API)(uint32_t *pLcs);
#if RCAR_LSI == RCAR_H3
#define ROM_GETLCS_API_ADDR	((ROM_GETLCS_API)0xEB1021B4U)
#elif RCAR_LSI == RCAR_M3
#define ROM_GETLCS_API_ADDR	((ROM_GETLCS_API)0xEB10415CU)
#elif RCAR_LSI == RCAR_V3M
#define ROM_GETLCS_API_ADDR	((ROM_GETLCS_API)0xEB10404CU)
#elif RCAR_LSI == RCAR_V3H
#define ROM_GETLCS_API_ADDR	((ROM_GETLCS_API)0xEB101940U)
#endif
#define LCS_CM			(0x0U)
#define LCS_DM			(0x1U)
#define LCS_SD			(0x3U)
#define LCS_SE			(0x5U)
#define LCS_FA			(0x7U)

/* R-Car Gen3 product check */
#if RCAR_LSI == RCAR_H3
#define TARGET_PRODUCT		RCAR_PRODUCT_H3
#define TARGET_NAME		"R-Car H3"
#elif RCAR_LSI == RCAR_M3
#define TARGET_PRODUCT		RCAR_PRODUCT_M3
#define TARGET_NAME		"R-Car M3"
#elif RCAR_LSI == RCAR_V3M
#define TARGET_PRODUCT		RCAR_PRODUCT_V3M
#define TARGET_NAME		"R-Car V3M"
#elif RCAR_LSI == RCAR_V3H
#define TARGET_PRODUCT		RCAR_PRODUCT_V3H
#define TARGET_NAME		"R-Car V3H"
#endif

/* CPG write protect registers */
#define	CPGWPR_PASSWORD		(0x5A5AFFFFU)
#define	CPGWPCR_PASSWORD	(0xA5A50000U)

/* RST Registers */
#define	RST_BASE		(0xE6160000U)
#define	RST_WDTRSTCR		(RST_BASE + 0x0054U)
#define	WDTRSTCR_PASSWORD	(0xA55A0000U)
#define	WDTRSTCR_RWDT_RSTMSK	((uint32_t)1U << 0U)

uint32_t loader_main(void)
{
	uint32_t reg;
	const char *str;
	const char *product_h3      = "H3";
	const char *product_m3      = "M3";
	const char *product_v3m     = "V3M";
	const char *product_v3h     = "V3H";
	const char *unknown         = "unknown";
#if 0
	const ROM_GETLCS_API ROM_GetLcs = ROM_GETLCS_API_ADDR;
	uint32_t lcs;
	const char *lcs_cm          = "CM";
	const char *lcs_dm          = "DM";
	const char *lcs_sd          = "SD";
	const char *lcs_se          = "SE";
	const char *lcs_fa          = "FA";
#endif

/*****************************************************************************
 *	Initialize Hardware
 *****************************************************************************/
	pfc_init();			/* Pin function initialize */
	bl2_cpg_init();			/* CPG initialize */
	console_init();			/* SCIF initialize */

	/* boot message */
	NOTICE("R-Car Gen3 Initial Program Loader(CR7) Rev.%s\n", version_of_renesas);

	reg = mmio_read_32(PRR);
	switch (reg & PRR_PRODUCT_MASK) {
	case PRR_PRODUCT_H3:
		str = product_h3;
		break;
	case PRR_PRODUCT_M3:
		str = product_m3;
		break;
	case PRR_PRODUCT_V3M:
		str = product_v3m;
		break;
	case PRR_PRODUCT_V3H:
		str = product_v3h;
		break;
	default:
		str = unknown;
		break;
	}
	NOTICE("PRR is R-Car %s ES%d.%d\n", str,
		((reg & PRR_MAJOR_MASK) >> PRR_MAJOR_SHIFT) + PRR_MAJOR_OFFSET,
		 (reg & PRR_MINOR_MASK));
	if((reg & RCAR_PRODUCT_MASK) != TARGET_PRODUCT) {
		ERROR("This IPL has been built for the %s.\n", TARGET_NAME);
		ERROR("Please write the correct IPL to flash memory.\n");
		panic();
	}

#if 0
	reg = ROM_GetLcs(&lcs);
	if (reg == 0U) {
		switch (lcs) {
		case LCS_CM:
			str = lcs_cm;
			break;
		case LCS_DM:
			str = lcs_dm;
			break;
		case LCS_SD:
			str = lcs_sd;
			break;
		case LCS_SE:
			str = lcs_se;
			break;
		case LCS_FA:
			str = lcs_fa;
			break;
		default:
			str = unknown;
			break;
		}
	} else {
		str = unknown;
	}
	NOTICE("LCM state is %s\n", str);
#endif

	InitDram();			/* DDR initialize */
	qos_init();			/* QoS configuration */

/*****************************************************************************
 *	Unmask the detection of RWDT overflow
 *****************************************************************************/
	reg = mmio_read_32(RST_WDTRSTCR);
	reg &= ~WDTRSTCR_RWDT_RSTMSK;
	reg |= WDTRSTCR_PASSWORD;
	mmio_write_32(RST_WDTRSTCR, reg);

/*****************************************************************************
 *	Release CPG write protect
 *****************************************************************************/
	mmio_write_32(CPG_CPGWPR, CPGWPR_PASSWORD);
	mmio_write_32(CPG_CPGWPCR, CPGWPCR_PASSWORD);

	return 0U;
}
