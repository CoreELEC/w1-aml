
#include "rf_calibration.h"
#ifdef RF_CALI_TEST
void t9026_wf5g_txdpd_rf_init(U8 tx_pa_gain, U8 tx_mxr_gain)
{
    RG_TOP_A15_FIELD_T    rg_top_a15;
    RG_TOP_A7_FIELD_T    rg_top_a7;
    RG_TX_A2_FIELD_T    rg_tx_a2;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_RX_A5_FIELD_T    rg_rx_a5;
    RG_RX_A68_FIELD_T    rg_rx_a68;
    RG_TOP_A19_FIELD_T    rg_top_a19;
    RG_SX_A25_FIELD_T    rg_sx_a25;
    RG_RX_A66_FIELD_T   rg_rx_a66;

    struct wf5g_rxdcoc_in wf5g_rxdcoc_in;
    struct wf5g_rxdcoc_result wf5g_rxdcoc_result;

    rg_top_a15.data = rf_read_register(RG_TOP_A15);
    rg_top_a15.b.rg_wf_adda_man_mode = 0x1;
    rf_write_register(RG_TOP_A15, rg_top_a15.data);

    rg_top_a7.data = rf_read_register(RG_TOP_A7);
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX2_EN = 0x1;
    rg_top_a7.b.RG_M2_WF_TRX_LF_LDO_RX_EN = 0x1;
    rf_write_register(RG_TOP_A7, rg_top_a7.data);

    rg_tx_a2.data = rf_read_register(RG_TX_A2);
    rg_tx_a2.b.RG_WF5G_TX_PA_GAIN_MAN = tx_pa_gain;
    rg_tx_a2.b.RG_WF5G_TX_MXR_GAIN_MAN = tx_mxr_gain;
    rg_tx_a2.b.RG_WF5G_TX_MAN_MODE = 0x1;
    rf_write_register(RG_TX_A2, rg_tx_a2.data);

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE = 0x1;
    rg_rx_a2.b.RG_WF_RX_BW_SEL = 0x3;
    rg_rx_a2.b.RG_WF_RX_ADCFS_SEL = 0x1;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x8c;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);

    rg_rx_a5.data = rf_read_register(RG_RX_A5);
    rg_rx_a5.b.RG_M2_WF_RX_MXR_BIAS_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF5G_RX_DPD_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF_RTX_ABB_RX_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF_RX_TIA_BIAS_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF_RX_RSSIPGA_BIAS_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF_RX_LPF_BIAS_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF_RX_TIA_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF_RX_RSSIPGA_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF_RX_LPF_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF_RX_LPF_I_DCOC_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF_RX_LPF_Q_DCOC_EN = 0x1;
    rf_write_register(RG_RX_A5, rg_rx_a5.data);

    rg_rx_a68.data = rf_read_register(RG_RX_A68);
    rg_rx_a68.b.RG_WF_RX_LPF_NOTCH_EN = 0x0;
    rf_write_register(RG_RX_A68, rg_rx_a68.data);

    rg_top_a19.data = rf_read_register(RG_TOP_A19);
    rg_top_a19.b.rg_m2_wf_wadc_enable_i = 0x1;
    rg_top_a19.b.rg_m2_wf_wadc_enable_q = 0x1;
    rf_write_register(RG_TOP_A19, rg_top_a19.data);

    rg_sx_a25.data = rf_read_register(RG_SX_A25);
    rg_sx_a25.b.RG_M2_WF_SX_LOG5G_DPDLO_EN = 0x1;
    rf_write_register(RG_SX_A25, rg_sx_a25.data);

    rg_rx_a66.data = rf_read_register(RG_RX_A66);
    rg_rx_a66.b.RG_WF5G_RX_DPD_ATT_CTRL = 0x0;//0x0;//0x1;
    rg_rx_a66.b.RG_WF5G_RX_DPD_GM_GAIN = 0x7;//0x07;//0x7;
    rf_write_register(RG_RX_A66, rg_rx_a66.data);

    wf5g_rxdcoc_in.bw_idx = 0;
    wf5g_rxdcoc_in.tia_idx = 0;
    wf5g_rxdcoc_in.lpf_idx = 1;
    wf5g_rxdcoc_in.wf_rx_gain = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
    wf5g_rxdcoc_in.manual_mode = 1;
    t9026_wf5g_rxdcoc(&wf5g_rxdcoc_in, &wf5g_rxdcoc_result);

}

#endif // RF_CALI_TEST
