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
/* SA3 */
/* 0x000C0154 */
//const unsigned int __attribute__ ((section (".sa3_rtos_addr"))) rtos_addr = 0x480A0800;
const unsigned int __attribute__ ((section (".sa3_rots_addr"))) rtos_addr = 0x480A0800;
/* 0x000C0264 */
const unsigned int __attribute__ ((section (".sa3_rtos_size"))) rtos_size = 0x00008000;
/* 0x000C0554 */
#if defined(V3HBL2_DRAM)
const unsigned int __attribute__ ((section (".sa3_bl2loader_addr"))) bl2loader_addr = 0x46000000;
#elif defined(V3HBL2_RTSRAM)
const unsigned int __attribute__ ((section (".sa3_bl2loader_addr"))) bl2loader_addr = 0xeb244000;
#else
const unsigned int __attribute__ ((section (".sa3_bl2loader_addr"))) bl2loader_addr = 0xe6304000;
#endif
/* 0x000C0664 */
const unsigned int __attribute__ ((section (".sa3_bl2loader_size"))) bl2loader_size = 0x00008000;

