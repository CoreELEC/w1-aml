#ifndef __T9023_CALB_H__
#define __T9023_CALB_H__



#define DPD_LOOP_IRR_EN 0
#define RXIRR_BW 1  //0:20M, 1:40M, 2:80M
//#define RUN_PT   1  //0:dbg calib; 1:performance


struct rx_dc_dbg
{
  unsigned int code_i;
  unsigned int code_q;
  unsigned int rd_reg;
};

struct rx_dc_result
{
   int num;
   int dci;
   int dcq;
   unsigned int code_i;
   unsigned int code_q;
};

struct rx_lna_output1
{
  unsigned int code;
  unsigned long long pwr;
};


struct rx_lna_output2
{
  unsigned char chn_idx;
  unsigned int  code_out;
};



struct _rx_rc
{
	unsigned int rxrc_out;
};

struct _rx_dc
{
	int rxdc_i;
	int rxdc_q;
	unsigned int rxdc_codei;
	unsigned int rxdc_codeq;
};

struct _tx_dc_irr
{
	unsigned int tx_dc_i;
	unsigned int tx_dc_q;
	unsigned int tx_alpha;
	unsigned int tx_theta;
};

struct _tx_dpd
{
	unsigned int tx_dpd_coeff_i[15];
	unsigned int tx_dpd_coeff_q[15];
	int          snr;
};

struct _rx_lna
{
	unsigned int rx_lna_code;
};

struct _rx_irr
{
	unsigned int coeff_eup[4];
	unsigned int coeff_pup[4];
	unsigned int coeff_elow[4];
	unsigned int coeff_plow[4];
};

struct calb_rst_out
{
	struct _rx_rc		 rx_rc;
	struct _rx_dc		 rx_dc[3][2][12];//bw,gain idx for addr 
	struct _tx_dc_irr	 tx_dc_irr[5][9];// idx by chn, gain
	struct _tx_dpd		 tx_dpd[5][3][3];// idx by chn,bw,gain
	struct _rx_lna       rx_lna[10]; // idx by chn
	struct _rx_irr       rx_irr[5][3]; //idx by bw,chn
};


struct calb_iqxel_out
{
	double Lo_leakage[5][9];
	double amp_imb[5][9];
	double phase_imb[5][9];
	double power[5][3][3];
	double evm[5][3][3];
	double snr[5][3];
};


void init_t9023(void);

void do_rxdc_calb(struct calb_rst_out* calb_val, 
				  unsigned int bw_idx, 
				  unsigned int tia_idx, 
				  unsigned int lpf_idx);

void set_rxdc_calb(struct calb_rst_out* calb_val, unsigned int bw_idx, unsigned int tia_idx, unsigned int lpf_idx);

void do_tx_dc_irr_calb(struct calb_rst_out* calb_val, unsigned int chn_idx, unsigned int gain_idx);

void do_tx_dpd_irr_calb(struct calb_rst_out* calb_val, 
						bool tx_dciq_manual, 
						unsigned int chn_idx,
						unsigned int bw_idx,
						unsigned int gain_idx);

void do_tx_dpd_calb(struct calb_rst_out* calb_val, 
					bool tx_dciq_manual,
					unsigned int chn_idx,
					unsigned int bw_idx,
					unsigned int gain_idx);

void do_rx_lna_calb(struct calb_rst_out* calb_val, 
					bool tx_dciq_manual,
					unsigned int chn,
					unsigned int chn_idx);


void do_rx_irr_calb(struct calb_rst_out* calb_val,
					bool tx_dciq_manual,
					unsigned chn_idx,
					unsigned int bw_idx);


unsigned int do_rx_rc_calb(struct calb_rst_out* calb_val);


unsigned int do_rx_rc_calb(struct calb_rst_out* calb_val);


void TxCalbCfg(int chn,int bw);


void RxCalbCfg(int chn,int bw);


void TrswchCfg(int chn,int bw);


void do_txdpd_lut_calb(void);



void TestRxDC(struct rx_dc_result* rx_dc_rst,
			  unsigned int * rst_cnt, 
			  struct rx_dc_dbg * rx_dc_dbg_info, 
			  unsigned int * dbg_inf_cnt);

void TestRxDC_417(struct calb_rst_out* calb_val,
				  struct rx_dc_result* rx_dc_rst,
				  unsigned int * rst_cnt, 
				  struct rx_dc_dbg * rx_dc_dbg_info, 
				  unsigned int * dbg_inf_cnt,
				  unsigned int bw_idx,
				  unsigned int tia_idx,
				  unsigned int lpf_idx );


void TestRxDCAll_417(struct calb_rst_out* calb_val,
				  struct rx_dc_result* rx_dc_rst,
				  unsigned int * rst_cnt, 
				  struct rx_dc_dbg * rx_dc_dbg_info, 
				  unsigned int * dbg_inf_cnt,
				  unsigned int tia_idx,
				  unsigned int lpf_idx );


void TestRxDCAll(void);
void TestRxLNA(struct rx_lna_output1* rx_lna_out1, 
			   unsigned int *out_cnt1,
			   struct rx_lna_output2* rx_lna_out2, 
			   unsigned int *out_cnt2);

void TestRxLNA_417(
				   struct calb_rst_out* calb_val,
				   bool tx_dciq_manual,
				   unsigned int chn,
				   unsigned int chn_idx,
				   struct rx_lna_output1* rx_lna_out1, 
				   unsigned int *out_cnt1,
				   struct rx_lna_output2* rx_lna_out2, 
				   unsigned int *out_cnt2);

int t9023_dpd_tx_func(void *param);

void t9023_channel_switch(int p_chn);
void t9023_bw_switch(int bw_idx);

void do_one_gain(unsigned int calb_item);

void t9023_top(unsigned int calb_item);

void sel_rf_para(char chn, char bw);
void set_txm_ct(unsigned int ch);

void reset_rxdc_rf_reg(void );

void do_rxdc_for_txirr(void);
void do_rxdc_for_txdpd(void);

#endif
