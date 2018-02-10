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
#include "ipmmu.h"
#include "mmio.h"
#include "debug.h"
#include "reg_rcar_gen3.h"

/************************************************************************/
/*	Definitions							*/
/************************************************************************/

/* Internal RAM base address */
#define	SYSRAM_BASE		(0xE6300000U_XX)

/* AXI-Bus registers definitions */
#define AXI_BASE		(0xE6784000U)
#define AXI_FUSACR		(AXI_BASE + 0x0020U)

#define FUSACR_EFUSASEL(x)	(((x) & 0xFFU) << 24U)
#define FUSACR_DFUSASEL(x)	(((x) & 0xFFU) << 16U)
#define FUSACR_SFUSASEL(x)	(((x) & 0x3FU) << 0U)

/* IPMMU registers definitions */
#define MMU0			(0x00U)		/* MMU0 */
#define MMU1			(0x01U)		/* MMU1 */
#define MMU2			(0x02U)		/* MMU2 */
#define MMU3			(0x03U)		/* MMU3 */
#define MMU4			(0x04U)		/* MMU4 */
#define MMU5			(0x05U)		/* MMU5 */
#define MMU6			(0x06U)		/* MMU6 */
#define MMU7			(0x07U)		/* MMU7 */
#define MAX_MMU			(8U)
#define MAX_UTLB		(20U)
#define UTLB_SH4A		(3U)		/* uTLB for SH-4A*/
#define MMU_NO			MMU0		/* Use MMU No.*/

#define IPMMU_MM_BASE		(0xE67B0000U)	/* IPMMU-MM */
#define IPMMU_RT_BASE		(0xFFC80000U)	/* IPMMU-RT */
#define SEC			(0x0000U)
#define NSEC			(0x0800U)

#define IMCTR_MM(s, x)		(IPMMU_MM_BASE + (s) + 0x0000U + ((x) * 0x40U))
#define IMTTBCR(s, x)		(IPMMU_MM_BASE + (s) + 0x0008U + ((x) * 0x40U))
#define IMBUSCR(s, x)		(IPMMU_MM_BASE + (s) + 0x000CU + ((x) * 0x40U))
#define IMTTLBR0(s, x)		(IPMMU_MM_BASE + (s) + 0x0010U + ((x) * 0x40U))
#define IMTTUBR0(s, x)		(IPMMU_MM_BASE + (s) + 0x0014U + ((x) * 0x40U))
#define IMTTLBR1(s, x)		(IPMMU_MM_BASE + (s) + 0x0018U + ((x) * 0x40U))
#define IMTTUBR1(s, x)		(IPMMU_MM_BASE + (s) + 0x001CU + ((x) * 0x40U))
#define IMSTR(s, x)		(IPMMU_MM_BASE + (s) + 0x0020U + ((x) * 0x40U))
#define IMMAIR0(s, x)		(IPMMU_MM_BASE + (s) + 0x0028U + ((x) * 0x40U))
#define IMMAIR1(s, x)		(IPMMU_MM_BASE + (s) + 0x002CU + ((x) * 0x40U))
#define IMCTR_RT(s, x)		(IPMMU_RT_BASE + (s) + 0x0000U + ((x) * 0x40U))
#define IMEAR(s, x)		(IPMMU_RT_BASE + (s) + 0x0030U + ((x) * 0x40U))
#define IMUCTR(s, x)		(IPMMU_RT_BASE + (s) + 0x0300U + ((x) * 0x10U))
#define IMUASID(s, x)		(IPMMU_RT_BASE + (s) + 0x0308U + ((x) * 0x10U))
#define IMSSTR			(IPMMU_RT_BASE + (s) + 0x0540U)
#define IMRAM0ERRCTR		(IPMMU_RT_BASE + (s) + 0x0560U)
#define IMRAM0ERRSTR		(IPMMU_RT_BASE + (s) + 0x0564U)
#define IMRAM1ERRCTR		(IPMMU_RT_BASE + (s) + 0x0568U)
#define IMRAM1ERRSTR		(IPMMU_RT_BASE + (s) + 0x056CU)

#define IPMMU_REGGUARD		(0xFFC50000U + 0x0808U)
#define IPMMU_FIX_TARGET	(0xFFC50000U + 0x0000U)
#define IPMMU_QOS_0		(0xFFC50000U + 0x0200U)

#define IMCTR_V64		(1U << 28U)
#define IMCTR_TRE		(1U << 17U)
#define IMCTR_AFE		(1U << 16U)
#define IMCTR_RTSEL(x)		(((x) & 0x07U) << 4U)
#define IMCTR_TREN		(1U << 3U)
#define IMCTR_INTEN		(1U << 2U)
#define IMCTR_FLUSH		(1U << 1U)
#define IMCTR_MMUEN		(1U << 0U)

#define IMTTBCR_EAE		(1U << 31U)
#define IMTTBCR_PMB		(1U << 30U)
#define IMTTBCR_SH1_NS		(SH_NS << 28U)
#define IMTTBCR_SH1_OS		(SH_OS << 28U)
#define IMTTBCR_SH1_IS		(SH_IS << 28U)
#define IMTTBCR_ORGN1_NC	(NC << 26U)
#define IMTTBCR_ORGN1_WBWA	(WBWA << 26U)
#define IMTTBCR_ORGN1_WT	(WT << 26U)
#define IMTTBCR_ORGN1_WB	(WB << 26U)
#define IMTTBCR_IRGN1_NC	(NC << 24U)
#define IMTTBCR_IRGN1_WBWA	(WBWA << 24U)
#define IMTTBCR_IRGN1_WT	(WT << 24U)
#define IMTTBCR_IRGN1_WB	(WB << 24U)
#define IMTTBCR_PGSZ		(1U << 23U)
#define IMTTBCR_SCSZ		(1U << 22U)
#define IMTTBCR_TSZ1(x)		(((x) & 0x3FU) << 16U)
#define IMTTBCR_SH0_NS		(SH_NS << 12U)
#define IMTTBCR_SH0_OS		(SH_OS << 12U)
#define IMTTBCR_SH0_IS		(SH_IS << 12U)
#define IMTTBCR_ORGN0_NC	(NC << 10U)
#define IMTTBCR_ORGN0_WBWA	(WBWA << 10U)
#define IMTTBCR_ORGN0_WT	(WT << 10U)
#define IMTTBCR_ORGN0_WB	(WB << 10U)
#define IMTTBCR_IRGN0_NC	(NC << 8U)
#define IMTTBCR_IRGN0_WBWA	(WBWA << 8U)
#define IMTTBCR_IRGN0_WT	(WT << 8U)
#define IMTTBCR_IRGN0_WB	(WB << 8U)
#define IMTTBCR_SL_3RD		(0U << 6U)	/* Start at third level */
#define IMTTBCR_SL_2ND		(1U << 6U)	/* Start at second level */
#define IMTTBCR_SL_1ST		(2U << 6U)	/* Start at first level */
#define IMTTBCR_TSZ0(x)		(((x) & 0x3FU) << 0U)

#define IMBUSCR_CCEN		(1U << 0U)

#define IMMAIR1_ATTR7(x)	(((x) & 0xFFU) << 24U)
#define IMMAIR1_ATTR6(x)	(((x) & 0xFFU) << 16U)
#define IMMAIR1_ATTR5(x)	(((x) & 0xFFU) << 8U)
#define IMMAIR1_ATTR4(x)	(((x) & 0xFFU) << 0U)
#define IMMAIR0_ATTR3(x)	(((x) & 0xFFU) << 24U)
#define IMMAIR0_ATTR2(x)	(((x) & 0xFFU) << 16U)
#define IMMAIR0_ATTR1(x)	(((x) & 0xFFU) << 8U)
#define IMMAIR0_ATTR0(x)	(((x) & 0xFFU) << 0U)

#define IMUCTR_FIXADDEN		(1U << 31)
#define IMUCTR_FIXADD(x)	(((x) & 0xFFU) << 16U)
#define IMUCTR_TTSEL(x)		(((x) & 0x0FU) << 4U)
#define IMUCTR_FLUSH		(1U << 1U)
#define IMUCTR_MMUEN		(1U << 0U)

#define IMUASID_ASID1(x)	((x) << 8U)
#define IMUASID_ASID0(x)	((x) << 0U)

/* IPMMU table definitions */
#define MMU_L1_TABLE_ADDR	(SYSRAM_BASE + 0x3000U)	/* 4KB aligned */
#define MMU_L2_TABLE_ADDR	(SYSRAM_BASE + 0x2000U)	/* 4KB aligned */
#define MMU_L3_TABLE_ADDR	(SYSRAM_BASE + 0x1000U)	/* 4KB aligned */

#define MMU_L1_TABLE_SIZE	(4U)
#define MMU_L2_TABLE_SIZE	(512U)
#define MMU_L3_TABLE_SIZE	(512U)

#define DESCRIPTER_INVALID	(0x00000000U)		/* [0] */
#define DESCRIPTER_BLOCK	(0x00000001U)		/* [1:0] */
#define DESCRIPTER_TABLE	(0x00000003U)		/* [1:0] */

#define LEVEL1_ADDR(x)		((x) & 0xC0000000U)	/* [39:30] */
#define LEVEL2_ADDR(x)		((x) & 0xFFE00000U)	/* [39:21] */
#define LEVEL3_ADDR(x)		((x) & 0xFFFFF000U)	/* [39:12] */
#define NEXT_LEVEL_ADDR(x)	((x) & 0xFFFFF000U)	/* [39:12] */

#define LEVEL1_ADDR_INC		(0x40000000U)		/* 1GB region */
#define LEVEL2_ADDR_INC		(0x00200000U)		/* 2MB region */
#define LEVEL3_ADDR_INC		(0x00001000U)		/* 4KB region */

#define LEVEL2_ADDR_BASE	(LEVEL1_ADDR_INC * 3U)	/* 0xC0000000 */
#define LEVEL3_ADDR_BASE	(LEVEL2_ADDR_BASE + LEVEL2_ADDR_INC * 305U)

#define NEXT_NS_TABLE		(1ULL << 63U)
#define NEXT_AP_TABLE_PL1	(1ULL << 61U)		/* Read/write at PL1 */
#define NEXT_AP_TABLE_RD	(2ULL << 61U)		/* Read/write at any */
#define NEXT_AP_TABLE_RD_PL1	(3ULL << 61U)		/* Read-only at PL1 */
#define NEXT_XN_TABLE		(1ULL << 60U)
#define NEXT_PXN_TABLE		(1ULL << 59U)

#define LATTR_NG		(1U << 11U)
#define LATTR_AF		(1U << 10U)
#define LATTR_SH(x)		(((x) & 0x00000003U) << 8U)
#define LATTR_AP(x)		(((x) & 0x00000003U) << 6U)
#define LATTR_NS		(1U << 5U)
#define LATTR_INDEX(x)		(((x) & 0x00000007U) << 2U)

#define AP_RW_PL1		(0U)			/* Read/write at PL1 */
#define AP_RW_ANY		(1U)			/* Read/write at any */
#define AP_RD_PL1		(2U)			/* Read-only at PL1 */
#define AP_RD_ANY		(3U)			/* Read-only at any */

#define ATTR0			(0U)			/* MAIR0.ATTR0 */
#define ATTR1			(1U)			/* MAIR0.ATTR1 */
#define ATTR2			(2U)			/* MAIR0.ATTR2 */
#define ATTR3			(3U)			/* MAIR0.ATTR3 */
#define ATTR4			(4U)			/* MAIR1.ATTR4 */
#define ATTR5			(5U)			/* MAIR1.ATTR5 */
#define ATTR6			(6U)			/* MAIR1.ATTR6 */
#define ATTR7			(7U)			/* MAIR1.ATTR7 */

#define NC			(0U)			/* Non-cacheable */
#define WBWA			(1U)			/* Write-Back Write-Allocate */
#define WT			(2U)			/* Write-Through */
#define WB			(3U)			/* Write-Back */

#define SH_NS			(0U)			/* Non-shareable */
#define SH_OS			(2U)			/* Outer Shareable */
#define SH_IS			(3U)			/* Inner Shareable */


static inline void dsb(void)
{
	__asm__ __volatile__ ("dsb sy");

}

/************************************************************************/
/*	System RAM ECC enable						*/
/************************************************************************/
void ecc_init(void)
{
#if RCAR_SYSRAM_ECC_ENABLE == 1
	/* System RAM ECC enable */
	mmio_write_32(AXI_FUSACR,  FUSACR_SFUSASEL(0x3FU));
	dsb();
	NOTICE("System RAM ECC enable(CR7)\n");
#endif
}

/************************************************************************/
/*	IPMMU-MM/IPMMU-RT enable					*/
/************************************************************************/
void ipmmu_init(void)
{
#if RCAR_SYSRAM_IPMMU_ENABLE == 1
	uint32_t i;
	uintptr_t p;
	uint64_t *lp;

	/* Generate IPMMU Level1 table */
	lp = (uint64_t *)MMU_L1_TABLE_ADDR;

	*lp = 	LEVEL1_ADDR(LEVEL1_ADDR_INC * 0U)	/* 0x00000000 */
			+ LATTR_SH(SH_NS)
			+ LATTR_AP(AP_RW_ANY)
			+ LATTR_INDEX(0U)
			+ DESCRIPTER_BLOCK;
	lp++;

	*lp = 	LEVEL1_ADDR(LEVEL1_ADDR_INC * 1U)	/* 0x40000000 */
			+ LATTR_SH(SH_NS)
			+ LATTR_AP(AP_RW_ANY)
			+ LATTR_INDEX(0U) 
			+ DESCRIPTER_BLOCK;
	lp++;

	*lp = 	LEVEL1_ADDR(LEVEL1_ADDR_INC * 2U)	/* 0x80000000 */
			+ LATTR_SH(SH_NS)
			+ LATTR_AP(AP_RW_ANY)
			+ LATTR_INDEX(0U)
			+ DESCRIPTER_BLOCK;
	lp++;

	*lp = 	NEXT_LEVEL_ADDR(MMU_L2_TABLE_ADDR)	/* pointer to Level2 */
			+ DESCRIPTER_TABLE;

	/* Generate IPMMU Level2 table */
	p = LEVEL2_ADDR_BASE;
	lp = (uint64_t *)MMU_L2_TABLE_ADDR;

	for (i =0U; i < MMU_L2_TABLE_SIZE; i++) {
		*lp = LEVEL2_ADDR(p)
			+ LATTR_SH(SH_NS)
			+ LATTR_AP(AP_RW_ANY)
			+ LATTR_INDEX(ATTR0)
			+ DESCRIPTER_BLOCK;
		p += LEVEL2_ADDR_INC;
		lp++;
	}

	lp = (uint64_t *)(MMU_L2_TABLE_ADDR + (sizeof(uint64_t) * 305U));
	*lp = NEXT_LEVEL_ADDR(MMU_L3_TABLE_ADDR)	/* pointer to level3 */
			+ DESCRIPTER_TABLE;

	/* Generate IPMMU Level3 table */
	lp = (uint64_t *)MMU_L3_TABLE_ADDR;

	p = LEVEL3_ADDR_BASE;
	for (i =0U; i <= 255U; i++) {		/* 0xE6200000 - 0xE62FE000 */
		*lp = LEVEL3_ADDR(p)
			+ LATTR_SH(SH_NS)
			+ LATTR_AP(AP_RW_ANY)
			+ LATTR_INDEX(ATTR0)
			+ DESCRIPTER_TABLE;
		p += LEVEL3_ADDR_INC;
		lp++;
	}

	for (i =256U; i <= 303U; i++) {		/* 0xE6300000 - 0xE632FF00 */
		*lp = LEVEL3_ADDR(p)
			+ LATTR_SH(SH_IS)
//			+ LATTR_SH(SH_NS)
			+ LATTR_AP(AP_RW_ANY)
			+ LATTR_INDEX(ATTR1)	/* MAIR0 ATTR1 */
			+ DESCRIPTER_TABLE;
		p += LEVEL3_ADDR_INC;
		if ((p & 0xF000U) == 0x8000U ) {
			p += 0x8000U;
		}
		lp++;
	}

	for (i =304U; i <= 351U; i++) {		/* 0xE6330000 - 0xE635F000 */
		*lp = DESCRIPTER_INVALID;
		lp++;
	}

	p = LEVEL3_ADDR_BASE + LEVEL3_ADDR_INC * 352U;
	for (i =352U; i <= 511U; i++) {		/* 0xE6360000 - 0xE63FF000 */
		*lp = LEVEL3_ADDR(p)
			+ LATTR_SH(SH_NS)
			+ LATTR_AP(AP_RW_ANY)
			+ LATTR_INDEX(ATTR0)
			+ DESCRIPTER_TABLE;
		p += LEVEL3_ADDR_INC;
		lp++;
	}
	dsb();

	/* IPMMU-MM register setting */
	mmio_write_32(IMTTLBR0(SEC, MMU_NO), MMU_L1_TABLE_ADDR);
	mmio_write_32(IMTTUBR0(SEC, MMU_NO), 0U);

	mmio_write_32(IMTTBCR(SEC, MMU_NO), IMTTBCR_EAE
					  + IMTTBCR_TSZ1(0U)
					  + IMTTBCR_SH0_IS
					  + IMTTBCR_ORGN0_WBWA
					  + IMTTBCR_IRGN0_WBWA
					  + IMTTBCR_SL_1ST
					  + IMTTBCR_TSZ0(0U));

	mmio_write_32(IMBUSCR(SEC, MMU_NO), 0x00000000U);

	mmio_write_32(IMSTR(SEC, MMU_NO), 0x0U);

	mmio_write_32(IMMAIR0(SEC, MMU_NO), 0x00004F00U); /* ATTR1 ATTR0 */
//	mmio_write_32(IMMAIR0(SEC, MMU_NO), 0x00000044U);
	dsb();

	mmio_write_32(IMCTR_MM(SEC, MMU_NO), IMCTR_INTEN
					   + IMCTR_FLUSH);
	dsb();
	mmio_write_32(IMCTR_MM(SEC, MMU_NO), IMCTR_INTEN
					   + IMCTR_MMUEN);
	dsb();

	/* IPMMU-RT register setting */
	mmio_write_32(IPMMU_REGGUARD, 0x00001234U);
	mmio_write_32(IPMMU_FIX_TARGET, 0x00010001U);
	mmio_write_32(IPMMU_QOS_0, 0x04040F0FU);
	dsb();

	mmio_write_32(IMCTR_RT(SEC, MMU_NO), IMCTR_INTEN
					   + IMCTR_FLUSH);
	dsb();
	mmio_write_32(IMCTR_RT(SEC, MMU_NO), IMCTR_INTEN
					   + IMCTR_MMUEN);
	dsb();

	for (i = 0; i < MAX_UTLB; i++) {
		if ( i ==  UTLB_SH4A ){
			/* excepted the uTLB for SH-4A(IMUCTR3/IMUASID3) */
		} else {
			mmio_write_32(IMUASID(SEC, i), IMUASID_ASID1(0U)
						     + IMUASID_ASID0(0U));
			mmio_write_32(IMUCTR(SEC, i), IMUCTR_TTSEL(MMU_NO)
						    + IMUCTR_FLUSH);
			dsb();
			mmio_write_32(IMUCTR(SEC, i), IMUCTR_TTSEL(MMU_NO)
						    + IMUCTR_MMUEN);
			dsb();
		}
	}
	NOTICE("IPMMU-RT/MM enable(CR7)\n");
#endif
}

void ipmmu_flush(void)
{
#if RCAR_SYSRAM_IPMMU_ENABLE == 1
	uint32_t reg;

	reg = mmio_read_32(IMCTR_RT(SEC, MMU_NO));
	if (reg & IMCTR_MMUEN) {
		/* Flush IPMMU-RT */
		reg = reg | IMCTR_FLUSH;
		mmio_write_32(IMCTR_RT(SEC, MMU_NO), reg);
		dsb();
	}
#endif
}
