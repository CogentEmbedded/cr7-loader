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
#include <string.h>
#include "reg_rcar_gen3.h"
#include "bit.h"
#include "debug.h"
#include "mmio.h"
#include "rpc_driver.h"
#include "dma_driver.h"
#include "ipmmu.h"

/*****************************************************************************
 *	Definitions
 *****************************************************************************/
#define RTOS_MAX_SIZE			(1024U * 1024U)
#define MAIN_LOADER_MAX_SIZE		( 178U * 1024U)

#define FLASH_BASE			(0x08000000U)
#define FLASH_SEC_BOOT_CERT_ADDR	(FLASH_BASE + 0x00000800U)
#define FLASH_RTOS_CERT_ADDR		(FLASH_BASE + 0x000C0000U)
#define FLASH_CA5X_CERT_ADDR		(FLASH_BASE + 0x000C0400U)
#define FLASH_RTOS_IMAGE_ADDR		(FLASH_BASE + 0x00740000U)
#define FLASH_CA5X_IMAGE_ADDR		(FLASH_BASE + 0x00140000U)

#if RCAR_LSI == RCAR_V3M
#define SYSRAM_BASE			(0xE6300000U)
#define SEC_BOOT_CERT_ADDR		(SYSRAM_BASE + 0x0400U)
#elif RCAR_LSI == RCAR_V3H
#define RTSRAM_BASE			(0xEB200000U)
#define SEC_BOOT_CERT_ADDR		(RTSRAM_BASE + 0x0400U)
#endif

#define DRAM_BASE			(0x40000000U)
#define RTOS_CERT_ADDR			(DRAM_BASE + 0x0000A0000U)
#define CA5X_CERT_ADDR			(DRAM_BASE   + 0x0800U)

typedef int32_t (* SECURE_BOOT_API)(uint32_t a, uint32_t b, void *c);

#define RCAR_SBROM_API		(0xeb103dec)


/*****************************************************************************
 *	Unions
 *****************************************************************************/


/*****************************************************************************
 *	Structs
 *****************************************************************************/


/*****************************************************************************
 *	Globals
 *****************************************************************************/


/*****************************************************************************
 *	Macros
 *****************************************************************************/


/*****************************************************************************
 *	Prototypes
 *****************************************************************************/
uint32_t rcar_bl2_addr(uint32_t *main_cpu_addr);

uint32_t rcar_bl2_addr(uint32_t *main_cpu_addr)
{
	uint32_t rtos_load_addr;
	uint32_t ca5x_load_addr;
	uint32_t size;
	uint32_t ret;
	SECURE_BOOT_API	sbrom_SecureBootAPI;

	INFO("loader_setup(CR7)\n");

/*****************************************************************************
 *	Initialize Hardware
 *****************************************************************************/
	initRPC();			/* Initialize RPC */
	initDMA();			/* Initialize DMA */

//	ipmmu_flush();
//	clearDMA(SYSRAM_BASE, 3072U);

/*****************************************************************************
 *	LCS judgement for secure boot
 *****************************************************************************/
#if RCAR_SECURE_BOOT == 1
1
{
	uint32_t lcs    = (mmio_read_32(LIFEC_CC_LCS) & 0x000000F0U) >> 4;
	uint32_t md     = (mmio_read_32(RST_MODEMR)   & 0x00000020U) >> 5;
	uint32_t softmd = (mmio_read_32(MFISSOFTMDR)  & 0x00000001U);

	if (lcs == 0x5U) { /* LCS=Secure */
		if (softmd == 0x1U) {
			sbrom_SecureBootAPI = NULL; /* LCS=Secure + Normal boot (temp setting) */
		} else {/* else is LCS=Secure + Secure boot */
			sbrom_SecureBootAPI = (SECURE_BOOT_API)RCAR_SBROM_API;
		}
	} else if (md == 0U) {	/* MD5=0 => LCS=CM/DM + Secure boot */
		sbrom_SecureBootAPI = (SECURE_BOOT_API)RCAR_SBROM_API;
	} else {
		sbrom_SecureBootAPI = NULL; /* MD5=1 => LCS=CM/DM + Normal boot */
	}
}
#else /* RCAR_SECURE_BOOT */
	sbrom_SecureBootAPI = NULL;
#endif /* RCAR_SECURE_BOOT */

	if (sbrom_SecureBootAPI != NULL) {
		NOTICE("Secure boot(CR7)\n");
	} else {
		NOTICE("Normal boot(CR7)\n");
	}

/*****************************************************************************
 *	Reload certificate from HyperFlash
 *****************************************************************************/
//	ipmmu_flush();
	execDMA(SEC_BOOT_CERT_ADDR, FLASH_SEC_BOOT_CERT_ADDR, 1024U);

/*****************************************************************************
 *	Load CR7 RTOS certificate from HyperFlash
 *****************************************************************************/
//	ipmmu_flush();
	execDMA(RTOS_CERT_ADDR, FLASH_RTOS_CERT_ADDR, 1024U);
	
/*****************************************************************************
 *	Load RTOS from HyperFlash
 *****************************************************************************/
	size = mmio_read_32(RTOS_CERT_ADDR + 0x264U);
	size = size * 4U;

	if (size == 0U) {
		ERROR("RTOS image size error\n");
		panic();
	} else if (size > RTOS_MAX_SIZE) {
		ERROR("RTOS image size error\n");
		panic();
	}

	rtos_load_addr = mmio_read_32(RTOS_CERT_ADDR + 0x154U);
//	ipmmu_flush();
	execDMA(rtos_load_addr, FLASH_RTOS_IMAGE_ADDR, size);
//	(void *)memcpy((void *)rtos_load_addr, (void *)0x08100000U, size);

	/* Clear System RAM(Generating ECC) */
//	ipmmu_flush();
//	clearDMA(rtos_load_addr + size, 0xE6330000U - (rtos_load_addr + size));
//	memset((void *)(rtos_load_addr + size), 0x0U, 0xE6330000U - (rtos_load_addr + size));

/*****************************************************************************
 *	CR7 RTOS authentication
 *****************************************************************************/
	if (sbrom_SecureBootAPI != NULL) {
		INFO("RTOS image Verification(CR7)\n");
		ipmmu_flush();
		ret = sbrom_SecureBootAPI(SEC_BOOT_CERT_ADDR, RTOS_CERT_ADDR, 0U);
		if (ret != 0) {
			ERROR("RTOS image Verification Failed!!!(0x%x)\n", ret);
			panic();
		} else {
			INFO("RTOS image Verification sucess\n");
		}
	}

/*****************************************************************************
 *	Load CA57 Loader certificate from HyperFlash
 *****************************************************************************/
	ipmmu_flush();
	execDMA(CA5X_CERT_ADDR, FLASH_CA5X_CERT_ADDR, 1024U);

/*****************************************************************************
 *	Load CA57 Loader from HyperFlash
 *****************************************************************************/
	size = mmio_read_32(CA5X_CERT_ADDR + 0x264U);
	size = size * 4U;

	if (size == 0U) {
		ERROR("CA5x Loader image size error\n");
		panic();
	} else if (size > MAIN_LOADER_MAX_SIZE) {
		ERROR("CA5x Loader image size error\n");
		panic();
	}

	ca5x_load_addr = mmio_read_32(CA5X_CERT_ADDR + 0x154U);
	NOTICE("CA5x Loader load address=0x%x CA5x Loader image size=0x%x\n", ca5x_load_addr, size);
//	ipmmu_flush();
	execDMA(ca5x_load_addr, FLASH_CA5X_IMAGE_ADDR, size);
	*main_cpu_addr = ca5x_load_addr;

/*****************************************************************************
 *	CA57 Loader authentication
 *****************************************************************************/
	if (sbrom_SecureBootAPI != NULL) {
		INFO("CA5x Loader image Verification(CR7)\n");
		ipmmu_flush();
		ret = sbrom_SecureBootAPI(SEC_BOOT_CERT_ADDR, CA5X_CERT_ADDR, 0);
		if (ret != 0) {
			ERROR("CA5x Loader image Verification Failed!!!(0x%x)\n", ret);
			panic();
		} else {
			INFO("CA5x Loader image Verification sucess\n");
		}
	}

	return rtos_load_addr;
}
