#ifndef _RF_CALIBRATION_H_
#define _RF_CALIBRATION_H_

#define RF_CALI_TEST

#include <linux/init.h>
#include "wifi_drv_reg_ops.h"
#include "osdep.h"

#include "rf_d_adda_core_reg.h"
#include "rf_d_adda_esti_reg.h"
#include "rf_d_adda_recv_reg.h"
#include "rf_d_adda_xmit_reg.h"
#include "rf_d_rx_reg.h"
#include "rf_d_sx_reg.h"
#include "rf_d_top_reg.h"
#include "rf_d_tx_reg.h"

//define rf read/write method
#define I2C_WF_AN_CFG (0xa0d000 + 0x18)
#define RF_BW_20M 0
#define RF_BW_40M 1
#define RF_BW_80M 2

#define  PHY_TX_LPF_BYPASS (0x00a0b000 + 0x234)
#define  PHY_PRIMARY_CHANNEL (0x00a0b000 + 0x22c)
#define  PHY_TEST_BUS (0x00a0b000 + 0x100)
#define  AGC_AGC_CCA_CTRL (0x00a08000 + 0x10)
//#define PUTS TRACE

typedef unsigned int U32;
typedef unsigned char U8;

enum{
    False = 0,
    True
};

typedef enum WF_MODE
{
  WF_SLEEP = 0,
  WF5G_SX,
  WF5G_TX,
  WF5G_RX,
  WF5G_TXDPD,
  WF2G_SX,
  WF2G_TX,
  WF2G_RX
} WF_MODE_T;

typedef enum CALI_MODE
{
    RXDCOC    = 0,
    TXDCIQ    = 1,
    TXTSSI    = 2,
    TXDPD     = 3,
    RXLNATANK = 4,
    RXIRR     = 5,
    IDEL      = 15
} CALI_MODE_T;

//sx calibration type
typedef enum SX_CALI_MODE
{
    CHANNEL_MAP = 16,
    PCODE_CALI = 8,
    VCO_AMP_CALI = 4,
    TCODE_CALI = 2,
    MXR_TANK = 1
} SX_CALI_MODE_T;

enum RF_RG_ACCESS_MODE
{
    I2C_MODE = 0,
    SPI_MODE
};


struct _rx_rc
{
    unsigned int rxrc_out;
    unsigned int rxrc_2g20M_out;
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
    int snr;
};

struct rx_dc_result
{
    int num;
    int dci;
    int dcq;
    unsigned int code_i;
    unsigned int code_q;
};

struct _rx_irr
{
    unsigned int coeff_eup[4];
    unsigned int coeff_pup[4];
    unsigned int coeff_elow[4];
    unsigned int coeff_plow[4];
};

struct _tx_tssi
{
    unsigned int txpwc_slope;
    unsigned int txpwc_offset;
    int gain_ratio;
   unsigned char tssi_chn_idx;
};

struct wf5g_rxdcoc_in
{
    unsigned int bw_idx;
    unsigned int tia_idx;
    unsigned int lpf_idx;
    unsigned int wf_rx_gain;
    bool manual_mode;
};

struct wf5g_rxdcoc_result
{
    struct _rx_dc cal_rx_dc;
};

struct rx_lna_output
{
    unsigned char channel_idx;
    unsigned int code_out;
};

struct _rx_lna
{
    unsigned int rx_lna_code;
};

struct wf5g_rxlnatank_result
{
    struct _rx_lna  cal_rx_lna;
};

struct wf5g_rxrc_result
{
    struct _rx_rc cal_rx_rc;
};

struct wf5g_rxirr_result
{
    struct _rx_irr cal_rx_irr;       //idx by bw, chn
};

struct wf5g_txdciq_result
{
    struct _tx_dc_irr cal_tx_dciq; //idx by chn, gain
};

struct wf5g_txtssi_result
{
    struct _tx_tssi cal_tx_tssi;
};

struct wf_txdpd_result
{
    struct _tx_dpd cal_tx_dpd;    //idx by chn, bw gain
};

struct calb_rst_out
{
    struct _rx_rc rx_rc;
    struct _rx_dc rx_dc[3][2][12];     //bw, gain idx for addr
    struct _tx_dc_irr tx_dc_irr[5][9]; //idx by chn, gain
    struct _tx_dpd tx_dpd[5][3][3];    //idx by chn, bw gain
    struct _rx_lna rx_lna[10];         //idx by chn
    struct _rx_irr rx_irr[5][3];       //idx by bw, chn
    struct _tx_tssi tx_tssi;
};

struct wf2g_rxdcoc_in
{
    unsigned int bw_idx;
    unsigned int tia_idx;
    unsigned int lpf_idx;
    unsigned int wf_rx_gain;
    bool manual_mode;
};

struct wf2g_rxdcoc_result
{
    struct _rx_dc cal_rx_dc;
};

struct wf2g_rxlnatank_result
{
    struct _rx_lna  cal_rx_lna;
};

struct wf2g_rxirr_result
{
    struct _rx_irr cal_rx_irr;       //idx by bw, chn
};

struct wf2g_txdciq_result
{
    struct _tx_dc_irr cal_tx_dciq; //idx by chn, gain
};

struct wf2g_txtssi_result
{
    struct _tx_tssi cal_tx_tssi;
};

//struct wf2g_txdpd_result
//{
//    struct _tx_dpd cal_tx_dpd;    //idx by chn, bw gain
//};

struct t9026_wf5g_calb_rst_out
{
    struct _rx_rc rx_rc;
    struct _rx_dc rx_dc[2][12];     //chn, tia gain, lpf gain
    struct _tx_dc_irr tx_dc_irr[5][2]; //idx by chn, gain
    struct _tx_dpd tx_dpd[5][3];    //idx by chn, bw
    struct _rx_lna rx_lna[10];         //idx by chn
    struct _rx_irr rx_irr[5][3];       //idx by chn,bw
    struct _tx_tssi tx_tssi[5];       //idx by chn
};

struct t9026_wf2g_calb_rst_out
{
    struct _rx_rc rx_rc;
    struct _rx_dc rx_dc[2][12];     //chn, tia gain, lpf gain
    struct _tx_dc_irr tx_dc_irr[2]; //idx by chn, gain
    struct _tx_dpd tx_dpd[3];    //idx by chn, bw
    struct _rx_lna rx_lna;         //idx by chn
    struct _rx_irr rx_irr[2];       //idx by chn,bw
    struct _tx_tssi tx_tssi;
};

//rf calibration API
void t9026_rf_mode_access(WF_MODE_T wf_mode);
void t9026_cali_mode_access(CALI_MODE_T cali_mode);
void t9026_cali_mode_exit(void);
void t9026_cali_end(void);

//WF5G
void t9026_update_reg(void);
void t9026_xosc_update(void);
void t9026_wf5g_sx(U8 channel, U8 osc_freq, SX_CALI_MODE_T sx_cali_mode);
unsigned int t9026_wf5g_rxrc(struct wf5g_rxrc_result * wf5g_rxrc_result);
void t9026_wf5g_rxdcoc(struct wf5g_rxdcoc_in * wf5g_rxdcoc_in, struct wf5g_rxdcoc_result* wf5g_rxdcoc_result);
void t9026_wf5g_rxlnatank(struct wf5g_rxlnatank_result * wf5g_rxlnatank_result, bool manual, U8 channel);
void t9026_wf5g_rxirr(struct wf5g_rxirr_result* wf5g_rxirr_result, unsigned int bw_idx, U8 for_dpd_flg, bool check_flg);
void t9026_wf5g_txiqdc(struct wf5g_txdciq_result* wf5g_txdciq_result, U8 tx_pa_gain, U8 tx_mxr_gain, bool check_flg);
void t9026_wf5g_txtssi(struct wf5g_txtssi_result * wf5g_txtssi_result, int ph, int pl, int vh, int vl);
void t9026_wf5g_txdpd_start(U8 sync_gap, U8 bw_idx, U8 gain);
void t9026_wf5g_txdpd_end(struct wf_txdpd_result* wf5g_txdpd_result,  U8 sync_gap);
void t9026_wf5g_txdpd_rf_revert(void);
void t9026_wf5g_rxdcoc_all(struct wf5g_rxdcoc_in* wf5g_rxdcoc_in, struct wf5g_rxdcoc_result* wf5g_rxdcoc_result);
void t9026_wf5g_txdpd_rf_init(U8 tx_pa_gain, U8 tx_mxr_gain);
void t9026_ofdm_send_packet(void);
void t9026_ofdm_stop_packet(void);
void t9026_rf_mode_auto(void);
void t9026_wf5g_calall_onefreq(int chn_idx, struct t9026_wf5g_calb_rst_out* wf5g_calb_rst_out, bool check_flg);
unsigned int duty_cycle_cali(void);
void t9026_bg_r_cali(void);
void t9026_rf_tx_modulation_access(U8 wf_modulation);
void t9026_wf5g_txctune(U8 channel);
//WF2G
//#ifdef RF_CALI_TEST_2G
void t9026_wf2g_sx(U8 channel, U8 osc_freq, SX_CALI_MODE_T sx_cali_mode);
void t9026_wf2g_rxdcoc(struct wf2g_rxdcoc_in * wf2g_rxdcoc_in, struct wf2g_rxdcoc_result* wf2g_rxdcoc_result);
void t9026_wf2g_rxlnatank(struct wf2g_rxlnatank_result * wf2g_rxlnatank_result, bool manual, U8 channel);
void t9026_wf2g_rxirr(struct wf2g_rxirr_result* wf2g_rxirr_result, unsigned int bw_idx, U8 for_dpd_flg, bool check_flg);
void t9026_wf2g_txiqdc(struct wf2g_txdciq_result* wf2g_txdciq_result, U8 tx_pa_gain, U8 tx_mxr_gain, bool check_flg);
void t9026_wf2g_txtssi(struct wf2g_txtssi_result * wf2g_txtssi_result, int ph, int pl, int vh, int vl);
void t9026_wf2g_txdpd_rf_init(U8 tx_pa_gain, U8 tx_mxr_gain);
void t9026_wf2g_txdpd_start(U8 sync_gap, U8 bw_idx);
void t9026_wf2g_txdpd_end(struct wf_txdpd_result* wf2g_txdpd_result,  U8 sync_gap);
void t9026_wf2g_txdpd_rf_revert(void);
void t9026_wf2g_calall_onefreq(U8 channel, struct t9026_wf2g_calb_rst_out* wf2g_calb_rst_out, bool check_flg);
//#endif


void rf_set_osc_freq(U8 division);
void rf_set_register_access_mode(enum RF_RG_ACCESS_MODE mode);
void rf_xosc_calibration(void);
void rf_bg_r_calibration(void);
void rf_duty_cycle_calibration(void);
void rf_sx_calibration(WF_MODE_T wf_mode, U8 channel, U8 division, SX_CALI_MODE_T sx_cali_mode);
void rf_switch_tcode_calibration(U8 sw_bnd);
void rf_rx_calibration(void);
void rf_dcoc_calibration(WF_MODE_T wf_mode);
void rf_dciq_calibration(WF_MODE_T wf_mode);
void rf_dpd_calibration_start(WF_MODE_T wf_mode);
void rf_dpd_calibration_end(void);
void rf_lnatank_calibration(WF_MODE_T wf_mode);
void rf_tssi_calibration(WF_MODE_T wf_mode);
void rf_rxirr_calibration(U8 rx_bw, WF_MODE_T wf_mode);
void rf_rc_calibration(U8 division, WF_MODE_T wf_mode);
unsigned int rf_read_register(unsigned int addr);
void rf_write_register(unsigned int addr,unsigned int data);
unsigned int fi_ahb_read(unsigned int addr);
void fi_ahb_write(unsigned int addr,unsigned int data);

int t9026_get_spectrum_snr_pwr(int freq_offset, U8 tone_mode);
void t9026_wf2g_txiqdc_check(U8 tx_gain);
void t9026_wf5g_txiqdc_check(void);
int t9026_wf2g_check_loop_rx_irr(struct wf2g_rxirr_result* wf2g_rxirr_result, unsigned int bw_idx);
int t9026_wf5g_check_loop_rx_irr(struct wf5g_rxirr_result* wf5g_rxirr_result, unsigned int bw_idx);

void t9026_adc_on(void);
void t9026_adc_off(void);
void t9026_dcoc_dig_comp(void);
void t9026_wf5g_txtssi_ratio(struct wf5g_txtssi_result * wf5g_txtssi_result, int slope_tmp, int offset_tmp, U8 tssi_chn_idx, U8 tx_mxr_gain);
void t9026_wf5g_txtssi_bias(int slope_tmp, int offset_tmp);
void t9026_wf2g_txtssi_ratio(struct wf2g_txtssi_result * wf2g_txtssi_result, int slope_tmp, int offset_tmp);
void t9026_wf5g_cal_for_scan(U8 channel, struct t9026_wf5g_calb_rst_out* wf5g_calb_rst_out, bool check_flg);
void t9026_wf5g_cal_for_linkA(U8 channel, U8 bw_idx, struct t9026_wf5g_calb_rst_out* wf5g_calb_rst_out, bool check_flg);
void t9026_wf5g_cal_for_linkB(U8 channel, U8 bw_idx, struct t9026_wf5g_calb_rst_out* wf5g_calb_rst_out, bool check_flg);
void t9026_wf2g_cal_for_scan(U8 channel, struct t9026_wf2g_calb_rst_out* wf2g_calb_rst_out, bool check_flg);
void t9026_wf2g_cal_for_link(U8 channel, U8 bw_idx, struct t9026_wf2g_calb_rst_out* wf2g_calb_rst_out, bool check_flg);
unsigned int efuse_manual_read(unsigned int addr);
unsigned int t9026_wf2g_rxrc_for_20m(struct wf5g_rxrc_result* wf2g_rxrc_result);
void t9026_wf5g_get_efuse_tssi(struct wf5g_txtssi_result * wf5g_txtssi_result, unsigned int channel);
#define RF_TIME_OUT_CNT (100000)
enum
{
    RF_DEBUG_NONE = 0,
    RF_DEBUG_RESULT = BIT(0),
    RF_DEBUG_SX = BIT(1),
    RF_DEBUG_RESULT_CHECK = BIT(5),
    RF_DEBUG_MSG = BIT(6),
    RF_DEBUG_INFO = BIT(7),
    RF_DEBUG_ALL = 0xFF
};

extern unsigned char rf_debug;

#define RF_DPRINTF( _m,  ...) \
do { \
    if (rf_debug & (_m)) \
        printk(__VA_ARGS__); \
} while (0)

#endif//_RF_CALIBRATION_H_

