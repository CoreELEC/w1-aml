
#include "rf_calibration.h"
#ifdef RF_CALI_TEST
void t9026_wf2g_txdpd_rf_init(U8 tx_pa_gain, U8 tx_mxr_gain)
{
    struct wf2g_rxdcoc_in wf2g_rxdcoc_in;
    struct wf2g_rxdcoc_result wf2g_rxdcoc_result;
    RG_TOP_A15_FIELD_T    rg_top_a15;
    RG_TOP_A29_FIELD_T    rg_top_a29;
    RG_TX_A6_FIELD_T    rg_tx_a6;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_RX_A90_FIELD_T    rg_rx_a90;
    RG_RX_A68_FIELD_T    rg_rx_a68;
    RG_TOP_A23_FIELD_T    rg_top_a23;
    RG_SX_A48_FIELD_T    rg_sx_a48;

    rg_top_a15.data = rf_read_register(RG_TOP_A15);
    rg_top_a15.b.rg_wf_dac_ampctrl_man = 0x2;
    rg_top_a15.b.rg_wf_adda_man_mode = 0x1;
    rf_write_register(RG_TOP_A15, rg_top_a15.data);

    rg_top_a29.data = rf_read_register(RG_TOP_A29);
    rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RX2_EN = 0x1;
    rg_top_a29.b.RG_M6_WF_TRX_LF_LDO_RX_EN = 0x1;
    rf_write_register(RG_TOP_A29, rg_top_a29.data);

    rg_tx_a6.data = rf_read_register(RG_TX_A6);
    rg_tx_a6.b.RG_WF2G_TX_PA_GAIN_MAN = tx_pa_gain;
    rg_tx_a6.b.RG_WF2G_TX_MXR_GAIN_MAN = tx_mxr_gain;
    rg_tx_a6.b.RG_WF2G_TX_MAN_MODE = 0x1;
    rf_write_register(RG_TX_A6, rg_tx_a6.data);

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE = 0x1;
    rg_rx_a2.b.RG_WF_RX_BW_SEL = 0x3;
    rg_rx_a2.b.RG_WF_RX_ADCFS_SEL = 0x1;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x8b;//'0x8a'
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);

    rg_rx_a90.data = rf_read_register(RG_RX_A90);
    rg_rx_a90.b.RG_M6_WF_RX_MXR_BIAS_EN = 0x1;
    rg_rx_a90.b.RG_M6_WF2G_RX_DPD_EN = 0x1;
    rg_rx_a90.b.RG_M6_WF_RTX_ABB_RX_EN = 0x1;
    rg_rx_a90.b.RG_M6_WF_RX_TIA_BIAS_EN = 0x1;
    rg_rx_a90.b.RG_M6_WF_RX_RSSIPGA_BIAS_EN = 0x1;
    rg_rx_a90.b.RG_M6_WF_RX_LPF_BIAS_EN = 0x1;
    rg_rx_a90.b.RG_M6_WF_RX_TIA_EN = 0x1;
    rg_rx_a90.b.RG_M6_WF_RX_RSSIPGA_EN = 0x1;
    rg_rx_a90.b.RG_M6_WF_RX_LPF_EN = 0x1;
    rg_rx_a90.b.RG_M6_WF_RX_LPF_I_DCOC_EN = 0x1;
    rg_rx_a90.b.RG_M6_WF_RX_LPF_Q_DCOC_EN = 0x1;
    rf_write_register(RG_RX_A90, rg_rx_a90.data);

    rg_rx_a68.data = rf_read_register(RG_RX_A68);
    rg_rx_a68.b.RG_WF_RX_LPF_NOTCH_EN = 0x0;
    rf_write_register(RG_RX_A68, rg_rx_a68.data);

    rg_top_a23.data = rf_read_register(RG_TOP_A23);
    //rg_top_a23.b.rg_m6_wf_radc_en = 0x1;
    rg_top_a23.b.rg_m6_wf_wadc_enable_i = 0x1;
    rg_top_a23.b.rg_m6_wf_wadc_enable_q = 0x1;
    rf_write_register(RG_TOP_A23, rg_top_a23.data);

	rg_sx_a48.data = rf_read_register(RG_SX_A48);
    rg_sx_a48.b.RG_M6_WF_SX_LOG2G_DPDLO_EN = 0x1;
    rf_write_register(RG_SX_A48, rg_sx_a48.data);

    wf2g_rxdcoc_in.bw_idx = 0;
    wf2g_rxdcoc_in.tia_idx = 0;
    wf2g_rxdcoc_in.lpf_idx = 1;
    wf2g_rxdcoc_in.wf_rx_gain = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
    wf2g_rxdcoc_in.manual_mode = 1;
    t9026_wf2g_rxdcoc(&wf2g_rxdcoc_in, &wf2g_rxdcoc_result);

}

#endif // RF_CALI_TEST
