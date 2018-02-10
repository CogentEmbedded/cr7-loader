/*
 * Copyright (c) 2013-2014, ARM Limited and Contributors. All rights reserved.
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

#ifndef __H_REG_RCAR_GEN3_
#define __H_REG_RCAR_GEN3_

/*********************** RCarGen3_MFI *************************/
#define	MFISSOFTMDR		0xE6260600		//SOFTMD register
#define	MFISSHESTSR		0xE6260604		//SHE status register

/*********************** RCarGen3_LIFEC *************************/
#define LIFEC_CC_LCS		0xE6110028		// cc_lcs  Life cycle state read

/*********************** RCarGen3_RST *************************/
#define	RST_MODEMR		0xE6160060		// Mode Monitor Register

/*********************** RCarGen3_DMA *************************/
#define	DMA_DMAOR		0xE6700060	//DMA operation register (for west channel)
#define DMA_CHCLR		0xE6700080	//DMA channel clear register (for west channel)
#define	DMA_SAR0		0xE6708000	//DMA source address register
#define	DMA_DAR0		0xE6708004	//DMA destination address register
#define	DMA_TCR0		0xE6708008	//DMA transfer count register
#define	DMA_CHCR0		0xE670800C	//DMA channel control register
#define	DMA_DMARS0		0xE6708040	//DMA extended resource register

/*********************** RCarGen3_RPC/QSPI *************************/
//RPC/QSPI

#define	RPC_BASE		0xEE200000
#define	RPC_CMNCR		(RPC_BASE + 0x0000)
//#define	RPC_DRCR		(RPC_BASE + 0x000C)
//#define	RPC_DRCMR		(RPC_BASE + 0x0010)
//#define	RPC_DRENR		(RPC_BASE + 0x001C)
#define	RPC_SMCR		(RPC_BASE + 0x0020)
#define	RPC_SMCMR		(RPC_BASE + 0x0024)
#define	RPC_SMENR		(RPC_BASE + 0x0030)
#define	RPC_CMNSR		(RPC_BASE + 0x0048)
//#define	RPC_DRDMCR		(RPC_BASE + 0x0058)
//#define	RPC_DRDRENR		(RPC_BASE + 0x005C)
//#define	RPC_PHY_OFFSET1	(RPC_BASE + 0x0080)
#define	RPC_PHY_INT		(RPC_BASE + 0x0088)


/*********************** RCarH3_PFC *************************/
// 5 . PFC

#define	PFC_BASE				0xE6060000
#define	RCarH3_PFC_PMMR			(PFC_BASE + 0x0000)	// LSI Multiplexed Pin Setting Mask Register
#define	RCarH3_PFC_GPSR5		(PFC_BASE + 0x0114)	// GPIO/peripheral function select register 5
#define	RCarH3_PFC_IPSR12		(PFC_BASE + 0x0230)	// Peripheral function select register 12
#define	RCarH3_PFC_MOD_SEL1		(PFC_BASE + 0x0504)	// Module select register 1


/*********************** RCarH3_SCIF ****************************************************/
// 51 . Serial Communication Interface with FIFO (SCIF)
/* H3 SCIF2 */
#define	H3_SCIF2_BASE		0xE6E88000

#define	RCarH3_SCIF2_SCSMR		(H3_SCIF2_BASE + 0x00)		// 16	Serial mode register
#define	RCarH3_SCIF2_SCBRR		(H3_SCIF2_BASE + 0x04)		// 8	Bit rate register
#define	RCarH3_SCIF2_SCSCR		(H3_SCIF2_BASE + 0x08)		// 16	Serial control register
#define	RCarH3_SCIF2_SCFTDR		(H3_SCIF2_BASE + 0x0C)		// 8	Transmit FIFO data register
#define	RCarH3_SCIF2_SCFSR		(H3_SCIF2_BASE + 0x10)		// 16	Serial status register
#define	RCarH3_SCIF2_SCFRDR		(H3_SCIF2_BASE + 0x14)		// 8	Receive FIFO data register
#define	RCarH3_SCIF2_SCFCR		(H3_SCIF2_BASE + 0x18)		// 16	FIFO control register
#define	RCarH3_SCIF2_SCFDR		(H3_SCIF2_BASE + 0x1C)		// 16	FIFO data count register
#define	RCarH3_SCIF2_SCSPTR		(H3_SCIF2_BASE + 0x20)		// 16	Serial port register
#define	RCarH3_SCIF2_SCLSR		(H3_SCIF2_BASE + 0x24)		// 16	Line status register
#define	RCarH3_SCIF2_DL			(H3_SCIF2_BASE + 0x30)		// 16	Frequency division register 
#define	RCarH3_SCIF2_CKS		(H3_SCIF2_BASE + 0x34)		// 16	Clock Select register
#define	RCarH3_SCIF2_SCFER		(H3_SCIF2_BASE + 0x44)		// 16	FIFO error count register

#define	RCarH3_SCIF2_SCSMRIR	(H3_SCIF2_BASE + 0x40)		// 16	Serial mode register



/*********************** RCarH3_CPG_Module Standby, Software Reset *************************/
// 7A . Module Standby, Software Reset

#define	H3_CPG_MSTPRST_BASE	0xE6150000

#define	RCarH3_CPG_MSTPSR3		(H3_CPG_MSTPRST_BASE+0x0048)	// Module stop status register 3
#define	RCarH3_CPG_SMSTPCR3		(H3_CPG_MSTPRST_BASE+0x013C)	// System module stop control register 3
//#define	RCarH3_CPG_SRCR3		(H3_CPG_MSTPRST_BASE+0x00B8)	// Software reset register 3

#define	RCarH3_CPG_CPGWPCR		(H3_CPG_MSTPRST_BASE+0x0904)	// CPG Write Protect Control Register
#define	RCarH3_CPG_CPGWPR		(H3_CPG_MSTPRST_BASE+0x0900)	// CPG Write Protect Register



#endif	/* __H_REG_RCAR_GEN3_ */
