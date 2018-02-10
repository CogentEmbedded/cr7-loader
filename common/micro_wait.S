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

#include "micro_wait.h"


#define	CPG_BASE		(0xE6150000)
#define CPG_SMSTPCR1		(0x0134)
#define CPG_CPGWPR		(0x0900)

#define MSTPCR1_TMU1		(1 << 24)

#define TMU3_BASE		(0xE6FC0000)
#define TMU_TSTR		(0x0004)
#define TMU_TCOR		(0x0008)
#define TMU_TCNT		(0x000C)
#define TMU_TCR			(0x0010)

#define TSTR1_TMU3		(1 << 0)


	.globl	micro_wait
#if (TMU3_MEASUREMENT == 1)
	.globl	init_TMU3
	.globl	start_TMU3
	.globl	snapshot_TCNT3
	.globl	stop_TMU3
#endif /* TMU3_MEASUREMENT */

	.align	5	/* Aligned with the cache line */

micro_wait:		/* Cortex-R7 800MHz */
	cmp	r0, #0
	beq	micro_wait_e

micro_wait_loop_1:
	ldr	r1, =#310
micro_wait_loop_2:
	subs	r1, r1, #1
	bne	micro_wait_loop_2
	subs	r0, r0, #1
	bne	micro_wait_loop_1
micro_wait_e:
	bx	lr


/* Time measurement function using the TMU3. */
#if (TMU3_MEASUREMENT == 1)
init_TMU3:
	ldr	r0, =CPG_BASE
	ldr	r1, [r0, #CPG_SMSTPCR1]
	and	r1, r1, #~MSTPCR1_TMU1
	mvn	r2, r1
	str	r2, [r0, #CPG_CPGWPR]
	str	r1, [r0, #CPG_SMSTPCR1]
	dsb	sy
	bx	lr


start_TMU3:
	ldr	r0, =TMU3_BASE
	mov	r1, #0xFFFFFFFF
	str	r1, [r0, #TMU_TCNT]

	ldr	r0, =TMU3_BASE
	ldrb	r1, [r0, #TMU_TSTR]
	orr	r1, r1, #TSTR1_TMU3
	strb	r1, [r0, #TMU_TSTR]
	bx	lr


snapshot_TCNT3:
	ldr	r0, =TMU3_BASE
	ldr	r0, [r0, #TMU_TCNT]
	bx	lr


stop_TMU3:
	ldr	r0, =TMU3_BASE
	ldrb	r1, [r0, #TMU_TSTR]
	and	r1, r1, #~TSTR1_TMU3
	strb	r1, [r0, #TMU_TSTR]
	bx	lr
#endif /* TMU3_MEASUREMENT */

	.end
