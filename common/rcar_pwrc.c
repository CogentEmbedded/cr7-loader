/*
 * Copyright (c) 2013-2014, ARM Limited and Contributors. All rights reserved.
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

//#include <bakery_lock.h>
#include <mmio.h>
//#include <debug.h>
//#include <arch.h>
//#include "../../rcar_def.h"
//#include "../../rcar_private.h"
#include "rcar_pwrc.h"

/****************************************************************************************************************************************************************/
/*																				*/
/*	Definition of R-CarGen3 register address														*/
/*																				*/
/****************************************************************************************************************************************************************/
/* Timer	*/
#define	RCAR_CNTC_BASE			(0xE6080000U)					/* The base addess of generic timer control register			*/
/* Reset	*/
#define	RCAR_CPGWPR			(0xE6150900U)					/* CPG write protect register						*/
#define	RCAR_MODEMR			(0xE6160060U)					/* Mode pin register							*/
#define	RCAR_MODEMR_A			(0xE6160060U)					/* Mode pin register for Assembly language				*/
#define	RCAR_CA57RESCNT			(0xE6160040U)					/* Reset control register for A57					*/
#define	RCAR_CA53RESCNT			(0xE6160044U)					/* Reset control register for A53					*/
#define	RCAR_CA53WUPCR			(0xE6151010U)					/* Wake-up control register for A53					*/
#define	RCAR_CA57WUPCR			(0xE6152010U)					/* Wake-up control register for A57					*/
#define	RCAR_CA53PSTR			(0xE6151040U)					/* Power status register for A53					*/
#define	RCAR_CA57PSTR			(0xE6152040U)					/* Power status register for A57					*/
#define	RCAR_CA53CPU0CR			(0xE6151100U)					/* The base address of CPU power status control register for A53	*/
#define	RCAR_CA57CPU0CR			(0xE6152100U)					/* The base address of CPU power status control register for A57	*/
#define	RCAR_CA53CPUCMCR		(0xE6151184U)					/* Common power control register for A53				*/
#define	RCAR_CA57CPUCMCR		(0xE6152184U)					/* Common power control register for A57				*/
#define	RCAR_WUPMSKCA57			(0xE6180014U)					/* Wake-up mask register for A57					*/
#define	RCAR_WUPMSKCA53			(0xE6180018U)					/* Wake-up mask register for A53					*/
/* SYSC		*/
#define	RCAR_PWRSR3			(0xE6180140U)					/* Power status register CA53-SCU					*/
#define	RCAR_PWRSR5			(0xE61801C0U)					/* Power status register CA57-SCU					*/
#define	RCAR_SYSCIER			(0xE618000CU)					/* Interrupt enable register						*/
#define	RCAR_SYSCIMR			(0xE6180010U)					/* Interrupt mask register						*/
#define	RCAR_SYSCSR			(0xE6180000U)					/* SYSC status register							*/
#define	RCAR_PWRONCR3			(0xE618014CU)					/* Power resume control register CA53-SCU				*/
#define	RCAR_PWRONCR5			(0xE61801CCU)					/* Power resume control register CA57-SCU				*/
#define	RCAR_PWRER3			(0xE6180154U)					/* Power shutoff/resume error register CA53-SCU				*/
#define	RCAR_PWRER5			(0xE61801D4U)					/* Power shutoff/resume error register CA57-SCU				*/
#define	RCAR_SYSCISR			(0xE6180004U)					/* Interrupt status register						*/
#define	RCAR_SYSCISCR			(0xE6180008U)					/* Interrupt status clear register					*/


#define	WUP_IRQ_SHIFT	(0U)
#define	WUP_FIQ_SHIFT	(8U)
#define	WUP_CSD_SHIFT	(16U)

#define	BIT_CA53_SCU	(1U<<21)
#define	BIT_CA57_SCU	(1U<<12)
#define	REQ_RESUME	(1U<<1)
#define	REQ_OFF		(1U<<0)
#define	STATUS_PWRUP	(1U<<1)
#define	STATUS_PWRDOWN	(1U<<0)

#define	STATE_CA57_CPU	(27U)
#define	STATE_CA53_CPU	(22U)

#define	RST_CA57CPU0BARL	(0xE61600C4U)
#define	RST_CA57CPU0BARH	(0xE61600C0U)
#define	RST_CA53CPU0BARL	(0xE6160084U)
#define	RST_CA53CPU0BARH	(0xE6160080U)

#define MAIN_CPU_NONE	(0)
#define MAIN_CPU_CA57	(1)
#define MAIN_CPU_CA53	(2)

#if RCAR_KICK_MAIN_CPU == MAIN_CPU_CA57
#define	res_reg		((uintptr_t)RCAR_CA57RESCNT)
#define	on_reg		((uintptr_t)RCAR_CA57WUPCR)
#define	upper_value	((uint32_t)0xA5A50000U)

#define	reg_SYSC_bit	((uint32_t)BIT_CA57_SCU)
#define	reg_PWRONCR	((uintptr_t)RCAR_PWRONCR5)
#define	reg_PWRSR	((uintptr_t)RCAR_PWRSR5)
#define	reg_PWRER	((uintptr_t)RCAR_PWRER5)

#elif RCAR_KICK_MAIN_CPU == MAIN_CPU_CA53
#define	res_reg		((uintptr_t)RCAR_CA53RESCNT)
#define	on_reg		((uintptr_t)RCAR_CA53WUPCR)
#define	upper_value	((uint32_t)0x5A5A0000U);

#define	reg_SYSC_bit	((uint32_t)BIT_CA53_SCU)
#define	reg_PWRONCR	((uintptr_t)RCAR_PWRONCR3)
#define	reg_PWRSR	((uintptr_t)RCAR_PWRSR3)
#define	reg_PWRER	((uintptr_t)RCAR_PWRER3)

#endif

#if (RCAR_KICK_MAIN_CPU != MAIN_CPU_NONE)
static void SCU_power_up(void);
#endif

void rcar_pwrc_cpuon(uint32_t boot_addr)
{
#if (RCAR_KICK_MAIN_CPU == MAIN_CPU_NONE)
	return;
#else
	const uint32_t cpu_no = 0U;
	uint32_t res_data;

#if (RCAR_KICK_MAIN_CPU == MAIN_CPU_CA57)
	/* CA57 boot address setting */
	mmio_write_32(RST_CA57CPU0BARL, boot_addr & 0xFFFFFFFFU);
	mmio_write_32(RST_CA57CPU0BARH, ((uint64_t)boot_addr >> 32U) & 0xFFFFFFFFU);
#elif (RCAR_KICK_MAIN_CPU == MAIN_CPU_CA53)
	/* CA53 boot address setting */
	mmio_write_32(RST_CA53CPU0BARL, boot_addr & 0xFFFFFFFFU);
	mmio_write_32(RST_CA53CPU0BARH, ((uint64_t)boot_addr >> 32U) & 0xFFFFFFFFU);
#endif
	SCU_power_up();
	mmio_write_32(RCAR_CPGWPR, ~((uint32_t)((uint32_t)1U << cpu_no)));
	mmio_write_32(on_reg, (uint32_t)((uint32_t)1U << cpu_no));
	res_data = mmio_read_32(res_reg) | upper_value;
	mmio_write_32(res_reg, (res_data & (~((uint32_t)1U << (3U - cpu_no)))));
#endif
}

#if (RCAR_KICK_MAIN_CPU != MAIN_CPU_NONE)
static void SCU_power_up()
{
	const uintptr_t reg_SYSCIER  = (uintptr_t)RCAR_SYSCIER;
	const uintptr_t reg_SYSCIMR  = (uintptr_t)RCAR_SYSCIMR;
	const uintptr_t reg_SYSCSR   = (uintptr_t)RCAR_SYSCSR;
	const uintptr_t reg_SYSCISR  = (uintptr_t)RCAR_SYSCISR;
	const uintptr_t reg_SYSCISCR = (uintptr_t)RCAR_SYSCISCR;

	if ((mmio_read_32(reg_PWRSR) & (uint32_t)STATUS_PWRDOWN) != 0x0000U) {
		/* set SYSCIER and SYSCIMR */
		mmio_write_32(reg_SYSCIER, (mmio_read_32(reg_SYSCIER) | reg_SYSC_bit));
		mmio_write_32(reg_SYSCIMR, (mmio_read_32(reg_SYSCIMR) | reg_SYSC_bit));
		do {
			/* SYSCSR[1]=1? */
			while ((mmio_read_32(reg_SYSCSR) & (uint32_t)REQ_RESUME) == 0U) {
				;
			}
			/* If SYSCSR[1]=1 then set bit in PWRONCRn to 1 */
			mmio_write_32(reg_PWRONCR, 0x0001U);
		} while ((mmio_read_32(reg_PWRER) & 0x0001U) != 0U);

		/* bit in SYSCISR=1 ? */
		while ((mmio_read_32(reg_SYSCISR) & reg_SYSC_bit) == 0U) {
			;
		}
		/* clear bit in SYSCISR */
		mmio_write_32(reg_SYSCISCR, reg_SYSC_bit);
	}
}
#endif
