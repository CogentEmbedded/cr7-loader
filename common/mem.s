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


	.global	memcpy
	.global	memset

/*****************************************************************************
 *	input:
 *	 r0: dest
 *	 r1: source
 *	 r2: copy bytes
 *
 *	output
 *	 r0: dest
 *****************************************************************************/
memcpy:
	mov	r12, r0		/* backup dest */

	cmp	r2, #4
	bcc	2f

	and	r3, r0, #0x03
	cmp	r3, #0
	bne	2f		/* unaligned dest address */

	and	r3, r1, #0x03
	cmp	r3, #0
	bne	2f		/* unaligned source address */

1:	/* aligned copy */
	ldr	r3, [r1], #4
	str	r3, [r0], #4
	subs	r2, r2, #4
	beq	3f
	cmp	r2, #4
	bcs	1b

2:	/* unaligned copy */
	ldrb	r3, [r1], #1
	strb	r3, [r0], #1
	subs	r2, r2, #1
	bne	2b
3:
	mov	r0, r12		/* restore dest */
	bx	lr


/*****************************************************************************
 *	input:
 *	 r0: dest
 *	 r1: val
 *	 r2: count
 *
 *	output
 *	 r0: dest
 *****************************************************************************/
memset:
	mov	r12, r0		/* backup dest */

	cmp	r2, #0
	beq	2f		/* no copy */
1:
	strb	r1, [r0], #1
	subs	r2, r2, #1
	bne	1b
2:
	mov	r0, r12		/* restore dest */
	bx	lr


	.end
