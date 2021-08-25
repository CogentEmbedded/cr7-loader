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


#define MODE_FIQ		(0x11)
#define MODE_IRQ		(0x12)
#define MODE_SVC		(0x13)
#define MODE_ABT		(0x17)
#define MODE_UND		(0x1B)
#define MODE_SYS		(0x1F)

#define	ARMREG_P_CR7SS		(0xE6271800)
#if (RCAR_LSI == RCAR_V3M)
#define	SYSRAM_BASE		(0xE6300000)
#elif (RCAR_LSI == RCAR_V3H)
#define	SYSRAM_BASE		(0xEB200000)
#endif
#define	SCU_BASE		(0xF0000000)

#define	TCM_ENABLE		(0x00000001)
#define	ITCM_BASE		(0xEB000000)
#define	DTCM_BASE		(0xEB020000)
#define	ITCMRR_INIT_TADA	(ITCM_BASE | TCM_ENABLE)
#define	DTCMRR_INIT_TADA	(DTCM_BASE | TCM_ENABLE)

#define	CPG_CPGWPR		(0xE6150900)
#define	APMU_CA57WUPCR		(0xE6152010)
#define	RST_CA57RESCNT		(0xE6160040)
#define	CA57CPU0BARH		(0xE61600C0)
#define	CA57CPU0BARL		(0xE61600C4)
#define	RST_CA53RESCNT		(0xE6160044)
#define	CA53CPU0BARH		(0xE6160080)
#define	CA53CPU0BARL		(0xE6160084)
#define	SYSC_SYSCISR		(0xE6180004)
#define	SYSC_SYSCSR		(0xE6180000)
#define	SYSC_SYSCIER		(0xE618000C)
#define	SYSC_SYSCIMR		(0xE6180010)
#define	SYSC_PWRONCR5		(0xE61801CC)
#define	SYSC_PWRER5		(0xE61801D4)

#define	RESCNT_KEYWORD		(0xA5A50000)

#define	STACK_BASE_ABT		(DTCM_BASE | 0x7080)
#define	STACK_BASE_UND		(DTCM_BASE | 0x7100)
#define	STACK_BASE_FIQ		(DTCM_BASE | 0x7180)
#define	STACK_BASE_IRQ		(DTCM_BASE | 0x7200)
#define	STACK_BASE_SVC		(DTCM_BASE | 0x8000)


#define WBWA			(0x00B)		/* Write-Back Write Allocate */
#define WT			(0x002)		/* Write-Through */
#define NC			(0x008)		/* Non-cacheable */
#define DEV			(0x001)		/* Device */
#define SO			(0x000)		/* Strongly Ordered */
#define SHARE			(0x004)		/* Shareable */
#define AP_FULL			(0x300)

#define REGION_SIZE_4KB		(0x16)
#define REGION_SIZE_8KB		(0x18)
#define REGION_SIZE_16KB	(0x1A)
#define REGION_SIZE_32KB	(0x1C)
#define REGION_SIZE_64KB	(0x1E)
#define REGION_SIZE_128KB	(0x20)
#define REGION_SIZE_256KB	(0x22)
#define REGION_SIZE_512KB	(0x24)
#define REGION_SIZE_1MB		(0x26)
#define REGION_SIZE_2MB		(0x28)
#define REGION_SIZE_4MB		(0x2A)
#define REGION_SIZE_8MB		(0x2C)
#define REGION_SIZE_16MB	(0x2E)
#define REGION_SIZE_32MB	(0x30)
#define REGION_SIZE_64MB	(0x32)
#define REGION_SIZE_128MB	(0x34)
#define REGION_SIZE_256MB	(0x36)
#define REGION_SIZE_512MB	(0x38)
#define REGION_SIZE_1GB		(0x3A)
#define REGION_SIZE_2GB		(0x3C)
#define REGION_SIZE_4GB		(0x3E)

#define	REGION_ENABLE		(0x1)

#define	REGION0_SIZE		(REGION_SIZE_4GB   | REGION_ENABLE)
#define	REGION1_SIZE		(REGION_SIZE_512MB | REGION_ENABLE)
#define	REGION2_SIZE		(REGION_SIZE_512KB | REGION_ENABLE)
#define	REGION3_SIZE		(REGION_SIZE_64KB  | REGION_ENABLE)
#define	REGION4_SIZE		(REGION_SIZE_64KB  | REGION_ENABLE)

#define REGION0_BASE		(0x00000000)
#define REGION1_BASE		(0xE0000000)
#define REGION2_BASE		SYSRAM_BASE
#define REGION3_BASE		(ITCM_BASE)
#define REGION4_BASE		(0xEB100000)

#define	REGION0_ATTR		(AP_FULL | WBWA | SHARE)
#define	REGION1_ATTR		(AP_FULL | DEV  | SHARE)
#define	REGION2_ATTR		(AP_FULL | WBWA | SHARE)
#define	REGION3_ATTR		(AP_FULL | WBWA | SHARE)
#define	REGION4_ATTR		(AP_FULL | WBWA | SHARE)


#define	SCTLR_I			(1 << 12)
#define	SCTLR_M			(1 << 0)


	.global	Vector
	.global	do_panic

/*****************************************************************************
 *	Vector table
 *****************************************************************************/
	.align	5

Vector:
	b	Start				/* Reset */
	b	Undef				/* Undefined Instruction */
	b	SWI				/* Supervisor Call */
	b	PAbort				/* Prefetch Abort */
	b	DAbort				/* Data Abort */
	nop					/* Not used */
	b	IRQ				/* IRQ interrupt */
	b	FIQ				/* FIQ interrupt */


/*****************************************************************************
 *	Reset Hander
 *****************************************************************************/
Start:
	/* initialize registers*/
	mov	r0,  #0
	mov	r1,  #0
	mov	r2,  #0
	mov	r3,  #0
	mov	r4,  #0
	mov	r5,  #0
	mov	r6,  #0
	mov	r7,  #0
	mov	r8,  #0
	mov	r9,  #0
	mov	r10, #0
	mov	r11, #0
	mov	r12, #0
	mov	lr,  #0

	/* stack initialize */
	msr	CPSR_c, #(0x17 | 0x80 | 0x40)	/* ABT */
	ldr	r0, =(0xEB020000 | 0x7080)	/* STACK_BASE_ABT */
	mov	sp, r0

	msr	CPSR_c, #(0x1B | 0x80 | 0x40)	/* UND */
	ldr	r0, =(0xEB020000 | 0x7100)	/* STACK_BASE_UND */
	mov	sp, r0

	msr	CPSR_c, #(0x11 | 0x80 | 0x40)	/* FIQ */
	ldr	r0, =(0xEB020000 | 0x7180)	/* STACK_BASE_FIQ */
	mov	sp, r0

	msr	CPSR_c, #(0x12 | 0x80 | 0x40)	/* IRQ */
	ldr	r0, =(0xEB020000 | 0x7200)	/* STACK_BASE_IRQ */
	mov	sp, r0

	msr	CPSR_c, #(0x13 | 0x80 | 0x40)	/* SVC */
	ldr	r0, =(0xEB020000 | 0x8000)	/* STACK_BASE_SVC */
	mov	sp, r0

	/* TCM configuration */
	mrc	p15, 0, r0, c9, c1, 0		/* DTCMRR */
	ldr	r1, =(0xEB020000 | 0x00000001)	/* DTCMRR_INIT_TADA */
	orr	r0, r0, r1
	mcr	p15, 0, r0, c9, c1, 0

	mrc	p15, 0, r0, c9, c1, 1		/* ITCMRR */
	ldr	r1, =(0xEB000000 | 0x00000001)	/* ITCMRR_INIT_TADA */
	orr	r0, r0, r1
	mcr	p15, 0, r0, c9, c1, 1

	/* Cache/TCM ECC error detection enable */
	mov	r0, #0
	mcr	p15, 0, r0, c15, c2, 0		/* DEER0 */
	mcr	p15, 0, r0, c15, c2, 1		/* DEER1 */
	mcr	p15, 0, r0, c15, c2, 2		/* DEER2 */
	mcr	p15, 0, r0, c15, c3, 0		/* IEER0 */
	mcr	p15, 0, r0, c15, c3, 1		/* IEER1 */
	mcr	p15, 0, r0, c15, c3, 2		/* IEER2 */
	mcr	p15, 0, r0, c15, c4, 0		/* DTCMEER */
	mcr	p15, 0, r0, c15, c5, 0		/* ITCMEER */

	mrc	p15, 0, r0, c1, c0, 1		/* ATCLR */
	mov	r0, #0x00000600			/* bit10, bit9 */
	orr	r0, r0, r1
	mcr	p15, 0, r0, c1, c0, 1

	ldr	r1, =(0xF0000000)		/* SCU_BASE */
	str	r0, [r1, #0x60]			/* ECC error bank #0 */
	str	r0, [r1, #0x64]			/* ECC error bank #1 */
	str	r0, [r1, #0x70]			/* ram access */
	str	r0, [r1, #0x74]			/* ram data */
	str	r0, [r1, #0x78]			/* ram ecc */

	/* Instruction cache enable */
	mrc	p15, 0, r1, c1, c0, 0		/* SCTLR */
	orr 	R1, R1, #1 << 12		/* I=1 */
	orr 	R1, R1, #1 << 11		/* Z=1 */
	mcr 	p15, 0, r0, c7, c5, 0		/* ICIALLU */
	mcr	p15, 0, r0, c7, c5, 6		/* BPIALL */
	mcr 	p15, 0, r1, c1, c0, 0		/* SCTLR */
	isb

	/* Loader Main */
	BL     loader_main

	/* Instruction cache invaldate & disable */
//	mrc	p15, 0, r0, c1, c0, 0		/* SCTLR */
//	bic	R0, R0, #1 << 12		/* I = 0 */
//	mrc	p15, 0, r0, c1, c0, 0
	mcr	p15, 0, r0, c7, c5, 0		/* ICIALLU */
	mcr	p15, 0, r0, c7, c5, 6		/* BPIALL */
	isb

	/* System RAM/SDRAM ECC enable */
	bl	ecc_init

#if (RCAR_LSI == RCAR_V3M)
	ldr	r2, =(0xe6303300)
#elif (RCAR_LSI == RCAR_V3H)
	ldr	r2, =(0xEB203300)
#endif
	mov	r0, #0
	mov	r1, R0
	strd	r0, r1, [r2], #8
	strd	r0, r1, [r2], #8
	strd	r0, r1, [r2], #8
	strd	r0, r1, [r2], #8
	strd	r0, r1, [r2], #8
	strd	r0, r1, [r2], #8
	strd	r0, r1, [r2], #8
	strd	r0, r1, [r2], #8

	/* IPMMU configuration */
	bl	ipmmu_init

	/* Get BL2 address and RTOS address */
	sub	sp, sp, #8
	mov	r0, sp
	bl	rcar_bl2_addr
	ldr	r1, [sp, #0]
	add	sp, sp, #8
	push	{r0, r1}			/* r0 = CR7 boot address */
	mov	r0, r1
	push	{r0, r1}			/* r0 = CA5x boot address */

	/* serial outpu wait */
	bl	console_wait

	/* CA53 bootup */
	pop	{r0, r1}
	bl	rcar_pwrc_cpuon

	/* wait u-boot initialize */
//	mov	r0,	#0x100000
//u_boot_loop:
//	subs	r0, r0, #1
//	bne	u_boot_loop

	/* Instruction cache invaldate & disable */
	mrc	p15, 0, r0, c1, c0, 0		/* SCTLR */
	bic	R0, R0, #0x1 <<12		/* I = 0 */
	bic	R0, R0, #0x1 <<11		/* Z = 0 */
	mrc	p15, 0, r0, c1, c0, 0
	mcr	p15, 0, r0, c7, c5, 0		/* ICIALLU */
	mcr	p15, 0, r0, c7, c5, 6		/* BPIALL */
	isb

	/* Jump to RTOS the address is set */
	pop	{r0, r1}
	bx	r0				/* RTOS_BASE */
	NOP
	NOP
	NOP

do_panic:
	nop
1:
	wfe
	b	1b


/*****************************************************************************
 *	Exception Handers
 *****************************************************************************/

	/* Undefined Instruction */
Undef:
	nop
1:
	wfe
	b	1b

	/* Supervisor Call */
SWI:
	nop
1:
	wfe
	b	1b

	/* Prefetch Abort */
PAbort:
	nop
1:
	wfe
	b	1b

	/* Data Abort */
DAbort:
	nop
1:
	wfe
	b	1b

	/* IRQ interrupt */
IRQ:
	nop
1:
	wfe
	b	1b

	/* FIQ interrupt */
FIQ:
	nop
1:
	wfe
	b	1b


	.end
