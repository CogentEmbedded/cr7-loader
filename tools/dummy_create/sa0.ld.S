/*
 * Copyright (c) 2015-2017, Renesas Electronics Corporation
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
SECTIONS
{
	. = 0x00000000;
	.rodata : {
		KEEP(*(.sa0_bootrom))
		/* A-side */
		. = 0x00003000;		/* magic number */
		KEEP(*(.sa0_magic_number_A))
		/* A-side IPL content cert "Start Address"(V3H Map) */
		. = 0x00003154;		/* H'00003000 + H'00000154 */
		KEEP(*(.sa0_cert_addr2_A))
		/* A-side IPL content cert "Start Address"(V3H Map) 2 */
		. = 0x000031D4;		/* H'00003000 + H'00000154 */
		KEEP(*(.sa0_cert_addr3_A))
		/* A-side IPL content cert "Size"(V3H Map) */
		. = 0x00003264;		/* H'00003000 + H'00000264 */
		KEEP(*(.sa0_cert_size2_A))
		/* A-side IPL content cert "Size"(V3H Map) 2 */
		. = 0x00003364;		/* H'00003000 + H'00000264 */
		KEEP(*(.sa0_cert_size3_A))
		/* B-Side */
		. = 0x00004000;		/* magic number */
		KEEP(*(.sa0_magic_number_B))
		/* B-side IPL content cert "Start Address"(V3H Map) */
		. = 0x00004154;		/* H'00003000 + H'00000154 */
		KEEP(*(.sa0_cert_addr2_B))
		/* B-side IPL content cert "Start Address"(V3H Map) 2 */
		. = 0x000041D4;		/* H'00003000 + H'00000154 */
		KEEP(*(.sa0_cert_addr3_B))
		/* B-side IPL content cert "Size"(V3H Map) */
		. = 0x00004264;		/* H'00003000 + H'00000264 */
		KEEP(*(.sa0_cert_size2_B))
		/* B-side IPL content cert "Size"(V3H Map) 2 */
		. = 0x00004364;		/* H'00003000 + H'00000264 */
		KEEP(*(.sa0_cert_size3_B))
	}
}
