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


OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(Vector)

MEMORY {
    RAM (rwx): ORIGIN = 0xEB204000, LENGTH = 0x0001C000
}

SECTIONS
{
    . = 0xEB204000;
    ASSERT(. == ALIGN(4096),
           "CR7 Loader address is not aligned on a page boundary.")

    ro . : {
        __RO_START__ = .;
        *loader.o(.text*)
        . = NEXT(1024);
        *rcar_version.o(.version*)
        common/*.o(.text*)
        loader/*.o(.text*)
        . = ALIGN(0x1000);
        pfc/*.o(.text*)
        *(.text.pfc_write_and_poll)
        *(.text.vch_nxt)
        *(.text.wait_dbcmd)
        *(.text.send_dbcmd)
        *(.text.ddr_regdef_adr)
        *(.text._tblcopy)
        *(.text._f_scale_adj)
        *(.text.set_freqchgack)
        *(.text.set_dfifrequency)
        *(.text.reg_ddrphy_read)
        *(.text.ddrphy_regif_idle)
        *(.text.reg_ddrphy_write)
        *(.text.ddr_setval_s)
        *(.text.ddr_setval)
        *(.text.ddr_setval_ach_s)
        *(.text.ddr_setval_ach)
        *(.text.unlikely.update_dly)
        *(.text.ddr_setval_ach_as)
        *(.text.unlikely.wdqdm_cp)
        *(.text.reg_ddrphy_write_a)
        *(.text._rx_offset_cal_updn)
        *(.text.pll3_set.constprop.12)
        *(.text.ddr_getval_s)
        *(.text.ddr_getval)
        . = ALIGN(0x1000);
        *(.text.change_lpddr4_en)
        *(.text.unlikely.swlvl1)
        *(.text.ddr_getval_ach_as)
        *(.text.rx_offset_cal)
        *(.text.unlikely.wdqdm_man1)
        *(.text.ddrphy_regif_chk)
        *(.text.ddrtbl_getval)
        *(.text.ddrtbl_setval)
        *(.text.pll3_control)
        . = ALIGN(0x1000);
        *(.text.ddrtbl_load)
        *(.text.pll3_freq)
        . = ALIGN(0x1000);
        *(.text.InitDram)
        *(.text.dbsc_regset)
        *(.text.dbsc_regset_pre)
        . = ALIGN(0x1000);
        *(.text.ddr_config)
        . = ALIGN(0x1000);
        *(.text.unlikely.wdqdm_man)
        *(.text.unlikely.rdqdm_man)
        . = ALIGN(0x1000);
        *(.text.init_ddr)
        . = ALIGN(0x1000);
        *(.text*)
        *(.rodata*)
        *(.vectors)
        __RO_END_UNALIGNED__ = .;
        /*
         * Memory page(s) mapped to this section will be marked as
         * read-only, executable.  No RW data from the next section must
         * creep in.  Ensure the rest of the current memory page is unused.
         */
        . = NEXT(4096);
        __RO_END__ = .;
    } >RAM

    .data . : {
        __DATA_START__ = .;
        *(.data*)
        __DATA_END__ = .;
    } >RAM

    stacks (NOLOAD) : {
        __STACKS_START__ = .;
        *(tzfw_normal_stacks)
        __STACKS_END__ = .;
    } >RAM

    /*
     * The .bss section gets initialised to 0 at runtime.
     * Its base address must be 16-byte aligned.
     */
    .bss : ALIGN(16) {
        __BSS_START__ = .;
        *(SORT_BY_ALIGNMENT(.bss*))
        *(COMMON)
        __BSS_END__ = .;
    } >RAM

    __BL2_END__ = .;

    __BSS_SIZE__ = SIZEOF(.bss);

    ASSERT(. <= 0xEB220000, "CR7 Loader image has exceeded its limit.")
}
