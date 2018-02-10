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
/* SA0 */
#define	CERT_ADDR	(0xEB204000U)
/* 0x00000000 */
const unsigned int __attribute__ ((section (".sa0_bootrom"))) bootrom_paramA = 0x00000101U;
/* 0x00003000 (V3H Map A side)*/
/* magic number */
const unsigned int __attribute__ ((section (".sa0_magic_number_A"))) magic_number_a = 0xE291F358U;
/* 0x00003154 */
const unsigned int __attribute__ ((section (".sa0_cert_addr2_A"))) cert_addr2_a = CERT_ADDR;
/* 0x00003264 */
const unsigned int __attribute__ ((section (".sa0_cert_size2_A"))) cert_size2_a = 0x00008000U;
/* 0x000031D4 */
const unsigned int __attribute__ ((section (".sa0_cert_addr3_A"))) cert_addr3_a = CERT_ADDR;
/* 0x00003364 */
const unsigned int __attribute__ ((section (".sa0_cert_size3_A"))) cert_size3_a = 0x00008000U;
/* 0x00004000 (V3H Map B side)*/
/* magic number */
const unsigned int __attribute__ ((section (".sa0_magic_number_B"))) magic_number_b = 0xE291F358U;
/* 0x00004154 */
const unsigned int __attribute__ ((section (".sa0_cert_addr2_B"))) cert_addr2_b = CERT_ADDR;
/* 0x00004264 */
const unsigned int __attribute__ ((section (".sa0_cert_size2_B"))) cert_size2_b = 0x00008000U;
/* 0x000041D4 */
const unsigned int __attribute__ ((section (".sa0_cert_addr3_B"))) cert_addr3_b = CERT_ADDR;
/* 0x00004364 */
const unsigned int __attribute__ ((section (".sa0_cert_size3_B"))) cert_size3_b = 0x00008000U;
