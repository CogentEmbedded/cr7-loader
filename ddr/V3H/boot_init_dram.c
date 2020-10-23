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
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <mmio.h>
#include <debug.h>
#include "ddr_regdef.h"
#include "init_dram_tbl_h3.h"
#include "init_dram_tbl_m3.h"
#include "init_dram_tbl_h3ver2.h"
#include "init_dram_tbl_m3n.h"
#include "boot_init_dram_regdef.h"
#include "boot_init_dram.h"
#include "dram_sub_func.h"
#include "micro_wait.h"

#define DDR_BACKUPMODE
#define FATAL_MSG(x) NOTICE(x)
/*******************************************************************************
 *	variables
 ******************************************************************************/
static uint32_t Prr_Product;
static uint32_t Prr_Cut;
char *pRCAR_DDR_VERSION;
uint32_t _cnf_BOARDTYPE;
static uint32_t *pDDR_REGDEF_TBL;
static uint32_t brd_clk;
static uint32_t brd_clkdiv;
static uint32_t ddr_mbps;
static uint32_t ddr_mbpsdiv;
static uint32_t ddr_tccd;
static struct _boardcnf *Boardcnf;
uint32_t ddr_phyvalid;
uint32_t ddr_density[DRAM_CH_CNT][CS_CNT];
static uint32_t ch_have_this_cs[CS_CNT];
static uint32_t rdqdm_dly[DRAM_CH_CNT][CS_CNT][SLICE_CNT*2][9];
static uint32_t rdqdm_le[DRAM_CH_CNT][CS_CNT][SLICE_CNT*2][9];
static uint32_t rdqdm_te[DRAM_CH_CNT][CS_CNT][SLICE_CNT*2][9];
static uint32_t rdqdm_nw[DRAM_CH_CNT][CS_CNT][SLICE_CNT*2][9];
static uint32_t rdqdm_win[DRAM_CH_CNT][CS_CNT][SLICE_CNT];
static uint32_t rdqdm_st[DRAM_CH_CNT][CS_CNT][SLICE_CNT*2];

static uint32_t wdqdm_le[DRAM_CH_CNT][CS_CNT][SLICE_CNT][9];
static uint32_t wdqdm_te[DRAM_CH_CNT][CS_CNT][SLICE_CNT][9];
static uint32_t wdqdm_dly[DRAM_CH_CNT][CS_CNT][SLICE_CNT][9];
static uint32_t wdqdm_st[DRAM_CH_CNT][CS_CNT][SLICE_CNT];
static uint32_t wdqdm_win[DRAM_CH_CNT][CS_CNT][SLICE_CNT];
static uint32_t max_density;
static uint32_t max_cs;
static uint32_t ddr0800_mul;
static uint32_t ddr_mul;
static uint32_t ddr_mbps;
static uint32_t DDR_PHY_SLICE_REGSET_OFS;
static uint32_t DDR_PHY_ADR_V_REGSET_OFS;
static uint32_t DDR_PHY_ADR_I_REGSET_OFS;
static uint32_t DDR_PHY_ADR_G_REGSET_OFS;
static uint32_t DDR_PI_REGSET_OFS;
static uint32_t DDR_PHY_SLICE_REGSET_SIZE;
static uint32_t DDR_PHY_ADR_V_REGSET_SIZE;
static uint32_t DDR_PHY_ADR_I_REGSET_SIZE;
static uint32_t DDR_PHY_ADR_G_REGSET_SIZE;
static uint32_t DDR_PI_REGSET_SIZE;
static uint32_t DDR_PHY_SLICE_REGSET_NUM;
static uint32_t DDR_PHY_ADR_V_REGSET_NUM;
static uint32_t DDR_PHY_ADR_I_REGSET_NUM;
static uint32_t DDR_PHY_ADR_G_REGSET_NUM;
static uint32_t DDR_PI_REGSET_NUM;
static uint32_t	DDR_PHY_ADR_I_NUM;
#define DDR_PHY_REGSET_MAX 128
#define DDR_PI_REGSET_MAX 320
static uint32_t _cnf_DDR_PHY_SLICE_REGSET[DDR_PHY_REGSET_MAX];
static uint32_t _cnf_DDR_PHY_ADR_V_REGSET[DDR_PHY_REGSET_MAX];
static uint32_t _cnf_DDR_PHY_ADR_I_REGSET[DDR_PHY_REGSET_MAX];
static uint32_t _cnf_DDR_PHY_ADR_G_REGSET[DDR_PHY_REGSET_MAX];
static uint32_t _cnf_DDR_PI_REGSET[DDR_PI_REGSET_MAX];
static uint32_t Pll3Mode;
static uint32_t loop_max;
#ifdef DDR_BACKUPMODE
uint32_t ddrBackup;
#endif

// not use qos_init
#define ddr_qos_init_setting

#ifdef ddr_qos_init_setting // only for non qos_init
#define OPERATING_FREQ			(400U)		//Mhz
#define BASE_SUB_SLOT_NUM		(0x6U)
#define SUB_SLOT_CYCLE			(0x7EU)		//126
#define QOSWT_WTSET0_CYCLE		((SUB_SLOT_CYCLE * BASE_SUB_SLOT_NUM * 1000U)/OPERATING_FREQ)	//unit:ns
 
uint32_t get_refperiod(void)
{
	return QOSWT_WTSET0_CYCLE;
}
#else// ddr_qos_init_setting // only for non qos_init
extern uint32_t get_refperiod(void);
#endif//ddr_qos_init_setting // only for non qos_init

#define _reg_PHY_RX_CAL_X_NUM 11
static const uint32_t _reg_PHY_RX_CAL_X[_reg_PHY_RX_CAL_X_NUM] = {
	_reg_PHY_RX_CAL_DQ0,
	_reg_PHY_RX_CAL_DQ1,
	_reg_PHY_RX_CAL_DQ2,
	_reg_PHY_RX_CAL_DQ3,
	_reg_PHY_RX_CAL_DQ4,
	_reg_PHY_RX_CAL_DQ5,
	_reg_PHY_RX_CAL_DQ6,
	_reg_PHY_RX_CAL_DQ7,
	_reg_PHY_RX_CAL_DM,
	_reg_PHY_RX_CAL_DQS,
	_reg_PHY_RX_CAL_FDBK
};

#define _reg_PHY_CLK_WRX_SLAVE_DELAY_NUM 10
static const uint32_t _reg_PHY_CLK_WRX_SLAVE_DELAY[_reg_PHY_CLK_WRX_SLAVE_DELAY_NUM] = {
	_reg_PHY_CLK_WRDQ0_SLAVE_DELAY,
	_reg_PHY_CLK_WRDQ1_SLAVE_DELAY,
	_reg_PHY_CLK_WRDQ2_SLAVE_DELAY,
	_reg_PHY_CLK_WRDQ3_SLAVE_DELAY,
	_reg_PHY_CLK_WRDQ4_SLAVE_DELAY,
	_reg_PHY_CLK_WRDQ5_SLAVE_DELAY,
	_reg_PHY_CLK_WRDQ6_SLAVE_DELAY,
	_reg_PHY_CLK_WRDQ7_SLAVE_DELAY,
	_reg_PHY_CLK_WRDM_SLAVE_DELAY,
	_reg_PHY_CLK_WRDQS_SLAVE_DELAY
};

#define _reg_PHY_RDDQS_X_FALL_SLAVE_DELAY_NUM 9
static const uint32_t _reg_PHY_RDDQS_X_FALL_SLAVE_DELAY[_reg_PHY_RDDQS_X_FALL_SLAVE_DELAY_NUM] = {
	_reg_PHY_RDDQS_DQ0_FALL_SLAVE_DELAY,
	_reg_PHY_RDDQS_DQ1_FALL_SLAVE_DELAY,
	_reg_PHY_RDDQS_DQ2_FALL_SLAVE_DELAY,
	_reg_PHY_RDDQS_DQ3_FALL_SLAVE_DELAY,
	_reg_PHY_RDDQS_DQ4_FALL_SLAVE_DELAY,
	_reg_PHY_RDDQS_DQ5_FALL_SLAVE_DELAY,
	_reg_PHY_RDDQS_DQ6_FALL_SLAVE_DELAY,
	_reg_PHY_RDDQS_DQ7_FALL_SLAVE_DELAY,
	_reg_PHY_RDDQS_DM_FALL_SLAVE_DELAY
};

#define _reg_PHY_RDDQS_X_RISE_SLAVE_DELAY_NUM 9
static const uint32_t _reg_PHY_RDDQS_X_RISE_SLAVE_DELAY[_reg_PHY_RDDQS_X_RISE_SLAVE_DELAY_NUM] = {
	_reg_PHY_RDDQS_DQ0_RISE_SLAVE_DELAY,
	_reg_PHY_RDDQS_DQ1_RISE_SLAVE_DELAY,
	_reg_PHY_RDDQS_DQ2_RISE_SLAVE_DELAY,
	_reg_PHY_RDDQS_DQ3_RISE_SLAVE_DELAY,
	_reg_PHY_RDDQS_DQ4_RISE_SLAVE_DELAY,
	_reg_PHY_RDDQS_DQ5_RISE_SLAVE_DELAY,
	_reg_PHY_RDDQS_DQ6_RISE_SLAVE_DELAY,
	_reg_PHY_RDDQS_DQ7_RISE_SLAVE_DELAY,
	_reg_PHY_RDDQS_DM_RISE_SLAVE_DELAY
};

#define _reg_PHY_PAD_TERM_X_NUM 8
static const uint32_t _reg_PHY_PAD_TERM_X[_reg_PHY_PAD_TERM_X_NUM] = {
	_reg_PHY_PAD_FDBK_TERM,
	_reg_PHY_PAD_DATA_TERM,
	_reg_PHY_PAD_DQS_TERM,
	_reg_PHY_PAD_ADDR_TERM,
	_reg_PHY_PAD_CLK_TERM,
	_reg_PHY_PAD_CKE_TERM,
	_reg_PHY_PAD_RST_TERM,
	_reg_PHY_PAD_CS_TERM
};

#define _reg_PHY_CLK_CACS_SLAVE_DELAY_X_NUM 10
static const uint32_t _reg_PHY_CLK_CACS_SLAVE_DELAY_X[_reg_PHY_CLK_CACS_SLAVE_DELAY_X_NUM] = {
	_reg_PHY_ADR0_CLK_WR_SLAVE_DELAY,
	_reg_PHY_ADR1_CLK_WR_SLAVE_DELAY,
	_reg_PHY_ADR2_CLK_WR_SLAVE_DELAY,
	_reg_PHY_ADR3_CLK_WR_SLAVE_DELAY,
	_reg_PHY_ADR4_CLK_WR_SLAVE_DELAY,
	_reg_PHY_ADR5_CLK_WR_SLAVE_DELAY,

	_reg_PHY_GRP_SLAVE_DELAY_0,
	_reg_PHY_GRP_SLAVE_DELAY_1,
	_reg_PHY_GRP_SLAVE_DELAY_2,
	_reg_PHY_GRP_SLAVE_DELAY_3
};

/*******************************************************************************
 *	Prototypes
 ******************************************************************************/
static inline int32_t vch_nxt(int32_t pos);
static void cpg_write_32(uint32_t a, uint32_t v);
static void pll3_set(uint32_t on, uint32_t high);
static void pll3_control(uint32_t on);
static inline void dsb_sev(void);
static void wait_dbcmd(void);
static void send_dbcmd(uint32_t cmd);
static uint32_t reg_ddrphy_read ( uint32_t phyno, uint32_t regadd);
static void reg_ddrphy_write ( uint32_t phyno, uint32_t regadd, uint32_t regdata);
static void reg_ddrphy_write_a ( uint32_t regadd, uint32_t regdata);
static inline uint32_t ddr_regdef ( uint32_t _regdef );
static inline uint32_t ddr_regdef_adr ( uint32_t _regdef );
static inline uint32_t ddr_regdef_lsb ( uint32_t _regdef );
static void ddr_setval_s ( uint32_t ch, uint32_t slice, uint32_t _regdef, uint32_t val);
static uint32_t ddr_getval_s ( uint32_t ch, uint32_t slice, uint32_t _regdef);
static void ddr_setval(uint32_t ch, uint32_t regdef, uint32_t val);
static void ddr_setval_ach_s(uint32_t slice, uint32_t regdef, uint32_t val);
static void ddr_setval_ach(uint32_t regdef, uint32_t val);
static void ddr_setval_ach_as(uint32_t regdef, uint32_t val);
static uint32_t ddr_getval(uint32_t ch, uint32_t regdef);
static uint32_t ddr_getval_ach(uint32_t regdef, uint32_t *p);
/* NOT USED
static uint32_t ddr_getval_ach_s(uint32_t slice, uint32_t regdef, uint32_t *p);
*/
static uint32_t ddr_getval_ach_as(uint32_t regdef, uint32_t *p);
static void _tblcopy(uint32_t *to, const uint32_t *from, uint32_t size);
static void ddrtbl_setval ( uint32_t *tbl, uint32_t _regdef, uint32_t val);
static uint32_t ddrtbl_getval ( uint32_t *tbl, uint32_t _regdef);
static uint32_t ddrphy_regif_chk(void);
static inline void ddrphy_regif_idle();
static uint16_t _f_scale(uint32_t ddr_mbps, uint32_t ddr_mbpsdiv, uint32_t ps, uint16_t cyc);
static void _f_scale_js2(uint32_t ddr_mbps, uint32_t ddr_mbpsdiv, uint16_t *js2);
static int16_t _f_scale_adj(int16_t ps);
__attribute__ ((noinline)) static void ddrtbl_load(void);
static void ddr_config_sub(void);
static void get_ca_swizzle( uint32_t ch, uint32_t ddr_csn, uint32_t *p_swz);
static void ddr_config_sub_h3v1x(void);
__attribute__ ((noinline)) static void ddr_config(void);
__attribute__ ((noinline)) static void dbsc_regset(void);
static void dbsc_regset_post(void);
__attribute__ ((noinline)) static uint32_t dfi_init_start(void);
static void change_lpddr4_en(uint32_t mode);
static uint32_t set_term_code(void) ;
static void ddr_register_set(uint32_t ch);
static inline uint32_t wait_freqchgreq(uint32_t assert);
static inline void set_freqchgack(uint32_t assert);
static inline void set_dfifrequency(uint32_t freq);
static uint32_t pll3_freq(uint32_t on);
static void update_dly(void);
static uint32_t pi_training_go(void);
__attribute__ ((noinline)) static uint32_t init_ddr(void);
static uint32_t swlvl1(uint32_t ddr_csn, uint32_t reg_cs, uint32_t reg_kick);
static void wdqdm_clr1(uint32_t ch, uint32_t ddr_csn);
static uint32_t wdqdm_ana1(uint32_t ch, uint32_t ddr_csn);
static uint32_t wdqdm_man1(void);
__attribute__ ((noinline)) static uint32_t wdqdm_man(void);
static void rdqdm_clr1(uint32_t ch, uint32_t ddr_csn);
static uint32_t rdqdm_ana1(uint32_t ch, uint32_t ddr_csn);
static uint32_t rdqdm_man1(void);
__attribute__ ((noinline)) static uint32_t rdqdm_man(void);

static int32_t _find_change(uint64_t val, uint32_t dir);
static uint32_t _rx_offset_cal_updn(uint32_t code);
static uint32_t rx_offset_cal(void);
static uint32_t rx_offset_cal_hw(void);
static void adjust_rddqs_latency(void);
static void adjust_wpath_latency(void);

struct DdrtData {
	int32_t  init_temp;	// Initial Temperature (do)
	uint32_t init_cal[4];	// Initial io-code (4 is for H3)
	uint32_t tcomp_cal[4];	// Temperature compensated io-code (4 is for H3)
};
struct DdrtData tcal;

__attribute__ ((noinline)) static void pvtcode_update(void);
static void pvtcode_update2(void);
static void ddr_padcal_tcompensate_getinit(uint32_t override);

/*******************************************************************************
 *	load board configuration
 ******************************************************************************/
#include "boot_init_dram_config.c"

/*******************************************************************************
 *	macro for channel selection loop
 ******************************************************************************/
static inline int32_t vch_nxt(int32_t pos)
{
	int32_t posn;
	for(posn=pos;posn<DRAM_CH_CNT; posn++){
		if(ddr_phyvalid & (1<<posn))break;
	}
	return posn;
}
#define foreach_vch(ch) \
for(ch=vch_nxt(0);ch<DRAM_CH_CNT;ch=vch_nxt(ch+1))

#define foreach_ech(ch) \
for(ch=0;ch<DRAM_CH_CNT;ch++)

/*******************************************************************************
 *	Printing functions
 ******************************************************************************/
#define MSG_LF(...)

/*******************************************************************************
 *	clock settings, reset control
 ******************************************************************************/
static void cpg_write_32(uint32_t a, uint32_t v)
{
	mmio_write_32(CPG_CPGWPR, ~v);
	mmio_write_32(a, v);
}

static void pll3_set(uint32_t on, uint32_t high)
{
	uint32_t dataL,dataDIV,dataMUL,tmpDIV;

	/* PLL3 disable */
	dataL = mmio_read_32(CPG_PLLECR);
	dataL &= ~CPG_PLLECR_PLL3E_BIT;
	cpg_write_32(CPG_PLLECR, dataL);
	dsb_sev();
	cpg_write_32(CPG_FRQCRD,	0x00030003);	/* PLL3 DIV resetting */
	dsb_sev();
	if(!on) {
		return;
	}

	/* PLL3 enable */
	dataL= CPG_MSTPCRM1_ZB3ST_BIT | mmio_read_32(CPG_MSTPCRM1);
	cpg_write_32(CPG_MSTPCRM1,	dataL);		/* zb3 clk stop */
	dsb_sev();

	/* PLL3 Restart */
	dataL = mmio_read_32(CPG_PLLECR);
	dataL |= CPG_PLLECR_PLL3E_BIT;
	cpg_write_32(CPG_PLLECR, dataL);
	dsb_sev();

	do {
		dataL=mmio_read_32(CPG_PLLECR);
	} while( (dataL&CPG_PLLECR_PLL3ST_BIT)==0 );
	dsb_sev();

	if(high){
		  tmpDIV  = (1000*ddr_mbpsdiv*brd_clkdiv)/(ddr_mul*brd_clk*ddr_mbpsdiv+1);
		  dataMUL = (ddr_mul * (tmpDIV +1)-1) << 24;
		  if(tmpDIV){
		    dataDIV = tmpDIV +1;
		  } else {
		    dataDIV = 0;
		  }
	} else {
		  tmpDIV  = (1000*ddr_mbpsdiv*brd_clkdiv)/(ddr0800_mul*brd_clk*ddr_mbpsdiv+1);
		  dataMUL = (ddr0800_mul * (tmpDIV +1)-1) << 24;
		  if(tmpDIV){
		    dataDIV = tmpDIV +1;
		  } else {
		    dataDIV = 0;
		  }
	}

	dataL= (0xFF80FF80 & mmio_read_32(CPG_FRQCRD));
	cpg_write_32(CPG_FRQCRD,	dataL);			/* PLL3 DIV resetting */
	dsb_sev();

	dataL= CPG_FRQCRB_KICK_BIT | mmio_read_32(CPG_FRQCRB);
	cpg_write_32(CPG_FRQCRB,	dataL);			/* DIV SET KICK */
	dsb_sev();

	/* PLL3 FREQ */
	cpg_write_32(CPG_PLL3CR,	dataMUL);		/* Set PLL3 freq */
	dsb_sev();

	do {
		dataL=mmio_read_32(CPG_PLLECR);
	} while( (dataL&CPG_PLLECR_PLL3ST_BIT)==0 );
	dsb_sev();

	dataL= (dataDIV <<16) | dataDIV | (0xFF80FF80 & mmio_read_32(CPG_FRQCRD));
	cpg_write_32(CPG_FRQCRD,	dataL);			/* PLL3 DIV resetting */
	dsb_sev();

	dataL= CPG_FRQCRB_KICK_BIT | mmio_read_32(CPG_FRQCRB);
	cpg_write_32(CPG_FRQCRB,	dataL);			/* DIV SET KICK */
	dsb_sev();

	do {
		dataL=mmio_read_32(CPG_PLLECR);
	} while( (dataL&CPG_PLLECR_PLL3ST_BIT)==0 );
	dsb_sev();

	dataL= (~CPG_MSTPCRM1_ZB3ST_BIT) & mmio_read_32(CPG_MSTPCRM1);
	cpg_write_32(CPG_MSTPCRM1,	dataL);			/* zb3 clk start */
	dsb_sev();

	return;
}

static void pll3_control(uint32_t on)
{
	if(on) {
		pll3_set(1,1);
		Pll3Mode=1;
		loop_max=2;
	} else {
		pll3_set(1,0);
		Pll3Mode=0;
		loop_max=8;
	}
}

/*******************************************************************************
 *	barrier
 ******************************************************************************/
static inline void dsb_sev(void)
{
	__asm__ __volatile__ ("dsb sy");
}

/*******************************************************************************
 *	DDR memory register access
 ******************************************************************************/
static void wait_dbcmd(void)
{
	uint32_t dataL;
	/* dummy read */
	dataL = mmio_read_32(DBSC_DBCMD);
	dsb_sev();
	while(1)
	{
		/* wait DBCMD 1=busy, 0=ready */
		dataL = mmio_read_32(DBSC_DBWAIT);
		dsb_sev();
		if((dataL & 0x00000001) == 0x0)	break;
	}
}

static void send_dbcmd(uint32_t cmd)
{
	/* dummy read */
	wait_dbcmd();
	mmio_write_32(DBSC_DBCMD, cmd);
	dsb_sev();
}

/*******************************************************************************
 *	DDRPHY register access (raw)
 ******************************************************************************/
static uint32_t reg_ddrphy_read ( uint32_t phyno, uint32_t regadd)
{
	uint32_t val;
	uint32_t loop;

	val = 0;
	if ((PRR_PRODUCT_M3N!=Prr_Product)&&(PRR_PRODUCT_V3H!=Prr_Product)) {
		mmio_write_32(DBSC_DBPDRGA(phyno), regadd);
		dsb_sev();

		while(mmio_read_32(DBSC_DBPDRGA(phyno)) != regadd){
			dsb_sev();
		}
		dsb_sev();

		for(loop=0;loop<loop_max;loop++){
			val = mmio_read_32(DBSC_DBPDRGD(phyno));
			dsb_sev();
		}
		(void)val;
	}else{
		mmio_write_32(DBSC_DBPDRGA(phyno), regadd|0x00004000);
		dsb_sev();
		while(mmio_read_32(DBSC_DBPDRGA(phyno))!=(regadd|0x0000C000)){dsb_sev();};
		val = mmio_read_32(DBSC_DBPDRGA(phyno));
		mmio_write_32(DBSC_DBPDRGA(phyno), regadd|0x00008000);
		while(mmio_read_32(DBSC_DBPDRGA(phyno))!=regadd){dsb_sev();};
		dsb_sev();

		mmio_write_32(DBSC_DBPDRGA(phyno), regadd|0x00008000);
		while(mmio_read_32(DBSC_DBPDRGA(phyno))!=regadd){dsb_sev();};

		dsb_sev();
		val = mmio_read_32(DBSC_DBPDRGD(phyno));
		dsb_sev();
		(void)val;
	}
	return val ;
}

static void reg_ddrphy_write ( uint32_t phyno, uint32_t regadd, uint32_t regdata)
{
	uint32_t val;
	uint32_t loop;

	if ((PRR_PRODUCT_M3N!=Prr_Product)&&(PRR_PRODUCT_V3H!=Prr_Product)) {
		mmio_write_32(DBSC_DBPDRGA(phyno), regadd);
		dsb_sev();
		for(loop=0;loop<loop_max;loop++){
			val = mmio_read_32(DBSC_DBPDRGA(phyno));
			dsb_sev();
		}
		mmio_write_32(DBSC_DBPDRGD(phyno),regdata);
		dsb_sev();

		for(loop=0;loop<loop_max;loop++){
			val = mmio_read_32(DBSC_DBPDRGD(phyno));
			dsb_sev();
		}
	}else{
		mmio_write_32(DBSC_DBPDRGA(phyno), regadd);
		dsb_sev();

		while(mmio_read_32(DBSC_DBPDRGA(phyno))!=regadd){dsb_sev();};
		dsb_sev();

		mmio_write_32(DBSC_DBPDRGD(phyno),regdata);
		dsb_sev();

		while(mmio_read_32(DBSC_DBPDRGA(phyno))!=(regadd|0x00008000)){dsb_sev();};
		mmio_write_32(DBSC_DBPDRGA(phyno), regadd|0x00008000);

		while(mmio_read_32(DBSC_DBPDRGA(phyno))!=regadd){dsb_sev();};
		dsb_sev();

		mmio_write_32(DBSC_DBPDRGA(phyno), regadd);
	}
	(void)val;
}

static void reg_ddrphy_write_a ( uint32_t regadd, uint32_t regdata)
{
	uint32_t ch;
	uint32_t val;
	uint32_t loop;

	if ((PRR_PRODUCT_M3N!=Prr_Product)&&(PRR_PRODUCT_V3H!=Prr_Product)) {
		foreach_vch(ch) {
			mmio_write_32(DBSC_DBPDRGA(ch), regadd);
			dsb_sev();
		}

		foreach_vch(ch) {
			mmio_write_32(DBSC_DBPDRGD(ch),regdata);
			dsb_sev();
		}

		for(loop=0;loop<loop_max;loop++){
			val = mmio_read_32(DBSC_DBPDRGD(0));
			dsb_sev();
		}
		(void)val;
	}else{
		foreach_vch(ch) {
			reg_ddrphy_write ( ch, regadd, regdata);
			dsb_sev();
		}
	}
}

static inline void ddrphy_regif_idle()
{
	uint32_t val;
	val = reg_ddrphy_read(0,ddr_regdef_adr(_reg_PI_INT_STATUS));
	dsb_sev();
	(void)val;
}

/*******************************************************************************
 *	DDRPHY register access (field modify)
 ******************************************************************************/
static inline uint32_t ddr_regdef ( uint32_t _regdef )
{
	return pDDR_REGDEF_TBL[_regdef];
}

static inline uint32_t ddr_regdef_adr ( uint32_t _regdef )
{
	return DDR_REGDEF_ADR(pDDR_REGDEF_TBL[_regdef]);
}

static inline uint32_t ddr_regdef_lsb ( uint32_t _regdef )
{
	return DDR_REGDEF_LSB(pDDR_REGDEF_TBL[_regdef]);
}

static inline uint32_t ddr_regdef_len ( uint32_t _regdef )
{
	return DDR_REGDEF_LEN(pDDR_REGDEF_TBL[_regdef]);
}

static void ddr_setval_s ( uint32_t ch, uint32_t slice, uint32_t _regdef, uint32_t val)
{
	uint32_t adr;
	uint32_t lsb;
	uint32_t len;
	uint32_t msk;
	uint32_t tmp;
	uint32_t regdef;
	regdef = ddr_regdef(_regdef);
	adr = DDR_REGDEF_ADR(regdef)+0x80*slice;
	len = DDR_REGDEF_LEN(regdef);
	lsb = DDR_REGDEF_LSB(regdef);
	if(len==0x20)
		msk = 0xffffffff;
	else
		msk = ((1U<<len)-1)<<lsb;

	tmp = reg_ddrphy_read(ch, adr);
	tmp = (tmp & (~msk)) | ((val<<lsb) & msk);
	reg_ddrphy_write(ch, adr, tmp);
}

static uint32_t ddr_getval_s ( uint32_t ch, uint32_t slice, uint32_t _regdef)
{
	uint32_t adr;
	uint32_t lsb;
	uint32_t len;
	uint32_t msk;
	uint32_t tmp;
	uint32_t regdef;
	regdef = ddr_regdef(_regdef);
	adr = DDR_REGDEF_ADR(regdef)+0x80*slice;
	len = DDR_REGDEF_LEN(regdef);
	lsb = DDR_REGDEF_LSB(regdef);
	if(len==0x20)
		msk = 0xffffffff;
	else
		msk = ((1U<<len)-1);

	tmp = reg_ddrphy_read(ch, adr);
	tmp = (tmp >> lsb) & msk;

	return tmp;
}

static void ddr_setval(uint32_t ch, uint32_t regdef, uint32_t val)
{
	ddr_setval_s(ch, 0, regdef, val);
}

static void ddr_setval_ach_s(uint32_t slice, uint32_t regdef, uint32_t val)
{
	uint32_t ch;
	foreach_vch(ch)
		ddr_setval_s(ch, slice, regdef, val);
}

static void ddr_setval_ach(uint32_t regdef, uint32_t val)
{
	ddr_setval_ach_s(0,regdef, val);
}

static void ddr_setval_ach_as(uint32_t regdef, uint32_t val)
{
	uint32_t slice;
	for(slice=0;slice<SLICE_CNT;slice++)
		ddr_setval_ach_s(slice, regdef, val);
}

static uint32_t ddr_getval(uint32_t ch, uint32_t regdef)
{
	return ddr_getval_s(ch, 0, regdef);
}

static uint32_t ddr_getval_ach(uint32_t regdef, uint32_t *p)
{
	uint32_t ch;
	foreach_vch(ch)
		p[ch] = ddr_getval_s(ch, 0, regdef);
	return p[0];
}
/* NOT_USED
static uint32_t ddr_getval_ach_s(uint32_t slice, uint32_t regdef, uint32_t *p)
{
	uint32_t ch;
	foreach_vch(ch)
		p[ch] = ddr_getval_s(ch, slice, regdef);
	return p[0];
}
*/

static uint32_t ddr_getval_ach_as(uint32_t regdef, uint32_t *p)
{
	uint32_t ch, slice;
	uint32_t *pp;
	pp = p;
	foreach_vch(ch)
		for(slice=0; slice<SLICE_CNT; slice++)
			*pp++ = ddr_getval_s(ch, slice, regdef);
	return p[0];
}

/*******************************************************************************
 *	handling functions for setteing ddrphy value table
 ******************************************************************************/
static void _tblcopy(uint32_t *to, const uint32_t *from, uint32_t size)
{
	uint32_t i;
	for (i=0;i<size;i++) {
		to[i] = from[i];
	}
}

static void ddrtbl_setval ( uint32_t *tbl, uint32_t _regdef, uint32_t val)
{
	uint32_t adr;
	uint32_t lsb;
	uint32_t len;
	uint32_t msk;
	uint32_t tmp;
	uint32_t adrmsk;
	uint32_t regdef;
	regdef = ddr_regdef(_regdef);
	adr = DDR_REGDEF_ADR(regdef);
	len = DDR_REGDEF_LEN(regdef);
	lsb = DDR_REGDEF_LSB(regdef);
	if(len==0x20)
		msk = 0xffffffff;
	else
		msk = ((1U<<len)-1)<<lsb;

	if(adr<0x400){
		adrmsk = 0xff;
	} else {
		adrmsk = 0x7f;
	}

	tmp = tbl[adr & adrmsk];
	tmp = (tmp & (~msk)) | ((val<<lsb) & msk);
	tbl[adr & adrmsk] = tmp;
}

static uint32_t ddrtbl_getval ( uint32_t *tbl, uint32_t _regdef)
{
	uint32_t adr;
	uint32_t lsb;
	uint32_t len;
	uint32_t msk;
	uint32_t tmp;
	uint32_t adrmsk;
	uint32_t regdef;
	regdef = ddr_regdef(_regdef);
	adr = DDR_REGDEF_ADR(regdef);
	len = DDR_REGDEF_LEN(regdef);
	lsb = DDR_REGDEF_LSB(regdef);
	if(len==0x20)
		msk = 0xffffffff;
	else
		msk = ((1U<<len)-1);

	if(adr<0x400){
		adrmsk = 0xff;
	} else {
		adrmsk = 0x7f;
	}

	tmp = tbl[adr & adrmsk];
	tmp = (tmp >> lsb) & msk;

	return tmp;
}

/*******************************************************************************
 *	DDRPHY register access handling
 ******************************************************************************/
static uint32_t ddrphy_regif_chk(void)
{
	uint32_t tmp_ach[DRAM_CH_CNT];
	uint32_t ch;
	uint32_t err;
	uint32_t PI_VERSION_CODE;

	if ((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11< Prr_Cut)) {
		PI_VERSION_CODE=0x2040;		// H3ver2.0
	} else if ((PRR_PRODUCT_M3N==Prr_Product)||(PRR_PRODUCT_V3H==Prr_Product)) {
		PI_VERSION_CODE=0x2040;		// M3Nver1.0/V3Hver1.0
	} else {
		PI_VERSION_CODE=0x2041;		// H3ver1.x/M3ver1.x
	}
	ddr_getval_ach(_reg_PI_VERSION, (uint32_t *)tmp_ach);
	err = 0;
	foreach_vch(ch) {
		if(PI_VERSION_CODE!=tmp_ach[ch])
			err = 1;
	}
	return err;
}

/*******************************************************************************
 *	functions and parameters for timing setting
 ******************************************************************************/
struct _jedec_spec1 {
	uint16_t fx3;
	uint8_t RLwoDBI;
	uint8_t RLwDBI;
	uint8_t WL;
	uint8_t nWR;
	uint8_t nRTP;
	uint8_t MR1;
	uint8_t MR2;
};
#define JS1_USABLEC_SPEC_LO 2
#define JS1_USABLEC_SPEC_HI 5
#define JS1_FREQ_TBL_NUM 8
#define JS1_MR1(f) (0x04 | ((f)<<4))
#define JS1_MR2(f) (0x00 | ((f)<<3) | (f))
const struct _jedec_spec1 js1[JS1_FREQ_TBL_NUM] = {
/*A	{  800,  6,  6,  4,  6 , 8, JS1_MR1(0), JS1_MR2(0) },   533.333Mbps*/
/*A	{ 1600, 10, 12,  6, 10 , 8, JS1_MR1(1), JS1_MR2(1) },  1066.666Mbps*/
/*A	{ 2400, 14, 16,  8, 16 , 8, JS1_MR1(2), JS1_MR2(2) },  1600.000Mbps*/
/*B*/	{  800,  6,  6,  4,  6 , 8, JS1_MR1(0), JS1_MR2(0)|0x40 }, /*  533.333Mbps*/
/*B*/	{ 1600, 10, 12,  8, 10 , 8, JS1_MR1(1), JS1_MR2(1)|0x40 }, /* 1066.666Mbps*/
/*B*/	{ 2400, 14, 16, 12, 16 , 8, JS1_MR1(2), JS1_MR2(2)|0x40 }, /* 1600.000Mbps*/
/*A*/	{ 3200, 20, 22, 10, 20 , 8, JS1_MR1(3), JS1_MR2(3) }, /* 2133.333Mbps*/
/*A*/	{ 4000, 24, 28, 12, 24 ,10, JS1_MR1(4), JS1_MR2(4) }, /* 2666.666Mbps*/
/*A*/	{ 4800, 28, 32, 14, 30 ,12, JS1_MR1(5), JS1_MR2(5) }, /* 3200.000Mbps*/
/*A*/	{ 5600, 32, 36, 16, 34 ,14, JS1_MR1(6), JS1_MR2(6) }, /* 3733.333Mbps*/
/*A*/	{ 6400, 36, 40, 18, 40 ,16, JS1_MR1(7), JS1_MR2(7) }  /* 4266.666Mbps*/
};

struct _jedec_spec2 {
	uint16_t ps;
	uint16_t cyc;
};

#define JS2_tSR 0
#define JS2_tXP 1
#define JS2_tRTP 2
#define JS2_tRCD 3
#define JS2_tRPpb 4
#define JS2_tRPab 5
#define JS2_tRAS 6
#define JS2_tWR 7
#define JS2_tWTR 8
#define JS2_tRRD 9
#define JS2_tPPD 10
#define JS2_tFAW 11
#define JS2_tDQSCK 12
#define JS2_tCKEHCMD 13
#define JS2_tCKELCMD 14
#define JS2_tCKELPD 15
#define JS2_tMRR 16
#define JS2_tMRW 17
#define JS2_tMRD 18
#define JS2_tZQCALns 19
#define JS2_tZQLAT 20
#define JS2_tIEdly 21
#define JS2_TBLCNT 22

#define JS2_tRCpb (JS2_TBLCNT)
#define JS2_tRCab (JS2_TBLCNT+1)
#define JS2_tRFCpb (JS2_TBLCNT+2)
#define JS2_tRFCab (JS2_TBLCNT+3)
#define JS2_CNT (JS2_TBLCNT+4)

#define JS2_DERATE 0
const struct _jedec_spec2 jedec_spec2[2][JS2_TBLCNT] = {
	{
/*tSR   */	{ 15000 , 3 },
/*tXP   */	{  7500 , 3 },
/*tRTP  */	{  7500 , 8 },
/*tRCD  */	{ 18000 , 4 },
/*tRPpb */	{ 18000 , 3 },
/*tRPab */	{ 21000 , 3 },
/*tRAS  */	{ 42000 , 3 },
/*tWR   */	{ 18000 , 4 },
/*tWTR  */	{ 10000 , 8 },
/*tRRD  */	{ 10000 , 4 },
/*tPPD  */	{     0 , 0 },
/*tFAW  */	{ 40000 , 0 },
/*tDQSCK*/	{  3500 , 0 },
/*tCKEHCMD*/	{  7500 , 3 },
/*tCKELCMD*/	{  7500 , 3 },
/*tCKELPD*/	{  7500 , 3 },
/*tMRR*/	{     0 , 8 },
/*tMRW*/	{ 10000 ,10 },
/*tMRD*/	{ 14000 ,10 },
/*tZQCALns*/	{  1000*10 ,0 },
/*tZQLAT*/	{ 30000 ,10 },
/*tIEdly*/	{ 10000 , 0 }
	},{
/*tSR   */	{ 15000 , 3 },
/*tXP   */	{  7500 , 3 },
/*tRTP  */	{  7500 , 8 },
/*tRCD  */	{ 19875 , 4 },
/*tRPpb */	{ 19875 , 3 },
/*tRPab */	{ 22875 , 3 },
/*tRAS  */	{ 43875 , 3 },
/*tWR   */	{ 18000 , 4 },
/*tWTR  */	{ 10000 , 8 },
/*tRRD  */	{ 10000 , 4 },
/*tPPD  */	{     0 , 0 },
/*tFAW  */	{ 40000 , 0 },
/*tDQSCK*/	{  3600 , 0 },
/*tCKEHCMD*/	{  7500 , 3 },
/*tCKELCMD*/	{  7500 , 3 },
/*tCKELPD*/	{  7500 , 3 },
/*tMRR*/	{     0 , 8 },
/*tMRW*/	{ 10000 ,10 },
/*tMRD*/	{ 14000 ,10 },
/*tZQCALns*/	{1000*10 ,0 },
/*tZQLAT*/	{ 30000 ,10 },
/*tIEdly*/	{ 10000 , 0 }
	}
};

/* pb, ab */
const uint16_t jedec_spec2_tRFC_pb_ab[2][7] = {
/*	4Gb, 6Gb, 8Gb,12Gb, 16Gb, 24Gb(TBD), 32Gb(TBD)	*/
	{
	 60,  90,  90, 140, 140, 280, 280
	},
	{
	130, 180, 180, 280, 280, 560, 560
	}
};

static uint32_t js1_ind;
static uint16_t js2[JS2_CNT];
static uint8_t RL;
static uint8_t WL;

static uint16_t _f_scale(uint32_t ddr_mbps, uint32_t ddr_mbpsdiv, uint32_t ps, uint16_t cyc)
{
	uint32_t tmp;
	uint32_t div;
	tmp = ps * ddr_mbps;
	div = tmp / (2000000 * ddr_mbpsdiv);
	if(tmp != (div*2000000*ddr_mbpsdiv))
		div = div + 1;

	if(div>cyc)
		return (uint16_t)div;
	return cyc;
}

static void _f_scale_js2(uint32_t ddr_mbps, uint32_t ddr_mbpsdiv, uint16_t *js2)
{
	int i;
	for(i=0;i<JS2_TBLCNT;i++) {
		js2[i] = _f_scale(ddr_mbps, ddr_mbpsdiv,
			1UL*jedec_spec2[JS2_DERATE][i].ps,
			jedec_spec2[JS2_DERATE][i].cyc);
	}

	if(js2[JS2_tIEdly]>=0x0e)
		js2[JS2_tIEdly]=0xe;

	js2[JS2_tRCpb] = js2[JS2_tRAS] + js2[JS2_tRPpb];
	js2[JS2_tRCab] = js2[JS2_tRAS] + js2[JS2_tRPab];
}

/* scaler for DELAY value */
static int16_t _f_scale_adj(int16_t ps)
{
	int32_t tmp;
	/*
	tmp = (int32_t)512 * ps * ddr_mbps /2 / ddr_mbpsdiv / 1000 / 1000;
		= ps * ddr_mbps /2 / ddr_mbpsdiv *512 / 8 / 8 / 125 / 125
		= ps * ddr_mbps / ddr_mbpsdiv *4 / 125 / 125
	*/
	tmp = (int32_t)4 * (int32_t)ps * (int32_t)ddr_mbps / (int32_t)ddr_mbpsdiv;
	tmp = (int32_t)tmp / (int32_t)15625;

	return (int16_t)tmp;
}

const uint32_t _reg_PI_MR1_DATA_Fx_CSx[2][CSAB_CNT] = {
	{
		_reg_PI_MR1_DATA_F0_0,
		_reg_PI_MR1_DATA_F0_1,
		_reg_PI_MR1_DATA_F0_2,
		_reg_PI_MR1_DATA_F0_3
	},
	{
		_reg_PI_MR1_DATA_F1_0,
		_reg_PI_MR1_DATA_F1_1,
		_reg_PI_MR1_DATA_F1_2,
		_reg_PI_MR1_DATA_F1_3
	}
};

const uint32_t _reg_PI_MR2_DATA_Fx_CSx[2][CSAB_CNT] = {
	{
		_reg_PI_MR2_DATA_F0_0,
		_reg_PI_MR2_DATA_F0_1,
		_reg_PI_MR2_DATA_F0_2,
		_reg_PI_MR2_DATA_F0_3
	},
	{
		_reg_PI_MR2_DATA_F1_0,
		_reg_PI_MR2_DATA_F1_1,
		_reg_PI_MR2_DATA_F1_2,
		_reg_PI_MR2_DATA_F1_3
	}
};

const uint32_t _reg_PI_MR3_DATA_Fx_CSx[2][CSAB_CNT] = {
	{
		_reg_PI_MR3_DATA_F0_0,
		_reg_PI_MR3_DATA_F0_1,
		_reg_PI_MR3_DATA_F0_2,
		_reg_PI_MR3_DATA_F0_3
	},
	{
		_reg_PI_MR3_DATA_F1_0,
		_reg_PI_MR3_DATA_F1_1,
		_reg_PI_MR3_DATA_F1_2,
		_reg_PI_MR3_DATA_F1_3
	}
};

const uint32_t _reg_PI_MR11_DATA_Fx_CSx[2][CSAB_CNT] = {
	{
		_reg_PI_MR11_DATA_F0_0,
		_reg_PI_MR11_DATA_F0_1,
		_reg_PI_MR11_DATA_F0_2,
		_reg_PI_MR11_DATA_F0_3
	},
	{
		_reg_PI_MR11_DATA_F1_0,
		_reg_PI_MR11_DATA_F1_1,
		_reg_PI_MR11_DATA_F1_2,
		_reg_PI_MR11_DATA_F1_3
	}
};

const uint32_t _reg_PI_MR12_DATA_Fx_CSx[2][CSAB_CNT] = {
	{
		_reg_PI_MR12_DATA_F0_0,
		_reg_PI_MR12_DATA_F0_1,
		_reg_PI_MR12_DATA_F0_2,
		_reg_PI_MR12_DATA_F0_3
	},
	{
		_reg_PI_MR12_DATA_F1_0,
		_reg_PI_MR12_DATA_F1_1,
		_reg_PI_MR12_DATA_F1_2,
		_reg_PI_MR12_DATA_F1_3
	}
};

const uint32_t _reg_PI_MR14_DATA_Fx_CSx[2][CSAB_CNT] = {
	{
		_reg_PI_MR14_DATA_F0_0,
		_reg_PI_MR14_DATA_F0_1,
		_reg_PI_MR14_DATA_F0_2,
		_reg_PI_MR14_DATA_F0_3
	},
	{
		_reg_PI_MR14_DATA_F1_0,
		_reg_PI_MR14_DATA_F1_1,
		_reg_PI_MR14_DATA_F1_2,
		_reg_PI_MR14_DATA_F1_3
	}
};

/*******************************************************************************
 * regif pll w/a   ( REGIF_H3ver2/M3N/V3H_WA )
 *******************************************************************************/
static void regif_pll_wa(void)
{
	uint32_t ch;

	if ((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut)) {
		reg_ddrphy_write_a ( ddr_regdef_adr(_reg_PHY_PLL_WAIT) ,(0x64<<ddr_regdef_lsb(_reg_PHY_PLL_WAIT)));
		reg_ddrphy_write_a ( ddr_regdef_adr(_reg_PHY_PLL_CTRL),
			 ddrtbl_getval(_cnf_DDR_PHY_ADR_G_REGSET, _reg_PHY_PLL_CTRL));

		reg_ddrphy_write_a ( ddr_regdef_adr(_reg_PHY_LP4_BOOT_PLL_CTRL),
			 ddrtbl_getval(_cnf_DDR_PHY_ADR_G_REGSET, _reg_PHY_LP4_BOOT_PLL_CTRL));

	} else {
		// PLL setting for PHY : M3,M3N,V3H,H3ver2
		reg_ddrphy_write_a(ddr_regdef_adr(_reg_PHY_PLL_WAIT) ,(0x5064<<ddr_regdef_lsb(_reg_PHY_PLL_WAIT)));

		reg_ddrphy_write_a(ddr_regdef_adr(_reg_PHY_PLL_CTRL),
			(ddrtbl_getval(_cnf_DDR_PHY_ADR_G_REGSET, _reg_PHY_PLL_CTRL_TOP)<<16)|
			 ddrtbl_getval(_cnf_DDR_PHY_ADR_G_REGSET, _reg_PHY_PLL_CTRL));
		reg_ddrphy_write_a(ddr_regdef_adr(_reg_PHY_PLL_CTRL_CA),
			ddrtbl_getval(_cnf_DDR_PHY_ADR_G_REGSET, _reg_PHY_PLL_CTRL_CA));

		reg_ddrphy_write_a(ddr_regdef_adr(_reg_PHY_LP4_BOOT_PLL_CTRL),
			(ddrtbl_getval(_cnf_DDR_PHY_ADR_G_REGSET, _reg_PHY_LP4_BOOT_PLL_CTRL_CA)<<16)|
			 ddrtbl_getval(_cnf_DDR_PHY_ADR_G_REGSET, _reg_PHY_LP4_BOOT_PLL_CTRL));
		reg_ddrphy_write_a(ddr_regdef_adr(_reg_PHY_LP4_BOOT_TOP_PLL_CTRL),
			ddrtbl_getval(_cnf_DDR_PHY_ADR_G_REGSET, _reg_PHY_LP4_BOOT_TOP_PLL_CTRL));
	}

	/* protect register interface */
	ddrphy_regif_idle();
	pll3_control(0);

	if ((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut)) {
	 // non
	} else {
		reg_ddrphy_write_a ( ddr_regdef_adr(_reg_PHY_DLL_RST_EN) ,(0x1<<ddr_regdef_lsb(_reg_PHY_DLL_RST_EN)));
		ddrphy_regif_idle();
	}

	/***********************************************************************
	init start
	***********************************************************************/
	/* dbdficnt0:
	 * dfi_dram_clk_disable=1
	 * dfi_frequency = 0
	 * freq_ratio = 01 (2:1)
	 * init_start =0
	 */
	foreach_vch(ch)
		mmio_write_32(DBSC_DBDFICNT(ch), 0x00000F10);
	dsb_sev();

	/* dbdficnt0:
	 * dfi_dram_clk_disable=1
	 * dfi_frequency = 0
	 * freq_ratio = 01 (2:1)
	 * init_start =1
	 */
	foreach_vch(ch)
		mmio_write_32(DBSC_DBDFICNT(ch), 0x00000F11);
	dsb_sev();

	foreach_vch(ch)
		while ((mmio_read_32(DBSC_PLL_LOCK(ch))&0x1f)!=0x1f);
	dsb_sev();

}

/*******************************************************************************
 *	load table data into DDR registers
 ******************************************************************************/
static void ddrtbl_load(void)
{
	int i;
	uint32_t slice;
	uint32_t csab;
	uint32_t adr;
	uint32_t dataL;
	uint32_t tmp[3];
	uint16_t dataS;

	/***********************************************************************
	TIMING REGISTERS
	***********************************************************************/
	/* search jedec_spec1 index */
	for(i=JS1_USABLEC_SPEC_LO;i<JS1_FREQ_TBL_NUM-1;i++) {
		if(js1[i].fx3 * 2 * ddr_mbpsdiv >= ddr_mbps*3)
			break;
	}
	if(JS1_USABLEC_SPEC_HI<i)
		js1_ind = JS1_USABLEC_SPEC_HI;
	else
		js1_ind = i;

	if(Boardcnf->dbi_en)
		RL = js1[js1_ind].RLwDBI;
	else
		RL = js1[js1_ind].RLwoDBI;

	WL = js1[js1_ind].WL;
	/* calculate jedec_spec2 */
	_f_scale_js2(ddr_mbps, ddr_mbpsdiv, js2);

	/***********************************************************************
	PREPARE TBL
	***********************************************************************/
	if (PRR_PRODUCT_H3==Prr_Product) {
		if (PRR_PRODUCT_11>=Prr_Cut) {
		// H3 ver1.0/1.1
			_tblcopy(_cnf_DDR_PHY_SLICE_REGSET,
				 DDR_PHY_SLICE_REGSET_H3, DDR_PHY_SLICE_REGSET_NUM_H3);
			_tblcopy(_cnf_DDR_PHY_ADR_V_REGSET,
				DDR_PHY_ADR_V_REGSET_H3, DDR_PHY_ADR_V_REGSET_NUM_H3);
			_tblcopy(_cnf_DDR_PHY_ADR_I_REGSET,
				DDR_PHY_ADR_I_REGSET_H3, DDR_PHY_ADR_I_REGSET_NUM_H3);
			_tblcopy(_cnf_DDR_PHY_ADR_G_REGSET,
				DDR_PHY_ADR_G_REGSET_H3, DDR_PHY_ADR_G_REGSET_NUM_H3);
			_tblcopy(_cnf_DDR_PI_REGSET,
				DDR_PI_REGSET_H3, DDR_PI_REGSET_NUM_H3);

			DDR_PHY_SLICE_REGSET_OFS = DDR_PHY_SLICE_REGSET_OFS_H3;
			DDR_PHY_ADR_V_REGSET_OFS = DDR_PHY_ADR_V_REGSET_OFS_H3;
			DDR_PHY_ADR_I_REGSET_OFS = DDR_PHY_ADR_I_REGSET_OFS_H3;
			DDR_PHY_ADR_G_REGSET_OFS = DDR_PHY_ADR_G_REGSET_OFS_H3;
			DDR_PI_REGSET_OFS = DDR_PI_REGSET_OFS_H3;
			DDR_PHY_SLICE_REGSET_SIZE = DDR_PHY_SLICE_REGSET_SIZE_H3;
			DDR_PHY_ADR_V_REGSET_SIZE = DDR_PHY_ADR_V_REGSET_SIZE_H3;
			DDR_PHY_ADR_I_REGSET_SIZE = DDR_PHY_ADR_I_REGSET_SIZE_H3;
			DDR_PHY_ADR_G_REGSET_SIZE = DDR_PHY_ADR_G_REGSET_SIZE_H3;
			DDR_PI_REGSET_SIZE = DDR_PI_REGSET_SIZE_H3;
			DDR_PHY_SLICE_REGSET_NUM = DDR_PHY_SLICE_REGSET_NUM_H3;
			DDR_PHY_ADR_V_REGSET_NUM = DDR_PHY_ADR_V_REGSET_NUM_H3;
			DDR_PHY_ADR_I_REGSET_NUM = DDR_PHY_ADR_I_REGSET_NUM_H3;
			DDR_PHY_ADR_G_REGSET_NUM = DDR_PHY_ADR_G_REGSET_NUM_H3;
			DDR_PI_REGSET_NUM = DDR_PI_REGSET_NUM_H3;

			DDR_PHY_ADR_I_NUM=1;
		} else {
		// H3 ver2.0
			_tblcopy(_cnf_DDR_PHY_SLICE_REGSET,
				DDR_PHY_SLICE_REGSET_H3VER2, DDR_PHY_SLICE_REGSET_NUM_H3VER2);
			_tblcopy(_cnf_DDR_PHY_ADR_V_REGSET,
				DDR_PHY_ADR_V_REGSET_H3VER2, DDR_PHY_ADR_V_REGSET_NUM_H3VER2);
			_tblcopy(_cnf_DDR_PHY_ADR_G_REGSET,
				DDR_PHY_ADR_G_REGSET_H3VER2, DDR_PHY_ADR_G_REGSET_NUM_H3VER2);
			_tblcopy(_cnf_DDR_PI_REGSET,
				DDR_PI_REGSET_H3VER2, DDR_PI_REGSET_NUM_H3VER2);

			DDR_PHY_SLICE_REGSET_OFS = DDR_PHY_SLICE_REGSET_OFS_H3VER2;
			DDR_PHY_ADR_V_REGSET_OFS = DDR_PHY_ADR_V_REGSET_OFS_H3VER2;
			DDR_PHY_ADR_G_REGSET_OFS = DDR_PHY_ADR_G_REGSET_OFS_H3VER2;
			DDR_PI_REGSET_OFS = DDR_PI_REGSET_OFS_H3VER2;
			DDR_PHY_SLICE_REGSET_SIZE = DDR_PHY_SLICE_REGSET_SIZE_H3VER2;
			DDR_PHY_ADR_V_REGSET_SIZE = DDR_PHY_ADR_V_REGSET_SIZE_H3VER2;
			DDR_PHY_ADR_G_REGSET_SIZE = DDR_PHY_ADR_G_REGSET_SIZE_H3VER2;
			DDR_PI_REGSET_SIZE = DDR_PI_REGSET_SIZE_H3VER2;
			DDR_PHY_SLICE_REGSET_NUM = DDR_PHY_SLICE_REGSET_NUM_H3VER2;
			DDR_PHY_ADR_V_REGSET_NUM = DDR_PHY_ADR_V_REGSET_NUM_H3VER2;
			DDR_PHY_ADR_G_REGSET_NUM = DDR_PHY_ADR_G_REGSET_NUM_H3VER2;
			DDR_PI_REGSET_NUM = DDR_PI_REGSET_NUM_H3VER2;

			DDR_PHY_ADR_I_NUM=0;
		}
	} else if (PRR_PRODUCT_M3==Prr_Product) {
	// M3 ver1.0/1.1
		_tblcopy(_cnf_DDR_PHY_SLICE_REGSET,
			DDR_PHY_SLICE_REGSET_M3, DDR_PHY_SLICE_REGSET_NUM_M3);
		_tblcopy(_cnf_DDR_PHY_ADR_V_REGSET,
			DDR_PHY_ADR_V_REGSET_M3, DDR_PHY_ADR_V_REGSET_NUM_M3);
		_tblcopy(_cnf_DDR_PHY_ADR_I_REGSET,
			DDR_PHY_ADR_I_REGSET_M3, DDR_PHY_ADR_I_REGSET_NUM_M3);
		_tblcopy(_cnf_DDR_PHY_ADR_G_REGSET,
			DDR_PHY_ADR_G_REGSET_M3, DDR_PHY_ADR_G_REGSET_NUM_M3);
		_tblcopy(_cnf_DDR_PI_REGSET,
			DDR_PI_REGSET_M3, DDR_PI_REGSET_NUM_M3);

		DDR_PHY_SLICE_REGSET_OFS = DDR_PHY_SLICE_REGSET_OFS_M3;
		DDR_PHY_ADR_V_REGSET_OFS = DDR_PHY_ADR_V_REGSET_OFS_M3;
		DDR_PHY_ADR_I_REGSET_OFS = DDR_PHY_ADR_I_REGSET_OFS_M3;
		DDR_PHY_ADR_G_REGSET_OFS = DDR_PHY_ADR_G_REGSET_OFS_M3;
		DDR_PI_REGSET_OFS = DDR_PI_REGSET_OFS_M3;
		DDR_PHY_SLICE_REGSET_SIZE = DDR_PHY_SLICE_REGSET_SIZE_M3;
		DDR_PHY_ADR_V_REGSET_SIZE = DDR_PHY_ADR_V_REGSET_SIZE_M3;
		DDR_PHY_ADR_I_REGSET_SIZE = DDR_PHY_ADR_I_REGSET_SIZE_M3;
		DDR_PHY_ADR_G_REGSET_SIZE = DDR_PHY_ADR_G_REGSET_SIZE_M3;
		DDR_PI_REGSET_SIZE = DDR_PI_REGSET_SIZE_M3;
		DDR_PHY_SLICE_REGSET_NUM = DDR_PHY_SLICE_REGSET_NUM_M3;
		DDR_PHY_ADR_V_REGSET_NUM = DDR_PHY_ADR_V_REGSET_NUM_M3;
		DDR_PHY_ADR_I_REGSET_NUM = DDR_PHY_ADR_I_REGSET_NUM_M3;
		DDR_PHY_ADR_G_REGSET_NUM = DDR_PHY_ADR_G_REGSET_NUM_M3;
		DDR_PI_REGSET_NUM = DDR_PI_REGSET_NUM_M3;

		DDR_PHY_ADR_I_NUM=2;
	} else {
	// M3N ver1.0 , V3H ver1.0
		_tblcopy(_cnf_DDR_PHY_SLICE_REGSET,
			DDR_PHY_SLICE_REGSET_M3N, DDR_PHY_SLICE_REGSET_NUM_M3N);
		_tblcopy(_cnf_DDR_PHY_ADR_V_REGSET,
			DDR_PHY_ADR_V_REGSET_M3N, DDR_PHY_ADR_V_REGSET_NUM_M3N);
		_tblcopy(_cnf_DDR_PHY_ADR_I_REGSET,
			DDR_PHY_ADR_I_REGSET_M3N, DDR_PHY_ADR_I_REGSET_NUM_M3N);
		_tblcopy(_cnf_DDR_PHY_ADR_G_REGSET,
			DDR_PHY_ADR_G_REGSET_M3N, DDR_PHY_ADR_G_REGSET_NUM_M3N);
		_tblcopy(_cnf_DDR_PI_REGSET,
			DDR_PI_REGSET_M3N, DDR_PI_REGSET_NUM_M3N);

		DDR_PHY_SLICE_REGSET_OFS = DDR_PHY_SLICE_REGSET_OFS_M3N;
		DDR_PHY_ADR_V_REGSET_OFS = DDR_PHY_ADR_V_REGSET_OFS_M3N;
		DDR_PHY_ADR_I_REGSET_OFS = DDR_PHY_ADR_I_REGSET_OFS_M3N;
		DDR_PHY_ADR_G_REGSET_OFS = DDR_PHY_ADR_G_REGSET_OFS_M3N;
		DDR_PI_REGSET_OFS = DDR_PI_REGSET_OFS_M3N;
		DDR_PHY_SLICE_REGSET_SIZE = DDR_PHY_SLICE_REGSET_SIZE_M3N;
		DDR_PHY_ADR_V_REGSET_SIZE = DDR_PHY_ADR_V_REGSET_SIZE_M3N;
		DDR_PHY_ADR_I_REGSET_SIZE = DDR_PHY_ADR_I_REGSET_SIZE_M3N;
		DDR_PHY_ADR_G_REGSET_SIZE = DDR_PHY_ADR_G_REGSET_SIZE_M3N;
		DDR_PI_REGSET_SIZE = DDR_PI_REGSET_SIZE_M3N;
		DDR_PHY_SLICE_REGSET_NUM = DDR_PHY_SLICE_REGSET_NUM_M3N;
		DDR_PHY_ADR_V_REGSET_NUM = DDR_PHY_ADR_V_REGSET_NUM_M3N;
		DDR_PHY_ADR_I_REGSET_NUM = DDR_PHY_ADR_I_REGSET_NUM_M3N;
		DDR_PHY_ADR_G_REGSET_NUM = DDR_PHY_ADR_G_REGSET_NUM_M3N;
		DDR_PI_REGSET_NUM = DDR_PI_REGSET_NUM_M3N;

		DDR_PHY_ADR_I_NUM=2;
	}

	/***********************************************************************
	PLL CODE CHANGE
	***********************************************************************/
	if ((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11==Prr_Cut)) {
		ddrtbl_setval(_cnf_DDR_PHY_ADR_G_REGSET, _reg_PHY_PLL_CTRL, 0x1142);
		ddrtbl_setval(_cnf_DDR_PHY_ADR_G_REGSET, _reg_PHY_LP4_BOOT_PLL_CTRL, 0x1142);
	}

	/***********************************************************************
	on fly gate adjust
	***********************************************************************/
	if ((PRR_PRODUCT_M3==Prr_Product) && (PRR_PRODUCT_10== Prr_Cut)) {
		ddrtbl_setval(_cnf_DDR_PHY_SLICE_REGSET, _reg_ON_FLY_GATE_ADJUST_EN, 0);
	}

	/***********************************************************************
	Adjust PI paramters
	***********************************************************************/
	dataS = js2[JS2_tIEdly];
	ddrtbl_setval(_cnf_DDR_PHY_SLICE_REGSET, _reg_PHY_RDDATA_EN_DLY, dataS);
	ddrtbl_setval(_cnf_DDR_PHY_SLICE_REGSET, _reg_PHY_RDDATA_EN_TSEL_DLY, (dataS-2));

	ddrtbl_setval(_cnf_DDR_PI_REGSET, _reg_PI_RDLAT_ADJ_F1, RL-dataS);
	if(ddrtbl_getval(_cnf_DDR_PHY_SLICE_REGSET, _reg_PHY_WRITE_PATH_LAT_ADD)){
		dataL = WL - 1;
	} else {
		dataL = WL;
	}
	ddrtbl_setval(_cnf_DDR_PI_REGSET, _reg_PI_WRLAT_ADJ_F1, dataL-2);
	ddrtbl_setval(_cnf_DDR_PI_REGSET, _reg_PI_WRLAT_F1, dataL);

	if(Boardcnf->dbi_en) {
		ddrtbl_setval(_cnf_DDR_PHY_SLICE_REGSET, _reg_PHY_DBI_MODE, 0x1);
		ddrtbl_setval(_cnf_DDR_PHY_SLICE_REGSET, _reg_PHY_WDQLVL_DATADM_MASK, 0x0);
	} else {
		ddrtbl_setval(_cnf_DDR_PHY_SLICE_REGSET, _reg_PHY_DBI_MODE, 0x0);
		ddrtbl_setval(_cnf_DDR_PHY_SLICE_REGSET, _reg_PHY_WDQLVL_DATADM_MASK, 0x100);
	}

	tmp[0] = js1[js1_ind].MR1;
	tmp[1] = js1[js1_ind].MR2;
	dataL = ddrtbl_getval(_cnf_DDR_PI_REGSET, _reg_PI_MR3_DATA_F1_0);
	if(Boardcnf->dbi_en)
		tmp[2] = dataL | 0xc0;
	else
		tmp[2] = dataL & ~0xc0;

	for(i=0;i<2;i++){
		for(csab=0;csab<CSAB_CNT;csab++){
			ddrtbl_setval(_cnf_DDR_PI_REGSET,
				_reg_PI_MR1_DATA_Fx_CSx[i][csab], tmp[0]);
			ddrtbl_setval(_cnf_DDR_PI_REGSET,
				_reg_PI_MR2_DATA_Fx_CSx[i][csab], tmp[1]);
			ddrtbl_setval(_cnf_DDR_PI_REGSET,
				_reg_PI_MR3_DATA_Fx_CSx[i][csab], tmp[2]);
		}
	}

	/***********************************************************************
	 DDRPHY INT START
	***********************************************************************/
	if ((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut)) {
		// non
	} else {
		regif_pll_wa();
	}

	/***********************************************************************
	FREQ_SEL_MULTICAST & PER_CS_TRAINING_MULTICAST SET (for safety)
	***********************************************************************/
	ddr_setval_ach(_reg_PHY_FREQ_SEL_MULTICAST_EN,0x1);
	ddr_setval_ach_as(_reg_PHY_PER_CS_TRAINING_MULTICAST_EN,0x1);

	/***********************************************************************
	SET DATA SLICE TABLE
	***********************************************************************/
	for( slice=0; slice<SLICE_CNT; slice++){
		adr = DDR_PHY_SLICE_REGSET_OFS + DDR_PHY_SLICE_REGSET_SIZE * slice;
		for( i=0; i<DDR_PHY_SLICE_REGSET_NUM; i++ ) {
			reg_ddrphy_write_a(adr+i, _cnf_DDR_PHY_SLICE_REGSET[i]);
		}
	}

	/***********************************************************************
	SET ADR SLICE TABLE
	***********************************************************************/
	adr = DDR_PHY_ADR_V_REGSET_OFS;
	for( i=0; i<DDR_PHY_ADR_V_REGSET_NUM; i++ ) {
		reg_ddrphy_write_a(adr+i, _cnf_DDR_PHY_ADR_V_REGSET[i]);
	}

	if (DDR_PHY_ADR_I_NUM > 0) {
		for( slice=0; slice<DDR_PHY_ADR_I_NUM; slice++){
			adr = DDR_PHY_ADR_I_REGSET_OFS + DDR_PHY_ADR_I_REGSET_SIZE * slice;
			for( i=0; i<DDR_PHY_ADR_I_REGSET_NUM; i++ ) {
				reg_ddrphy_write_a(adr+i, _cnf_DDR_PHY_ADR_I_REGSET[i]);
			}
		}
	}

	/***********************************************************************
	SET ADRCTRL SLICE TABLE
	***********************************************************************/
	adr = DDR_PHY_ADR_G_REGSET_OFS;
	for( i=0; i<DDR_PHY_ADR_G_REGSET_NUM; i++ ) {
		reg_ddrphy_write_a(adr+i ,_cnf_DDR_PHY_ADR_G_REGSET[i]);
	}

	/***********************************************************************
	SET PI REGISTERS
	***********************************************************************/
	adr = DDR_PI_REGSET_OFS;
	for( i=0; i<DDR_PI_REGSET_NUM; i++ )
	{
		reg_ddrphy_write_a(adr+i, _cnf_DDR_PI_REGSET[i]);
	}
}

/*******************************************************************************
 *	CONFIGURE DDR REGISTERS
 ******************************************************************************/
static void ddr_config_sub(void)
{
	int32_t i;
	uint32_t ch, slice;
	uint32_t dataL;
	uint32_t tmp;
	uint8_t high_byte[SLICE_CNT];
	foreach_vch(ch){
	/***********************************************************************
	BOARD SETTINGS (DQ,DM,VREF_DRIVING)
	***********************************************************************/
		for(slice=0;slice<SLICE_CNT;slice++){
			high_byte[slice] = (Boardcnf->ch[ch].dqs_swap >> (4*slice))%2;
			ddr_setval_s(ch, slice, _reg_PHY_DQ_DM_SWIZZLE0, Boardcnf->ch[ch].dq_swap[slice]);
			ddr_setval_s(ch, slice, _reg_PHY_DQ_DM_SWIZZLE1, Boardcnf->ch[ch].dm_swap[slice]);
			if( high_byte[slice] ) {
				/* HIGHER 16 BYTE */
				ddr_setval_s(ch, slice, _reg_PHY_CALVL_VREF_DRIVING_SLICE,0);
			} else {
				/* LOWER 16 BYTE */
				ddr_setval_s(ch, slice, _reg_PHY_CALVL_VREF_DRIVING_SLICE,1);
			}
		}

	/***********************************************************************
		BOARD SETTINGS (CA,ADDR_SEL)
	***********************************************************************/
		const uint32_t _par_CALVL_DEVICE_MAP=1;
		dataL = Boardcnf->ch[ch].ca_swap | 0x00888888;

		/* --- ADR_CALVL_SWIZZLE --- */
		if (PRR_PRODUCT_M3==Prr_Product) {
			ddr_setval(ch, _reg_PHY_ADR_CALVL_SWIZZLE0_0, dataL);
			ddr_setval(ch, _reg_PHY_ADR_CALVL_SWIZZLE1_0, 0x00000000);
			ddr_setval(ch, _reg_PHY_ADR_CALVL_SWIZZLE0_1, dataL);
			ddr_setval(ch, _reg_PHY_ADR_CALVL_SWIZZLE1_1, 0x00000000);
			ddr_setval(ch, _reg_PHY_ADR_CALVL_DEVICE_MAP, _par_CALVL_DEVICE_MAP);
		} else {
			ddr_setval(ch, _reg_PHY_ADR_CALVL_SWIZZLE0, dataL);
			ddr_setval(ch, _reg_PHY_ADR_CALVL_SWIZZLE1, 0x00000000);
			ddr_setval(ch, _reg_PHY_CALVL_DEVICE_MAP, _par_CALVL_DEVICE_MAP);
		}

		/* --- ADR_ADDR_SEL --- */
		if ((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11< Prr_Cut)) {
			dataL= 0x00FFFFFF & Boardcnf->ch[ch].ca_swap;
		} else {
			dataL=0;
			tmp = Boardcnf->ch[ch].ca_swap;
			for(i=0;i<6;i++){
				dataL |= ((tmp&0xf)<<(i*5));
				tmp=tmp>>4;
			}
		}
		ddr_setval(ch, _reg_PHY_ADR_ADDR_SEL, dataL);

	/***********************************************************************
		BOARD SETTINGS (BYTE_ORDER_SEL)
	***********************************************************************/
		if(PRR_PRODUCT_M3==Prr_Product) {
			/* --- DATA_BYTE_SWAP --- */
			dataL=0;
			tmp=Boardcnf->ch[ch].dqs_swap;
			for(i=0;i<4;i++){
				dataL |= ((tmp&0x3)<<(i*2));
				tmp = tmp>>4;
			}
		} else {
			/* --- DATA_BYTE_SWAP --- */
			dataL = Boardcnf->ch[ch].dqs_swap;
			ddr_setval(ch, _reg_PI_DATA_BYTE_SWAP_EN, 1);
			ddr_setval(ch, _reg_PI_DATA_BYTE_SWAP_SLICE0, (dataL       ) & 0x0f );
			ddr_setval(ch, _reg_PI_DATA_BYTE_SWAP_SLICE1, (dataL >> 4*1) & 0x0f );
			ddr_setval(ch, _reg_PI_DATA_BYTE_SWAP_SLICE2, (dataL >> 4*2) & 0x0f );
			ddr_setval(ch, _reg_PI_DATA_BYTE_SWAP_SLICE3, (dataL >> 4*3) & 0x0f );

			ddr_setval(ch, _reg_PHY_DATA_BYTE_ORDER_SEL_HIGH, 0x0);
		}
		ddr_setval(ch, _reg_PHY_DATA_BYTE_ORDER_SEL, dataL);
	}
}

static void get_ca_swizzle( uint32_t ch, uint32_t ddr_csn, uint32_t *p_swz)
{
	uint32_t slice;
	uint32_t tmp;
	uint32_t tgt;
	if(ddr_csn/2) {
		tgt=3;
	} else {
		tgt=1;
	}

	for(slice=0;slice<SLICE_CNT;slice++) {
		tmp = (Boardcnf->ch[ch].dqs_swap >> (4*slice)) & 0xf;
		if(tgt==tmp)break;
	}
	tmp = Boardcnf->ch[ch].ca_swap;
	if(slice%2)
		tmp |= 0x00888888;
	*p_swz = tmp;
}

static void ddr_config_sub_h3v1x(void)
{
	uint32_t ch, slice;
	uint32_t dataL;
	uint32_t tmp;
	uint8_t high_byte[SLICE_CNT];

	foreach_vch(ch){
		uint32_t ca_swizzle;
		uint32_t ca;
		uint32_t csmap;
	/***********************************************************************
		BOARD SETTINGS (DQ,DM,VREF_DRIVING)
	***********************************************************************/
		csmap=0;
		for(slice=0;slice<SLICE_CNT;slice++){
			tmp = (Boardcnf->ch[ch].dqs_swap >> (4*slice)) & 0xf;
			high_byte[slice] = tmp%2;
			if(tmp==1 && (slice>=2))csmap|=0x05;
			if(tmp==3 && (slice>=2))csmap|=0x50;
			ddr_setval_s(ch, slice, _reg_PHY_DQ_SWIZZLING, Boardcnf->ch[ch].dq_swap[slice]);
			if( high_byte[slice] ) {
				/* HIGHER 16 BYTE */
				ddr_setval_s(ch, slice, _reg_PHY_CALVL_VREF_DRIVING_SLICE,0);
			} else {
				/* LOWER 16 BYTE */
				ddr_setval_s(ch, slice, _reg_PHY_CALVL_VREF_DRIVING_SLICE,1);
			}
		}
	/***********************************************************************
		BOARD SETTINGS (CA,ADDR_SEL)
	***********************************************************************/
		ca=Boardcnf->ch[ch].ca_swap;
		ddr_setval(ch, _reg_PHY_ADR_ADDR_SEL, ca);
		ddr_setval(ch, _reg_PHY_CALVL_CS_MAP, csmap);

		get_ca_swizzle(ch, 0, &ca_swizzle);

		ddr_setval(ch, _reg_PHY_ADR_CALVL_SWIZZLE0_0, ca_swizzle);
		ddr_setval(ch, _reg_PHY_ADR_CALVL_SWIZZLE1_0, 0x00000000);
		ddr_setval(ch, _reg_PHY_ADR_CALVL_SWIZZLE0_1, 0x00000000);
		ddr_setval(ch, _reg_PHY_ADR_CALVL_SWIZZLE1_1, 0x00000000);
		ddr_setval(ch, _reg_PHY_ADR_CALVL_DEVICE_MAP, 1);

		for(slice=0;slice<SLICE_CNT;slice++){
			const uint8_t o_mr15=0x55;
			const uint8_t o_mr20=0x55;
			const uint16_t o_mr32_mr40 = 0x5a3c;
			uint32_t o_inv;
			uint32_t inv;
			uint32_t bit_soc;
			uint32_t bit_mem;
			uint32_t j;

			ddr_setval_s(ch, slice, _reg_PI_RDLVL_PATTERN_NUM, 1);
			ddr_setval_s(ch, slice, _reg_PI_RDLVL_PATTERN_START, 8);

			if(high_byte[slice])
				o_inv = o_mr20;
			else
				o_inv = o_mr15;

			tmp = Boardcnf->ch[ch].dq_swap[slice];
			inv = 0;
			j = 0;
			for(bit_soc=0;bit_soc<8;bit_soc++) {
				bit_mem = (tmp>>(4*bit_soc)) & 0x0f;
				j|=(1U<<bit_mem);
				if(o_inv & (1U<<bit_mem))
					inv |= (1U<<bit_soc);
			}
			dataL = o_mr32_mr40;
			if(!high_byte[slice])dataL |= (inv<<24);
			if( high_byte[slice])dataL |= (inv<<16);
			ddr_setval_s(ch, slice, _reg_PHY_LP4_RDLVL_PATT8, dataL);
		}
	}
}

static void ddr_config(void)
{
	int32_t i;
	uint32_t ch, slice;
	uint32_t dataL;
	uint32_t tmp;

	if((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut)) {
		ddr_config_sub_h3v1x();
	} else {
		ddr_config_sub();   // H3ver2.0/M3N/V3H is same as M3
	}

	/***********************************************************************
	WDQ_USER_PATT
	***********************************************************************/
	foreach_vch(ch){
		union {
			uint32_t ui32[4];
			uint8_t ui8[16];
		} patt;
		uint16_t patm;
		for(slice=0;slice<SLICE_CNT;slice++) {
			patm=0;
			for(i=0;i<16;i++){
				tmp = Boardcnf->ch[ch].wdqlvl_patt[i];
				patt.ui8[i] = tmp & 0xff;
				if(tmp&0x100)
					patm |= (1<<i);
			}
			ddr_setval_s(ch, slice, _reg_PHY_USER_PATT0, patt.ui32[0]);
			ddr_setval_s(ch, slice, _reg_PHY_USER_PATT1, patt.ui32[1]);
			ddr_setval_s(ch, slice, _reg_PHY_USER_PATT2, patt.ui32[2]);
			ddr_setval_s(ch, slice, _reg_PHY_USER_PATT3, patt.ui32[3]);
			ddr_setval_s(ch, slice, _reg_PHY_USER_PATT4, patm);
		}
	}

	/***********************************************************************
	CACS DLY
	***********************************************************************/
	dataL = Boardcnf->cacs_dly + _f_scale_adj(Boardcnf->cacs_dly_adj);
	set_dfifrequency(0x1f);
	foreach_vch(ch){
		int16_t adj;
		for(i=0;i<_reg_PHY_CLK_CACS_SLAVE_DELAY_X_NUM;i++){
			adj = _f_scale_adj(Boardcnf->ch[ch].cacs_adj[i]);
			ddr_setval(ch, _reg_PHY_CLK_CACS_SLAVE_DELAY_X[i],
				dataL + adj
			);
		}
	}
	set_dfifrequency(0x00);

	/***********************************************************************
	WDQDM DLY
	***********************************************************************/
	dataL = Boardcnf->dqdm_dly_w;
	foreach_vch(ch){
		int8_t _adj;
		int16_t adj;
		uint32_t dq;
		for(slice=0;slice<SLICE_CNT;slice++){
			for(i=0;i<=8;i++){
				dq = slice*8+i;
				if(i==8)
					_adj = Boardcnf->ch[ch].dm_adj_w[slice];
				else
					_adj = Boardcnf->ch[ch].dq_adj_w[dq];
				adj = _f_scale_adj(_adj);
				ddr_setval_s(ch, slice,
					_reg_PHY_CLK_WRX_SLAVE_DELAY[i],
					dataL + adj
				);
			}
		}
	}

	/***********************************************************************
	RDQDM DLY
	***********************************************************************/
	dataL = Boardcnf->dqdm_dly_r;
	foreach_vch(ch){
		int8_t _adj;
		int16_t adj;
		uint32_t dq;
		for(slice=0;slice<SLICE_CNT;slice++){
			for(i=0;i<=8;i++){
				dq = slice*8+i;
				if(i==8)
					_adj = Boardcnf->ch[ch].dm_adj_r[slice];
				else
					_adj = Boardcnf->ch[ch].dq_adj_r[dq];
				adj = _f_scale_adj(_adj);
				ddr_setval_s(ch, slice,
					_reg_PHY_RDDQS_X_FALL_SLAVE_DELAY[i],
					dataL + adj
				);
				ddr_setval_s(ch, slice,
					_reg_PHY_RDDQS_X_RISE_SLAVE_DELAY[i],
					dataL + adj
				);
			}
		}
	}
}

/*******************************************************************************
 *	DBSC register setting functions
 ******************************************************************************/
__attribute__ ((noinline)) static void dbsc_regset_pre(void)
{
	uint32_t ch, csab;
	uint32_t dataL;

	/***********************************************************************
	PRIMARY SETTINGS
	***********************************************************************/
	/* LPDDR4, BL=16, DFI interface */
	mmio_write_32(DBSC_DBKIND, 0x0000000a);
	mmio_write_32(DBSC_DBBL, 0x00000002);
	mmio_write_32(DBSC_DBPHYCONF0, 0x00000001);

	/* FREQRATIO=2 */
	mmio_write_32(DBSC_DBSYSCONF1, 0x00000002);

	/* Chanel map (H3ver1.x) */
	if((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut))
		mmio_write_32(DBSC_DBSCHCNT1, 0x00001010);

	/* DRAM SIZE REGISTER:
	 * set all ranks as density=0(4Gb) for PHY initialization
	 */
	foreach_vch(ch)
		for(csab=0;csab<4;csab++)
			mmio_write_32(DBSC_DBMEMCONF(ch,csab), DBMEMCONF_REGD(0));

	if(PRR_PRODUCT_M3==Prr_Product) {
		dataL = 0xe4e4e4e4;
		foreach_ech(ch) {
			if((Boardcnf->phyvalid & (1<<ch)))
				dataL = (dataL &(~(0x000000FF << (ch*8))))
					|((	  (Boardcnf->ch[ch].dqs_swap & 0x0003)
						|((Boardcnf->ch[ch].dqs_swap & 0x0030) >> 2)
						|((Boardcnf->ch[ch].dqs_swap & 0x0300) >> 4)
						|((Boardcnf->ch[ch].dqs_swap & 0x3000) >> 6)	)<< (ch*8));
		}
		mmio_write_32(DBSC_DBBSWAP, dataL);
	}
}

static void dbsc_regset(void)
{
	int32_t i;
	uint32_t ch;
	uint32_t dataL;
	uint32_t tmp[4];
	/* RFC */
	js2[JS2_tRFCpb] = _f_scale(ddr_mbps, ddr_mbpsdiv,
		1UL*jedec_spec2_tRFC_pb_ab[0][max_density] * 1000, 0);
	js2[JS2_tRFCab] = _f_scale(ddr_mbps, ddr_mbpsdiv,
		1UL*jedec_spec2_tRFC_pb_ab[1][max_density] * 1000, 0);

	/* DBTR0.CL  : RL */
	mmio_write_32(DBSC_DBTR(0), RL);

	/* DBTR1.CWL : WL */
	mmio_write_32(DBSC_DBTR(1), WL);

	/* DBTR2.AL  : 0 */
	mmio_write_32(DBSC_DBTR(2), 0);

	/* DBTR3.TRCD: tRCD */
	mmio_write_32(DBSC_DBTR(3), js2[JS2_tRCD]);

	/* DBTR4.TRPA,TRP: tRPab,tRPpb */
	mmio_write_32(DBSC_DBTR(4), (js2[JS2_tRPab]<<16) | js2[JS2_tRPpb]);

	/* DBTR5.TRC : use tRCpb */
	mmio_write_32(DBSC_DBTR(5), js2[JS2_tRCpb]);

	/* DBTR6.TRAS : tRAS */
	mmio_write_32(DBSC_DBTR(6), js2[JS2_tRAS]);

	/* DBTR7.TRRD : tRRD */
	mmio_write_32(DBSC_DBTR(7), (js2[JS2_tRRD]<<16) | js2[JS2_tRRD]);

	/* DBTR8.TFAW : tFAW */
	mmio_write_32(DBSC_DBTR(8), js2[JS2_tFAW]);

	/* DBTR9.TRDPR : tRTP */
	mmio_write_32(DBSC_DBTR(9), js2[JS2_tRTP]);

	/* DBTR10.TWR : nWR */
	mmio_write_32(DBSC_DBTR(10), js1[js1_ind].nWR);

	/* DBTR11.TRDWR : RL + tDQSCK + BL/2 + Rounddown(tRPST) - WL + tWPRE */
	mmio_write_32(DBSC_DBTR(11),
		RL + js2[JS2_tDQSCK] + (16/2) + 1 - WL + 2 +2);

	/* DBTR12.TWRRD : WL + 1 + BL/2 + tWTR */
	dataL = WL + 1 + (16/2) + js2[JS2_tWTR];
	mmio_write_32(DBSC_DBTR(12), (dataL<<16) | dataL);

	/* DBTR13.TRFCPB,TRFCAB : tRFCpb, tRFCab */
	mmio_write_32(DBSC_DBTR(13),
		(js2[JS2_tRFCab]));

	/* DBTR14.TCKEHDLL,tCKEH : tCKEHCMD,tCKEHCMD */
	mmio_write_32(DBSC_DBTR(14),
		(js2[JS2_tCKEHCMD] <<16) | (js2[JS2_tCKEHCMD]));

	/* DBTR15.TCKESR,TCKEL : tSR,tCKELPD */
	mmio_write_32(DBSC_DBTR(15),
		(js2[JS2_tSR] <<16) | (js2[JS2_tCKELPD]));

	/* DBTR16 & DBTR24*/
	/* WDQL : tphy_wrlat + tphy_wrdata */
	tmp[0] = ddrtbl_getval(_cnf_DDR_PI_REGSET, _reg_PI_WRLAT_F1);
	/* DQENLTNCY : tphy_wrlat = WL-2 */
	tmp[1] = ddrtbl_getval(_cnf_DDR_PI_REGSET, _reg_PI_WRLAT_ADJ_F1);
	/* DQL : tphy_rdlat + trdata_en */
	/* it is not important for dbsc */
	tmp[2] = RL + 16;
	/* DQIENLTNCY : trdata_en */
	tmp[3] = ddrtbl_getval(_cnf_DDR_PI_REGSET, _reg_PI_RDLAT_ADJ_F1) - 1;
	mmio_write_32(DBSC_DBTR(16),
		(tmp[3]<<24) | (tmp[2]<<16) | (tmp[1]<<8) | tmp[0]);

	/* WRCSLAT = WRLAT -5*/
	tmp[0] -= 5;
	/* WRCSGAP = 5 */
	tmp[1] = 5;
	/* RDCSLAT = RDLAT_ADJ +2*/
	tmp[2] = tmp[3];
	/* RDCSGAP = 6 */
	tmp[3] = 6;
	mmio_write_32(DBSC_DBTR(24),
		(tmp[3]<<24) | (tmp[2]<<16) | (tmp[1]<<8) | tmp[0]);

	/* DBTR17.TMODRD,TMOD,TRDMR: tMRR,tMRD,(0)*/
	mmio_write_32(DBSC_DBTR(17), (js2[JS2_tMRR]<<24)|(js2[JS2_tMRD]<<16));

	/* DBTR18.RODTL, RODTA, WODTL, WODTA : do not use in LPDDR4 */
	mmio_write_32(DBSC_DBTR(18), 0);

	/* DBTR19.TZQCL, TZQCS : do not use in LPDDR4 */
	mmio_write_32(DBSC_DBTR(19), 0);

	/* DBTR20.TXSDLL, TXS : tRFCab+tCKEHCMD */
	dataL = js2[JS2_tRFCab] + js2[JS2_tCKEHCMD];
	mmio_write_32(DBSC_DBTR(20), (dataL<<16) | dataL);

	/* DBTR21.TCCD */
	/* DBTR23.TCCD */
	/* H3ver1.0 cannot use TBTR23 feature */
	if(ddr_tccd==8 &&
		!((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_10>=Prr_Cut))
	) {
		dataL=8;
		mmio_write_32(DBSC_DBTR(21), (dataL<<16) | dataL);
		mmio_write_32(DBSC_DBTR(23), 0x00000002);
	} else if(ddr_tccd<=11) {
		dataL=11;
		mmio_write_32(DBSC_DBTR(21), (dataL<<16) | dataL);
		mmio_write_32(DBSC_DBTR(23), 0x00000000);
	} else {
		dataL=ddr_tccd;
		mmio_write_32(DBSC_DBTR(21), (dataL<<16) | dataL);
		mmio_write_32(DBSC_DBTR(23), 0x00000000);
	}

	/* DBTR22.ZQLAT : */
	dataL = js2[JS2_tZQCALns]*100;	// 1000 * 1000 ps
	dataL = (dataL<<16) | (js2[JS2_tZQLAT]+24 +20);
	mmio_write_32(DBSC_DBTR(22), dataL);

	/* DBTR25 : do not use in LPDDR4 */
	mmio_write_32(DBSC_DBTR(25), 0);

	/* DBTR24 : */
	/*
	 * DBSC_DBRNK2 rkrr
	 * DBSC_DBRNK3 rkrw
	 * DBSC_DBRNK4 rkwr
	 * DBSC_DBRNK5 rkww
	 */
 #define _par_DBRNK_VAL		(0x7007)

	for(i=0;i<4;i++){
		uint32_t dataL2;
		dataL=(_par_DBRNK_VAL>>(i*4)) & 0xf;
		dataL2=0;
		foreach_vch(ch){
			dataL2=(dataL2<<4) | dataL;
		}
		mmio_write_32(DBSC_DBRNK(2+i), dataL2);
	}
	mmio_write_32(DBSC_DBADJ0, 0x00000000);

	/***********************************************************************
	timing registers for Scheduler
	***********************************************************************/
	/* SCFCTST0 */
	/* SCFCTST0 ACT-ACT*/
	tmp[3] = 1UL * js2[JS2_tRCpb] * 800 * ddr_mbpsdiv /ddr_mbps;
	/* SCFCTST0 RDA-ACT*/
	tmp[2] = 1UL * ((16/2) + js2[JS2_tRTP] -8 + js2[JS2_tRPpb]) * 800 * ddr_mbpsdiv /ddr_mbps;
	/* SCFCTST0 WRA-ACT*/
	tmp[1] = 1UL * (WL + 1 + (16/2) + js1[js1_ind].nWR) * 800 * ddr_mbpsdiv /ddr_mbps;
	/* SCFCTST0 PRE-ACT*/
	tmp[0] = 1UL * js2[JS2_tRPpb];
	mmio_write_32(DBSC_SCFCTST0, (tmp[3]<<24) | (tmp[2]<<16) | (tmp[1]<<8) | tmp[0]);

	/* SCFCTST1 */
	/* SCFCTST1 RD-WR*/
	tmp[3] = 1UL * (mmio_read_32(DBSC_DBTR(11)) & 0xff) * 800 * ddr_mbpsdiv /ddr_mbps;
	/* SCFCTST1 WR-RD*/
	tmp[2] = 1UL * (mmio_read_32(DBSC_DBTR(12)) & 0xff) * 800 * ddr_mbpsdiv /ddr_mbps;
	/* SCFCTST1 ACT-RD/WR*/
	tmp[1] = 1UL * js2[JS2_tRCD] * 800 * ddr_mbpsdiv /ddr_mbps;
	/* SCFCTST1 ASYNCOFS*/
	tmp[0] = 12;
	mmio_write_32(DBSC_SCFCTST1, (tmp[3]<<24) | (tmp[2]<<16) | (tmp[1]<<8) | tmp[0]);

	/* DBSCHRW1 */
	/* DBSCHRW1 SCTRFCAB*/
	tmp[0] = 1UL * js2[JS2_tRFCab] * 800 * ddr_mbpsdiv /ddr_mbps;
	if(tmp[0] <  0x4b) tmp[0] = 0x4b;
	mmio_write_32(DBSC_DBSCHRW1, tmp[0]);

	/***********************************************************************
	QOS and CAM
	***********************************************************************/
#ifdef ddr_qos_init_setting // only for non qos_init
	mmio_write_32(QOSCTRL_RAEN,  0x00000000U);

	/*wbkwait(0004), wbkmdhi(4,2),wbkmdlo(1,8) */
	mmio_write_32(DBSC_DBCAM0CNF1, 0x00043218);
	/*0(fillunit),8(dirtymax),4(dirtymin)*/
	mmio_write_32(DBSC_DBCAM0CNF2, 0x000000F4);
	/*stop_tolerance*/
	mmio_write_32(DBSC_DBSCHRW0, 0x22421111);
	/*rd-wr/wr-rd toggle priority*/
	mmio_write_32(DBSC_SCFCTST2, 0x012F1123);
	mmio_write_32(DBSC_DBSCHSZ0, 0x00000001);
	mmio_write_32(DBSC_DBSCHCNT0, 0x000F0037);

	/* QoS Settings */
	mmio_write_32(DBSC_DBSCHQOS00,  0x00000F00U);
	mmio_write_32(DBSC_DBSCHQOS01,  0x00000B00U);
	mmio_write_32(DBSC_DBSCHQOS02,  0x00000000U);
	mmio_write_32(DBSC_DBSCHQOS03,  0x00000000U);
	mmio_write_32(DBSC_DBSCHQOS40,  0x00000300U);
	mmio_write_32(DBSC_DBSCHQOS41,  0x000002F0U);
	mmio_write_32(DBSC_DBSCHQOS42,  0x00000200U);
	mmio_write_32(DBSC_DBSCHQOS43,  0x00000100U);
	mmio_write_32(DBSC_DBSCHQOS90,  0x00000100U);
	mmio_write_32(DBSC_DBSCHQOS91,  0x000000F0U);
	mmio_write_32(DBSC_DBSCHQOS92,  0x000000A0U);
	mmio_write_32(DBSC_DBSCHQOS93,  0x00000040U);
	mmio_write_32(DBSC_DBSCHQOS120, 0x00000040U);
	mmio_write_32(DBSC_DBSCHQOS121, 0x00000030U);
	mmio_write_32(DBSC_DBSCHQOS122, 0x00000020U);
	mmio_write_32(DBSC_DBSCHQOS123, 0x00000010U);
	mmio_write_32(DBSC_DBSCHQOS130, 0x00000100U);
	mmio_write_32(DBSC_DBSCHQOS131, 0x000000F0U);
	mmio_write_32(DBSC_DBSCHQOS132, 0x000000A0U);
	mmio_write_32(DBSC_DBSCHQOS133, 0x00000040U);
	mmio_write_32(DBSC_DBSCHQOS140, 0x000000C0U);
	mmio_write_32(DBSC_DBSCHQOS141, 0x000000B0U);
	mmio_write_32(DBSC_DBSCHQOS142, 0x00000080U);
	mmio_write_32(DBSC_DBSCHQOS143, 0x00000040U);
	mmio_write_32(DBSC_DBSCHQOS150, 0x00000040U);
	mmio_write_32(DBSC_DBSCHQOS151, 0x00000030U);
	mmio_write_32(DBSC_DBSCHQOS152, 0x00000020U);
	mmio_write_32(DBSC_DBSCHQOS153, 0x00000010U);

	mmio_write_32(QOSCTRL_RAEN,  0x00000001U);
#endif//ddr_qos_init_setting
	/* H3ver1.1 need to set monitor function */
	if ((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11==Prr_Cut)) {
		mmio_write_32(DBSC_DBMONCONF4, 0x00700000);
	}

	if (PRR_PRODUCT_H3==Prr_Product){
		if(PRR_PRODUCT_10==Prr_Cut){
			/* resrdis, simple mode, sc off	*/
			mmio_write_32(DBSC_DBBCAMDIS, 0x00000007);
		} else if(PRR_PRODUCT_11==Prr_Cut){
			/* resrdis, simple mode		*/
			mmio_write_32(DBSC_DBBCAMDIS, 0x00000005);
		} else {/* H3ver2.0			*/
			/* resrdis			*/
			mmio_write_32(DBSC_DBBCAMDIS, 0x00000001);
		}
	} else {	/* M3/M3N/V3H			*/
			/* resrdis			*/
			mmio_write_32(DBSC_DBBCAMDIS, 0x00000001);
	}
}

static void dbsc_regset_post(void)
{
	uint32_t ch,cs;
	uint32_t dataL;
	uint32_t slice,rdlat_max;

	rdlat_max = 0 ;
	foreach_vch(ch){
		for(slice=0;slice<SLICE_CNT;slice++){
			dataL = ddr_getval_s(ch, slice,	_reg_PHY_RDDQS_LATENCY_ADJUST);
			if(dataL>rdlat_max) rdlat_max = dataL ;
		}
	}
	mmio_write_32(DBSC_DBTR(24),
		((rdlat_max +2)<<16) + mmio_read_32(DBSC_DBTR(24)));

	/* set ddr density information */
	foreach_vch(ch){
		for(cs=0;cs<CS_CNT;cs++){
			if(ddr_density[ch][cs]==0xff) {
				mmio_write_32(DBSC_DBMEMCONF(ch,cs),0);
			} else {
				mmio_write_32(DBSC_DBMEMCONF(ch,cs),DBMEMCONF_REGD(ddr_density[ch][cs]));
			}
		}
		mmio_write_32(DBSC_DBMEMCONF(ch,2), 0x00000000);
		mmio_write_32(DBSC_DBMEMCONF(ch,3), 0x00000000);
	}

	mmio_write_32(DBSC_DBBUS0CNF1,0x00000010);

	/*set DBI */
	if(Boardcnf->dbi_en)
		mmio_write_32(DBSC_DBDBICNT, 0x00000003);

	/* H3ver2 DBI wa */
	if ((((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11<Prr_Cut))
	    |(PRR_PRODUCT_M3N==Prr_Product)|(PRR_PRODUCT_V3H==Prr_Product)) && (Boardcnf->dbi_en))
		reg_ddrphy_write_a(0x00001010,	0x01000000);

	/*set REFCYCLE */
	dataL = (get_refperiod())*ddr_mbps/2000/ddr_mbpsdiv ;
	mmio_write_32(DBSC_DBRFCNF1, 0x00080000 | (dataL&0xffff));
	mmio_write_32(DBSC_DBRFCNF2, 0x00010000|DBSC_REFINTS);

	/* periodec phy ctrl update enable*/
	mmio_write_32(DBSC_DBCALCNF, 0x01000010);
	if ((((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11<Prr_Cut))
	    |(PRR_PRODUCT_M3N==Prr_Product)|(PRR_PRODUCT_V3H==Prr_Product)) && (Boardcnf->dbi_en))
		mmio_write_32(DBSC_DBDFICUPDCNF,0x28240001);

#ifdef DDR_BACKUPMODE
	if(ddrBackup==DRAM_BOOT_STATUS_WARM){
		send_dbcmd(0x08840001);
		wait_dbcmd();
		send_dbcmd(0x0A840001);
		wait_dbcmd();

		send_dbcmd(0x04840010);
		wait_dbcmd();
	}
#endif//DDR_BACKUPMODE

	mmio_write_32(DBSC_DBRFEN, 0x00000001);
	/* dram access enable */
	mmio_write_32(DBSC_DBACEN, 0x00000001);

	MSG_LF("dbsc_regset_post(done)");

}

/*******************************************************************************
 *	DFI_INIT_START
 ******************************************************************************/
static uint32_t dfi_init_start(void)
{
	uint32_t ch;
	uint32_t phytrainingok;
	uint32_t retry;
	uint32_t dataL;
	const uint32_t RETRY_MAX = 0x10000;

	/***********************************************************************
	set IE=1 when init_start_disable==0
	***********************************************************************/
	if(ddrtbl_getval(_cnf_DDR_PHY_ADR_G_REGSET, _reg_PHY_CAL_MODE_0) & 1) {
		ddr_setval_ach_as(_reg_PHY_IE_MODE, 0x0);
	} else {
		ddr_setval_ach_as(_reg_PHY_IE_MODE, 0x1);
	}

	if ((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut)) {

	/***********************************************************************
		PLL3 Disable
	***********************************************************************/
		/* protect register interface */
		ddrphy_regif_idle();

		pll3_control(0);
	/***********************************************************************
		init start
	***********************************************************************/
		/* dbdficnt0:
		 * dfi_dram_clk_disable=1
		 * dfi_frequency = 0
		 * freq_ratio = 01 (2:1)
		 * init_start =0
		 */
		foreach_vch(ch)
			mmio_write_32(DBSC_DBDFICNT(ch), 0x00000F10);
		dsb_sev();

		/* dbdficnt0:
		 * dfi_dram_clk_disable=1
		 * dfi_frequency = 0
		 * freq_ratio = 01 (2:1)
		 * init_start =1
		 */
		foreach_vch(ch)
			mmio_write_32(DBSC_DBDFICNT(ch), 0x00000F11);
		dsb_sev();

	} else {
		ddr_setval_ach_as(_reg_PHY_DLL_RST_EN, 0x2);
		dsb_sev();
		ddrphy_regif_idle();
	}

	/* dll_rst negate */
	foreach_vch(ch)
		mmio_write_32(DBSC_DBPDCNT3(ch), 0x0000CF01);
	dsb_sev();

	/***********************************************************************
	wait init_complete
	***********************************************************************/
	phytrainingok=0;
	retry=0;
	while(retry++<RETRY_MAX) {
		foreach_vch(ch){
			dataL = mmio_read_32(DBSC_INITCOMP(ch));
			if(dataL & 0x00000001)
				phytrainingok |= (1<<ch);
		}
		dsb_sev();
		if(phytrainingok == ddr_phyvalid)
			break;
		if(retry%256==0)
			ddr_setval_ach_as(_reg_SC_PHY_RX_CAL_START, 0x1);
	}

	/***********************************************************************
	all ch ok?
	***********************************************************************/
	if((phytrainingok & ddr_phyvalid) != ddr_phyvalid){
		return (0xff);
	}
	/* dbdficnt0:
	 * dfi_dram_clk_disable=0
	 * dfi_frequency = 0
	 * freq_ratio = 01 (2:1)
	 * init_start =0
	 */
	foreach_vch(ch)
		mmio_write_32(DBSC_DBDFICNT(ch),0x00000010);
	dsb_sev();

	return 0;
}

/*******************************************************************************
 *	drivablity setting : CMOS MODE ON/OFF
 ******************************************************************************/
static void change_lpddr4_en(uint32_t mode)
{
	uint32_t ch;
	uint32_t i;
	uint32_t dataL;
	const uint32_t _reg_PHY_PAD_DRIVE_X[3] = {
		_reg_PHY_PAD_ADDR_DRIVE,
		_reg_PHY_PAD_CLK_DRIVE,
		_reg_PHY_PAD_CS_DRIVE
	};
	for(i=0; i<3; i++) {
		foreach_vch(ch){
			dataL = ddr_getval(ch, _reg_PHY_PAD_DRIVE_X[i]);
			if(mode) {
				dataL |= (1<<14);
			} else {
				dataL &= ~(1<<14);
			}
			ddr_setval(ch, _reg_PHY_PAD_DRIVE_X[i], dataL);
		}
	}
}

/*******************************************************************************
 *	drivablity setting
 ******************************************************************************/
static uint32_t set_term_code(void)
{
	int32_t i;
	uint32_t ch,index;
	uint32_t dataL;
	uint32_t chip_id[2];
	uint32_t term_code;
	uint32_t override;
	term_code = ddrtbl_getval(_cnf_DDR_PHY_ADR_G_REGSET,
		_reg_PHY_PAD_DATA_TERM);
	override = 0;
	for(i=0;i<2;i++)
		chip_id[i] = mmio_read_32(LIFEC_CHIPID(i));

	index=0;
	while(1) {
		if(TermcodeBySample[index][0] == 0xffffffff) {
			break;
		}
		if( ( TermcodeBySample[index][0] == chip_id[0])
		&& ( TermcodeBySample[index][1] == chip_id[1])) {
			term_code = TermcodeBySample[index][2];
			override = 1;
			break;
		}
		index++;
	}

	if(override) {
		for(index=0;index<_reg_PHY_PAD_TERM_X_NUM;index++) {
			dataL = ddrtbl_getval(_cnf_DDR_PHY_ADR_G_REGSET, _reg_PHY_PAD_TERM_X[index]);
			dataL = (dataL & ~0x0001ffff) | term_code;
			ddr_setval_ach(_reg_PHY_PAD_TERM_X[index], dataL);
		}
	} else if((PRR_PRODUCT_M3==Prr_Product) && (PRR_PRODUCT_10==Prr_Cut)) {
		// non
	} else {
		ddr_setval_ach_as(_reg_PHY_IE_MODE, 1);
		ddr_setval_ach(_reg_PHY_PAD_TERM_X[0],
			(ddrtbl_getval(_cnf_DDR_PHY_ADR_G_REGSET, _reg_PHY_PAD_TERM_X[0]) & 0xFFFE0000));
		ddr_setval_ach(_reg_PHY_CAL_CLEAR_0, 1);
		ddr_setval_ach(_reg_PHY_CAL_START_0, 1);
		foreach_vch(ch){
			do {
				dataL = ddr_getval(ch, _reg_PHY_CAL_RESULT2_OBS_0);
			} while(!(dataL&0x00800000));
		}
		if(PRR_PRODUCT_11>=Prr_Cut) {
			foreach_vch(ch){
				uint32_t pvtr;
				uint32_t pvtp;
				uint32_t pvtn;
				dataL = ddr_getval(ch, _reg_PHY_PAD_TERM_X[0]);
				pvtr = (dataL >> 12) & 0x1f;
				pvtr += 8;
				if(pvtr>0x1f)
					pvtr = 0x1f;
				dataL = ddr_getval(ch, _reg_PHY_CAL_RESULT2_OBS_0);
				pvtn = (dataL >> 6) & 0x03f;
				pvtp = (dataL >> 0) & 0x03f;

				for(index=0;index<_reg_PHY_PAD_TERM_X_NUM;index++) {
					dataL = ddrtbl_getval(_cnf_DDR_PHY_ADR_G_REGSET, _reg_PHY_PAD_TERM_X[index]);
					dataL = (dataL & ~0x0001ffff)
						| (pvtr<<12)
						| (pvtn<<6)
						| (pvtp);
					ddr_setval(ch, _reg_PHY_PAD_TERM_X[index],dataL);
				}
			}
		}
		 else { // M3ver1.1/H3ver2.0
			foreach_vch(ch){
				for(index=0;index<_reg_PHY_PAD_TERM_X_NUM;index++) {
					dataL = ddr_getval(ch, _reg_PHY_PAD_TERM_X[index]);
					ddr_setval(ch, _reg_PHY_PAD_TERM_X[index], (dataL & 0xFFFE0FFF) | 0x00015000);
				}
			}
		}
	}
	if((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut)) {
	// non
	} else {
		ddr_padcal_tcompensate_getinit(override);
	}
	return 0;
}

/*******************************************************************************
 *	DDR mode register setting
 ******************************************************************************/
static void ddr_register_set(uint32_t ch)
{
	int32_t fspwp;
	uint32_t chind;
	uint32_t tmp;

	chind = ch<<20;
	for(fspwp=1;fspwp>=0;fspwp--){
		/*MR13,fspwp*/
		send_dbcmd(0x0e040d08|chind|(fspwp<<6));	// high current mode

		tmp = ddrtbl_getval(_cnf_DDR_PI_REGSET, _reg_PI_MR1_DATA_Fx_CSx[fspwp][0]);
		send_dbcmd(0x0e040100|chind|tmp);

		tmp = ddrtbl_getval(_cnf_DDR_PI_REGSET, _reg_PI_MR2_DATA_Fx_CSx[fspwp][0]);
		send_dbcmd(0x0e040200|chind|tmp);

		tmp = ddrtbl_getval(_cnf_DDR_PI_REGSET, _reg_PI_MR3_DATA_Fx_CSx[fspwp][0]);
		send_dbcmd(0x0e040300|chind|tmp);

		tmp = ddrtbl_getval(_cnf_DDR_PI_REGSET, _reg_PI_MR11_DATA_Fx_CSx[fspwp][0]);
		send_dbcmd(0x0e040b00|chind|tmp);

		tmp = ddrtbl_getval(_cnf_DDR_PI_REGSET, _reg_PI_MR12_DATA_Fx_CSx[fspwp][0]);
		send_dbcmd(0x0e040c00|chind|tmp);

		tmp = ddrtbl_getval(_cnf_DDR_PI_REGSET, _reg_PI_MR14_DATA_Fx_CSx[fspwp][0]);
		send_dbcmd(0x0e040e00|chind|tmp);
		/* MR22 */
		send_dbcmd(0x0e041600|chind|0x16);
	}
}

/*******************************************************************************
 *	Training handshake functions
 ******************************************************************************/
static inline uint32_t wait_freqchgreq(uint32_t assert)
{
	uint32_t dataL;
	uint32_t count;
	uint32_t ch;
	count = 100000;

	/* H3 ver1.0/1.1 cannot see frqchg_req */
	if((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut)) {
		return 0;
	}

	if(assert) {
		do {
			dataL = 1;
			foreach_vch(ch){
				dataL &= mmio_read_32(DBSC_DBPDSTAT(ch));
			}
			count = count -1;
		} while( ((dataL&0x1)!=0x1) & (count !=0));
	} else {
		do {
			dataL = 0;
			foreach_vch(ch){
				dataL |= mmio_read_32(DBSC_DBPDSTAT(ch));
			}
			count = count -1;
		} while( ((dataL&0x1)!=0x0) & (count !=0));
	}

	return (count==0);
}

static inline void set_freqchgack(uint32_t assert)
{
	uint32_t ch;
	uint32_t dataL;
	if(assert)
		dataL = 0x0CF20000;
	else
		dataL = 0x00000000;

	foreach_vch(ch)
		mmio_write_32(DBSC_DBPDCNT2(ch), dataL);
}

static inline void set_dfifrequency(uint32_t freq)
{
	uint32_t ch;
	if((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut)) {
		foreach_vch(ch)
			mmio_clrsetbits_32(DBSC_DBPDCNT1(ch), 0x1f, freq);
	} else {
		foreach_vch(ch){
			mmio_clrsetbits_32(DBSC_DBDFICNT(ch), 0x1f<<24, (freq << 24));
		}
	}
	dsb_sev();
}

static uint32_t pll3_freq(uint32_t on)
{
	uint32_t timeout;

	timeout = wait_freqchgreq(1);

	if(timeout) {
		return (1);
	}

	pll3_control(on);
	set_dfifrequency(on);

	set_freqchgack(1);
	timeout = wait_freqchgreq(0);
	set_freqchgack(0);

	if(timeout) {
		FATAL_MSG("Time out[2]");
		return (1);
	}
	return (0);
}

/*******************************************************************************
 *	update dly
 ******************************************************************************/
static void update_dly(void)
{
	ddr_setval_ach(_reg_SC_PHY_MANUAL_UPDATE, 0x1);
	ddr_setval_ach(_reg_PHY_ADRCTL_MANUAL_UPDATE, 0x1);
}

/*******************************************************************************
 *	training by pi
 ******************************************************************************/
static uint32_t pi_training_go(void)
{
	uint32_t flag;
	uint32_t dataL;
	uint32_t retry;
	const uint32_t RETRY_MAX = 4096*16;
	uint32_t ch;

	uint32_t mst_ch;
	uint32_t cur_frq;
	uint32_t complete;
	uint32_t frqchg_req;

	/* ********************************************************************* */

	/***********************************************************************
	pi_start
	***********************************************************************/
	ddr_setval_ach(_reg_PI_START, 0x1);
	foreach_vch(ch)
		ddr_getval( ch, _reg_PI_INT_STATUS);

	/* set dfi_phymstr_ack = 1 */
	mmio_write_32(DBSC_DBDFIPMSTRCNF, 0x00000001);
	dsb_sev();

	/***********************************************************************
	wait pi_int_status[0]
	***********************************************************************/
	mst_ch=0;
	flag=0;
	complete=0;
	cur_frq=0;
	retry=RETRY_MAX;
	do {
		frqchg_req = mmio_read_32(DBSC_DBPDSTAT(mst_ch)) & 0x1;

		/* H3 ver1.x cannot see frqchg_req */
		if((PRR_PRODUCT_H3==Prr_Product)
		&& (PRR_PRODUCT_11>=Prr_Cut)) {
			if((retry%4096)==1) {
				frqchg_req = 1;
			} else {
				frqchg_req = 0;
			}
		}

		if(frqchg_req){
			if(cur_frq){
				flag=pll3_freq(0);
				cur_frq=0;
			} else {
				flag=pll3_freq(1);
				cur_frq=1;
			}
			if(flag)break;
		} else {
			if(cur_frq){
				foreach_vch(ch){
					if(complete & (1<<ch))continue;
					dataL= ddr_getval( ch, _reg_PI_INT_STATUS);
					if(dataL & 0x1){
						complete |= (1<<ch);
					}
				}
				if(complete==ddr_phyvalid)break;
			}
		}
	} while(--retry);
	foreach_vch(ch){
		/* dummy read */
		dataL = ddr_getval_s(ch, 0,_reg_PHY_CAL_RESULT2_OBS_0);
		dataL = ddr_getval(ch, _reg_PI_INT_STATUS);
		ddr_setval(ch, _reg_PI_INT_ACK, dataL);
	}
	if(ddrphy_regif_chk()){
		return(0xfd);
	}
	return complete;
}

/*******************************************************************************
 *	Initialize ddr
 ******************************************************************************/
static uint32_t init_ddr(void)
{
	int32_t	i;
	uint32_t dataL;
	uint32_t phytrainingok;
	uint32_t ch;
	uint32_t err;

	MSG_LF("init_ddr:0\n");

#ifdef DDR_BACKUPMODE
	dram_get_boot_status(&ddrBackup);
#endif

	/***********************************************************************
	unlock phy
	***********************************************************************/
	/* Unlock DDRPHY register(AGAIN) */
	foreach_vch(ch)
		mmio_write_32(DBSC_DBPDLK(ch), 0x0000A55A);
	dsb_sev();

	if ((((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11<Prr_Cut))
	    |(PRR_PRODUCT_M3N==Prr_Product)|(PRR_PRODUCT_V3H==Prr_Product)) && (Boardcnf->dbi_en))
		reg_ddrphy_write_a(0x00001010, 0x01000001);
	else
		reg_ddrphy_write_a(0x00001010, 0x00000001);
	/* dbsc register set */
	dbsc_regset_pre();

	/***********************************************************************
	load ddrphy registers
	***********************************************************************/
	ddrtbl_load();

	/***********************************************************************
	config ddrphy registers
	***********************************************************************/
	ddr_config();

	/***********************************************************************
	dfi_reset assert
	***********************************************************************/
	foreach_vch(ch)
		mmio_write_32(DBSC_DBPDCNT0(ch), 0x01);
	dsb_sev();

	/***********************************************************************
	dbsc register set
	***********************************************************************/
	dbsc_regset();
	MSG_LF("init_ddr:1\n");

	/***********************************************************************
	dfi_reset negate
	***********************************************************************/
	foreach_vch(ch)
		mmio_write_32(DBSC_DBPDCNT0(ch), 0x00);
	dsb_sev();

	/***********************************************************************
	dfi_init_start (start ddrphy)
	***********************************************************************/
	err=dfi_init_start();
	if(err){
		return INITDRAM_ERR_I;
	}
	MSG_LF("init_ddr:2\n");

	/***********************************************************************
	ddr backupmode end
	***********************************************************************/
#ifdef DDR_BACKUPMODE
	if(ddrBackup) {
		NOTICE("[WARM_BOOT]\n");
	} else {
		NOTICE("[COLD_BOOT]\n");
	}
	err=dram_update_boot_status(ddrBackup);
	if(err){
		NOTICE("[BOOT_STATUS_UPDATE_ERROR]");
		return INITDRAM_ERR_I;
	}
#endif
	MSG_LF("init_ddr:3\n");

	/***********************************************************************
	override term code after dfi_init_complete
	***********************************************************************/
	err=set_term_code();
	if(err){
		return INITDRAM_ERR_I;
	}
	MSG_LF("init_ddr:4\n");

	/***********************************************************************
	rx offset calibration
	***********************************************************************/
	if ((PRR_PRODUCT_11< Prr_Cut)|(PRR_PRODUCT_M3N==Prr_Product)|(PRR_PRODUCT_V3H==Prr_Product)) {
		err = rx_offset_cal_hw();
	} else {
		err = rx_offset_cal();
	}
	if(err)return(INITDRAM_ERR_O);
	MSG_LF("init_ddr:5\n");

	/***********************************************************************
	set ie_mode=1
	***********************************************************************/
	ddr_setval_ach_as(_reg_PHY_IE_MODE, 0x1);

	/***********************************************************************
	check register i/f is alive
	***********************************************************************/
	err = ddrphy_regif_chk();
	if(err) {
		return(INITDRAM_ERR_O);
	}
	MSG_LF("init_ddr:6\n");

	/***********************************************************************
	phy initialize end
	***********************************************************************/

	/***********************************************************************
	setup DDR mode registers
	***********************************************************************/
	/* CMOS MODE */
	change_lpddr4_en(0);

	ch=0x08;

	/* PDE */
	send_dbcmd(0x08040000|(0x00100000 * ch));

	/* PDX */
	send_dbcmd(0x08040001|(0x00100000 * ch));

	/* MR22 (ODTCS & RQZ */
	send_dbcmd(0x0e041600|(0x00100000 * ch)|0x16);

	/* ZQCAL start */
	send_dbcmd(0x0d04004F|(0x00100000 * ch));

	/* ZQLAT */
	send_dbcmd(0x0d040051|(0x00100000 * ch));

	/***********************************************************************
	setup DDR mode registers
	***********************************************************************/
	foreach_vch(ch) {
		ddr_register_set(ch);
	}
	/* LPDDR4 MODE */
	change_lpddr4_en(1);

	MSG_LF("init_ddr:7\n");

	/***********************************************************************
	mask CS_MAP if RANKx is not found
	***********************************************************************/
	foreach_vch(ch){
		dataL = ddr_getval(ch, _reg_PI_CS_MAP);
		if(!(ch_have_this_cs[0] & (1<<ch))) dataL = dataL & 0xa;
		if(!(ch_have_this_cs[1] & (1<<ch))) dataL = dataL & 0x5;
		ddr_setval(ch, _reg_PI_CS_MAP, dataL);
	}

	/***********************************************************************
	exec pi_training
	***********************************************************************/
	ddr_setval_ach_as(_reg_PHY_PER_CS_TRAINING_MULTICAST_EN,0);
	ddr_setval_ach_as(_reg_PHY_PER_CS_TRAINING_EN, 1);

	phytrainingok = pi_training_go();

	if(ddr_phyvalid != (phytrainingok & ddr_phyvalid)) {
		return(INITDRAM_ERR_T|phytrainingok);
	}

	MSG_LF("init_ddr:8\n");

	/***********************************************************************
	CACS DLY ADJUST
	***********************************************************************/
	dataL = Boardcnf->cacs_dly + _f_scale_adj(Boardcnf->cacs_dly_adj);
	foreach_vch(ch){
		int16_t adj;
		for(i=0;i<_reg_PHY_CLK_CACS_SLAVE_DELAY_X_NUM;i++){
			adj = _f_scale_adj(Boardcnf->ch[ch].cacs_adj[i]);
			ddr_setval(ch, _reg_PHY_CLK_CACS_SLAVE_DELAY_X[i],
				dataL + adj
			);
		}
	}
	update_dly();
	MSG_LF("init_ddr:9\n");

	/***********************************************************************
	H3 fix rd latency to avoid bug in elasitic buffe
	***********************************************************************/
	if((PRR_PRODUCT_H3==Prr_Product)
	&& (PRR_PRODUCT_11>=Prr_Cut)) {
		adjust_rddqs_latency();
	}

	/***********************************************************************
	Adjust Write path latency
	***********************************************************************/
	if(ddrtbl_getval(_cnf_DDR_PHY_SLICE_REGSET, _reg_PHY_WRITE_PATH_LAT_ADD))
		adjust_wpath_latency();

	/***********************************************************************
	RDQLVL Training
	***********************************************************************/
	err=rdqdm_man();
	if(err) {
		return(INITDRAM_ERR_T);
	}
	update_dly();
	MSG_LF("init_ddr:10\n");

	/***********************************************************************
	WDQLVL Training
	***********************************************************************/
	err=wdqdm_man();
	if(err) {
		return(INITDRAM_ERR_T);
	}
	update_dly();
	MSG_LF("init_ddr:11\n");

	/***********************************************************************
	training complete, setup dbsc
	***********************************************************************/
	if (((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11<Prr_Cut))
	    |(PRR_PRODUCT_M3N==Prr_Product)|(PRR_PRODUCT_V3H==Prr_Product)) {
		ddr_setval_ach_as(_reg_PHY_DFI40_POLARITY,0x0);
		ddr_setval_ach(_reg_PI_DFI40_POLARITY,0x0);
	}

	dbsc_regset_post();
	MSG_LF("init_ddr:12\n");
	return phytrainingok;
}

/*******************************************************************************
 *	SW LEVELING COMMON
 ******************************************************************************/

static uint32_t swlvl1(uint32_t ddr_csn, uint32_t reg_cs, uint32_t reg_kick)
{
	uint32_t ch;
	uint32_t dataL;
	uint32_t retry;
	uint32_t waiting;
	uint32_t err;

	const uint32_t RETRY_MAX=0x1000;

	err=0;
	/* set EXIT -> OP_DONE is cleared */
	ddr_setval_ach(_reg_PI_SWLVL_EXIT, 0x1);

	/* kick */
	foreach_vch(ch){
		if(ch_have_this_cs[ddr_csn%2] & (1<<ch)){
			ddr_setval(ch, reg_cs, ddr_csn);
			ddr_setval(ch, reg_kick, 0x1);
		}
	}
	foreach_vch(ch){
		/*PREPARE ADDR REGISTER (for SWLVL_OP_DONE)*/
		ddr_getval( ch, _reg_PI_SWLVL_OP_DONE);
	}
	waiting=ch_have_this_cs[ddr_csn%2];
	dsb_sev();
	retry=RETRY_MAX;
	do {
		foreach_vch(ch){
			if(!(waiting&(1<<ch)))
				continue;
			dataL= ddr_getval( ch, _reg_PI_SWLVL_OP_DONE);
			if(dataL&0x1)	waiting&=~(1<<ch);
		}
		retry--;
	} while (waiting && (retry>0));
	if(retry==0){
		err=1;
	}

	dsb_sev();
	/* set EXIT -> OP_DONE is cleared */
	ddr_setval_ach(_reg_PI_SWLVL_EXIT, 0x1);
	dsb_sev();

	return err;
}

/*******************************************************************************
 *	WDQ TRAINING
 ******************************************************************************/
static void wdqdm_clr1(uint32_t ch, uint32_t ddr_csn)
{
	int32_t i,k;
	uint32_t cs,slice;
	uint32_t dataL;
	cs = ddr_csn%2;
	dataL = Boardcnf->dqdm_dly_w;
	for(slice=0;slice<SLICE_CNT;slice++){
		k=(Boardcnf->ch[ch].dqs_swap>>(4*slice))&0xf;
		if(((k>=2) && (ddr_csn<2)) || ((k<2) && (ddr_csn>=2)))continue;

		for(i=0;i<=8;i++){
			if(ch_have_this_cs[CS_CNT-1-cs]&(1<<ch))
				wdqdm_dly[ch][cs][slice][i]=wdqdm_dly[ch][CS_CNT-1-cs][slice][i];
			else
				wdqdm_dly[ch][cs][slice][i]=dataL;
			wdqdm_le[ch][cs][slice][i]=0;
			wdqdm_te[ch][cs][slice][i]=0;
		}
		wdqdm_st[ch][cs][slice]=0;
		wdqdm_win[ch][cs][slice]=0;
	}
}

static uint32_t wdqdm_ana1(uint32_t ch, uint32_t ddr_csn)
{
	int32_t i,k;
	uint32_t cs,slice;
	uint32_t dataL;
	uint32_t err;
	const uint32_t _par_WDQLVL_RETRY_THRES=0x7c0;

	int32_t min_win;
	int32_t win;
	int8_t _adj;
	int16_t adj;
	uint32_t dq;

	err = 0;
	for(slice=0;slice<SLICE_CNT;slice+=1){
		k=(Boardcnf->ch[ch].dqs_swap>>(4*slice)) & 0xf;
		if(((k>=2) && (ddr_csn<2)) || ((k<2) && (ddr_csn>=2)))continue;

		cs = ddr_csn%2;
		ddr_setval_s(ch, slice, _reg_PHY_PER_CS_TRAINING_INDEX, cs);
		ddr_getval_s(ch, slice, _reg_PHY_PER_CS_TRAINING_INDEX);
		for(i=0;i<9;i++){
			dq = slice*8+i;
			if(i==8)
				_adj = Boardcnf->ch[ch].dm_adj_w[slice];
			else
				_adj = Boardcnf->ch[ch].dq_adj_w[dq];
			adj = _f_scale_adj(_adj);

			dataL = ddr_getval_s(ch, slice, _reg_PHY_CLK_WRX_SLAVE_DELAY[i])+adj;
			ddr_setval_s(ch, slice, _reg_PHY_CLK_WRX_SLAVE_DELAY[i], dataL);
			wdqdm_dly[ch][cs][slice][i]  =dataL;
		}
		ddr_setval_s(ch, slice, _reg_PHY_PER_CS_TRAINING_EN, 0);
		dataL = ddr_getval_s(ch, slice, _reg_PHY_WDQLVL_STATUS_OBS);
		wdqdm_st[ch][cs][slice] = dataL;
		min_win=INT_LEAST32_MAX;
		for(i=0;i<=8;i++){
			ddr_setval_s(ch, slice, _reg_PHY_WDQLVL_DQDM_OBS_SELECT, i);

			dataL = ddr_getval_s(ch, slice, _reg_PHY_WDQLVL_DQDM_TE_DLY_OBS);
			wdqdm_te[ch][cs][slice][i] = dataL;
			dataL = ddr_getval_s(ch, slice, _reg_PHY_WDQLVL_DQDM_LE_DLY_OBS);
			wdqdm_le[ch][cs][slice][i] = dataL;
			win=(int32_t)wdqdm_te[ch][cs][slice][i]-wdqdm_le[ch][cs][slice][i];
			if(min_win>win)
				min_win=win;
			if( dataL >= _par_WDQLVL_RETRY_THRES) err = 2;
		}
		wdqdm_win[ch][cs][slice] = min_win;
		ddr_setval_s(ch, slice, _reg_PHY_PER_CS_TRAINING_EN, 1);
	}
	return err;
}

static void wdqdm_cp(uint32_t ddr_csn, uint32_t restore) {
	uint32_t i;
	uint32_t ch,slice;
	uint32_t tgt_cs,src_cs;
	uint32_t tmp_r;

	foreach_vch(ch){
		for(tgt_cs=0;tgt_cs<CS_CNT;tgt_cs++) {
			for(slice=0;slice<SLICE_CNT;slice++){
				ddr_setval_s(ch,slice,_reg_PHY_PER_CS_TRAINING_INDEX, tgt_cs);
				src_cs = ddr_csn%2;
				if(!(ch_have_this_cs[1] & (1<<ch)))
					src_cs = 0;
				for(i=0;i<=4;i+=4){
					if(restore)
						tmp_r = rdqdm_dly[ch][tgt_cs][slice][i];
					else
						tmp_r = rdqdm_dly[ch][src_cs][slice][i];

					ddr_setval_s(ch, slice, _reg_PHY_RDDQS_X_RISE_SLAVE_DELAY[i], tmp_r);
				}
			}
		}
	}
}

static uint32_t wdqdm_man1(void)
{
	int32_t k;
	uint32_t ch,cs,slice;
	uint32_t ddr_csn;
	uint32_t dataL;
	uint32_t err;
	uint32_t err_flg;

	uint32_t high_dq[DRAM_CH_CNT];
	uint32_t mr14_csab0_bak[DRAM_CH_CNT];

	foreach_vch(ch){
		high_dq[ch]=0;
		for(slice=0;slice<SLICE_CNT;slice++) {
			k=(Boardcnf->ch[ch].dqs_swap>>(4*slice)) & 0xf;
			if(k>=2)
				high_dq[ch]|=(1<<slice);
		}
	}

	if ((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut))
		ddr_setval_ach(_reg_PI_16BIT_DRAM_CONNECT, 0x0);

	err=0;
	/* CLEAR PREV RESULT */
	for(cs=0;cs<CS_CNT;cs++) {
		ddr_setval_ach_as(_reg_PHY_PER_CS_TRAINING_INDEX, cs);
		if (((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11<Prr_Cut))
		    |(PRR_PRODUCT_M3N==Prr_Product)|(PRR_PRODUCT_V3H==Prr_Product)) {
			ddr_setval_ach_as(_reg_SC_PHY_WDQLVL_CLR_PREV_RESULTS, 0x1);
		} else {
			ddr_setval_ach_as(_reg_PHY_WDQLVL_CLR_PREV_RESULTS, 0x1);
		}
	}
	ddrphy_regif_idle();

	err_flg=0;

	for(ddr_csn=0;ddr_csn<CSAB_CNT;ddr_csn++) {
		if ((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut)) {
			foreach_vch(ch){
				dataL = mmio_read_32(DBSC_DBDFICNT(ch));
				dataL &= ~(0x00ffU<<16);

				if(ddr_csn>=2)
					k = (high_dq[ch]^0x0f);
				else
					k =  high_dq[ch];
				dataL |= (k<<16);
				mmio_write_32(DBSC_DBDFICNT(ch), dataL);
				ddr_setval(ch, _reg_PI_WDQLVL_RESP_MASK, k);
			}
		}
		if(
			((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut))
		||	((PRR_PRODUCT_M3==Prr_Product) && (PRR_PRODUCT_10>=Prr_Cut))
		) {
			wdqdm_cp(ddr_csn,0);
		}

		foreach_vch(ch){
			dataL = ddr_getval(ch,_reg_PI_MR14_DATA_Fx_CSx[1][ddr_csn]);
			ddr_setval(ch,_reg_PI_MR14_DATA_Fx_CSx[1][0],dataL);
		}

		/* KICK WDQLVL */
		err = swlvl1(ddr_csn, _reg_PI_WDQLVL_CS, _reg_PI_WDQLVL_REQ);
		if(err)
			goto err_exit;

		if(ddr_csn==0)
			foreach_vch(ch){
				mr14_csab0_bak[ch] = ddr_getval(ch,_reg_PI_MR14_DATA_Fx_CSx[1][0]);
			}
		else
			foreach_vch(ch){
				ddr_setval(ch,_reg_PI_MR14_DATA_Fx_CSx[1][0],mr14_csab0_bak[ch]);
			}
		foreach_vch(ch){
			if(!(ch_have_this_cs[ddr_csn%2] & (1<<ch))) {
				wdqdm_clr1(ch, ddr_csn);
				continue;
			}
			err = wdqdm_ana1(ch, ddr_csn);
			if(err)
				err_flg |= (1<<(ddr_csn*4+ch));
			ddrphy_regif_idle();
		}
	}
err_exit:
	ddr_setval_ach(_reg_PI_16BIT_DRAM_CONNECT, 0x1);
	foreach_vch(ch){
		dataL = mmio_read_32(DBSC_DBDFICNT(ch));
		dataL &= ~(0x00ffU<<16);
		mmio_write_32(DBSC_DBDFICNT(ch), dataL);
		ddr_setval(ch, _reg_PI_WDQLVL_RESP_MASK, 0);
	}
	if (err) {
		NOTICE("Fail: %s:%d:%s\n",__func__, __LINE__, __DATE__);
	}
	return (err_flg|err);
}

static uint32_t wdqdm_man(void)
{
	uint32_t err,retry_cnt;
	const uint32_t retry_max=0x10;

	ddr_setval_ach(_reg_PI_TDFI_WDQLVL_RW, (DBSC_DBTR(11)&0xFF) +12);
	if (((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11<Prr_Cut))
	    |(PRR_PRODUCT_M3N==Prr_Product)|(PRR_PRODUCT_V3H==Prr_Product)) {
		ddr_setval_ach(_reg_PI_TDFI_WDQLVL_WR_F1, (DBSC_DBTR(12)&0xFF) +1);
	} else {
		ddr_setval_ach(_reg_PI_TDFI_WDQLVL_WR, (DBSC_DBTR(12)&0xFF) +1);
	}
	ddr_setval_ach(_reg_PI_TRFC_F1, (DBSC_DBTR(13)&0x1FF));

	retry_cnt=0;
	do {
		if(((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11<Prr_Cut))
		    |(PRR_PRODUCT_M3N==Prr_Product)|(PRR_PRODUCT_V3H==Prr_Product)|(PRR_PRODUCT_M3==Prr_Product)) {
			uint32_t	ch,ddr_csn,mr14_bkup[4][4];

			ddr_setval_ach(_reg_PI_WDQLVL_VREF_EN, 0x1);
			ddr_setval_ach(_reg_PI_WDQLVL_VREF_NORMAL_STEPSIZE, 0x1);
			if((PRR_PRODUCT_M3N==Prr_Product)|(PRR_PRODUCT_V3H==Prr_Product)){
				ddr_setval_ach(_reg_PI_WDQLVL_VREF_DELTA_F1, 0x0C);
			} else {
				ddr_setval_ach(_reg_PI_WDQLVL_VREF_DELTA, 0x0C);
			}
			dsb_sev();
			err = wdqdm_man1();
			foreach_vch(ch){
				for(ddr_csn=0;ddr_csn<CSAB_CNT;ddr_csn++){
					mr14_bkup[ch][ddr_csn] = ddr_getval(ch,_reg_PI_MR14_DATA_Fx_CSx[1][ddr_csn]);
					dsb_sev();
				}
			}

			if((PRR_PRODUCT_M3N==Prr_Product)|(PRR_PRODUCT_V3H==Prr_Product)){
				ddr_setval_ach(_reg_PI_WDQLVL_VREF_DELTA_F1, 0x04);
			} else {
				ddr_setval_ach(_reg_PI_WDQLVL_VREF_DELTA, 0x04);
			}
			pvtcode_update();
			err = wdqdm_man1();
			foreach_vch(ch){
				for(ddr_csn=0;ddr_csn<CSAB_CNT;ddr_csn++){
					mr14_bkup[ch][ddr_csn] = (mr14_bkup[ch][ddr_csn] + ddr_getval(ch,_reg_PI_MR14_DATA_Fx_CSx[1][ddr_csn]) )/2;
					ddr_setval(ch,_reg_PI_MR14_DATA_Fx_CSx[1][ddr_csn],mr14_bkup[ch][ddr_csn]);
				}
			}

			ddr_setval_ach(_reg_PI_WDQLVL_VREF_NORMAL_STEPSIZE, 0x0);
			if((PRR_PRODUCT_M3N==Prr_Product)|(PRR_PRODUCT_V3H==Prr_Product)){
				ddr_setval_ach(_reg_PI_WDQLVL_VREF_DELTA_F1, 0x0);
				ddr_setval_ach(_reg_PI_WDQLVL_VREF_INITIAL_START_POINT_F1,0x0);
				ddr_setval_ach(_reg_PI_WDQLVL_VREF_INITIAL_STOP_POINT_F1, 0x0);
			} else {
				ddr_setval_ach(_reg_PI_WDQLVL_VREF_DELTA, 0x0);
				ddr_setval_ach(_reg_PI_WDQLVL_VREF_INITIAL_START_POINT,0x0);
				ddr_setval_ach(_reg_PI_WDQLVL_VREF_INITIAL_STOP_POINT, 0x0);
			}
			ddr_setval_ach(_reg_PI_WDQLVL_VREF_INITIAL_STEPSIZE,   0x0);

			pvtcode_update2();
			err = wdqdm_man1();
			ddr_setval_ach(_reg_PI_WDQLVL_VREF_EN,0x0);

		} else {
			err = wdqdm_man1();
		}
	} while(err && (++retry_cnt<retry_max));

	if(
		((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut))
	||	((PRR_PRODUCT_M3==Prr_Product) && (PRR_PRODUCT_10>=Prr_Cut))
	) {
		wdqdm_cp(0,1);
	}

	return (retry_cnt>=retry_max);
}

/*******************************************************************************
 *	RDQ TRAINING
 ******************************************************************************/
static void rdqdm_clr1(uint32_t ch, uint32_t ddr_csn)
{
	int32_t i,k;
	uint32_t cs,slice;
	uint32_t dataL;
	cs = ddr_csn%2;
	dataL = Boardcnf->dqdm_dly_r;
	for(slice=0;slice<SLICE_CNT;slice++){
		k=(Boardcnf->ch[ch].dqs_swap>>(4*slice))&0xf;
		if(((k>=2) && (ddr_csn<2)) || ((k<2) && (ddr_csn>=2)))continue;

		for(i=0;i<=8;i++){
			if(ch_have_this_cs[CS_CNT-1-cs]&(1<<ch)) {
				rdqdm_dly[ch][cs][slice][i]=
				rdqdm_dly[ch][CS_CNT-1-cs][slice][i];
				rdqdm_dly[ch][cs][slice+SLICE_CNT][i]=
				rdqdm_dly[ch][CS_CNT-1-cs][slice+SLICE_CNT][i];
			}
			else {
				rdqdm_dly[ch][cs][slice][i]=dataL;
				rdqdm_dly[ch][cs][slice+SLICE_CNT][i]=dataL;
			}
			rdqdm_le[ch][cs][slice][i]=0;
			rdqdm_le[ch][cs][slice+SLICE_CNT][i]=0;
			rdqdm_te[ch][cs][slice][i]=0;
			rdqdm_te[ch][cs][slice+SLICE_CNT][i]=0;
			rdqdm_nw[ch][cs][slice][i]=0;
			rdqdm_nw[ch][cs][slice+SLICE_CNT][i]=0;
		}
		rdqdm_st[ch][cs][slice]=0;
		rdqdm_win[ch][cs][slice]=0;
	}
}

static uint32_t rdqdm_ana1(uint32_t ch, uint32_t ddr_csn)
{
	int32_t i,k;
	uint32_t cs,slice;
	uint32_t dataL;
	uint32_t err;
	int8_t _adj;
	int16_t adj;
	uint32_t dq;
	err=0;
	for(slice=0;slice<SLICE_CNT;slice++){
		int32_t min_win;
		int32_t win;
		uint32_t rdq_status_obs_select;
		k=(Boardcnf->ch[ch].dqs_swap>>(4*slice))&0xf;
		if(((k>=2) && (ddr_csn<2)) || ((k<2) && (ddr_csn>=2)))continue;

		cs = ddr_csn%2;
		ddr_setval_s(ch, slice, _reg_PHY_PER_CS_TRAINING_INDEX, cs);
		ddrphy_regif_idle();

		ddr_getval_s(ch, slice, _reg_PHY_PER_CS_TRAINING_INDEX);
		ddrphy_regif_idle();

		for(i=0;i<=8;i++){
			dq = slice*8+i;
			if(i==8)
				_adj = Boardcnf->ch[ch].dm_adj_r[slice];
			else
				_adj = Boardcnf->ch[ch].dq_adj_r[dq];

			adj = _f_scale_adj(_adj);

			dataL = ddr_getval_s(ch, slice, _reg_PHY_RDDQS_X_RISE_SLAVE_DELAY[i])+adj;
			ddr_setval_s(ch, slice, _reg_PHY_RDDQS_X_RISE_SLAVE_DELAY[i], dataL);
			rdqdm_dly[ch][cs][slice][i]=dataL;

			dataL = ddr_getval_s(ch, slice, _reg_PHY_RDDQS_X_FALL_SLAVE_DELAY[i])+adj;
			ddr_setval_s(ch, slice, _reg_PHY_RDDQS_X_FALL_SLAVE_DELAY[i], dataL);
			rdqdm_dly[ch][cs][slice+SLICE_CNT][i]=dataL;
		}
		min_win=INT_LEAST32_MAX;
		for(i=0;i<=8;i++){
			dataL = ddr_getval_s(ch, slice, _reg_PHY_RDLVL_STATUS_OBS);
			rdqdm_st[ch][cs][slice]=dataL;
			rdqdm_st[ch][cs][slice+SLICE_CNT]=dataL;
			/* k : rise/fall */
			for(k=0;k<2;k++){
				if(i==8){
					rdq_status_obs_select = 16+8*k;
				} else {
					rdq_status_obs_select = i+k*8;
				}
				ddr_setval_s(ch, slice, _reg_PHY_RDLVL_RDDQS_DQ_OBS_SELECT, rdq_status_obs_select);

				dataL = ddr_getval_s(ch, slice, _reg_PHY_RDLVL_RDDQS_DQ_LE_DLY_OBS);
				rdqdm_le[ch][cs][slice+SLICE_CNT*k][i]=dataL;

				dataL = ddr_getval_s(ch, slice, _reg_PHY_RDLVL_RDDQS_DQ_TE_DLY_OBS);
				rdqdm_te[ch][cs][slice+SLICE_CNT*k][i]=dataL;

				dataL = ddr_getval_s(ch, slice, _reg_PHY_RDLVL_RDDQS_DQ_NUM_WINDOWS_OBS);
				rdqdm_nw[ch][cs][slice+SLICE_CNT*k][i]=dataL;

				win=(int32_t)rdqdm_te[ch][cs][slice+SLICE_CNT*k][i]-rdqdm_le[ch][cs][slice+SLICE_CNT*k][i];
				if(i!=8){
					if(min_win>win)
						min_win=win;
				}
			}
		}
		rdqdm_win[ch][cs][slice]=min_win;
		if(min_win<=0) {
			err = 2;
		}
	}
	return(err);
}

static uint32_t rdqdm_man1(void)
{
	uint32_t ch;
	uint32_t ddr_csn;
	uint32_t err;

	err = 0;

	for(ddr_csn=0;ddr_csn<CSAB_CNT;ddr_csn++) {
		/* KICK RDQLVL */
		err = swlvl1(ddr_csn, _reg_PI_RDLVL_CS, _reg_PI_RDLVL_REQ);
		if(err)goto err_exit;

		foreach_vch(ch){
			if(!(ch_have_this_cs[ddr_csn%2] & (1<<ch))) {
				rdqdm_clr1(ch, ddr_csn);
				ddrphy_regif_idle();
				continue;
			}
			err = rdqdm_ana1(ch, ddr_csn);
			ddrphy_regif_idle();
			if(err)
				goto err_exit;
		}
	}
err_exit:
	return(err);
}

static uint32_t rdqdm_man(void)
{
	uint32_t err,retry_cnt;
	const uint32_t retry_max=0x01;

	ddr_setval_ach_as(_reg_PHY_DQ_TSEL_ENABLE,
		0x00000004 | ddrtbl_getval(_cnf_DDR_PHY_SLICE_REGSET,_reg_PHY_DQ_TSEL_ENABLE));
	ddr_setval_ach_as(_reg_PHY_DQS_TSEL_ENABLE,
		0x00000004 | ddrtbl_getval(_cnf_DDR_PHY_SLICE_REGSET,_reg_PHY_DQS_TSEL_ENABLE));
	ddr_setval_ach_as(_reg_PHY_DQ_TSEL_SELECT,
		0xFF0FFFFF & ddrtbl_getval(_cnf_DDR_PHY_SLICE_REGSET,_reg_PHY_DQ_TSEL_SELECT));
	ddr_setval_ach_as(_reg_PHY_DQS_TSEL_SELECT,
		0xFF0FFFFF & ddrtbl_getval(_cnf_DDR_PHY_SLICE_REGSET,_reg_PHY_DQS_TSEL_SELECT));

	retry_cnt=0;
	do {
		err = rdqdm_man1();
		ddrphy_regif_idle();
	} while(err && (++retry_cnt<retry_max));

	ddr_setval_ach_as(_reg_PHY_DQ_TSEL_ENABLE,
		ddrtbl_getval(_cnf_DDR_PHY_SLICE_REGSET,_reg_PHY_DQ_TSEL_ENABLE));
	ddr_setval_ach_as(_reg_PHY_DQS_TSEL_ENABLE,
		ddrtbl_getval(_cnf_DDR_PHY_SLICE_REGSET,_reg_PHY_DQS_TSEL_ENABLE));
	ddr_setval_ach_as(_reg_PHY_DQ_TSEL_SELECT,
		ddrtbl_getval(_cnf_DDR_PHY_SLICE_REGSET,_reg_PHY_DQ_TSEL_SELECT));
	ddr_setval_ach_as(_reg_PHY_DQS_TSEL_SELECT,
		ddrtbl_getval(_cnf_DDR_PHY_SLICE_REGSET,_reg_PHY_DQS_TSEL_SELECT));

	return (retry_cnt>=retry_max);
}

/*******************************************************************************
 *	rx offset calibration
 ******************************************************************************/
static int32_t _find_change(uint64_t val, uint32_t dir)
{
	int32_t i;
	uint32_t startval;
	uint32_t curval;
	const uint32_t VAL_END=0x3f;

	if(dir==0) {
		startval=(val&0x1);
		for(i=1;i<=VAL_END;i++) {
			curval=(val>>i) & 0x1;
			if(curval!=startval)
				return(i);
		}
		return(VAL_END);
	} else {
		startval=(val>>dir)&0x1;
		for(i=dir-1;i>=0;i--) {
			curval=(val>>i) & 0x1;
			if(curval!=startval)
				return(i);
		}
		return(0);
	}
}

static uint32_t _rx_offset_cal_updn(uint32_t code)
{
	const uint32_t CODE_MAX=0x40;
	uint32_t tmp;

	if ((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut)) {
		if(code==0)
			tmp = (1<<6) | (CODE_MAX-1);
		else if(code<=0x20)
			tmp = ((CODE_MAX-1-(0x20-code)*2)<<6) | (CODE_MAX-1);
		else
			tmp = ((CODE_MAX-1)<<6) | (CODE_MAX-1-(code-0x20)*2);
	} else {
		if(code==0)
			tmp = (1<<6) | (CODE_MAX-1);
		else
			tmp = (code<<6) | (CODE_MAX-code);
	}
	return tmp;
}

#define RX_OFFSET_FAST
static uint32_t rx_offset_cal(void)
{
	uint32_t index;
	uint32_t code;
	const uint32_t CODE_MAX=0x40;
	const uint32_t CODE_STEP=2;
	uint32_t ch, slice;
	uint32_t tmp;
	uint32_t tmp_ach_as[DRAM_CH_CNT][SLICE_CNT];
	uint64_t val[DRAM_CH_CNT][SLICE_CNT][_reg_PHY_RX_CAL_X_NUM];
#ifdef RX_OFFSET_FAST
	uint32_t adr_st;
	adr_st=ddr_regdef_adr(_reg_PHY_RX_CAL_X[0]);
#endif
	ddr_setval_ach_as(_reg_PHY_IE_MODE, 0x1);
	ddr_setval_ach_as(_reg_PHY_RX_CAL_OVERRIDE, 0x1);
	foreach_vch(ch){
		for(slice=0; slice<SLICE_CNT; slice++) {
			for(index=0;index<_reg_PHY_RX_CAL_X_NUM;index++) {
				val[ch][slice][index] = 0;
			}
		}
	}

	for(code=0;code<CODE_MAX/CODE_STEP;code++) {
		tmp = _rx_offset_cal_updn(code*CODE_STEP);
#ifdef RX_OFFSET_FAST
		tmp=tmp|(tmp<<16);
		for(index=0;index<(_reg_PHY_RX_CAL_X_NUM+1)/2;index++) {
			for(slice=0;slice<4;slice++)
				reg_ddrphy_write_a(adr_st+0x80*slice+index,tmp);
		}
#else
		for(index=0;index<_reg_PHY_RX_CAL_X_NUM;index++) {
			ddr_setval_ach_as(_reg_PHY_RX_CAL_X[index], tmp);
		}
#endif
		dsb_sev();
		ddr_getval_ach_as(_reg_PHY_RX_CAL_OBS, (uint32_t *)tmp_ach_as);

		foreach_vch(ch){
			for(slice=0; slice<SLICE_CNT; slice++) {
				tmp=tmp_ach_as[ch][slice];
				for(index=0;index<_reg_PHY_RX_CAL_X_NUM;index++) {
					if(tmp & (1U<<index)) {
						val[ch][slice][index] |= (1ULL<<code);
					} else {
						val[ch][slice][index] &= ~(1ULL<<code);
					}
				}
			}
		}
	}
	foreach_vch(ch){
		for(slice=0; slice<SLICE_CNT; slice++) {
			for(index=0;index<_reg_PHY_RX_CAL_X_NUM;index++) {
				uint64_t tmpval;
				int32_t lsb,msb;
				tmpval = val[ch][slice][index];
				lsb = _find_change(tmpval, 0);
				msb = _find_change(tmpval, (CODE_MAX/CODE_STEP)-1);
				tmp = (lsb+msb)>>1;

				tmp = _rx_offset_cal_updn(tmp*CODE_STEP);
				ddr_setval_s(ch, slice, _reg_PHY_RX_CAL_X[index], tmp);
			}
		}
	}
	ddr_setval_ach_as(_reg_PHY_RX_CAL_OVERRIDE, 0x0);
	ddr_setval_ach_as(_reg_PHY_IE_MODE, 0x0);
	return 0;
}

static uint32_t rx_offset_cal_hw(void) {
	uint32_t ch, slice;
	uint32_t retry;
	uint32_t complete;
	uint32_t tmp;
	uint32_t tmp_ach_as[DRAM_CH_CNT][SLICE_CNT];
	ddr_setval_ach_as(_reg_PHY_IE_MODE, 0x1);
	ddr_setval_ach_as(_reg_PHY_RX_CAL_X[9], 0x0);
	ddr_setval_ach_as(_reg_PHY_RX_CAL_OVERRIDE, 0x0);
	ddr_setval_ach_as(_reg_PHY_RX_CAL_SAMPLE_WAIT, 0xf);

	retry = 0;
	while(retry<4096) {
		if((retry & 0xff) == 0) {
			ddr_setval_ach_as(_reg_SC_PHY_RX_CAL_START, 0x1);
		}
		ddr_getval_ach_as(_reg_PHY_RX_CAL_X[9], (uint32_t *)tmp_ach_as);
		complete = 1;
		foreach_vch(ch){
			for(slice=0;slice<SLICE_CNT;slice++) {
				tmp = tmp_ach_as[ch][slice];
				tmp = (tmp&0x3f) + ((tmp>>6)&0x3f);
				if (((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11<Prr_Cut))
				    |(PRR_PRODUCT_M3N==Prr_Product)|(PRR_PRODUCT_V3H==Prr_Product)) {
					if(tmp!=0x3E)complete=0;
				} else {
					if(tmp!=0x40)complete=0;
				}
			}
		}
		if(complete)break;

		retry++;
	}
	ddr_setval_ach_as(_reg_PHY_IE_MODE, 0x0);
	return (complete==0);
}

/*******************************************************************************
 *	adjust rddqs latency
 ******************************************************************************/
static void adjust_rddqs_latency(void)
{
	uint32_t ch,slice;
	uint32_t dly;
	uint32_t maxlatx2;
	uint32_t tmp;
	uint32_t rdlat_adjx2[SLICE_CNT];
	foreach_vch(ch){
		maxlatx2 = 0;
		for(slice=0;slice<SLICE_CNT;slice++) {
			ddr_setval_s(ch, slice, _reg_PHY_PER_CS_TRAINING_INDEX, 0);

			dly = ddr_getval_s(ch, slice, _reg_PHY_RDDQS_GATE_SLAVE_DELAY);
			tmp = ddr_getval_s(ch, slice, _reg_PHY_RDDQS_LATENCY_ADJUST);
			/* note gate_slave_delay[9] is always 0 */
			tmp = (tmp<<1) + (dly>>8);
			rdlat_adjx2[slice] = tmp;
			if(maxlatx2<tmp)
				maxlatx2=tmp;
		}
		maxlatx2 = ((maxlatx2+1)>>1)<<1;
		for(slice=0;slice<SLICE_CNT;slice++) {
			tmp = maxlatx2 - rdlat_adjx2[slice];
			tmp = (tmp>>1);
			if(tmp){
				ddr_setval_s(ch, slice, _reg_PHY_RPTR_UPDATE,
				ddr_getval_s(ch, slice, _reg_PHY_RPTR_UPDATE)+1);
			}
		}
	}
}

/*******************************************************************************
 *	adjust wpath latency
 ******************************************************************************/
static void adjust_wpath_latency(void)
{
	uint32_t ch,cs,slice;
	uint32_t dly;
	uint32_t wpath_add;
	const uint32_t _par_EARLY_THRESHOLD_VAL=0x180;
	foreach_vch(ch){
		for(slice=0;slice<SLICE_CNT;slice+=1) {
			for(cs=0;cs<CS_CNT;cs++){
				ddr_setval_s(ch, slice, _reg_PHY_PER_CS_TRAINING_INDEX, cs);
				ddr_getval_s(ch, slice, _reg_PHY_PER_CS_TRAINING_INDEX);
				dly = ddr_getval_s(ch, slice, _reg_PHY_CLK_WRDQS_SLAVE_DELAY);
				if(dly<=_par_EARLY_THRESHOLD_VAL)continue;

				wpath_add = ddr_getval_s(ch, slice, _reg_PHY_WRITE_PATH_LAT_ADD);
				ddr_setval_s(ch, slice,  _reg_PHY_WRITE_PATH_LAT_ADD, wpath_add-1);
			}
		}
	}
}

/*******************************************************************************
 *	DDR Initialize entry
 ******************************************************************************/
int32_t InitDram(void)
{
	uint32_t ch,cs;
	uint32_t dataL;
	uint32_t failcount;

	dataL = *((volatile uint32_t*)CPG_MSTPSR5);
	if(dataL & BIT22){	// case THS/TSC Standby
		dataL &= ~(BIT22);
		*((volatile uint32_t*)CPG_CPGWPR)   = ~dataL;
		*((volatile uint32_t*)CPG_SMSTPCR5) =  dataL;
		while( (BIT22) & *((volatile uint32_t*)CPG_MSTPSR5) );  // wait bit=0
	}
#if 0
	/***********************************************************************
	THS1 ONx
	***********************************************************************/
	*((volatile uint32_t*)THS1_CTSR) = 0x00000001;
#endif

	/***********************************************************************
	Judge product and cut
	***********************************************************************/
	Prr_Product = mmio_read_32(PRR) & PRR_PRODUCT_MASK;
	Prr_Cut = mmio_read_32(PRR) & PRR_CUT_MASK;

	if (Prr_Product == PRR_PRODUCT_H3) {
	  if(PRR_PRODUCT_11>=Prr_Cut){
		pDDR_REGDEF_TBL = (uint32_t *)&DDR_REGDEF_TBL[0][0];
	  } else {
		mmio_write_32(DBSC_DBSYSCNT0, 0x00001234);
		pDDR_REGDEF_TBL = (uint32_t *)&DDR_REGDEF_TBL[2][0]; // Ver2.0
	  }
	} else if (Prr_Product == PRR_PRODUCT_M3) {
		pDDR_REGDEF_TBL = (uint32_t *)&DDR_REGDEF_TBL[1][0];
	} else if ((Prr_Product == PRR_PRODUCT_M3N)|(Prr_Product == PRR_PRODUCT_V3H)) {
		mmio_write_32(DBSC_DBSYSCNT0, 0x00001234);
		pDDR_REGDEF_TBL = (uint32_t *)&DDR_REGDEF_TBL[3][0];
	} else {
		FATAL_MSG("DDR:Unknown Product\n");
		return 0xff;
	}

	/***********************************************************************
	Judge board type
	***********************************************************************/
	_cnf_BOARDTYPE = boardcnf_get_brd_type();
	if(_cnf_BOARDTYPE>=BOARDNUM){
		FATAL_MSG("DDR:Unknown Board\n");
		return 0xff;
	} else {
//		NOTICE("[BoardType=%d]",_cnf_BOARDTYPE );
	}

	Boardcnf = (struct _boardcnf *)&boardcnfs[_cnf_BOARDTYPE];
	ddr_phyvalid = Boardcnf->phyvalid;
	max_density=0;
	max_cs=0;
	for(cs=0;cs<CS_CNT;cs++){
		ch_have_this_cs[cs]=0;
	}
	foreach_vch(ch){
		for(cs=0;cs<CS_CNT;cs++){
			dataL = Boardcnf->ch[ch].ddr_density[cs];
			ddr_density[ch][cs]=dataL;

			if(dataL==0xff)continue;
			if(dataL>max_density)
				max_density=dataL;
			if((cs==1) && (PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut))
				continue;
			if(cs==1)
				max_cs=1;
			ch_have_this_cs[cs] |= (1<<ch);
		}
	}

	/***********************************************************************
	Judge board clock frequency (in MHz)
	***********************************************************************/
	boardcnf_get_brd_clk(_cnf_BOARDTYPE, &brd_clk, &brd_clkdiv);

	/***********************************************************************
	Judge ddr operating frequency clock(in Mbps)
	***********************************************************************/
	boardcnf_get_ddr_mbps(_cnf_BOARDTYPE, &ddr_mbps, &ddr_mbpsdiv);

	ddr0800_mul = CLK_DIV(800,2,brd_clk,brd_clkdiv);

	ddr_mul = CLK_DIV(ddr_mbps,ddr_mbpsdiv*2,brd_clk,brd_clkdiv);

	/***********************************************************************
	Adjust tccd
	***********************************************************************/
	uint32_t mul,div;
	uint32_t bus_mbps, bus_mbpsdiv;
	uint32_t tmp_tccd;

	dataL = mmio_read_32(CPG_PLL1CR);
	mul = (dataL>>24)+1;
	if(dataL & 0x80)
		div = 2;
	else
		div = 1;

	bus_mbps = brd_clk * mul * 2;
	bus_mbpsdiv = brd_clkdiv * div;

	tmp_tccd = CLK_DIV(ddr_mbps*8, ddr_mbpsdiv, bus_mbps, bus_mbpsdiv);
	if(8*ddr_mbps*bus_mbpsdiv != tmp_tccd * bus_mbps * ddr_mbpsdiv)
		tmp_tccd = tmp_tccd + 1;

	if(tmp_tccd<8)
		ddr_tccd = 8;
	else
		ddr_tccd = tmp_tccd;

	NOTICE("CR7: DDR%d(%s)", ddr_mbps/ddr_mbpsdiv, RCAR_DDR_VERSION);

	MSG_LF("Start\n");

	/***********************************************************************
	initialize DDRPHY
	***********************************************************************/

	pll3_set(1,1);
	pll3_control(1);
	dataL=init_ddr();
	if(dataL==ddr_phyvalid) {
		 failcount =0;
	}
	else {
		 failcount =1;
	}

	/***********************************************************************
	initialize DDRPHY
	***********************************************************************/
//	NOTICE("..%d\n",failcount);

	foreach_vch(ch)
		mmio_write_32(DBSC_DBPDLK(ch), 0x00000000);
	if(((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11< Prr_Cut))
	   |(PRR_PRODUCT_M3N==Prr_Product)|(PRR_PRODUCT_V3H==Prr_Product)) {
		mmio_write_32(DBSC_DBSYSCNT0, 0x00000000);
	}

	if(failcount==0) {
		return INITDRAM_OK;
	} else {
		return INITDRAM_NG;
	}
}

void pvtcode_update(void){
	uint32_t	ch;
	uint32_t	pvtp[4],pvtn[4],pvtp_init,pvtn_init;
	int32_t		pvtp_tmp,pvtn_tmp;

	foreach_vch(ch){
		pvtn_init = (tcal.tcomp_cal[ch] & 0xFC0)>>6;
		pvtp_init = (tcal.tcomp_cal[ch] & 0x03F)>>0;

		if(8912*pvtp_init > 44230){
			pvtp_tmp=   (5000 + 8912*pvtp_init - 44230) /10000;
		} else {
			pvtp_tmp= -((-(5000 + 8912*pvtp_init - 44230)) /10000);
		}
		pvtn_tmp=  (5000 +  5776 * pvtn_init + 30280 )	/10000;

		pvtn[ch]= pvtn_tmp + pvtn_init;
		pvtp[ch]= pvtp_tmp + pvtp_init;

		if(pvtn[ch] >63) {
			pvtn[ch]=63;
			pvtp[ch]=(pvtp_tmp)*(63-6*pvtn_tmp -pvtn_init)/(pvtn_tmp) +6*pvtp_tmp + pvtp_init;
		}

		if((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut)) {
			reg_ddrphy_write(ch, ddr_regdef_adr(_reg_PHY_PAD_FDBK_TERM),pvtp[ch]| (pvtn[ch]<<6)|(tcal.tcomp_cal[ch]&0xfffff000));
			reg_ddrphy_write(ch, ddr_regdef_adr(_reg_PHY_PAD_DATA_TERM),pvtp[ch]| (pvtn[ch]<<6)|(tcal.tcomp_cal[ch]&0xfffff000));
			reg_ddrphy_write(ch, ddr_regdef_adr(_reg_PHY_PAD_DQS_TERM ),pvtp[ch]| (pvtn[ch]<<6)|(tcal.tcomp_cal[ch]&0xfffff000));
			reg_ddrphy_write(ch, ddr_regdef_adr(_reg_PHY_PAD_ADDR_TERM),pvtp[ch]| (pvtn[ch]<<6)|(tcal.tcomp_cal[ch]&0xfffff000));
			reg_ddrphy_write(ch, ddr_regdef_adr(_reg_PHY_PAD_CS_TERM  ),pvtp[ch]| (pvtn[ch]<<6)|(tcal.tcomp_cal[ch]&0xfffff000));
		} else {
			reg_ddrphy_write(ch, ddr_regdef_adr(_reg_PHY_PAD_FDBK_TERM),pvtp[ch]| (pvtn[ch]<<6)|0x00035000);
			reg_ddrphy_write(ch, ddr_regdef_adr(_reg_PHY_PAD_DATA_TERM),pvtp[ch]| (pvtn[ch]<<6)|0x00015000);
			reg_ddrphy_write(ch, ddr_regdef_adr(_reg_PHY_PAD_DQS_TERM ),pvtp[ch]| (pvtn[ch]<<6)|0x00015000);
			reg_ddrphy_write(ch, ddr_regdef_adr(_reg_PHY_PAD_ADDR_TERM),pvtp[ch]| (pvtn[ch]<<6)|0x00015000);
			reg_ddrphy_write(ch, ddr_regdef_adr(_reg_PHY_PAD_CS_TERM  ),pvtp[ch]| (pvtn[ch]<<6)|0x00015000);
		}
	}
}

void pvtcode_update2(void){
	uint32_t	ch;
	foreach_vch(ch){
		reg_ddrphy_write(ch, ddr_regdef_adr(_reg_PHY_PAD_FDBK_TERM),tcal.init_cal[ch]|0x00020000);
		reg_ddrphy_write(ch, ddr_regdef_adr(_reg_PHY_PAD_DATA_TERM),tcal.init_cal[ch]);
		reg_ddrphy_write(ch, ddr_regdef_adr(_reg_PHY_PAD_DQS_TERM ),tcal.init_cal[ch]);
		reg_ddrphy_write(ch, ddr_regdef_adr(_reg_PHY_PAD_ADDR_TERM),tcal.init_cal[ch]);
		reg_ddrphy_write(ch, ddr_regdef_adr(_reg_PHY_PAD_CS_TERM  ),tcal.init_cal[ch]);
	}
}

void ddr_padcal_tcompensate_getinit(uint32_t override)
{
	uint32_t ch;
	uint32_t dataL;
	uint32_t pvtp,pvtn;

	tcal.init_temp = 0;
	for(ch=0;ch<4;ch++){
		tcal.init_cal[ch] =0;
		tcal.tcomp_cal[ch] =0;
	}

	foreach_vch(ch) {
		tcal.init_cal[ch]  = ddr_getval(ch, _reg_PHY_PAD_TERM_X[1]);
		tcal.tcomp_cal[ch] = ddr_getval(ch, _reg_PHY_PAD_TERM_X[1]);
	}

	if(!override){
		dataL = *((volatile uint32_t*)THS1_TEMP);
		if(dataL < 2800){	tcal.init_temp = (143 * (int32_t)dataL - 359000)/1000;}
			else	{	tcal.init_temp = (121 * (int32_t)dataL - 296300)/1000;}

		foreach_vch(ch){
			pvtp = (tcal.init_cal[ch] >> 0) & 0x000003F;
			pvtn = (tcal.init_cal[ch] >> 6) & 0x000003F;
			if((int32_t)pvtp > ((tcal.init_temp * 29 -3625)/1000))
				pvtp = (int32_t)pvtp +((3625 - tcal.init_temp * 29 )/1000);
			else
				pvtp = 0;

			if((int32_t)pvtn > ((tcal.init_temp * 54 -6750)/1000))
				pvtn = (int32_t)pvtn +((6750 - tcal.init_temp * 54)/1000);
			else
				pvtn = 0;

			if((PRR_PRODUCT_H3==Prr_Product) && (PRR_PRODUCT_11>=Prr_Cut)) {
				tcal.init_cal[ch] = (tcal.init_cal[ch] & 0xfffff000) | (pvtn << 6) |(pvtp);
			} else {
				tcal.init_cal[ch] = 0x00015000 | (pvtn << 6) |(pvtp);
			}
		}
		tcal.init_temp =125;
	}
}

uint8_t get_boardcnf_phyvalid(void)
{
	return Boardcnf->phyvalid ;
}

/*******************************************************************************
 *	END
 ******************************************************************************/
