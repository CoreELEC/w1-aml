#include "rf_calibration.h"

#ifdef RF_CALI_TEST

void t9026_wf5g_rxirr(struct wf5g_rxirr_result* wf5g_rxirr_result, unsigned int bw_idx, U8 for_dpd_flg, bool check_flg)
{
    RG_TOP_A15_FIELD_T    rg_top_a15;
    RG_TOP_A7_FIELD_T    rg_top_a7;
    RG_SX_A25_FIELD_T    rg_sx_a25;
    RG_TX_A2_FIELD_T    rg_tx_a2;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_TX_A3_FIELD_T    rg_tx_a3;
    RG_RX_A5_FIELD_T    rg_rx_a5;
    RG_TOP_A19_FIELD_T    rg_top_a19;
    RG_RX_A11_FIELD_T    rg_rx_a11;
    RG_RECV_A11_FIELD_T    rg_recv_a11;
    RG_XMIT_A56_FIELD_T    rg_xmit_a56;
    RG_XMIT_A46_FIELD_T    rg_xmit_a46;
    RG_XMIT_A106_FIELD_T    rg_xmit_a106;
    RG_ESTI_A11_FIELD_T    rg_esti_a11;
    RG_XMIT_A3_FIELD_T    rg_xmit_a3;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4;
    RG_CORE_A5_FIELD_T    rg_core_a5;
    RG_ESTI_A30_FIELD_T    rg_esti_a30;
    RG_ESTI_A31_FIELD_T    rg_esti_a31;
    RG_ESTI_A32_FIELD_T    rg_esti_a32;
    RG_ESTI_A33_FIELD_T    rg_esti_a33;
    RG_ESTI_A34_FIELD_T    rg_esti_a34;
    RG_ESTI_A35_FIELD_T    rg_esti_a35;
    RG_ESTI_A36_FIELD_T    rg_esti_a36;
    RG_ESTI_A37_FIELD_T    rg_esti_a37;
    RG_ESTI_A38_FIELD_T    rg_esti_a38;
    RG_ESTI_A39_FIELD_T    rg_esti_a39;
    RG_ESTI_A40_FIELD_T    rg_esti_a40;
    RG_ESTI_A41_FIELD_T    rg_esti_a41;
    RG_ESTI_A42_FIELD_T    rg_esti_a42;
    RG_ESTI_A43_FIELD_T    rg_esti_a43;
    RG_ESTI_A44_FIELD_T    rg_esti_a44;
    RG_ESTI_A45_FIELD_T    rg_esti_a45;
    RG_ESTI_A46_FIELD_T    rg_esti_a46;
    RG_ESTI_A47_FIELD_T    rg_esti_a47;
    RG_ESTI_A48_FIELD_T    rg_esti_a48;
    RG_ESTI_A49_FIELD_T    rg_esti_a49;
    RG_ESTI_A50_FIELD_T    rg_esti_a50;
    RG_ESTI_A51_FIELD_T    rg_esti_a51;
    RG_ESTI_A52_FIELD_T    rg_esti_a52;
    RG_ESTI_A53_FIELD_T    rg_esti_a53;
    RG_ESTI_A3_FIELD_T    rg_esti_a3;
    RG_ESTI_A4_FIELD_T    rg_esti_a4;
    RG_ESTI_A5_FIELD_T    rg_esti_a5;
    RG_ESTI_A6_FIELD_T    rg_esti_a6;
    RG_ESTI_A7_FIELD_T    rg_esti_a7;
    RG_ESTI_A8_FIELD_T    rg_esti_a8;
    RG_ESTI_A9_FIELD_T    rg_esti_a9;
    RG_ESTI_A10_FIELD_T   rg_esti_a10;
    RG_RECV_A2_FIELD_T    rg_recv_a2;

    // Internal variables definitions
    bool    RG_TOP_A15_saved = False;
    RG_TOP_A15_FIELD_T    rg_top_a15_i;
    // Internal variables definitions
    bool    RG_TOP_A7_saved = False;
    RG_TOP_A7_FIELD_T    rg_top_a7_i;
    // Internal variables definitions
    bool    RG_SX_A25_saved = False;
    RG_SX_A25_FIELD_T    rg_sx_a25_i;
    // Internal variables definitions
    bool    RG_TX_A2_saved = False;
    RG_TX_A2_FIELD_T    rg_tx_a2_i;
    // Internal variables definitions
    bool    RG_RX_A2_saved = False;
    RG_RX_A2_FIELD_T    rg_rx_a2_i;
    bool    RG_TX_A3_saved = False;
    RG_TX_A3_FIELD_T    rg_tx_a3_i;
    // Internal variables definitions
    bool    RG_RX_A5_saved = False;
    RG_RX_A5_FIELD_T    rg_rx_a5_i;
    // Internal variables definitions
    bool    RG_TOP_A19_saved = False;
    RG_TOP_A19_FIELD_T    rg_top_a19_i;
    // Internal variables definitions
    bool    RG_RX_A11_saved = False;
    RG_RX_A11_FIELD_T    rg_rx_a11_i;
    // Internal variables definitions
    bool    RG_XMIT_A56_saved = False;
    RG_XMIT_A56_FIELD_T    rg_xmit_a56_i;
    // Internal variables definitions
    bool    RG_XMIT_A46_saved = False;
    RG_XMIT_A46_FIELD_T    rg_xmit_a46_i;
    // Internal variables definitions
    bool    RG_XMIT_A106_saved = False;
    RG_XMIT_A106_FIELD_T    rg_xmit_a106_i;
    // Internal variables definitions
    bool    RG_ESTI_A11_saved = False;
    RG_ESTI_A11_FIELD_T    rg_esti_a11_i;
    // Internal variables definitions
    bool    RG_XMIT_A3_saved = False;
    RG_XMIT_A3_FIELD_T    rg_xmit_a3_i;
    // Internal variables definitions
    bool    RG_XMIT_A4_saved = False;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4_i;

    unsigned int unlock_cnt = 0;
    unsigned int TIME_OUT_CNT = RF_TIME_OUT_CNT;
    unsigned int rx_irr_rdy;
    unsigned int coeff_eup0;
    unsigned int coeff_eup1;
    unsigned int coeff_eup2;
    unsigned int coeff_eup3;
    unsigned int coeff_pup0;
    unsigned int coeff_pup1;
    unsigned int coeff_pup2;
    unsigned int coeff_pup3;
    unsigned int coeff_elow0;
    unsigned int coeff_elow1;
    unsigned int coeff_elow2;
    unsigned int coeff_elow3;
    unsigned int coeff_plow0;
    unsigned int coeff_plow1;
    unsigned int coeff_plow2;
    unsigned int coeff_plow3;
    struct wf5g_rxdcoc_in wf5g_rxdcoc_in;
    struct wf5g_rxdcoc_result wf5g_rxdcoc_result;
    CALI_MODE_T cali_mode = RXIRR;

    rg_rx_a2_i.b.RG_WF_RX_BBBM_MAN_MODE = 0;
    rg_rx_a2_i.b.RG_WF_RX_BW_SEL = 0;
    rg_rx_a2_i.b.RG_WF_RX_ADCFS_SEL = 0;
    rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN =0;
    rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE = 0;

    if (for_dpd_flg == 0) {
        rg_top_a15.data = rf_read_register(RG_TOP_A15);
        if(!RG_TOP_A15_saved) {
            RG_TOP_A15_saved = True;
            rg_top_a15_i.b.rg_wf_adda_man_mode = rg_top_a15.b.rg_wf_adda_man_mode;
            rg_top_a15_i.b.rg_wf_dac_ampctrl_man = rg_top_a15.b.rg_wf_dac_ampctrl_man;
        }
        rg_top_a15.b.rg_wf_adda_man_mode = 0x1;
        rg_top_a15.b.rg_wf_dac_ampctrl_man = 0x2;
        rf_write_register(RG_TOP_A15, rg_top_a15.data);

        rg_top_a7.data = rf_read_register(RG_TOP_A7);
        if(!RG_TOP_A7_saved) {
            RG_TOP_A7_saved = True;
            rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RXB_EN = rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RXB_EN;
            rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RX1_EN = rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX1_EN;
            rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RX2_EN = rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX2_EN;
            rg_top_a7_i.b.RG_M2_WF_TRX_LF_LDO_RX_EN = rg_top_a7.b.RG_M2_WF_TRX_LF_LDO_RX_EN;
        }
        rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RXB_EN = 0x1;
        rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX1_EN = 0x1;
        rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX2_EN = 0x1;
        rg_top_a7.b.RG_M2_WF_TRX_LF_LDO_RX_EN = 0x1;
        rf_write_register(RG_TOP_A7, rg_top_a7.data);

        rg_sx_a25.data = rf_read_register(RG_SX_A25);
        if (!RG_SX_A25_saved) {
            RG_SX_A25_saved = True;
            rg_sx_a25_i.b.RG_M2_WF_SX_LOG5G_RXLO_EN = rg_sx_a25.b.RG_M2_WF_SX_LOG5G_RXLO_EN;
        }
        rg_sx_a25.b.RG_M2_WF_SX_LOG5G_RXLO_EN = 0x1;
        rf_write_register(RG_SX_A25, rg_sx_a25.data);

        rg_tx_a2.data = rf_read_register(RG_TX_A2);
        if (!RG_TX_A2_saved) {
            RG_TX_A2_saved = True;
            rg_tx_a2_i.b.RG_WF5G_TX_RXIQCAL_EN = rg_tx_a2.b.RG_WF5G_TX_RXIQCAL_EN;
            rg_tx_a2_i.b.RG_WF5G_TX_MAN_MODE = rg_tx_a2.b.RG_WF5G_TX_MAN_MODE;
        }
        rg_tx_a2.b.RG_WF5G_TX_RXIQCAL_EN = 0x1;
        rg_tx_a2.b.RG_WF5G_TX_MAN_MODE = 0x1;
        rf_write_register(RG_TX_A2, rg_tx_a2.data);

        rg_tx_a3.data = rf_read_register(RG_TX_A3);
        if(!RG_TX_A3_saved) {
            RG_TX_A3_saved = True;
            rg_tx_a3_i.b.RG_M2_WF5G_TX_PA_EN = rg_tx_a3.b.RG_M2_WF5G_TX_PA_EN;
        }
        rg_tx_a3.b.RG_M2_WF5G_TX_PA_EN = 0x0;
        rf_write_register(RG_TX_A3, rg_tx_a3.data);

        rg_rx_a5.data = rf_read_register(RG_RX_A5);
        if(!RG_RX_A5_saved) {
            RG_RX_A5_saved = True;
            rg_rx_a5_i.b.RG_M2_WF_RX_LNA_BIAS_EN = rg_rx_a5.b.RG_M2_WF_RX_LNA_BIAS_EN;
            rg_rx_a5_i.b.RG_M2_WF_RX_GM_BIAS_EN = rg_rx_a5.b.RG_M2_WF_RX_GM_BIAS_EN;
            rg_rx_a5_i.b.RG_M2_WF_RX_MXR_BIAS_EN = rg_rx_a5.b.RG_M2_WF_RX_MXR_BIAS_EN;
            rg_rx_a5_i.b.RG_M2_WF5G_RX_LNA_EN = rg_rx_a5.b.RG_M2_WF5G_RX_LNA_EN;
            rg_rx_a5_i.b.RG_M2_WF5G_RX_GM_EN = rg_rx_a5.b.RG_M2_WF5G_RX_GM_EN;
            rg_rx_a5_i.b.RG_M2_WF5G_RX_MXR_EN = rg_rx_a5.b.RG_M2_WF5G_RX_MXR_EN;
            rg_rx_a5_i.b.RG_M2_WF5G_RX_IRRCAL_EN = rg_rx_a5.b.RG_M2_WF5G_RX_IRRCAL_EN;
            rg_rx_a5_i.b.RG_M2_WF_RTX_ABB_RX_EN = rg_rx_a5.b.RG_M2_WF_RTX_ABB_RX_EN;
            rg_rx_a5_i.b.RG_M2_WF_RX_TIA_BIAS_EN = rg_rx_a5.b.RG_M2_WF_RX_TIA_BIAS_EN;
            rg_rx_a5_i.b.RG_M2_WF_RX_RSSIPGA_BIAS_EN = rg_rx_a5.b.RG_M2_WF_RX_RSSIPGA_BIAS_EN;
            rg_rx_a5_i.b.RG_M2_WF_RX_LPF_BIAS_EN = rg_rx_a5.b.RG_M2_WF_RX_LPF_BIAS_EN;
            rg_rx_a5_i.b.RG_M2_WF_RX_TIA_EN = rg_rx_a5.b.RG_M2_WF_RX_TIA_EN;
            rg_rx_a5_i.b.RG_M2_WF_RX_RSSIPGA_EN = rg_rx_a5.b.RG_M2_WF_RX_RSSIPGA_EN;
            rg_rx_a5_i.b.RG_M2_WF_RX_LPF_EN = rg_rx_a5.b.RG_M2_WF_RX_LPF_EN;
            rg_rx_a5_i.b.RG_M2_WF_RX_LPF_I_DCOC_EN = rg_rx_a5.b.RG_M2_WF_RX_LPF_I_DCOC_EN;
            rg_rx_a5_i.b.RG_M2_WF_RX_LPF_Q_DCOC_EN = rg_rx_a5.b.RG_M2_WF_RX_LPF_Q_DCOC_EN;
        }
        rg_rx_a5.b.RG_M2_WF_RX_LNA_BIAS_EN = 0x1;
        rg_rx_a5.b.RG_M2_WF_RX_GM_BIAS_EN = 0x1;
        rg_rx_a5.b.RG_M2_WF_RX_MXR_BIAS_EN = 0x1;
        rg_rx_a5.b.RG_M2_WF5G_RX_LNA_EN = 0x1;
        rg_rx_a5.b.RG_M2_WF5G_RX_GM_EN = 0x1;
        rg_rx_a5.b.RG_M2_WF5G_RX_MXR_EN = 0x1;
        rg_rx_a5.b.RG_M2_WF5G_RX_IRRCAL_EN = 0x1;
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

        rg_top_a19.data = rf_read_register(RG_TOP_A19);
        if(!RG_TOP_A19_saved) {
            RG_TOP_A19_saved = True;
            rg_top_a19_i.b.rg_m2_wf_radc_en = rg_top_a19.b.rg_m2_wf_radc_en;
            rg_top_a19_i.b.rg_m2_wf_wadc_enable_i = rg_top_a19.b.rg_m2_wf_wadc_enable_i;
            rg_top_a19_i.b.rg_m2_wf_wadc_enable_q = rg_top_a19.b.rg_m2_wf_wadc_enable_q;
        }
        rg_top_a19.b.rg_m2_wf_radc_en = 0x1;
        rg_top_a19.b.rg_m2_wf_wadc_enable_i = 0x1;
        rg_top_a19.b.rg_m2_wf_wadc_enable_q = 0x1;
        rf_write_register(RG_TOP_A19, rg_top_a19.data);

        rg_rx_a11.data = rf_read_register(RG_RX_A11);
        if(!RG_RX_A11_saved) {
            RG_RX_A11_saved = True;
            rg_rx_a11_i.b.RG_WF_RXFE_GAIN_G2 = rg_rx_a11.b.RG_WF_RXFE_GAIN_G2;
        }
        rg_rx_a11.b.RG_WF_RXFE_GAIN_G2 = 0x2a;                        // Change RXFE G2 GM as high gain for Max. gain IRR performance
        rf_write_register(RG_RX_A11, rg_rx_a11.data);

        if(bw_idx == RF_BW_20M)
        {
            rg_rx_a2.data = rf_read_register(RG_RX_A2);
            if(!RG_RX_A2_saved) {
                RG_RX_A2_saved = True;
                rg_rx_a2_i.b.RG_WF_RX_BBBM_MAN_MODE = rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE;
                rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
                rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE;
                rg_rx_a2_i.b.RG_WF_RX_BW_SEL = rg_rx_a2.b.RG_WF_RX_BW_SEL;
                rg_rx_a2_i.b.RG_WF_RX_ADCFS_SEL = rg_rx_a2.b.RG_WF_RX_ADCFS_SEL;
            }
            rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE = 0x1;
            rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x21;
            rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
            rg_rx_a2.b.RG_WF_RX_BW_SEL = 0x0;                         // 20MHGz BW
            rg_rx_a2.b.RG_WF_RX_ADCFS_SEL = 0x0;                         // adc 160MHz
            rf_write_register(RG_RX_A2, rg_rx_a2.data);
        }
        else if(bw_idx == RF_BW_40M)
        {
            rg_rx_a2.data = rf_read_register(RG_RX_A2);
            if(!RG_RX_A2_saved) {
                RG_RX_A2_saved = True;
                rg_rx_a2_i.b.RG_WF_RX_BBBM_MAN_MODE = rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE;
                rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
                rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE;
                rg_rx_a2_i.b.RG_WF_RX_BW_SEL = rg_rx_a2.b.RG_WF_RX_BW_SEL;
                rg_rx_a2_i.b.RG_WF_RX_ADCFS_SEL = rg_rx_a2.b.RG_WF_RX_ADCFS_SEL;
            }
            rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE = 0x1;
            rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x21;
            rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
            rg_rx_a2.b.RG_WF_RX_BW_SEL = 0x1;                         // 40MHz BW
            rg_rx_a2.b.RG_WF_RX_ADCFS_SEL = 0x0;                         // adc 160MHz
            rf_write_register(RG_RX_A2, rg_rx_a2.data);

        }
        else if(bw_idx == RF_BW_80M)
        {
            rg_rx_a2.data = rf_read_register(RG_RX_A2);
            if(!RG_RX_A2_saved) {
                RG_RX_A2_saved = True;
                rg_rx_a2_i.b.RG_WF_RX_BBBM_MAN_MODE = rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE;
                rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
                rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE;
                rg_rx_a2_i.b.RG_WF_RX_BW_SEL = rg_rx_a2.b.RG_WF_RX_BW_SEL;
                rg_rx_a2_i.b.RG_WF_RX_ADCFS_SEL = rg_rx_a2.b.RG_WF_RX_ADCFS_SEL;
            }
            rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE = 0x1;
            rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x23;
            rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
            rg_rx_a2.b.RG_WF_RX_BW_SEL = 0x2;                         // 80MHz BW
            rg_rx_a2.b.RG_WF_RX_ADCFS_SEL = 0x1;                         // adc 320MHz
            rf_write_register(RG_RX_A2, rg_rx_a2.data);
        } //if(bw_idx == RF_BW_20M)

        wf5g_rxdcoc_in.bw_idx = 0;
        wf5g_rxdcoc_in.tia_idx = 0;
        wf5g_rxdcoc_in.lpf_idx = 1;
        wf5g_rxdcoc_in.wf_rx_gain = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
        wf5g_rxdcoc_in.manual_mode = 1;
        t9026_wf5g_rxdcoc(&wf5g_rxdcoc_in, &wf5g_rxdcoc_result);
    }

    fi_ahb_write(PHY_TX_LPF_BYPASS, 0x1);

    rg_recv_a11.data = fi_ahb_read(RG_RECV_A11);
    rg_recv_a11.b.rg_rxirr_man_mode = 0x0;
    rg_recv_a11.b.rg_rxirr_bypass = 0x1;
    fi_ahb_write(RG_RECV_A11, rg_recv_a11.data);

    rg_xmit_a56.data = fi_ahb_read(RG_XMIT_A56);
    if(!RG_XMIT_A56_saved) {
        RG_XMIT_A56_saved = True;
        rg_xmit_a56_i.b.rg_txdpd_comp_bypass = rg_xmit_a56.b.rg_txdpd_comp_bypass;
        rg_xmit_a56_i.b.rg_txdpd_comp_bypass_man = rg_xmit_a56.b.rg_txdpd_comp_bypass_man;
    }
    rg_xmit_a56.b.rg_txdpd_comp_bypass = 0x1;                         // new_set_reg(0xe4f0, 0x15008080);   //TX DPD bypass
    rg_xmit_a56.b.rg_txdpd_comp_bypass_man = 0x1;
    fi_ahb_write(RG_XMIT_A56, rg_xmit_a56.data);

    rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
    if(!RG_XMIT_A46_saved) {
        RG_XMIT_A46_saved = True;
        rg_xmit_a46_i.b.rg_txpwc_comp_man_sel = rg_xmit_a46.b.rg_txpwc_comp_man_sel;
        rg_xmit_a46_i.b.rg_txpwc_comp_man = rg_xmit_a46.b.rg_txpwc_comp_man;
    }
    rg_xmit_a46.b.rg_txpwc_comp_man_sel = 0x1;
    rg_xmit_a46.b.rg_txpwc_comp_man = 0x20;
    fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);

    rg_xmit_a106.data = fi_ahb_read(RG_XMIT_A106);
    if(!RG_XMIT_A106_saved) {
        RG_XMIT_A106_saved = True;
        rg_xmit_a106_i.b.rg_tx_atten_factor = rg_xmit_a106.b.rg_tx_atten_factor;
    }
    rg_xmit_a106.b.rg_tx_atten_factor = 0x08;
    fi_ahb_write(RG_XMIT_A106, rg_xmit_a106.data);

    rg_esti_a11.data = fi_ahb_read(RG_ESTI_A11);
    if(!RG_ESTI_A11_saved) {
        RG_ESTI_A11_saved = True;
        rg_esti_a11_i.b.rg_rxirr_read_response = rg_esti_a11.b.rg_rxirr_read_response;
        rg_esti_a11_i.b.rg_rxirr_read_response_bypass = rg_esti_a11.b.rg_rxirr_read_response_bypass;
    }
    rg_esti_a11.b.rg_rxirr_read_response = 0x0;
    rg_esti_a11.b.rg_rxirr_read_response_bypass = 0x1;
    fi_ahb_write(RG_ESTI_A11, rg_esti_a11.data);

    rg_xmit_a3.data = fi_ahb_read(RG_XMIT_A3);
    if(!RG_XMIT_A3_saved) {
        RG_XMIT_A3_saved = True;
        rg_xmit_a3_i.b.rg_tg2_f_sel = rg_xmit_a3.b.rg_tg2_f_sel;
        rg_xmit_a3_i.b.rg_tg2_tone_gain = rg_xmit_a3.b.rg_tg2_tone_gain;
        rg_xmit_a3_i.b.rg_tg2_enable = rg_xmit_a3.b.rg_tg2_enable;
        rg_xmit_a3_i.b.rg_tg2_tone_40 = rg_xmit_a3.b.rg_tg2_tone_40;
        rg_xmit_a3_i.b.rg_tg2_tone_man_en = rg_xmit_a3.b.rg_tg2_tone_man_en;
    }
    rg_xmit_a3.b.rg_tg2_f_sel = 0x200000;
    rg_xmit_a3.b.rg_tg2_tone_gain = 0x0;
    rg_xmit_a3.b.rg_tg2_enable = 0x1;
    rg_xmit_a3.b.rg_tg2_tone_40 = 0x0;
    rg_xmit_a3.b.rg_tg2_tone_man_en = 0x0;
    fi_ahb_write(RG_XMIT_A3, rg_xmit_a3.data);

    rg_xmit_a4.data = fi_ahb_read(RG_XMIT_A4);
    if(!RG_XMIT_A4_saved) {
        RG_XMIT_A4_saved = True;
        rg_xmit_a4_i.b.rg_tx_signal_sel = rg_xmit_a4.b.rg_tx_signal_sel;
    }
    rg_xmit_a4.b.rg_tx_signal_sel = 0x1;                         // new_set_reg(0xe410, 0x00000001);//1: select single tone
    fi_ahb_write(RG_XMIT_A4, rg_xmit_a4.data);

    //fi_ahb_write(0xe5b8, 0x00000002);

    if(bw_idx == RF_BW_20M)
    {
        rg_core_a5.data = fi_ahb_read(RG_CORE_A5);
        rg_core_a5.b.rg_rx_bw_man = 0x0;                         //  new_set_reg(0xe014, 0x00110000);//set RX IRR BW to 40M
        rg_core_a5.b.rg_rx_bw_man_en = 0x1;                         //  new_set_reg(0xe014, 0x00110000);//set RX IRR BW to 40M
        fi_ahb_write(RG_CORE_A5, rg_core_a5.data);

        rg_esti_a30.data = fi_ahb_read(RG_ESTI_A30);
        rg_esti_a30.b.rg_irr_tone_freq_0 = 0x199999;                    // 4M  (ec38: 81999990)
        fi_ahb_write(RG_ESTI_A30, rg_esti_a30.data);

        rg_esti_a31.data = fi_ahb_read(RG_ESTI_A31);
        rg_esti_a31.b.rg_irr_tone_freq_1 = 0x399999;                    // 9M  (ec38: 83999990)
        fi_ahb_write(RG_ESTI_A31, rg_esti_a31.data);

        rg_esti_a32.data = fi_ahb_read(RG_ESTI_A32);
        rg_esti_a32.b.rg_irr_tone_freq_2 = 0x1c66666;                   //  -9M  (ec38: 9c666660)
        fi_ahb_write(RG_ESTI_A32, rg_esti_a32.data);

        rg_esti_a33.data = fi_ahb_read(RG_ESTI_A33);
        rg_esti_a33.b.rg_irr_tone_freq_3 = 0x1e66666;                   //  -4M  (ec38: 9e666660)
        fi_ahb_write(RG_ESTI_A33, rg_esti_a33.data);

        rg_esti_a34.data = fi_ahb_read(RG_ESTI_A34);
        rg_esti_a34.b.rg_irr_tone_freq_4 = 0x0;
        fi_ahb_write(RG_ESTI_A34, rg_esti_a34.data);

        rg_esti_a35.data = fi_ahb_read(RG_ESTI_A35);
        rg_esti_a35.b.rg_irr_tone_freq_5 = 0x0;
        fi_ahb_write(RG_ESTI_A35, rg_esti_a35.data);

        rg_esti_a36.data = fi_ahb_read(RG_ESTI_A36);
        rg_esti_a36.b.rg_irr_tone_freq_6 = 0x0;
        fi_ahb_write(RG_ESTI_A36, rg_esti_a36.data);

        rg_esti_a37.data = fi_ahb_read(RG_ESTI_A37);
        rg_esti_a37.b.rg_irr_tone_freq_7 = 0x0;
        fi_ahb_write(RG_ESTI_A37, rg_esti_a37.data);

        rg_esti_a38.data = fi_ahb_read(RG_ESTI_A38);
        rg_esti_a38.b.rg_irr_interp_coef_up0 = 0xb42f;
        rg_esti_a38.b.rg_irr_interp_coef_up1 = 0x2868;
        fi_ahb_write(RG_ESTI_A38, rg_esti_a38.data);

        rg_esti_a39.data = fi_ahb_read(RG_ESTI_A39);
        rg_esti_a39.b.rg_irr_interp_coef_up2 = 0x0;
        rg_esti_a39.b.rg_irr_interp_coef_up3 = 0x0;
        fi_ahb_write(RG_ESTI_A39, rg_esti_a39.data);

        rg_esti_a40.data = fi_ahb_read(RG_ESTI_A40);
        rg_esti_a40.b.rg_irr_interp_coef_up4 = 0x4fd1;
        rg_esti_a40.b.rg_irr_interp_coef_up5 = 0xd798;
        fi_ahb_write(RG_ESTI_A40, rg_esti_a40.data);

        rg_esti_a41.data = fi_ahb_read(RG_ESTI_A41);
        rg_esti_a41.b.rg_irr_interp_coef_up6 = 0x0;
        rg_esti_a41.b.rg_irr_interp_coef_up7 = 0x0;
        fi_ahb_write(RG_ESTI_A41, rg_esti_a41.data);

        rg_esti_a42.data = fi_ahb_read(RG_ESTI_A42);
        rg_esti_a42.b.rg_irr_interp_coef_up8 = 0x0;
        rg_esti_a42.b.rg_irr_interp_coef_up9 = 0x0;
        fi_ahb_write(RG_ESTI_A42, rg_esti_a42.data);

        rg_esti_a43.data = fi_ahb_read(RG_ESTI_A43);
        rg_esti_a43.b.rg_irr_interp_coef_upa = 0x0;
        rg_esti_a43.b.rg_irr_interp_coef_upb = 0x0;
        fi_ahb_write(RG_ESTI_A43, rg_esti_a43.data);

        rg_esti_a44.data = fi_ahb_read(RG_ESTI_A44);
        rg_esti_a44.b.rg_irr_interp_coef_upc = 0x0;
        rg_esti_a44.b.rg_irr_interp_coef_upd = 0x0;
        fi_ahb_write(RG_ESTI_A44, rg_esti_a44.data);

        rg_esti_a45.data = fi_ahb_read(RG_ESTI_A45);
        rg_esti_a45.b.rg_irr_interp_coef_upe = 0x0;
        rg_esti_a45.b.rg_irr_interp_coef_upf = 0x0;
        fi_ahb_write(RG_ESTI_A45, rg_esti_a45.data);

        rg_esti_a46.data = fi_ahb_read(RG_ESTI_A46);
        rg_esti_a46.b.rg_irr_interp_coef_low0 = 0x4fd1;
        rg_esti_a46.b.rg_irr_interp_coef_low1 = 0xd798;
        fi_ahb_write(RG_ESTI_A46, rg_esti_a46.data);

        rg_esti_a47.data = fi_ahb_read(RG_ESTI_A47);
        rg_esti_a47.b.rg_irr_interp_coef_low2 = 0x0;
        rg_esti_a47.b.rg_irr_interp_coef_low3 = 0x0;
        fi_ahb_write(RG_ESTI_A47, rg_esti_a47.data);

        rg_esti_a48.data = fi_ahb_read(RG_ESTI_A48);
        rg_esti_a48.b.rg_irr_interp_coef_low4 = 0xb42f;
        rg_esti_a48.b.rg_irr_interp_coef_low5 = 0x2868;
        fi_ahb_write(RG_ESTI_A48, rg_esti_a48.data);

        rg_esti_a49.data = fi_ahb_read(RG_ESTI_A49);
        rg_esti_a49.b.rg_irr_interp_coef_low6 = 0x0;
        rg_esti_a49.b.rg_irr_interp_coef_low7 = 0x0;
        fi_ahb_write(RG_ESTI_A49, rg_esti_a49.data);

        rg_esti_a50.data = fi_ahb_read(RG_ESTI_A50);
        rg_esti_a50.b.rg_irr_interp_coef_low8 = 0x0;
        rg_esti_a50.b.rg_irr_interp_coef_low9 = 0x0;
        fi_ahb_write(RG_ESTI_A50, rg_esti_a50.data);

        rg_esti_a51.data = fi_ahb_read(RG_ESTI_A51);
        rg_esti_a51.b.rg_irr_interp_coef_lowa = 0x0;
        rg_esti_a51.b.rg_irr_interp_coef_lowb = 0x0;
        fi_ahb_write(RG_ESTI_A51, rg_esti_a51.data);

        rg_esti_a52.data = fi_ahb_read(RG_ESTI_A52);
        rg_esti_a52.b.rg_irr_interp_coef_lowc = 0x0;
        rg_esti_a52.b.rg_irr_interp_coef_lowd = 0x0;
        fi_ahb_write(RG_ESTI_A52, rg_esti_a52.data);

        rg_esti_a53.data = fi_ahb_read(RG_ESTI_A53);
        rg_esti_a53.b.rg_irr_interp_coef_lowe = 0x0;
        rg_esti_a53.b.rg_irr_interp_coef_lowf = 0x0;
        fi_ahb_write(RG_ESTI_A53, rg_esti_a53.data);
    }
    else if(bw_idx == RF_BW_40M)
    {
        rg_core_a5.data = fi_ahb_read(RG_CORE_A5);
        rg_core_a5.b.rg_rx_bw_man = 0x1;                         //  new_set_reg(0xe014, 0x00110000);//set RX IRR BW to 40M
        rg_core_a5.b.rg_rx_bw_man_en = 0x1;                         //  new_set_reg(0xe014, 0x00110000);//set RX IRR BW to 40M
        fi_ahb_write(RG_CORE_A5, rg_core_a5.data);

        rg_esti_a30.data = fi_ahb_read(RG_ESTI_A30);
        rg_esti_a30.b.rg_irr_tone_freq_0 = 0x266666;                    // //  6M  (ec38: 82666660)
        fi_ahb_write(RG_ESTI_A30, rg_esti_a30.data);

        rg_esti_a31.data = fi_ahb_read(RG_ESTI_A31);
        rg_esti_a31.b.rg_irr_tone_freq_1 = 0x733333;                    // // 18M  (ec38: 87333330)
        fi_ahb_write(RG_ESTI_A31, rg_esti_a31.data);

        rg_esti_a32.data = fi_ahb_read(RG_ESTI_A32);
        rg_esti_a32.b.rg_irr_tone_freq_2 = 0x18ccccc;                   // //-18M  (ec38: 98ccccc0)
        fi_ahb_write(RG_ESTI_A32, rg_esti_a32.data);

        rg_esti_a33.data = fi_ahb_read(RG_ESTI_A33);
        rg_esti_a33.b.rg_irr_tone_freq_3 = 0x1d99999;                   // // -6M  (ec38: 9d999990)
        fi_ahb_write(RG_ESTI_A33, rg_esti_a33.data);

        rg_esti_a34.data = fi_ahb_read(RG_ESTI_A34);
        rg_esti_a34.b.rg_irr_tone_freq_4 = 0x0;
        fi_ahb_write(RG_ESTI_A34, rg_esti_a34.data);

        rg_esti_a35.data = fi_ahb_read(RG_ESTI_A35);
        rg_esti_a35.b.rg_irr_tone_freq_5 = 0x0;
        fi_ahb_write(RG_ESTI_A35, rg_esti_a35.data);

        rg_esti_a36.data = fi_ahb_read(RG_ESTI_A36);
        rg_esti_a36.b.rg_irr_tone_freq_6 = 0x0;
        fi_ahb_write(RG_ESTI_A36, rg_esti_a36.data);

        rg_esti_a37.data = fi_ahb_read(RG_ESTI_A37);
        rg_esti_a37.b.rg_irr_tone_freq_7 = 0x0;
        fi_ahb_write(RG_ESTI_A37, rg_esti_a37.data);

        rg_esti_a38.data = fi_ahb_read(RG_ESTI_A38);
        rg_esti_a38.b.rg_irr_interp_coef_up0 = 0xf1a6;
        rg_esti_a38.b.rg_irr_interp_coef_up1 = 0x970;
        fi_ahb_write(RG_ESTI_A38, rg_esti_a38.data);

        rg_esti_a39.data = fi_ahb_read(RG_ESTI_A39);
        rg_esti_a39.b.rg_irr_interp_coef_up2 = 0x0;
        rg_esti_a39.b.rg_irr_interp_coef_up3 = 0x0;
        fi_ahb_write(RG_ESTI_A39, rg_esti_a39.data);

        rg_esti_a40.data = fi_ahb_read(RG_ESTI_A40);
        rg_esti_a40.b.rg_irr_interp_coef_up4 = 0x125a;
        rg_esti_a40.b.rg_irr_interp_coef_up5 = 0xf690;
        fi_ahb_write(RG_ESTI_A40, rg_esti_a40.data);

        rg_esti_a41.data = fi_ahb_read(RG_ESTI_A41);
        rg_esti_a41.b.rg_irr_interp_coef_up6 = 0x0;
        rg_esti_a41.b.rg_irr_interp_coef_up7 = 0x0;
        fi_ahb_write(RG_ESTI_A41, rg_esti_a41.data);

        rg_esti_a42.data = fi_ahb_read(RG_ESTI_A42);
        rg_esti_a42.b.rg_irr_interp_coef_up8 = 0x0;
        rg_esti_a42.b.rg_irr_interp_coef_up9 = 0x0;
        fi_ahb_write(RG_ESTI_A42, rg_esti_a42.data);

        rg_esti_a43.data = fi_ahb_read(RG_ESTI_A43);
        rg_esti_a43.b.rg_irr_interp_coef_upa = 0x0;
        rg_esti_a43.b.rg_irr_interp_coef_upb = 0x0;
        fi_ahb_write(RG_ESTI_A43, rg_esti_a43.data);

        rg_esti_a44.data = fi_ahb_read(RG_ESTI_A44);
        rg_esti_a44.b.rg_irr_interp_coef_upc = 0x0;
        rg_esti_a44.b.rg_irr_interp_coef_upd = 0x0;
        fi_ahb_write(RG_ESTI_A44, rg_esti_a44.data);

        rg_esti_a45.data = fi_ahb_read(RG_ESTI_A45);
        rg_esti_a45.b.rg_irr_interp_coef_upe = 0x0;
        rg_esti_a45.b.rg_irr_interp_coef_upf = 0x0;
        fi_ahb_write(RG_ESTI_A45, rg_esti_a45.data);

        rg_esti_a46.data = fi_ahb_read(RG_ESTI_A46);
        rg_esti_a46.b.rg_irr_interp_coef_low0 = 0x125a;
        rg_esti_a46.b.rg_irr_interp_coef_low1 = 0xf690;
        fi_ahb_write(RG_ESTI_A46, rg_esti_a46.data);

        rg_esti_a47.data = fi_ahb_read(RG_ESTI_A47);
        rg_esti_a47.b.rg_irr_interp_coef_low2 = 0x0;
        rg_esti_a47.b.rg_irr_interp_coef_low3 = 0x0;
        fi_ahb_write(RG_ESTI_A47, rg_esti_a47.data);

        rg_esti_a48.data = fi_ahb_read(RG_ESTI_A48);
        rg_esti_a48.b.rg_irr_interp_coef_low4 = 0xf1a6;
        rg_esti_a48.b.rg_irr_interp_coef_low5 = 0x970;
        fi_ahb_write(RG_ESTI_A48, rg_esti_a48.data);

        rg_esti_a49.data = fi_ahb_read(RG_ESTI_A49);
        rg_esti_a49.b.rg_irr_interp_coef_low6 = 0x0;
        rg_esti_a49.b.rg_irr_interp_coef_low7 = 0x0;
        fi_ahb_write(RG_ESTI_A49, rg_esti_a49.data);

        rg_esti_a50.data = fi_ahb_read(RG_ESTI_A50);
        rg_esti_a50.b.rg_irr_interp_coef_low8 = 0x0;
        rg_esti_a50.b.rg_irr_interp_coef_low9 = 0x0;
        fi_ahb_write(RG_ESTI_A50, rg_esti_a50.data);

        rg_esti_a51.data = fi_ahb_read(RG_ESTI_A51);
        rg_esti_a51.b.rg_irr_interp_coef_lowa = 0x0;
        rg_esti_a51.b.rg_irr_interp_coef_lowb = 0x0;
        fi_ahb_write(RG_ESTI_A51, rg_esti_a51.data);

        rg_esti_a52.data = fi_ahb_read(RG_ESTI_A52);
        rg_esti_a52.b.rg_irr_interp_coef_lowc = 0x0;
        rg_esti_a52.b.rg_irr_interp_coef_lowd = 0x0;
        fi_ahb_write(RG_ESTI_A52, rg_esti_a52.data);

        rg_esti_a53.data = fi_ahb_read(RG_ESTI_A53);
        rg_esti_a53.b.rg_irr_interp_coef_lowe = 0x0;
        rg_esti_a53.b.rg_irr_interp_coef_lowf = 0x0;
        fi_ahb_write(RG_ESTI_A53, rg_esti_a53.data);
    }
    else if(bw_idx == RF_BW_80M)
    {
        rg_core_a5.data = fi_ahb_read(RG_CORE_A5);
        rg_core_a5.b.rg_rx_bw_man = 0x2;                         //  new_set_reg(0xe014, 0x00110000);//set RX IRR BW to 40M
        rg_core_a5.b.rg_rx_bw_man_en = 0x1;                         //  new_set_reg(0xe014, 0x00110000);//set RX IRR BW to 40M
        fi_ahb_write(RG_CORE_A5, rg_core_a5.data);

        rg_esti_a30.data = fi_ahb_read(RG_ESTI_A30);
        rg_esti_a30.b.rg_irr_tone_freq_0 = 0x4ccccc;                    // //  12M (ec38 : 84ccccc0)
        fi_ahb_write(RG_ESTI_A30, rg_esti_a30.data);

        rg_esti_a31.data = fi_ahb_read(RG_ESTI_A31);
        rg_esti_a31.b.rg_irr_tone_freq_1 = 0xa66666;                    // //  26M (ec38 : 8a666660)
        fi_ahb_write(RG_ESTI_A31, rg_esti_a31.data);

        rg_esti_a32.data = fi_ahb_read(RG_ESTI_A32);
        rg_esti_a32.b.rg_irr_tone_freq_2 = 0xd33333;                    // //  33M (ec38 : 8d333330)
        fi_ahb_write(RG_ESTI_A32, rg_esti_a32.data);

        rg_esti_a33.data = fi_ahb_read(RG_ESTI_A33);
        rg_esti_a33.b.rg_irr_tone_freq_3 = 0xf00000;                    // //  37.5M
        fi_ahb_write(RG_ESTI_A33, rg_esti_a33.data);

        rg_esti_a34.data = fi_ahb_read(RG_ESTI_A34);
        rg_esti_a34.b.rg_irr_tone_freq_4 = 0x1100000;                   // //-37.5M 
        fi_ahb_write(RG_ESTI_A34, rg_esti_a34.data);

        rg_esti_a35.data = fi_ahb_read(RG_ESTI_A35);
        rg_esti_a35.b.rg_irr_tone_freq_5 = 0x12ccccc;                   // //-33M (ec38 : 92ccccc0)
        fi_ahb_write(RG_ESTI_A35, rg_esti_a35.data);

        rg_esti_a36.data = fi_ahb_read(RG_ESTI_A36);
        rg_esti_a36.b.rg_irr_tone_freq_6 = 0x1599999;                   // //-26M (ec38 : 95999990)
        fi_ahb_write(RG_ESTI_A36, rg_esti_a36.data);

        rg_esti_a37.data = fi_ahb_read(RG_ESTI_A37);
        rg_esti_a37.b.rg_irr_tone_freq_7 = 0x1b33333;                   // //-12M  (ec38 : 9b333330)
        fi_ahb_write(RG_ESTI_A37, rg_esti_a37.data);

        rg_esti_a38.data = fi_ahb_read(RG_ESTI_A38);
        rg_esti_a38.b.rg_irr_interp_coef_up0 = 0xf5d7;
        rg_esti_a38.b.rg_irr_interp_coef_up1 = 0x0ab6;
        fi_ahb_write(RG_ESTI_A38, rg_esti_a38.data);

        rg_esti_a39.data = fi_ahb_read(RG_ESTI_A39);
        rg_esti_a39.b.rg_irr_interp_coef_up2 = 0xfb13;
        rg_esti_a39.b.rg_irr_interp_coef_up3 = 0x02c3;
        fi_ahb_write(RG_ESTI_A39, rg_esti_a39.data);

        rg_esti_a40.data = fi_ahb_read(RG_ESTI_A40);
        rg_esti_a40.b.rg_irr_interp_coef_up4 = 0x429b;
        rg_esti_a40.b.rg_irr_interp_coef_up5 = 0xc7af;
        fi_ahb_write(RG_ESTI_A40, rg_esti_a40.data);

        rg_esti_a41.data = fi_ahb_read(RG_ESTI_A41);
        rg_esti_a41.b.rg_irr_interp_coef_up6 = 0x2019;
        rg_esti_a41.b.rg_irr_interp_coef_up7 = 0xf345;
        fi_ahb_write(RG_ESTI_A41, rg_esti_a41.data);

        rg_esti_a42.data = fi_ahb_read(RG_ESTI_A42);
        rg_esti_a42.b.rg_irr_interp_coef_up8 = 0x892b;
        rg_esti_a42.b.rg_irr_interp_coef_up9 = 0x6465;
        fi_ahb_write(RG_ESTI_A42, rg_esti_a42.data);

        rg_esti_a43.data = fi_ahb_read(RG_ESTI_A43);
        rg_esti_a43.b.rg_irr_interp_coef_upa = 0xc7f8;
        rg_esti_a43.b.rg_irr_interp_coef_upb = 0x1289;
        fi_ahb_write(RG_ESTI_A43, rg_esti_a43.data);

        rg_esti_a44.data = fi_ahb_read(RG_ESTI_A44);
        rg_esti_a44.b.rg_irr_interp_coef_upc = 0x4263;
        rg_esti_a44.b.rg_irr_interp_coef_upd = 0xc936;
        fi_ahb_write(RG_ESTI_A44, rg_esti_a44.data);

        rg_esti_a45.data = fi_ahb_read(RG_ESTI_A45);
        rg_esti_a45.b.rg_irr_interp_coef_upe = 0x1cdd;
        rg_esti_a45.b.rg_irr_interp_coef_upf = 0xf76f;
        fi_ahb_write(RG_ESTI_A45, rg_esti_a45.data);

        rg_esti_a46.data = fi_ahb_read(RG_ESTI_A46);
        rg_esti_a46.b.rg_irr_interp_coef_low0 = 0x4263;
        rg_esti_a46.b.rg_irr_interp_coef_low1 = 0xc936;
        fi_ahb_write(RG_ESTI_A46, rg_esti_a46.data);

        rg_esti_a47.data = fi_ahb_read(RG_ESTI_A47);
        rg_esti_a47.b.rg_irr_interp_coef_low2 = 0x1cdd;
        rg_esti_a47.b.rg_irr_interp_coef_low3 = 0xf76f;
        fi_ahb_write(RG_ESTI_A47, rg_esti_a47.data);

        rg_esti_a48.data = fi_ahb_read(RG_ESTI_A48);
        rg_esti_a48.b.rg_irr_interp_coef_low4 = 0x892b;
        rg_esti_a48.b.rg_irr_interp_coef_low5 = 0x6465;
        fi_ahb_write(RG_ESTI_A48, rg_esti_a48.data);

        rg_esti_a49.data = fi_ahb_read(RG_ESTI_A49);
        rg_esti_a49.b.rg_irr_interp_coef_low6 = 0xc7f8;
        rg_esti_a49.b.rg_irr_interp_coef_low7 = 0x1289;
        fi_ahb_write(RG_ESTI_A49, rg_esti_a49.data);

        rg_esti_a50.data = fi_ahb_read(RG_ESTI_A50);
        rg_esti_a50.b.rg_irr_interp_coef_low8 = 0x429b;
        rg_esti_a50.b.rg_irr_interp_coef_low9 = 0xc7af;
        fi_ahb_write(RG_ESTI_A50, rg_esti_a50.data);

        rg_esti_a51.data = fi_ahb_read(RG_ESTI_A51);
        rg_esti_a51.b.rg_irr_interp_coef_lowa = 0x2019;
        rg_esti_a51.b.rg_irr_interp_coef_lowb = 0xf345;
        fi_ahb_write(RG_ESTI_A51, rg_esti_a51.data);

        rg_esti_a52.data = fi_ahb_read(RG_ESTI_A52);
        rg_esti_a52.b.rg_irr_interp_coef_lowc = 0xf5d7;
        rg_esti_a52.b.rg_irr_interp_coef_lowd = 0x0ab6;
        fi_ahb_write(RG_ESTI_A52, rg_esti_a52.data);

        rg_esti_a53.data = fi_ahb_read(RG_ESTI_A53);
        rg_esti_a53.b.rg_irr_interp_coef_lowe = 0xfb13;
        rg_esti_a53.b.rg_irr_interp_coef_lowf = 0x02c3;
        fi_ahb_write(RG_ESTI_A53, rg_esti_a53.data);
    } //if(bw_idx == RF_BW_20M)

    t9026_cali_mode_access(cali_mode);

    rg_esti_a11.data = fi_ahb_read(RG_ESTI_A11);
    rx_irr_rdy = rg_esti_a11.b.ro_rxirr_ready;
    while(unlock_cnt < TIME_OUT_CNT)
    {
        if(rx_irr_rdy == 0)
        {
            //g_fw_funcs.system_wait(1);

            rg_esti_a11.data = fi_ahb_read(RG_ESTI_A11);
            rx_irr_rdy = rg_esti_a11.b.ro_rxirr_ready;
            RF_DPRINTF(RF_DEBUG_INFO, "unlock_cnt:%d,RX IRR NOT READY...\n", unlock_cnt);
            unlock_cnt = unlock_cnt + 1;
        }
        else
        {
            RF_DPRINTF(RF_DEBUG_INFO, "RX IRR READY\n");
            break;
        } //if(rx_irr_rdy == 0)
    } // while(unlock_cnt < TIME_OUT_CNT)

    rg_esti_a3.data = fi_ahb_read(RG_ESTI_A3);
    coeff_eup0 = rg_esti_a3.b.ro_rxirr_eup_0;
    coeff_eup1 = rg_esti_a3.b.ro_rxirr_eup_1;

    rg_esti_a4.data = fi_ahb_read(RG_ESTI_A4);
    coeff_eup2 = rg_esti_a4.b.ro_rxirr_eup_2;
    coeff_eup3 = rg_esti_a4.b.ro_rxirr_eup_3;

    rg_esti_a5.data = fi_ahb_read(RG_ESTI_A5);
    coeff_pup0 = rg_esti_a5.b.ro_rxirr_pup_0;
    coeff_pup1 = rg_esti_a5.b.ro_rxirr_pup_1;

    rg_esti_a6.data = fi_ahb_read(RG_ESTI_A6);
    coeff_pup2 = rg_esti_a6.b.ro_rxirr_pup_2;
    coeff_pup3 = rg_esti_a6.b.ro_rxirr_pup_3;

    rg_esti_a7.data = fi_ahb_read(RG_ESTI_A7);
    coeff_elow0 = rg_esti_a7.b.ro_rxirr_elow_0;
    coeff_elow1 = rg_esti_a7.b.ro_rxirr_elow_1;

    rg_esti_a8.data = fi_ahb_read(RG_ESTI_A8);
    coeff_elow2 = rg_esti_a8.b.ro_rxirr_elow_2;
    coeff_elow3 = rg_esti_a8.b.ro_rxirr_elow_3;

    rg_esti_a9.data = fi_ahb_read(RG_ESTI_A9);
    coeff_plow0 = rg_esti_a9.b.ro_rxirr_plow_0;
    coeff_plow1 = rg_esti_a9.b.ro_rxirr_plow_1;

    rg_esti_a10.data = fi_ahb_read(RG_ESTI_A10);
    coeff_plow2 = rg_esti_a10.b.ro_rxirr_plow_2;
    coeff_plow3 = rg_esti_a10.b.ro_rxirr_plow_3;

#if 0
    if(bw_idx == RF_BW_20M)
    {
        fi_ahb_write(PHY_PRIMARY_CHANNEL, 0x80000000);//80M:a1; 40M;90; 20M:80
    }
    else if(bw_idx == RF_BW_40M)
    {
        fi_ahb_write(PHY_PRIMARY_CHANNEL,  0x91000000);//80M:a1; 40M;90; 20M:80
    }
    else if(bw_idx == RF_BW_80M)
    {
        fi_ahb_write(PHY_PRIMARY_CHANNEL, 0xa1000000);//80M:a1; 40M;90; 20M:80
    } //if(bw_idx == RF_BW_20M)
#endif
    wf5g_rxirr_result->cal_rx_irr.coeff_eup[0] = coeff_eup0;
    wf5g_rxirr_result->cal_rx_irr.coeff_eup[1] = coeff_eup1;
    wf5g_rxirr_result->cal_rx_irr.coeff_eup[2] = coeff_eup2;
    wf5g_rxirr_result->cal_rx_irr.coeff_eup[3] = coeff_eup3;
    wf5g_rxirr_result->cal_rx_irr.coeff_pup[0] = coeff_pup0;
    wf5g_rxirr_result->cal_rx_irr.coeff_pup[1] = coeff_pup1;
    wf5g_rxirr_result->cal_rx_irr.coeff_pup[2] = coeff_pup2;
    wf5g_rxirr_result->cal_rx_irr.coeff_pup[3] = coeff_pup3;
    wf5g_rxirr_result->cal_rx_irr.coeff_elow[0] = coeff_elow0;
    wf5g_rxirr_result->cal_rx_irr.coeff_elow[1] = coeff_elow1;
    wf5g_rxirr_result->cal_rx_irr.coeff_elow[2] = coeff_elow2;
    wf5g_rxirr_result->cal_rx_irr.coeff_elow[3] = coeff_elow3;
    wf5g_rxirr_result->cal_rx_irr.coeff_plow[0] = coeff_plow0;
    wf5g_rxirr_result->cal_rx_irr.coeff_plow[1] = coeff_plow1;
    wf5g_rxirr_result->cal_rx_irr.coeff_plow[2] = coeff_plow2;
    wf5g_rxirr_result->cal_rx_irr.coeff_plow[3] = coeff_plow3;
    //RF_DPRINTF(RF_DEBUG_RESULT, "unlock_cnt:%d\n", unlock_cnt);
    RF_DPRINTF(RF_DEBUG_RESULT, "coeff_eup0:%x\ncoeff_eup1:%x\ncoeff_eup2:%x\ncoeff_eup3:%x\n", coeff_eup0, coeff_eup1, coeff_eup2, coeff_eup3);
    RF_DPRINTF(RF_DEBUG_RESULT, "coeff_pup0:%x\ncoeff_pup1:%x\ncoeff_pup2:%x\ncoeff_pup3:%x\n", coeff_pup0, coeff_pup1, coeff_pup2, coeff_pup3);
    RF_DPRINTF(RF_DEBUG_RESULT, "coeff_elow0:%x\ncoeff_elow1:%x\ncoeff_elow2:%x\ncoeff_elow3:%x\n", coeff_elow0, coeff_elow1, coeff_elow2, coeff_elow3);
    RF_DPRINTF(RF_DEBUG_RESULT, "coeff_plow0:%x\ncoeff_plow1:%x\ncoeff_plow2:%x\ncoeff_plow3:%x\n", coeff_plow0, coeff_plow1, coeff_plow2, coeff_plow3);
    //open rx irr compensation
    rg_recv_a11.data = fi_ahb_read(RG_RECV_A11);
    rg_recv_a11.b.rg_rxirr_man_mode = 0x0;
    rg_recv_a11.b.rg_rxirr_bypass = 0x0;
    fi_ahb_write(RG_RECV_A11, rg_recv_a11.data);

    if(check_flg == 1)
    {
        t9026_wf5g_check_loop_rx_irr(wf5g_rxirr_result, bw_idx);
    }

    t9026_cali_mode_exit();
    if (for_dpd_flg == 0)
    {
        // Revert the original value before calibration back
        rg_top_a15.data = rf_read_register(RG_TOP_A15);
        rg_top_a15.b.rg_wf_adda_man_mode = rg_top_a15_i.b.rg_wf_adda_man_mode;
        rg_top_a15.b.rg_wf_dac_ampctrl_man = rg_top_a15_i.b.rg_wf_dac_ampctrl_man;
        rf_write_register(RG_TOP_A15, rg_top_a15.data);
        // Revert the original value before calibration back
        rg_top_a7.data = rf_read_register(RG_TOP_A7);
        rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RXB_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RXB_EN;
        rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX1_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RX1_EN;
        rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX2_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RX2_EN;
        rg_top_a7.b.RG_M2_WF_TRX_LF_LDO_RX_EN = rg_top_a7_i.b.RG_M2_WF_TRX_LF_LDO_RX_EN;
        rf_write_register(RG_TOP_A7, rg_top_a7.data);
        // Revert the original value before calibration back
        rg_sx_a25.data = rf_read_register(RG_SX_A25);
        rg_sx_a25.b.RG_M2_WF_SX_LOG5G_RXLO_EN = rg_sx_a25_i.b.RG_M2_WF_SX_LOG5G_RXLO_EN;
        rf_write_register(RG_SX_A25, rg_sx_a25.data);
        // Revert the original value before calibration back
        rg_tx_a2.data = rf_read_register(RG_TX_A2);
        rg_tx_a2.b.RG_WF5G_TX_RXIQCAL_EN = rg_tx_a2_i.b.RG_WF5G_TX_RXIQCAL_EN;
        rg_tx_a2.b.RG_WF5G_TX_MAN_MODE = rg_tx_a2_i.b.RG_WF5G_TX_MAN_MODE;
        rf_write_register(RG_TX_A2, rg_tx_a2.data);
        // Revert the original value before calibration back
        rg_tx_a3.data = rf_read_register(RG_TX_A3);
        rg_tx_a3.b.RG_M2_WF5G_TX_PA_EN = rg_tx_a3_i.b.RG_M2_WF5G_TX_PA_EN;
        rf_write_register(RG_TX_A3, rg_tx_a3.data);
        // Revert the original value before calibration back
        rg_rx_a5.data = rf_read_register(RG_RX_A5);
        rg_rx_a5.b.RG_M2_WF_RX_LNA_BIAS_EN = rg_rx_a5_i.b.RG_M2_WF_RX_LNA_BIAS_EN;
        rg_rx_a5.b.RG_M2_WF_RX_GM_BIAS_EN = rg_rx_a5_i.b.RG_M2_WF_RX_GM_BIAS_EN;
        rg_rx_a5.b.RG_M2_WF_RX_MXR_BIAS_EN = rg_rx_a5_i.b.RG_M2_WF_RX_MXR_BIAS_EN;
        rg_rx_a5.b.RG_M2_WF5G_RX_LNA_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_LNA_EN;
        rg_rx_a5.b.RG_M2_WF5G_RX_GM_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_GM_EN;
        rg_rx_a5.b.RG_M2_WF5G_RX_MXR_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_MXR_EN;
        rg_rx_a5.b.RG_M2_WF5G_RX_IRRCAL_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_IRRCAL_EN;
        rg_rx_a5.b.RG_M2_WF_RTX_ABB_RX_EN = rg_rx_a5_i.b.RG_M2_WF_RTX_ABB_RX_EN;
        rg_rx_a5.b.RG_M2_WF_RX_TIA_BIAS_EN = rg_rx_a5_i.b.RG_M2_WF_RX_TIA_BIAS_EN;
        rg_rx_a5.b.RG_M2_WF_RX_RSSIPGA_BIAS_EN = rg_rx_a5_i.b.RG_M2_WF_RX_RSSIPGA_BIAS_EN;
        rg_rx_a5.b.RG_M2_WF_RX_LPF_BIAS_EN = rg_rx_a5_i.b.RG_M2_WF_RX_LPF_BIAS_EN;
        rg_rx_a5.b.RG_M2_WF_RX_TIA_EN = rg_rx_a5_i.b.RG_M2_WF_RX_TIA_EN;
        rg_rx_a5.b.RG_M2_WF_RX_RSSIPGA_EN = rg_rx_a5_i.b.RG_M2_WF_RX_RSSIPGA_EN;
        rg_rx_a5.b.RG_M2_WF_RX_LPF_EN = rg_rx_a5_i.b.RG_M2_WF_RX_LPF_EN;
        rg_rx_a5.b.RG_M2_WF_RX_LPF_I_DCOC_EN = rg_rx_a5_i.b.RG_M2_WF_RX_LPF_I_DCOC_EN;
        rg_rx_a5.b.RG_M2_WF_RX_LPF_Q_DCOC_EN = rg_rx_a5_i.b.RG_M2_WF_RX_LPF_Q_DCOC_EN;
        rf_write_register(RG_RX_A5, rg_rx_a5.data);
        // Revert the original value before calibration back
        rg_top_a19.data = rf_read_register(RG_TOP_A19);
        rg_top_a19.b.rg_m2_wf_radc_en = rg_top_a19_i.b.rg_m2_wf_radc_en;
        rg_top_a19.b.rg_m2_wf_wadc_enable_i = rg_top_a19_i.b.rg_m2_wf_wadc_enable_i;
        rg_top_a19.b.rg_m2_wf_wadc_enable_q = rg_top_a19_i.b.rg_m2_wf_wadc_enable_q;
        rf_write_register(RG_TOP_A19, rg_top_a19.data);
        // Revert the original value before calibration back
        rg_rx_a11.data = rf_read_register(RG_RX_A11);
        rg_rx_a11.b.RG_WF_RXFE_GAIN_G2 = rg_rx_a11_i.b.RG_WF_RXFE_GAIN_G2;
        rf_write_register(RG_RX_A11, rg_rx_a11.data);
        rg_rx_a2.data = rf_read_register(RG_RX_A2);
        rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE = rg_rx_a2_i.b.RG_WF_RX_BBBM_MAN_MODE;
        rg_rx_a2.b.RG_WF_RX_GAIN_MAN = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN;
        rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE;
        rg_rx_a2.b.RG_WF_RX_BW_SEL = rg_rx_a2_i.b.RG_WF_RX_BW_SEL;
        rg_rx_a2.b.RG_WF_RX_ADCFS_SEL = rg_rx_a2_i.b.RG_WF_RX_ADCFS_SEL;
        rf_write_register(RG_RX_A2, rg_rx_a2.data);
    }
    // Revert the original value before calibration back
    rg_xmit_a56.data = fi_ahb_read(RG_XMIT_A56);
    rg_xmit_a56.b.rg_txdpd_comp_bypass = rg_xmit_a56_i.b.rg_txdpd_comp_bypass;
    rg_xmit_a56.b.rg_txdpd_comp_bypass_man = rg_xmit_a56_i.b.rg_txdpd_comp_bypass_man;
    fi_ahb_write(RG_XMIT_A56, rg_xmit_a56.data);
    // Revert the original value before calibration back
    rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
    rg_xmit_a46.b.rg_txpwc_comp_man_sel = rg_xmit_a46_i.b.rg_txpwc_comp_man_sel;
    rg_xmit_a46.b.rg_txpwc_comp_man = rg_xmit_a46_i.b.rg_txpwc_comp_man;
    fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);
    // Revert the original value before calibration back
    rg_xmit_a106.data = fi_ahb_read(RG_XMIT_A106);
    rg_xmit_a106.b.rg_tx_atten_factor = rg_xmit_a106_i.b.rg_tx_atten_factor;
    fi_ahb_write(RG_XMIT_A106, rg_xmit_a106.data);
    // Revert the original value before calibration back
    rg_esti_a11.data = fi_ahb_read(RG_ESTI_A11);
    rg_esti_a11.b.rg_rxirr_read_response = rg_esti_a11_i.b.rg_rxirr_read_response;
    rg_esti_a11.b.rg_rxirr_read_response_bypass = rg_esti_a11_i.b.rg_rxirr_read_response_bypass;
    fi_ahb_write(RG_ESTI_A11, rg_esti_a11.data);
    // Revert the original value before calibration back
    rg_xmit_a3.data = fi_ahb_read(RG_XMIT_A3);
    rg_xmit_a3.b.rg_tg2_f_sel = rg_xmit_a3_i.b.rg_tg2_f_sel;
    rg_xmit_a3.b.rg_tg2_tone_gain = rg_xmit_a3_i.b.rg_tg2_tone_gain;
    rg_xmit_a3.b.rg_tg2_enable = rg_xmit_a3_i.b.rg_tg2_enable;
    rg_xmit_a3.b.rg_tg2_tone_40 = rg_xmit_a3_i.b.rg_tg2_tone_40;
    rg_xmit_a3.b.rg_tg2_tone_man_en = rg_xmit_a3_i.b.rg_tg2_tone_man_en;
    fi_ahb_write(RG_XMIT_A3, rg_xmit_a3.data);
    // Revert the original value before calibration back
    rg_xmit_a4.data = fi_ahb_read(RG_XMIT_A4);
    rg_xmit_a4.b.rg_tx_signal_sel = rg_xmit_a4_i.b.rg_tx_signal_sel;
    fi_ahb_write(RG_XMIT_A4, rg_xmit_a4.data);

    rg_recv_a2.data = fi_ahb_read(RG_RECV_A2);
    rg_recv_a2.b.rg_rxi_dcoft = 0;
    rg_recv_a2.b.rg_rxq_dcoft = 0;
    fi_ahb_write(RG_RECV_A2, rg_recv_a2.data);
}

#endif // RF_CALI_TEST

