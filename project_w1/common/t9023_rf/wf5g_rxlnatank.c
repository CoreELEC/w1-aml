
#ifdef RF_CALI_TEST
#include "rf_calibration.h"
struct rx_lna_output wf5g_rxlnatank(struct wf5g_rxlnatank_result * wf5g_rxlnatank_result, bool manual, U8 channel)
{
    RG_TOP_A7_FIELD_T    rg_top_a7;
    RG_SX_A25_FIELD_T    rg_sx_a25;
    RG_TX_A2_FIELD_T    rg_tx_a2;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_RX_A5_FIELD_T    rg_rx_a5;
    RG_RX_A60_FIELD_T    rg_rx_a60;
    RG_XMIT_A3_FIELD_T    rg_xmit_a3;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4;
    RG_XMIT_A56_FIELD_T    rg_xmit_a56;
    RG_RECV_A11_FIELD_T    rg_recv_a11;
    RG_ESTI_A14_FIELD_T    rg_esti_a14;
    RG_ESTI_A64_FIELD_T    rg_esti_a64;
    RG_ESTI_A65_FIELD_T    rg_esti_a65;
    RG_XMIT_A106_FIELD_T    rg_xmit_a106;
    RG_ESTI_A18_FIELD_T    rg_esti_a18;
    RG_RX_A8_FIELD_T    rg_rx_a8;
    RG_RX_A7_FIELD_T    rg_rx_a7;
    RG_ESTI_A94_FIELD_T    rg_esti_a94;

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
    // Internal variables definitions
    bool    RG_RX_A5_saved = False;
    RG_RX_A5_FIELD_T    rg_rx_a5_i;
    // Internal variables definitions
    bool    RG_RX_A60_saved = False;
    RG_RX_A60_FIELD_T    rg_rx_a60_i;
    // Internal variables definitions
    bool    RG_XMIT_A3_saved = False;
    RG_XMIT_A3_FIELD_T    rg_xmit_a3_i;
    // Internal variables definitions
    bool    RG_XMIT_A4_saved = False;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4_i;
    // Internal variables definitions
    bool    RG_XMIT_A56_saved = False;
    RG_XMIT_A56_FIELD_T    rg_xmit_a56_i;
    // Internal variables definitions
    bool    RG_RECV_A11_saved = False;
    RG_RECV_A11_FIELD_T    rg_recv_a11_i;
    // Internal variables definitions
    bool    RG_ESTI_A14_saved = False;
    RG_ESTI_A14_FIELD_T    rg_esti_a14_i;

    struct rx_lna_output rx_lna_out;
    int i = 0;

    unsigned int code = 0;
    unsigned int code_out = 0;
    unsigned int pwr_out = 0;
    int max_i = 0;

    unsigned int rx_lna_ready = 0;
    unsigned int unlock_cnt = 0;
    unsigned int TIME_OUT_CNT = 300;
    unsigned int channel_idx = 0;

    rg_top_a7.data = rf_read_register(RG_TOP_A7);
    if(!RG_TOP_A7_saved) {
        RG_TOP_A7_saved = True;
        rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RX1_EN = rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX1_EN;
        rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RX2_EN = rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX2_EN;
        rg_top_a7_i.b.RG_M2_WF_TRX_LF_LDO_RX_EN = rg_top_a7.b.RG_M2_WF_TRX_LF_LDO_RX_EN;
    }
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX1_EN = 0x1;
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX2_EN = 0x1;
    rg_top_a7.b.RG_M2_WF_TRX_LF_LDO_RX_EN = 0x1;
    rf_write_register(RG_TOP_A7, rg_top_a7.data);

    rg_sx_a25.data = rf_read_register(RG_SX_A25);
    if(!RG_SX_A25_saved) {
        RG_SX_A25_saved = True;
        rg_sx_a25_i.b.RG_M2_WF_SX_LOG5G_RXLO_EN = rg_sx_a25.b.RG_M2_WF_SX_LOG5G_RXLO_EN;
    }
    rg_sx_a25.b.RG_M2_WF_SX_LOG5G_RXLO_EN = 0x1;
    rf_write_register(RG_SX_A25, rg_sx_a25.data);

    rg_tx_a2.data = rf_read_register(RG_TX_A2);
    if(!RG_TX_A2_saved) {
        RG_TX_A2_saved = True;
        rg_tx_a2_i.b.RG_WF5G_TXM_RX_IQ_IQ_CAL_EN = rg_tx_a2.b.RG_WF5G_TXM_RX_IQ_IQ_CAL_EN;
    }
    rg_tx_a2.b.RG_WF5G_TXM_RX_IQ_IQ_CAL_EN = 0x1;
    rf_write_register(RG_TX_A2, rg_tx_a2.data);

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    if(!RG_RX_A2_saved) {
        RG_RX_A2_saved = True;
        rg_rx_a2_i.b.RG_WF_RX_BBBM_MAN_MODE = rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE;
    }
    rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE = 0x1;                         // Use default 20MHz BW setting
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x25;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);

    rg_rx_a5.data = rf_read_register(RG_RX_A5);
    if(!RG_RX_A5_saved) {
        RG_RX_A5_saved = True;
        rg_rx_a5_i.b.RG_M2_WF5G_RX_FE_TOPBIAS_EN = rg_rx_a5.b.RG_M2_WF5G_RX_FE_TOPBIAS_EN;
        rg_rx_a5_i.b.RG_M2_WF5G_RX_LNA_BIAS_EN = rg_rx_a5.b.RG_M2_WF5G_RX_LNA_BIAS_EN;
        rg_rx_a5_i.b.RG_M2_WF5G_RX_GM_BIAS_EN = rg_rx_a5.b.RG_M2_WF5G_RX_GM_BIAS_EN;
        rg_rx_a5_i.b.RG_M2_WF5G_RX_MXR_BIAS_EN = rg_rx_a5.b.RG_M2_WF5G_RX_MXR_BIAS_EN;
        rg_rx_a5_i.b.RG_M2_WF5G_RX_LNA_TRSW_EN = rg_rx_a5.b.RG_M2_WF5G_RX_LNA_TRSW_EN;
        rg_rx_a5_i.b.RG_M2_WF5G_RX_LNA_EN = rg_rx_a5.b.RG_M2_WF5G_RX_LNA_EN;
        rg_rx_a5_i.b.RG_M2_WF5G_RX_GM_EN = rg_rx_a5.b.RG_M2_WF5G_RX_GM_EN;
        rg_rx_a5_i.b.RG_M2_WF5G_RX_MXR_EN = rg_rx_a5.b.RG_M2_WF5G_RX_MXR_EN;
        rg_rx_a5_i.b.RG_M2_WF5G_RX_LNA_CAL_EN = rg_rx_a5.b.RG_M2_WF5G_RX_LNA_CAL_EN;
        rg_rx_a5_i.b.RG_M2_WF5G_RX_IRRCAL_EN = rg_rx_a5.b.RG_M2_WF5G_RX_IRRCAL_EN;
        rg_rx_a5_i.b.RG_M2_WF_RTX_ABB_RX_EN = rg_rx_a5.b.RG_M2_WF_RTX_ABB_RX_EN;
        rg_rx_a5_i.b.RG_M2_WF_RX_TIA_BIAS_EN = rg_rx_a5.b.RG_M2_WF_RX_TIA_BIAS_EN;
        rg_rx_a5_i.b.RG_M2_WF_RX_RSSIPGA_BIAS_EN = rg_rx_a5.b.RG_M2_WF_RX_RSSIPGA_BIAS_EN;
        rg_rx_a5_i.b.RG_M2_WF_RX_LPF_BIAS_EN = rg_rx_a5.b.RG_M2_WF_RX_LPF_BIAS_EN;
        rg_rx_a5_i.b.RG_M2_WF_RX_TIA_EN = rg_rx_a5.b.RG_M2_WF_RX_TIA_EN;
        rg_rx_a5_i.b.RG_M2_WF_RX_RSSIPGA_EN = rg_rx_a5.b.RG_M2_WF_RX_RSSIPGA_EN;
        rg_rx_a5_i.b.RG_M2_WF_RX_LPF_EN = rg_rx_a5.b.RG_M2_WF_RX_LPF_EN;
        rg_rx_a5_i.b.RG_M2_WF_RX_LPF_I_DCOC_EN = rg_rx_a5.b.RG_M2_WF_RX_LPF_I_DCOC_EN;
        rg_rx_a5_i.b.RG_M2_WF_RX_LPF_Q_DCOC_EN = 0x1;
        rg_rx_a5_i.b.RG_M2_WF5G_TXLPBK2TIA_EN = rg_rx_a5.b.RG_M2_WF5G_TXLPBK2TIA_EN;
    }
    rg_rx_a5.b.RG_M2_WF5G_RX_FE_TOPBIAS_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF5G_RX_LNA_BIAS_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF5G_RX_GM_BIAS_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF5G_RX_MXR_BIAS_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF5G_RX_LNA_TRSW_EN = 0x0;
    rg_rx_a5.b.RG_M2_WF5G_RX_LNA_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF5G_RX_GM_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF5G_RX_MXR_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF5G_RX_LNA_CAL_EN = 0x1;
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
    rg_rx_a5.b.RG_M2_WF5G_TXLPBK2TIA_EN = 0x1;
    rf_write_register(RG_RX_A5, rg_rx_a5.data);

    rg_rx_a60.data = rf_read_register(RG_RX_A60);
    if(!RG_RX_A60_saved) {
        RG_RX_A60_saved = True;
        rg_rx_a60_i.b.RG_WF_TX_LPF_GAIN = rg_rx_a60.b.RG_WF_TX_LPF_GAIN;
    }
    rg_rx_a60.b.RG_WF_TX_LPF_GAIN = 0x0;
    rf_write_register(RG_RX_A60, rg_rx_a60.data);

     fi_ahb_write(PHY_TX_LPF_BYPASS, 0x1);

    rg_xmit_a3.data = fi_ahb_read(RG_XMIT_A3);
    if(!RG_XMIT_A3_saved) {
        RG_XMIT_A3_saved = True;
        rg_xmit_a3_i.b.rg_tg2_f_sel = rg_xmit_a3.b.rg_tg2_f_sel;
        rg_xmit_a3_i.b.rg_tg2_enable = rg_xmit_a3.b.rg_tg2_enable;
        rg_xmit_a3_i.b.rg_tg2_tone_man_en = rg_xmit_a3.b.rg_tg2_tone_man_en;
    }
    rg_xmit_a3.b.rg_tg2_f_sel = 0x66666;                     // new_set_reg(0xe40c, 0x90066666);//open tone2(1M)
    rg_xmit_a3.b.rg_tg2_enable = 0x1;
    rg_xmit_a3.b.rg_tg2_tone_man_en = 0x1;
    fi_ahb_write(RG_XMIT_A3, rg_xmit_a3.data);

    rg_xmit_a4.data = fi_ahb_read(RG_XMIT_A4);
    if(!RG_XMIT_A4_saved) {
        RG_XMIT_A4_saved = True;
        rg_xmit_a4_i.b.rg_tx_signal_sel = rg_xmit_a4.b.rg_tx_signal_sel;
    }
    rg_xmit_a4.b.rg_tx_signal_sel = 0x1;                         // new_set_reg(0xe410, 0x00000001);//1: select single tone
    fi_ahb_write(RG_XMIT_A4, rg_xmit_a4.data);

    rg_xmit_a56.data = fi_ahb_read(RG_XMIT_A56);
    if(!RG_XMIT_A56_saved) {
        RG_XMIT_A56_saved = True;
        rg_xmit_a56_i.b.rg_txdpd_comp_bypass = rg_xmit_a56.b.rg_txdpd_comp_bypass;
    }
    rg_xmit_a56.b.rg_txdpd_comp_bypass = 0x1;                         // new_set_reg(0xe4f0, 0x15008080);   //TX DPD bypass
    fi_ahb_write(RG_XMIT_A56, rg_xmit_a56.data);

    rg_recv_a11.data = fi_ahb_read(RG_RECV_A11);
    if(!RG_RECV_A11_saved) {
        RG_RECV_A11_saved = True;
        rg_recv_a11_i.b.rg_rxirr_bypass = rg_recv_a11.b.rg_rxirr_bypass;
    }
    rg_recv_a11.b.rg_rxirr_bypass = 0x1;                         // i2c_set_reg_fragment(0xe82c, 4, 4, 1); //RX IRR bypass
    fi_ahb_write(RG_RECV_A11, rg_recv_a11.data);

    rg_esti_a14.data = fi_ahb_read(RG_ESTI_A14);
    if(!RG_ESTI_A14_saved) {
        RG_ESTI_A14_saved = True;
        rg_esti_a14_i.b.rg_dc_rm_leaky_factor = rg_esti_a14.b.rg_dc_rm_leaky_factor;
    }
    rg_esti_a14.b.rg_dc_rm_leaky_factor = 0x5;
    fi_ahb_write(RG_ESTI_A14, rg_esti_a14.data);

    rg_esti_a64.data = fi_ahb_read(RG_ESTI_A64);
    rg_esti_a64.b.rg_adda_wait_count = 0x1000;
    fi_ahb_write(RG_ESTI_A64, rg_esti_a64.data);

    rg_esti_a65.data = fi_ahb_read(RG_ESTI_A65);
    rg_esti_a65.b.rg_adda_calc_count = 0x1000;
    fi_ahb_write(RG_ESTI_A65, rg_esti_a65.data);

    rg_xmit_a106.data = fi_ahb_read(RG_XMIT_A106);
    rg_xmit_a106.b.rg_tx_atten_factor = 0x2;
    fi_ahb_write(RG_XMIT_A106, rg_xmit_a106.data);

    rg_esti_a18.data = fi_ahb_read(RG_ESTI_A18);
    rg_esti_a18.b.rg_lnatank_read_response_bypass = 0x0;
    rg_esti_a18.b.rg_lnatank_read_response = 0x0;
    fi_ahb_write(RG_ESTI_A18, rg_esti_a18.data);

    CALI_MODE_T cali_mode = RXLNATANK;
    cali_mode_access(cali_mode);

    rg_rx_a8.data = rf_read_register(RG_RX_A8);
    rg_rx_a8.b.RG_WF5G_RX_LNA_TANK_SEL_MAN_MODE = manual;
    rf_write_register(RG_RX_A8, rg_rx_a8.data);

    for (code = 0; code < 16; code++)
    {
        if (manual == 0)
        {
            if (channel >= 180)
            {
                channel_idx = 0;

                rg_rx_a7.data = rf_read_register(RG_RX_A7);
                rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND0 = code;                        // Band0 : 4900-5000MHz
                rf_write_register(RG_RX_A7, rg_rx_a7.data);

            }
            else if (channel < 20)
            {
                channel_idx = 1;

                rg_rx_a7.data = rf_read_register(RG_RX_A7);
                rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND1 = code;                        // Band1 : 5000-5100MHz
                rf_write_register(RG_RX_A7, rg_rx_a7.data);

            }
            else if (channel < 40)
            {
                channel_idx = 2;

                rg_rx_a7.data = rf_read_register(RG_RX_A7);
                rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND2 = code;                        // Band2 : 5100-5200MHz
                rf_write_register(RG_RX_A7, rg_rx_a7.data);

            }
            else if (channel < 60)
            {
                channel_idx = 3;

                rg_rx_a7.data = rf_read_register(RG_RX_A7);
                rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND3 = code;                        // Band3 : 5200-5300MHz
                rf_write_register(RG_RX_A7, rg_rx_a7.data);

            }
            else if (channel < 80)
            { 
                channel_idx = 4;

                rg_rx_a7.data = rf_read_register(RG_RX_A7);
                rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND4 = code;                        // Band4 : 5300-5400MHz
                rf_write_register(RG_RX_A7, rg_rx_a7.data);

            }
            else if (channel < 100)
            {
                channel_idx = 5;

                rg_rx_a7.data = rf_read_register(RG_RX_A7);
                rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND5 = code;                        // Band5 : 5400-5500MHz
                rf_write_register(RG_RX_A7, rg_rx_a7.data);

            }
            else if (channel < 120)
            { 
                channel_idx = 6;

                rg_rx_a7.data = rf_read_register(RG_RX_A7);
                rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND6 = code;                        // Band6 : 5500-5600MHz
                rf_write_register(RG_RX_A7, rg_rx_a7.data);

            }
            else if (channel < 140)
            { 
                channel_idx = 7;

                rg_rx_a7.data = rf_read_register(RG_RX_A7);
                rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND7 = code;                        // Band7 : 5600-5700MHz
                rf_write_register(RG_RX_A7, rg_rx_a7.data);

            }
            else if (channel < 160)
            {
                channel_idx = 8;

                rg_rx_a8.data = rf_read_register(RG_RX_A8);
                rg_rx_a8.b.RG_WF5G_RX_LNA_TANK_SEL_BAND8 = code;                        // Band8 : 5700-5800MHz
                rf_write_register(RG_RX_A8, rg_rx_a8.data);

            }
            else if (channel < 180)
            {
                channel_idx = 9;

                rg_rx_a8.data = rf_read_register(RG_RX_A8);
                rg_rx_a8.b.RG_WF5G_RX_LNA_TANK_SEL_BAND9 = code;                        // Band9 : 5800-5900MHz
                rf_write_register(RG_RX_A8, rg_rx_a8.data);

            } // if(channel >= 180)
        }
        else
        {

            rg_rx_a8.data = rf_read_register(RG_RX_A8);
            rg_rx_a8.b.RG_WF5G_RX_LNA_TANK_SEL_MAN = code;                        //  Manual Tank SEL
            rf_write_register(RG_RX_A8, rg_rx_a8.data);

        } // if(manual == 0)

        rg_esti_a18.data = fi_ahb_read(RG_ESTI_A18);
        rg_esti_a18.b.rg_lnatank_read_response = 0x1;                         // i2c_set_reg_fragment(0xec48, 31, 31, 1);//open soft response
        fi_ahb_write(RG_ESTI_A18, rg_esti_a18.data);

        rg_esti_a18.data = fi_ahb_read(RG_ESTI_A18);
        rx_lna_ready = rg_esti_a18.b.ro_lnatank_esti_code_ready;
        while (unlock_cnt < TIME_OUT_CNT)
        {
            if (rx_lna_ready == 0)
            {

                rg_esti_a18.data = fi_ahb_read(RG_ESTI_A18);
                rx_lna_ready = rg_esti_a18.b.ro_lnatank_esti_code_ready;
                unlock_cnt = unlock_cnt + 1;
            }
            else
            {
                break;
            } // if(rx_lna_ready == 0)
        } // while(unlock_cnt < TIME_OUT_CNT)

        rg_esti_a18.data = fi_ahb_read(RG_ESTI_A18);
        rg_esti_a18.b.rg_lnatank_read_response = 0x0;                         // i2c_set_reg_fragment(0xec48, 31, 31, 1);//open soft response
        fi_ahb_write(RG_ESTI_A18, rg_esti_a18.data);

        rg_esti_a94.data = fi_ahb_read(RG_ESTI_A94);
        pwr_out = rg_esti_a94.b.ro_lnatank_esti_pow;//<17.1>

        if (max_i++ > 1024)
        {
            return rx_lan_out;
        }
    } // for(code = 0; code < 16; code ++)
    unlock_cnt = 0;

    rg_esti_a18.data = fi_ahb_read(RG_ESTI_A18);
    rx_lna_ready = rg_esti_a18.b.ro_lnatank_esti_code_ready;
    while (unlock_cnt < TIME_OUT_CNT)
    {
        if (rx_lna_ready == 0)
        {

            rg_esti_a18.data = fi_ahb_read(RG_ESTI_A18);
            rx_lna_ready = rg_esti_a18.b.ro_lnatank_esti_code_ready;
            unlock_cnt = unlock_cnt + 1;
        }
        else
        {
            break;
        }  // if(rx_lna_ready == 0)
    }  // while(unlock_cnt < TIME_OUT_CNT)

    rg_esti_a18.data = fi_ahb_read(RG_ESTI_A18);
code_out = rg_esti_a18.b.ro_lnatank_esti_code;

    if (manual == 0)   //write back calibration result
    {
        if (channel >= 180)
        {

            rg_rx_a7.data = rf_read_register(RG_RX_A7);
            rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND0 = code_out;                    // Band0 : 4900-5000MHz
            rf_write_register(RG_RX_A7, rg_rx_a7.data);

         }
         else if (channel < 20)
         {

            rg_rx_a7.data = rf_read_register(RG_RX_A7);
            rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND1 = code_out;                    // Band1 : 5000-5100MHz
            rf_write_register(RG_RX_A7, rg_rx_a7.data);

         }
         else if (channel < 40)
         {

            rg_rx_a7.data = rf_read_register(RG_RX_A7);
            rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND2 = code_out;                    // Band2 : 5100-5200MHz
            rf_write_register(RG_RX_A7, rg_rx_a7.data);

         }
         else if (channel < 60)
         {

            rg_rx_a7.data = rf_read_register(RG_RX_A7);
            rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND3 = code_out;                    // Band3 : 5200-5300MHz
            rf_write_register(RG_RX_A7, rg_rx_a7.data);

         }
         else if (channel < 80)
         {

            rg_rx_a7.data = rf_read_register(RG_RX_A7);
            rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND4 = code_out;                    // Band4 : 5300-5400MHz
            rf_write_register(RG_RX_A7, rg_rx_a7.data);

         }
         else if (channel < 100)
         {

            rg_rx_a7.data = rf_read_register(RG_RX_A7);
            rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND5 = code_out;                    // Band5 : 5400-5500MHz
            rf_write_register(RG_RX_A7, rg_rx_a7.data);

         }
         else if (channel < 120)
         {

            rg_rx_a7.data = rf_read_register(RG_RX_A7);
            rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND6 = code_out;                    // Band6 : 5500-5600MHz
            rf_write_register(RG_RX_A7, rg_rx_a7.data);

         }
         else if (channel < 140)
         {

            rg_rx_a7.data = rf_read_register(RG_RX_A7);
            rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND7 = code_out;                    // Band7 : 5600-5700MHz
            rf_write_register(RG_RX_A7, rg_rx_a7.data);

         }
         else if (channel < 160)
         {

            rg_rx_a8.data = rf_read_register(RG_RX_A8);
            rg_rx_a8.b.RG_WF5G_RX_LNA_TANK_SEL_BAND8 = code_out;                    // Band8 : 5700-5800MHz
            rf_write_register(RG_RX_A8, rg_rx_a8.data);

         }
         else if (channel < 180)
         {

            rg_rx_a8.data = rf_read_register(RG_RX_A8);
            rg_rx_a8.b.RG_WF5G_RX_LNA_TANK_SEL_BAND9 = code_out;                    // Band9 : 5800-5900MHz
            rf_write_register(RG_RX_A8, rg_rx_a8.data);

        } // if(channel>=180)
    }
    else
    {

        rg_rx_a8.data = rf_read_register(RG_RX_A8);
        rg_rx_a8.b.RG_WF5G_RX_LNA_TANK_SEL_MAN = code_out;                    //  Manual Tank SEL
        rf_write_register(RG_RX_A8, rg_rx_a8.data);

    } // if(manual == 0)

    wf5g_rxlnatank_result->cal_rx_lna[channel_idx].rx_lna_code = code_out;
    rx_lna_out.channel_idx = channel_idx;
    rx_lna_out.code_out = code_out;
    return rx_lna_out;

    // Revert the original value before calibration back
    rg_top_a7.data = rf_read_register(RG_TOP_A7);
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
    rg_tx_a2.b.RG_WF5G_TXM_RX_IQ_IQ_CAL_EN = rg_tx_a2_i.b.RG_WF5G_TXM_RX_IQ_IQ_CAL_EN;
    rf_write_register(RG_TX_A2, rg_tx_a2.data);
    // Revert the original value before calibration back
    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE = rg_rx_a2_i.b.RG_WF_RX_BBBM_MAN_MODE;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);
    // Revert the original value before calibration back
    rg_rx_a5.data = rf_read_register(RG_RX_A5);
    rg_rx_a5.b.RG_M2_WF5G_RX_FE_TOPBIAS_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_FE_TOPBIAS_EN;
    rg_rx_a5.b.RG_M2_WF5G_RX_LNA_BIAS_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_LNA_BIAS_EN;
    rg_rx_a5.b.RG_M2_WF5G_RX_GM_BIAS_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_GM_BIAS_EN;
    rg_rx_a5.b.RG_M2_WF5G_RX_MXR_BIAS_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_MXR_BIAS_EN;
    rg_rx_a5.b.RG_M2_WF5G_RX_LNA_TRSW_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_LNA_TRSW_EN;
    rg_rx_a5.b.RG_M2_WF5G_RX_LNA_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_LNA_EN;
    rg_rx_a5.b.RG_M2_WF5G_RX_GM_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_GM_EN;
    rg_rx_a5.b.RG_M2_WF5G_RX_MXR_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_MXR_EN;
    rg_rx_a5.b.RG_M2_WF5G_RX_LNA_CAL_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_LNA_CAL_EN;
    rg_rx_a5.b.RG_M2_WF5G_RX_IRRCAL_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_IRRCAL_EN;
    rg_rx_a5.b.RG_M2_WF_RTX_ABB_RX_EN = rg_rx_a5_i.b.RG_M2_WF_RTX_ABB_RX_EN;
    rg_rx_a5.b.RG_M2_WF_RX_TIA_BIAS_EN = rg_rx_a5_i.b.RG_M2_WF_RX_TIA_BIAS_EN;
    rg_rx_a5.b.RG_M2_WF_RX_RSSIPGA_BIAS_EN = rg_rx_a5_i.b.RG_M2_WF_RX_RSSIPGA_BIAS_EN;
    rg_rx_a5.b.RG_M2_WF_RX_LPF_BIAS_EN = rg_rx_a5_i.b.RG_M2_WF_RX_LPF_BIAS_EN;
    rg_rx_a5.b.RG_M2_WF_RX_TIA_EN = rg_rx_a5_i.b.RG_M2_WF_RX_TIA_EN;
    rg_rx_a5.b.RG_M2_WF_RX_RSSIPGA_EN = rg_rx_a5_i.b.RG_M2_WF_RX_RSSIPGA_EN;
    rg_rx_a5.b.RG_M2_WF_RX_LPF_EN = rg_rx_a5_i.b.RG_M2_WF_RX_LPF_EN;
    rg_rx_a5.b.RG_M2_WF_RX_LPF_I_DCOC_EN = rg_rx_a5_i.b.RG_M2_WF_RX_LPF_I_DCOC_EN;
    rg_rx_a5.b.RG_M2_WF5G_TXLPBK2TIA_EN = rg_rx_a5_i.b.RG_M2_WF5G_TXLPBK2TIA_EN;
    rf_write_register(RG_RX_A5, rg_rx_a5.data);
    // Revert the original value before calibration back
    rg_rx_a60.data = rf_read_register(RG_RX_A60);
    rg_rx_a60.b.RG_WF_TX_LPF_GAIN = rg_rx_a60_i.b.RG_WF_TX_LPF_GAIN;
    rf_write_register(RG_RX_A60, rg_rx_a60.data);
    // Revert the original value before calibration back
    rg_xmit_a3.data = fi_ahb_read(RG_XMIT_A3);
    rg_xmit_a3.b.rg_tg2_f_sel = rg_xmit_a3_i.b.rg_tg2_f_sel;
    rg_xmit_a3.b.rg_tg2_enable = rg_xmit_a3_i.b.rg_tg2_enable;
    rg_xmit_a3.b.rg_tg2_tone_man_en = rg_xmit_a3_i.b.rg_tg2_tone_man_en;
    fi_ahb_write(RG_XMIT_A3, rg_xmit_a3.data);
    // Revert the original value before calibration back
    rg_xmit_a4.data = fi_ahb_read(RG_XMIT_A4);
    rg_xmit_a4.b.rg_tx_signal_sel = rg_xmit_a4_i.b.rg_tx_signal_sel;
    fi_ahb_write(RG_XMIT_A4, rg_xmit_a4.data);
    // Revert the original value before calibration back
    rg_xmit_a56.data = fi_ahb_read(RG_XMIT_A56);
    rg_xmit_a56.b.rg_txdpd_comp_bypass = rg_xmit_a56_i.b.rg_txdpd_comp_bypass;
    fi_ahb_write(RG_XMIT_A56, rg_xmit_a56.data);
    // Revert the original value before calibration back
    rg_recv_a11.data = fi_ahb_read(RG_RECV_A11);
    rg_recv_a11.b.rg_rxirr_bypass = rg_recv_a11_i.b.rg_rxirr_bypass;
    fi_ahb_write(RG_RECV_A11, rg_recv_a11.data);
    // Revert the original value before calibration back
    rg_esti_a14.data = fi_ahb_read(RG_ESTI_A14);
    rg_esti_a14.b.rg_dc_rm_leaky_factor = rg_esti_a14_i.b.rg_dc_rm_leaky_factor;
    fi_ahb_write(RG_ESTI_A14, rg_esti_a14.data);
    cali_mode_exit();
}

#endif //RF_CALI_TEST
