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

	.global __aeabi_uidivmod
	.global __aeabi_uidiv
	.global __aeabi_idivmod
	.global __aeabi_idiv


/*****************************************************************************
 *	input:
 *	 r0: divided
 *	 r1: divisor
 *
 *	output
 *	 r0: quot
 *       r1: rem
 *****************************************************************************/

__aeabi_uidivmod:
	push	{r4, r5}
	mov	r4, #0
	mov	r5, #1

	clz	r2, r1
	mov	r3, r1, LSL r2
1:
	cmp	r3, r0
	subls	r0, r0, r3
	addls	r4, r5, LSL r2

	lsr	r3, r3, #1
	subs	r2, r2, #1
	bpl	1b

	mov	r1, r0
	mov	r0, r4

	pop	{r4, r5}
	bx	lr

/*****************************************************************************
 *	input:
 *	 r0: divided
 *	 r1: divisor
 *
 *	output
 *	 r0: quot
 *****************************************************************************/

__aeabi_uidiv:
	b	__aeabi_uidivmod	/* same as __aeabi_uidivmod */

/*****************************************************************************
 *	input:
 *	 r0: divided
 *	 r1: divisor
 *
 *	output
 *	 r0: quot
 *       r1: rem
 *****************************************************************************/

__aeabi_idivmod:

	subs	r2, r0, #0
	push	{r4, r5, r6, lr}
	movlt	r4, #1
	rsblt	r2, r2, #0
	movge	r4, #0
	cmp	r1, #0
	movlt	lr, #1
	rsblt	r1, r1, #0
	movge	lr, #0
	mov	r3, #0

4:
	cmp	r2, r1, lsl r3
	sublt	r3, r3, #1
	lsllt	r1, r1, r3
	blt	3f
	add	r3, r3, #1
	cmp	r3, #32
	bne	4b
	mov	r3, #0

3:
	mov	r0, #0
	mov	r5, #1
	mov	ip, r0

2:
	cmp	ip, r3
	bgt	1f
	cmp	r2, #0
	beq	1f
	cmp	r2, r1
	subge	r6, r3, ip
	subge	r2, r2, r1
	orrge	r0, r0, r5, lsl r6
	asr	r1, r1, #1
	add	ip, ip, #1
	b	2b

1:
	cmp	r4, lr
	rsbne	r0, r0, #0
	mov	r1, r0
	tst	r4, lr
//	ldr	r0, [pc, #8]	; e6302940 <idiv+0xa0>
	rsbne	r2, r2, #0
	pop	{r4, r5, r6, lr}
	bx	lr
//	.word	0xe63071ac
	
/*****************************************************************************
 *	input:
 *	 r0: divided
 *	 r1: divisor
 *
 *	output
 *	 r0: quot
 *****************************************************************************/

__aeabi_idiv:
	b	__aeabi_idivmod	/* same as __aeabi_idivmod */
	

    .end
