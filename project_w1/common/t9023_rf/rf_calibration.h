#ifndef _RF_CALIBRATION_H_
#define _RF_CALIBRATION_H_

#include "rf_d_top_reg.h"

//define rf read/write method 
#define I2C_WF_AN_CFG (0xa0d000 + 0x18)
#define RF_BW_20M 0
#define RF_BW_40M 1
#define RF_BW_80M 2

enum{
    False = 0,
    True 
};

typedef enum WF_MODE
{
    WF5G_SLEEP = 0,
    WF5G_SX, 
    WF5G_TX, 
    WF5G_RX, 
    WF2G_SLEEP, 
    WF2G_SX, 
    WF2G_TX, 
    WF2G_RX 
} WF_MODE_T;

typedef enum WF5G_CALI_MODE
{
    WF5G_RXDCOC    = 0, 
    WF5G_TXDCIQ    = 1,
    WF5G_TXTSSI    = 2,
    WF5G_TXDPD     = 3,
    WF5G_RXLNATANK = 4,
    WF5G_RXIRR     = 5,
    WF5G_IDEL      = 15
} WF5G_CALI_MODE_T;

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
};

struct wf5g_rxdcoc_in
{
    unsigned int bw_idx;
    unsigned int tia_idx;
    unsigned int lpf_idx;
    unsigned int wf_rx_gain;
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
    struct _rx_lna  cal_rx_lna[10];
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

struct wf5g_txdpd_result
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

//rf calibration API
void wf5g_sx(U8 channel, U8 osc_freq, SX_CALI_MODE_T sx_cali_mode);
unsigned int wf5g_rxrc(struct wf5g_rxrc_result * wf5g_rxrc_result);
void wf5g_rxdcoc(struct wf5g_rxdcoc_in * wf5g_rxdcoc_in, struct wf5g_rxdcoc_result* wf5g_rxdcoc_result);
struct rx_lna_output wf5g_rxlnatank(struct wf5g_rxlnatank_result * wf5g_rxlnatank_result, bool manual, U8 channel);
void wf5g_rxirr(struct wf5g_rxirr_result* wf5g_rxirr_result, unsigned int bw_idx); 
void wf5g_txiqdc(struct wf5g_txdciq_result* wf5g_txdciq_result, U8 tx_pa_gain, U8 tx_mxr_gain);
void wf5g_txtssi(struct wf5g_txtssi_result * wf5g_txtssi_result, int ph, int pl, int vh, int vl);
void wf5g_txdpd(struct wf5g_txdpd_result* wf5g_txdpd_result,  U8 sync_gap, U8 tx_pa_gain, U8 tx_mxr_gain);
void wf5g_txdpd_start(U8 sync_gap, U8 tx_pa_gain, U8 tx_mxr_gain);
void wf5g_txdpd_end(struct wf5g_txdpd_result* wf5g_txdpd_result,  U8 sync_gap);
unsigned int duty_cycle_cali();
void bg_r_cali();
void wf5g_rf_mode_access(WF_MODE_T wf_mode);
void wf5g_cali_mode_access(WF5G_CALI_MODE_T wf5g_cali_mode);
void wf5g_cali_mode_exit(void);



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

typedef U32 (*rf_read_register_ptr)(U32 addr);
typedef void (*rf_write_register_ptr)(U32 addr, U32 data);

extern rf_read_register_ptr rf_read_register;
extern rf_write_register_ptr rf_write_register;

#endif//_RF_CALIBRATION_H_

