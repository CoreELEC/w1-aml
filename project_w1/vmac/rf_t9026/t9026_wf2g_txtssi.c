#include "rf_calibration.h"

#ifdef RF_CALI_TEST
extern int log_10(int in);
extern int exp_10(int in);
void t9026_wf2g_txtssi(struct wf2g_txtssi_result * wf2g_txtssi_result, int ph, int pl, int vh, int vl)
{
    unsigned int txpwc_rdy = 0;
    unsigned int unlock_cnt = 0;
    unsigned int TIME_OUT_CNT = RF_TIME_OUT_CNT;
    int slope_tmp;
    int offset_tmp;
    //float slope;
    //float offset;
    struct wf2g_rxdcoc_in wf2g_rxdcoc_in;
    struct wf2g_rxdcoc_result wf2g_rxdcoc_result;
    RG_TOP_A29_FIELD_T    rg_top_a29;
    RG_TX_A5_FIELD_T    rg_tx_a5;
    RG_TX_A6_FIELD_T    rg_tx_a6;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_RX_A90_FIELD_T    rg_rx_a90;
    RG_RX_A68_FIELD_T    rg_rx_a68;
    RG_TOP_A23_FIELD_T    rg_top_a23;
    RG_ESTI_A64_FIELD_T    rg_esti_a64;
    RG_XMIT_A46_FIELD_T    rg_xmit_a46;
    RG_XMIT_A3_FIELD_T    rg_xmit_a3;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4;
    RG_ESTI_A14_FIELD_T    rg_esti_a14;
    RG_ESTI_A67_FIELD_T    rg_esti_a67;
    RG_ESTI_A68_FIELD_T    rg_esti_a68;
    RG_ESTI_A69_FIELD_T    rg_esti_a69;

    // Internal variables definitions
    bool    RG_TOP_A29_saved = False;
    RG_TOP_A29_FIELD_T    rg_top_a29_i;
    // Internal variables definitions
    bool    RG_TX_A5_saved = False;
    RG_TX_A5_FIELD_T    rg_tx_a5_i;
    // Internal variables definitions
    bool    RG_TX_A6_saved = False;
    RG_TX_A6_FIELD_T    rg_tx_a6_i;
    // Internal variables definitions
    bool    RG_RX_A2_saved = False;
    RG_RX_A2_FIELD_T    rg_rx_a2_i;
    // Internal variables definitions
    bool    RG_RX_A90_saved = False;
    RG_RX_A90_FIELD_T    rg_rx_a90_i;
    // Internal variables definitions
    bool    RG_RX_A68_saved = False;
    RG_RX_A68_FIELD_T    rg_rx_a68_i;
    // Internal variables definitions
    bool    RG_TOP_A23_saved = False;
    RG_TOP_A23_FIELD_T    rg_top_a23_i;
    // Internal variables definitions
    bool    RG_XMIT_A46_saved = False;
    RG_XMIT_A46_FIELD_T    rg_xmit_a46_i;
    // Internal variables definitions
    bool    RG_XMIT_A3_saved = False;
    RG_XMIT_A3_FIELD_T    rg_xmit_a3_i;
    // Internal variables definitions
    bool    RG_XMIT_A4_saved = False;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4_i;
    // Internal variables definitions
    bool    RG_ESTI_A14_saved = False;
    RG_ESTI_A14_FIELD_T    rg_esti_a14_i;

    CALI_MODE_T cali_mode = TXTSSI;
    rg_xmit_a46_i.b.rg_txpwc_comp_man_sel = 0;

    rg_top_a29.data = rf_read_register(RG_TOP_A29);
    if(!RG_TOP_A29_saved) {
        RG_TOP_A29_saved = True;
        rg_top_a29_i.b.RG_M6_WF_TRX_HF_LDO_RXB_EN = rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RXB_EN;
        rg_top_a29_i.b.RG_M6_WF_TRX_HF_LDO_RX1_EN = rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RX1_EN;
        rg_top_a29_i.b.RG_M6_WF_TRX_HF_LDO_RX2_EN = rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RX2_EN;
        rg_top_a29_i.b.RG_M6_WF_TRX_LF_LDO_RX_EN = rg_top_a29.b.RG_M6_WF_TRX_LF_LDO_RX_EN;
    }
    rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RXB_EN = 0x1;
    rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RX1_EN = 0x1;
    rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RX2_EN = 0x1;
    rg_top_a29.b.RG_M6_WF_TRX_LF_LDO_RX_EN = 0x1;
    rf_write_register(RG_TOP_A29, rg_top_a29.data);

    rg_tx_a5.data = rf_read_register(RG_TX_A5);
    if(!RG_TX_A5_saved) {
        RG_TX_A5_saved = True;
        rg_tx_a5_i.b.RG_WF2G_TX_TSSI_EN = rg_tx_a5.b.RG_WF2G_TX_TSSI_EN;
    }
    rg_tx_a5.b.RG_WF2G_TX_TSSI_EN = 0x1;
    rf_write_register(RG_TX_A5, rg_tx_a5.data);

    rg_tx_a6.data = rf_read_register(RG_TX_A6);
    if(!RG_TX_A6_saved) {
        RG_TX_A6_saved = True;
        rg_tx_a6_i.b.RG_WF2G_TX_MAN_MODE = rg_tx_a6.b.RG_WF2G_TX_MAN_MODE;
    }
    rg_tx_a6.b.RG_WF2G_TX_MAN_MODE = 0x1;
    rf_write_register(RG_TX_A6, rg_tx_a6.data);

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    if(!RG_RX_A2_saved) {
        RG_RX_A2_saved = True;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE;
    }
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x96;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);

    rg_rx_a90.data = rf_read_register(RG_RX_A90);
    if(!RG_RX_A90_saved) {
        RG_RX_A90_saved = True;
        rg_rx_a90_i.b.RG_M6_WF_RTX_ABB_RX_EN = rg_rx_a90.b.RG_M6_WF_RTX_ABB_RX_EN;
        rg_rx_a90_i.b.RG_M6_WF_RX_TIA_BIAS_EN = rg_rx_a90.b.RG_M6_WF_RX_TIA_BIAS_EN;
        rg_rx_a90_i.b.RG_M6_WF_RX_RSSIPGA_BIAS_EN = rg_rx_a90.b.RG_M6_WF_RX_RSSIPGA_BIAS_EN;
        rg_rx_a90_i.b.RG_M6_WF_RX_LPF_BIAS_EN = rg_rx_a90.b.RG_M6_WF_RX_LPF_BIAS_EN;
        rg_rx_a90_i.b.RG_M6_WF_RX_TIA_EN = rg_rx_a90.b.RG_M6_WF_RX_TIA_EN;
        rg_rx_a90_i.b.RG_M6_WF_RX_RSSIPGA_EN = rg_rx_a90.b.RG_M6_WF_RX_RSSIPGA_EN;
        rg_rx_a90_i.b.RG_M6_WF_RX_LPF_EN = rg_rx_a90.b.RG_M6_WF_RX_LPF_EN;
        rg_rx_a90_i.b.RG_M6_WF_RX_LPF_I_DCOC_EN = rg_rx_a90.b.RG_M6_WF_RX_LPF_I_DCOC_EN;
        rg_rx_a90_i.b.RG_M6_WF_RX_LPF_Q_DCOC_EN = rg_rx_a90.b.RG_M6_WF_RX_LPF_Q_DCOC_EN;
    }
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
    if(!RG_RX_A68_saved) {
        RG_RX_A68_saved = True;
        rg_rx_a68_i.b.RG_WF2G_TXLPBK2TIA_EN = rg_rx_a68.b.RG_WF2G_TXLPBK2TIA_EN;
    }
    rg_rx_a68.b.RG_WF2G_TXLPBK2TIA_EN = 0x1;
    rf_write_register(RG_RX_A68, rg_rx_a68.data);

    rg_top_a23.data = rf_read_register(RG_TOP_A23);
    if(!RG_TOP_A23_saved) {
        RG_TOP_A23_saved = True;
        rg_top_a23_i.b.rg_m6_wf_radc_en = rg_top_a23.b.rg_m6_wf_radc_en;
        rg_top_a23_i.b.rg_m6_wf_wadc_enable_i = rg_top_a23.b.rg_m6_wf_wadc_enable_i;
        rg_top_a23_i.b.rg_m6_wf_wadc_enable_q = rg_top_a23.b.rg_m6_wf_wadc_enable_q;
    }
    rg_top_a23.b.rg_m6_wf_radc_en = 0x1;
    rg_top_a23.b.rg_m6_wf_wadc_enable_i = 0x1;
    rg_top_a23.b.rg_m6_wf_wadc_enable_q = 0x1;
    rf_write_register(RG_TOP_A23, rg_top_a23.data);

    //do rx dc
    wf2g_rxdcoc_in.bw_idx = 0;
    wf2g_rxdcoc_in.tia_idx = 0;
    wf2g_rxdcoc_in.lpf_idx = 1;
    wf2g_rxdcoc_in.wf_rx_gain = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
    wf2g_rxdcoc_in.manual_mode = 1;
    t9026_wf2g_rxdcoc(&wf2g_rxdcoc_in, &wf2g_rxdcoc_result);

    rg_esti_a64.data = fi_ahb_read(RG_ESTI_A64);
    rg_esti_a64.b.rg_adda_wait_count = 0x10;
    fi_ahb_write(RG_ESTI_A64, rg_esti_a64.data);

    rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
    if(!RG_XMIT_A46_saved) {
        RG_XMIT_A46_saved = True;
        rg_xmit_a46_i.b.rg_txpwc_comp_man_sel = rg_xmit_a46.b.rg_txpwc_comp_man_sel;
    }
    rg_xmit_a46.b.rg_txpwc_comp_man_sel = 0x0;
    fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);

    rg_xmit_a3.data = fi_ahb_read(RG_XMIT_A3);
    if(!RG_XMIT_A3_saved) {
        RG_XMIT_A3_saved = True;
        rg_xmit_a3_i.b.rg_tg2_f_sel = rg_xmit_a3.b.rg_tg2_f_sel;
        rg_xmit_a3_i.b.rg_tg2_enable = rg_xmit_a3.b.rg_tg2_enable;
        rg_xmit_a3_i.b.rg_tg2_tone_man_en = rg_xmit_a3.b.rg_tg2_tone_man_en;
    }
    rg_xmit_a3.b.rg_tg2_f_sel = 0x200000;                    // new_set_reg(0xe40c, 0x90066666);//open tone2(1M)
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

    rg_esti_a14.data = fi_ahb_read(RG_ESTI_A14);
    if(!RG_ESTI_A14_saved) {
        RG_ESTI_A14_saved = True;
        rg_esti_a14_i.b.rg_dc_rm_leaky_factor = rg_esti_a14.b.rg_dc_rm_leaky_factor;
    }
    rg_esti_a14.b.rg_dc_rm_leaky_factor = 0x7;
    fi_ahb_write(RG_ESTI_A14, rg_esti_a14.data);

    rg_esti_a67.data = fi_ahb_read(RG_ESTI_A67);
    rg_esti_a67.b.rg_txpwc_ph = ph;
    rg_esti_a67.b.rg_txpwc_pl = pl;
    fi_ahb_write(RG_ESTI_A67, rg_esti_a67.data);

    rg_esti_a68.data = fi_ahb_read(RG_ESTI_A68);
    rg_esti_a68.b.rg_txpwc_vh_amp = vh;
    rg_esti_a68.b.rg_txpwc_vl_amp = vl;
    rg_esti_a68.b.rg_txpwc_dc_cnt = 0x3;
    fi_ahb_write(RG_ESTI_A68, rg_esti_a68.data);

 
    t9026_cali_mode_access(cali_mode);

    rg_esti_a69.data = fi_ahb_read(RG_ESTI_A69);
    txpwc_rdy = rg_esti_a69.b.ro_txpwc_ready;
    while(unlock_cnt < TIME_OUT_CNT)
    {
        if(txpwc_rdy == 0)
        {

            rg_esti_a69.data = fi_ahb_read(RG_ESTI_A69);
            txpwc_rdy = rg_esti_a69.b.ro_txpwc_ready;
            RF_DPRINTF(RF_DEBUG_INFO, "TX TSSI NOT READY...\n");
            unlock_cnt = unlock_cnt + 1; 
        }
        else
        {
            RF_DPRINTF(RF_DEBUG_INFO, "TX TSSI READY\n");
            break;
        } //if(rx_irr_rdy == 0)
    } // while(unlock_cnt < TIME_OUT_CNT)

    rg_esti_a69.data = fi_ahb_read(RG_ESTI_A69);
    wf2g_txtssi_result->cal_tx_tssi.txpwc_slope = rg_esti_a69.b.ro_txpwc_slope;
    wf2g_txtssi_result->cal_tx_tssi.txpwc_offset = rg_esti_a69.b.ro_txpwc_offset;
    slope_tmp = rg_esti_a69.b.ro_txpwc_slope;  //9.5
    offset_tmp = rg_esti_a69.b.ro_txpwc_offset;  //15.9

    RF_DPRINTF(RF_DEBUG_RESULT, "unlock_cnt:%d, TX TSSI: txpwc_slope_tmp = %x, txpwc_offset_tmp = %x\n", unlock_cnt, slope_tmp, offset_tmp);
/*
    if(slope_tmp & BIT(8))
    {
            slope_tmp = slope_tmp - (1 << 9);
     }
     slope = (slope_tmp)*1.0/(1<<4);

    if(offset_tmp & BIT(14))
    {
            offset_tmp = offset_tmp - (1 << 15);
     }
     offset = (offset_tmp)*1.0/(1<<6);

	 wf2g_txtssi_result->cal_tx_tssi.slope = slope;
     wf2g_txtssi_result->cal_tx_tssi.offset = offset;
     PUTS("TX TSSI: txpwc_slope = %f, txpwc_offset = %f\n", slope, offset);
    */
    

    // Revert the original value before calibration back
    rg_top_a29.data = rf_read_register(RG_TOP_A29);
    rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RXB_EN = rg_top_a29_i.b.RG_M6_WF_TRX_HF_LDO_RXB_EN;
    rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RX1_EN = rg_top_a29_i.b.RG_M6_WF_TRX_HF_LDO_RX1_EN;
    rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RX2_EN = rg_top_a29_i.b.RG_M6_WF_TRX_HF_LDO_RX2_EN;
    rg_top_a29.b.RG_M6_WF_TRX_LF_LDO_RX_EN = rg_top_a29_i.b.RG_M6_WF_TRX_LF_LDO_RX_EN;
    rf_write_register(RG_TOP_A29, rg_top_a29.data);
    // Revert the original value before calibration back
    rg_tx_a5.data = rf_read_register(RG_TX_A5);
    rg_tx_a5.b.RG_WF2G_TX_TSSI_EN = rg_tx_a5_i.b.RG_WF2G_TX_TSSI_EN;
    rf_write_register(RG_TX_A5, rg_tx_a5.data);
    // Revert the original value before calibration back
    rg_tx_a6.data = rf_read_register(RG_TX_A6);
    rg_tx_a6.b.RG_WF2G_TX_MAN_MODE = rg_tx_a6_i.b.RG_WF2G_TX_MAN_MODE;
    rf_write_register(RG_TX_A6, rg_tx_a6.data);
    // Revert the original value before calibration back
    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);
    // Revert the original value before calibration back
    rg_rx_a90.data = rf_read_register(RG_RX_A90);
    rg_rx_a90.b.RG_M6_WF_RTX_ABB_RX_EN = rg_rx_a90_i.b.RG_M6_WF_RTX_ABB_RX_EN;
    rg_rx_a90.b.RG_M6_WF_RX_TIA_BIAS_EN = rg_rx_a90_i.b.RG_M6_WF_RX_TIA_BIAS_EN;
    rg_rx_a90.b.RG_M6_WF_RX_RSSIPGA_BIAS_EN = rg_rx_a90_i.b.RG_M6_WF_RX_RSSIPGA_BIAS_EN;
    rg_rx_a90.b.RG_M6_WF_RX_LPF_BIAS_EN = rg_rx_a90_i.b.RG_M6_WF_RX_LPF_BIAS_EN;
    rg_rx_a90.b.RG_M6_WF_RX_TIA_EN = rg_rx_a90_i.b.RG_M6_WF_RX_TIA_EN;
    rg_rx_a90.b.RG_M6_WF_RX_RSSIPGA_EN = rg_rx_a90_i.b.RG_M6_WF_RX_RSSIPGA_EN;
    rg_rx_a90.b.RG_M6_WF_RX_LPF_EN = rg_rx_a90_i.b.RG_M6_WF_RX_LPF_EN;
    rg_rx_a90.b.RG_M6_WF_RX_LPF_I_DCOC_EN = rg_rx_a90_i.b.RG_M6_WF_RX_LPF_I_DCOC_EN;
    rg_rx_a90.b.RG_M6_WF_RX_LPF_Q_DCOC_EN = rg_rx_a90_i.b.RG_M6_WF_RX_LPF_Q_DCOC_EN;
    rf_write_register(RG_RX_A90, rg_rx_a90.data);
    // Revert the original value before calibration back
    rg_rx_a68.data = rf_read_register(RG_RX_A68);
    rg_rx_a68.b.RG_WF2G_TXLPBK2TIA_EN = rg_rx_a68_i.b.RG_WF2G_TXLPBK2TIA_EN;
    rf_write_register(RG_RX_A68, rg_rx_a68.data);
    // Revert the original value before calibration back
    rg_top_a23.data = rf_read_register(RG_TOP_A23);
    rg_top_a23.b.rg_m6_wf_radc_en = rg_top_a23_i.b.rg_m6_wf_radc_en;
    rg_top_a23.b.rg_m6_wf_wadc_enable_i = rg_top_a23_i.b.rg_m6_wf_wadc_enable_i;
    rg_top_a23.b.rg_m6_wf_wadc_enable_q = rg_top_a23_i.b.rg_m6_wf_wadc_enable_q;
    rf_write_register(RG_TOP_A23, rg_top_a23.data);
    // Revert the original value before calibration back
    rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
    rg_xmit_a46.b.rg_txpwc_comp_man_sel = rg_xmit_a46_i.b.rg_txpwc_comp_man_sel;
    fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);
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
    rg_esti_a14.data = fi_ahb_read(RG_ESTI_A14);
    rg_esti_a14.b.rg_dc_rm_leaky_factor = rg_esti_a14_i.b.rg_dc_rm_leaky_factor;
    fi_ahb_write(RG_ESTI_A14, rg_esti_a14.data);
    t9026_cali_mode_exit();
    RF_DPRINTF(RF_DEBUG_INFO, "TX TSSI calib finish \n");
}


void t9026_wf2g_txtssi_ratio(struct wf2g_txtssi_result * wf2g_txtssi_result, int slope_tmp, int offset_tmp)
{
    int slope;
    int offset;
    int dci;
    int dcq;
    int dci_init;
    int dcq_init;
    int tssi_dc;
    int pre_digital_gain = 52;
    int new_digital_gain = 52;
    int final_digital_gain;
    int target_pout = 1800;
    int real_pout;
    int gain_ratio;
    int retrynum = 0;
    int v_dc_pow = 0;
    int v_dc_dB = 0;
    unsigned int rx_dc_rdy;
    unsigned int unlock_cnt = 0;
    unsigned int TIME_OUT_CNT = RF_TIME_OUT_CNT;
    struct wf2g_rxdcoc_in wf2g_rxdcoc_in;
    struct wf2g_rxdcoc_result wf2g_rxdcoc_result;
    RG_TOP_A15_FIELD_T    rg_top_a15;
    RG_TOP_A29_FIELD_T    rg_top_a29;
    RG_TX_A5_FIELD_T    rg_tx_a5;
    RG_TX_A6_FIELD_T    rg_tx_a6;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_RX_A90_FIELD_T    rg_rx_a90;
    RG_RX_A68_FIELD_T    rg_rx_a68;
    RG_TOP_A23_FIELD_T    rg_top_a23;
    RG_ESTI_A64_FIELD_T    rg_esti_a64;
    RG_ESTI_A65_FIELD_T    rg_esti_a65;
    RG_ESTI_A17_FIELD_T  rg_esti_a17;
    RG_ESTI_A90_FIELD_T  rg_esti_a90;
    RG_ESTI_A91_FIELD_T  rg_esti_a91;
    RG_XMIT_A46_FIELD_T    rg_xmit_a46;
    RG_XMIT_A3_FIELD_T    rg_xmit_a3;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4;
    RG_XMIT_A56_FIELD_T    rg_xmit_a56;
    RG_RECV_A11_FIELD_T    rg_recv_a11;

    // Internal variables definitions
    bool    RG_TOP_A15_saved = False;
    RG_TOP_A15_FIELD_T    rg_top_a15_i;
    // Internal variables definitions
    bool    RG_TOP_A29_saved = False;
    RG_TOP_A29_FIELD_T    rg_top_a29_i;
    // Internal variables definitions
    bool    RG_TX_A5_saved = False;
    RG_TX_A5_FIELD_T    rg_tx_a5_i;
    // Internal variables definitions
    bool    RG_TX_A6_saved = False;
    RG_TX_A6_FIELD_T    rg_tx_a6_i;
    // Internal variables definitions
    bool    RG_RX_A2_saved = False;
    RG_RX_A2_FIELD_T    rg_rx_a2_i;
    // Internal variables definitions
    bool    RG_RX_A90_saved = False;
    RG_RX_A90_FIELD_T    rg_rx_a90_i;
    // Internal variables definitions
    bool    RG_RX_A68_saved = False;
    RG_RX_A68_FIELD_T    rg_rx_a68_i;
    // Internal variables definitions
    bool    RG_TOP_A23_saved = False;
    RG_TOP_A23_FIELD_T    rg_top_a23_i;
    // Internal variables definitions
    bool    RG_XMIT_A46_saved = False;
    RG_XMIT_A46_FIELD_T    rg_xmit_a46_i;
    // Internal variables definitions
    bool    RG_XMIT_A3_saved = False;
    RG_XMIT_A3_FIELD_T    rg_xmit_a3_i;
    // Internal variables definitions
    bool    RG_XMIT_A4_saved = False;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4_i;
    // Internal variables definitions
    bool    RG_ESTI_A17_saved = False;
    RG_ESTI_A17_FIELD_T  rg_esti_a17_i;

    CALI_MODE_T cali_mode = RXDCOC;
    rg_xmit_a46_i.b.rg_txpwc_comp_man_sel = 0;

    if(slope_tmp & BIT(8))
   {
        slope_tmp = slope_tmp - (1 << 9);
    }
    //slope = (slope_tmp)*1.0/(1<<4) * 64;
    slope = (slope_tmp)*4;

    if(offset_tmp & BIT(14))
   {
        offset_tmp = offset_tmp - (1 << 15);
    }
    //offset = (offset_tmp)*1.0/(1<<6) * 64;
    offset = (offset_tmp);
    printk("slope  = %d, offset = %d \r\n", slope, offset);

    rg_top_a15.data = rf_read_register(RG_TOP_A15);
    if(!RG_TOP_A15_saved) {
        RG_TOP_A15_saved = True;
        rg_top_a15_i.b.rg_wf_adda_man_mode = rg_top_a15.b.rg_wf_adda_man_mode;
        rg_top_a15_i.b.rg_wf_dac_ampctrl_man = rg_top_a15.b.rg_wf_dac_ampctrl_man;
    }
    rg_top_a15.b.rg_wf_adda_man_mode = 0x1;
    rg_top_a15.b.rg_wf_dac_ampctrl_man = 0x2;
    rf_write_register(RG_TOP_A15, rg_top_a15.data);

    rg_top_a29.data = rf_read_register(RG_TOP_A29);
    if(!RG_TOP_A29_saved) {
        RG_TOP_A29_saved = True;
        rg_top_a29_i.b.RG_M6_WF_TRX_HF_LDO_RXB_EN = rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RXB_EN;
        rg_top_a29_i.b.RG_M6_WF_TRX_HF_LDO_RX1_EN = rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RX1_EN;
        rg_top_a29_i.b.RG_M6_WF_TRX_HF_LDO_RX2_EN = rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RX2_EN;
        rg_top_a29_i.b.RG_M6_WF_TRX_LF_LDO_RX_EN = rg_top_a29.b.RG_M6_WF_TRX_LF_LDO_RX_EN;
    }
    rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RXB_EN = 0x1;
    rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RX1_EN = 0x1;
    rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RX2_EN = 0x1;
    rg_top_a29.b.RG_M6_WF_TRX_LF_LDO_RX_EN = 0x1;
    rf_write_register(RG_TOP_A29, rg_top_a29.data);

    rg_tx_a5.data = rf_read_register(RG_TX_A5);
    if(!RG_TX_A5_saved) {
        RG_TX_A5_saved = True;
        rg_tx_a5_i.b.RG_WF2G_TX_TSSI_EN = rg_tx_a5.b.RG_WF2G_TX_TSSI_EN;
    }
    rg_tx_a5.b.RG_WF2G_TX_TSSI_EN = 0x1;
    rf_write_register(RG_TX_A5, rg_tx_a5.data);

    rg_tx_a6.data = rf_read_register(RG_TX_A6);
    if(!RG_TX_A6_saved) {
        RG_TX_A6_saved = True;
        rg_tx_a6_i.b.RG_WF2G_TX_MAN_MODE = rg_tx_a6.b.RG_WF2G_TX_MAN_MODE;
    }
    rg_tx_a6.b.RG_WF2G_TX_MAN_MODE = 0x1;
    rf_write_register(RG_TX_A6, rg_tx_a6.data);

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    if(!RG_RX_A2_saved) {
        RG_RX_A2_saved = True;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE;
    }
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x96;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);

    rg_rx_a90.data = rf_read_register(RG_RX_A90);
    if(!RG_RX_A90_saved) {
        RG_RX_A90_saved = True;
        rg_rx_a90_i.b.RG_M6_WF_RTX_ABB_RX_EN = rg_rx_a90.b.RG_M6_WF_RTX_ABB_RX_EN;
        rg_rx_a90_i.b.RG_M6_WF_RX_TIA_BIAS_EN = rg_rx_a90.b.RG_M6_WF_RX_TIA_BIAS_EN;
        rg_rx_a90_i.b.RG_M6_WF_RX_RSSIPGA_BIAS_EN = rg_rx_a90.b.RG_M6_WF_RX_RSSIPGA_BIAS_EN;
        rg_rx_a90_i.b.RG_M6_WF_RX_LPF_BIAS_EN = rg_rx_a90.b.RG_M6_WF_RX_LPF_BIAS_EN;
        rg_rx_a90_i.b.RG_M6_WF_RX_TIA_EN = rg_rx_a90.b.RG_M6_WF_RX_TIA_EN;
        rg_rx_a90_i.b.RG_M6_WF_RX_RSSIPGA_EN = rg_rx_a90.b.RG_M6_WF_RX_RSSIPGA_EN;
        rg_rx_a90_i.b.RG_M6_WF_RX_LPF_EN = rg_rx_a90.b.RG_M6_WF_RX_LPF_EN;
        rg_rx_a90_i.b.RG_M6_WF_RX_LPF_I_DCOC_EN = rg_rx_a90.b.RG_M6_WF_RX_LPF_I_DCOC_EN;
        rg_rx_a90_i.b.RG_M6_WF_RX_LPF_Q_DCOC_EN = rg_rx_a90.b.RG_M6_WF_RX_LPF_Q_DCOC_EN;
    }
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
    if(!RG_RX_A68_saved) {
        RG_RX_A68_saved = True;
        rg_rx_a68_i.b.RG_WF2G_TXLPBK2TIA_EN = rg_rx_a68.b.RG_WF2G_TXLPBK2TIA_EN;
    }
    rg_rx_a68.b.RG_WF2G_TXLPBK2TIA_EN = 0x1;
    rf_write_register(RG_RX_A68, rg_rx_a68.data);

    rg_top_a23.data = rf_read_register(RG_TOP_A23);
    if(!RG_TOP_A23_saved) {
        RG_TOP_A23_saved = True;
        rg_top_a23_i.b.rg_m6_wf_radc_en = rg_top_a23.b.rg_m6_wf_radc_en;
        rg_top_a23_i.b.rg_m6_wf_wadc_enable_i = rg_top_a23.b.rg_m6_wf_wadc_enable_i;
        rg_top_a23_i.b.rg_m6_wf_wadc_enable_q = rg_top_a23.b.rg_m6_wf_wadc_enable_q;
    }
    rg_top_a23.b.rg_m6_wf_radc_en = 0x1;
    rg_top_a23.b.rg_m6_wf_wadc_enable_i = 0x1;
    rg_top_a23.b.rg_m6_wf_wadc_enable_q = 0x1;
    rf_write_register(RG_TOP_A23, rg_top_a23.data);

    //close dpd compensation
    rg_xmit_a56.data = fi_ahb_read(RG_XMIT_A56);
    rg_xmit_a56.b.rg_txdpd_comp_type = 0x1;
    rg_xmit_a56.b.rg_txdpd_comp_bypass = 0x1;
    rg_xmit_a56.b.rg_txdpd_comp_bypass_man = 0x1;
    rg_xmit_a56.b.rg_txdpd_comp_coef_man_sel = 0x1;
    fi_ahb_write(RG_XMIT_A56, rg_xmit_a56.data);

    //close rx irr compensation
    rg_recv_a11.data = fi_ahb_read(RG_RECV_A11);
    rg_recv_a11.b.rg_rxirr_man_mode = 0x0;
    rg_recv_a11.b.rg_rxirr_bypass = 0x1;
    fi_ahb_write(RG_RECV_A11, rg_recv_a11.data);

    //do rx dc
    wf2g_rxdcoc_in.bw_idx = 0;
    wf2g_rxdcoc_in.tia_idx = 0;
    wf2g_rxdcoc_in.lpf_idx = 1;
    wf2g_rxdcoc_in.wf_rx_gain = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
    wf2g_rxdcoc_in.manual_mode = 1;
    t9026_wf2g_rxdcoc(&wf2g_rxdcoc_in, &wf2g_rxdcoc_result);

    dci_init = wf2g_rxdcoc_result.cal_rx_dc.rxdc_i;
    dcq_init = wf2g_rxdcoc_result.cal_rx_dc.rxdc_q;

    rg_xmit_a3.data = fi_ahb_read(RG_XMIT_A3);
    if(!RG_XMIT_A3_saved) {
        RG_XMIT_A3_saved = True;
        rg_xmit_a3_i.b.rg_tg2_f_sel = rg_xmit_a3.b.rg_tg2_f_sel;
        rg_xmit_a3_i.b.rg_tg2_enable = rg_xmit_a3.b.rg_tg2_enable;
        rg_xmit_a3_i.b.rg_tg2_tone_man_en = rg_xmit_a3.b.rg_tg2_tone_man_en;
    }
    rg_xmit_a3.b.rg_tg2_f_sel = 0x200000;                    // new_set_reg(0xe40c, 0xa0200000);//open tone2(5M)
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

    rg_esti_a64.data = fi_ahb_read(RG_ESTI_A64);
    rg_esti_a64.b.rg_adda_wait_count = 0x10;
    fi_ahb_write(RG_ESTI_A64, rg_esti_a64.data);

    rg_esti_a65.data = fi_ahb_read(RG_ESTI_A65);
    rg_esti_a65.b.rg_adda_calc_count = 0x4000;//0x4000;
    fi_ahb_write(RG_ESTI_A65, rg_esti_a65.data);

    while(retrynum < 3)
    {
        rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
        if(!RG_XMIT_A46_saved) {
            RG_XMIT_A46_saved = True;
            rg_xmit_a46_i.b.rg_txpwc_comp_man_sel = rg_xmit_a46.b.rg_txpwc_comp_man_sel;
            rg_xmit_a46_i.b.rg_txpwc_comp_man = rg_xmit_a46.b.rg_txpwc_comp_man;
        }
        rg_xmit_a46.b.rg_txpwc_comp_man_sel = 0x1;
        rg_xmit_a46.b.rg_txpwc_comp_man = new_digital_gain;
        fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);

        t9026_cali_mode_access(cali_mode);

        rg_esti_a17.data = fi_ahb_read(RG_ESTI_A17);
        if(!RG_ESTI_A17_saved) {
            RG_ESTI_A17_saved = True;
            rg_esti_a17_i.b.rg_dcoc_read_response = rg_esti_a17.b.rg_dcoc_read_response;
        }
        rg_esti_a17.b.rg_dcoc_read_response = 0x1;                         // new_set_reg(0xec44, 0x80000000);//read response bypass to 0, soft read ready response to 1
        fi_ahb_write(RG_ESTI_A17, rg_esti_a17.data);

        rg_esti_a17.data = fi_ahb_read(RG_ESTI_A17);
        rx_dc_rdy = rg_esti_a17.b.ro_dcoc_code_ready;
        unlock_cnt = 0;
        while (unlock_cnt < TIME_OUT_CNT)
        {
            if (rx_dc_rdy == 0)
            {
                rg_esti_a17.data = fi_ahb_read(RG_ESTI_A17);
                rx_dc_rdy = rg_esti_a17.b.ro_dcoc_code_ready;
                RF_DPRINTF(RF_DEBUG_INFO, "unlock_cnt:%d, RX DC NOT READY...\n", unlock_cnt);
                unlock_cnt = unlock_cnt + 1;
            }
            else
            {
                RF_DPRINTF(RF_DEBUG_INFO, "RX DC ready\n");
                break;
            }
        }
        rg_esti_a90.data = fi_ahb_read(RG_ESTI_A90);
        rg_esti_a91.data = fi_ahb_read(RG_ESTI_A91);
        dci = rg_esti_a90.data;
        dcq = rg_esti_a91.data;

        if(dci & BIT(29))
        {
            dci = dci - (1 << 30);
        }
        dci = dci >> (5 + 14);
        if(dcq & BIT(29))
        {
            dcq = dcq - (1 << 30);
        }
        dcq = dcq >> (5 + 14);
        printk("dci  = %d, dcq = %d \r\n", dci, dcq);
        //tssi_dc = log10(pow_a((dci - dci_init)*1.0/512, 2) + pow_a((dcq - dcq_init)*1.0/512, 2)) * 10;
        v_dc_pow = (dci - dci_init) * (dci - dci_init) + (dcq - dcq_init) * (dcq - dcq_init);
	 v_dc_dB = log_10(v_dc_pow) - 217; //54*4;//20*log10(512) = 54.1854(dB)
	 tssi_dc = v_dc_dB;

        rg_esti_a17.data = fi_ahb_read(RG_ESTI_A17);
        rg_esti_a17.b.rg_dcoc_read_response = 0x0;                         // new_set_reg(0xec44, 0x80000000);//read response bypass to 0, soft read ready response to 1
        fi_ahb_write(RG_ESTI_A17, rg_esti_a17.data);

        t9026_cali_mode_exit();

	 real_pout = (tssi_dc * 16  - offset) * 100/(slope);
	 	
        RF_DPRINTF(RF_DEBUG_RESULT, "real_pout = %d \r\n", real_pout);

        if(abs(target_pout - real_pout) <= 50)
        {
		final_digital_gain = new_digital_gain;
		break;
        }else{
		new_digital_gain = new_digital_gain * (exp_10((target_pout - real_pout) * 128/100/2)/1024)/1024;
		final_digital_gain = new_digital_gain;
		retrynum++;
		continue;
        }
    }
    gain_ratio = final_digital_gain * 100/pre_digital_gain ;
    if (gain_ratio < 70)
    {
        gain_ratio = 70;
    }
    else if (gain_ratio > 110)
    {
        gain_ratio = 110;
    }
    wf2g_txtssi_result->cal_tx_tssi.gain_ratio = gain_ratio;
    RF_DPRINTF(RF_DEBUG_RESULT,"final_digital_gain = %d, gain_ratio = %d \r\n",final_digital_gain, gain_ratio);
    
    // Revert the original value before calibration back
    rg_top_a15.data = rf_read_register(RG_TOP_A15);
    rg_top_a15.b.rg_wf_adda_man_mode = rg_top_a15_i.b.rg_wf_adda_man_mode;
    rg_top_a15.b.rg_wf_dac_ampctrl_man = rg_top_a15_i.b.rg_wf_dac_ampctrl_man;
    rf_write_register(RG_TOP_A15, rg_top_a15.data);
    // Revert the original value before calibration back
    rg_top_a29.data = rf_read_register(RG_TOP_A29);
    rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RXB_EN = rg_top_a29_i.b.RG_M6_WF_TRX_HF_LDO_RXB_EN;
    rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RX1_EN = rg_top_a29_i.b.RG_M6_WF_TRX_HF_LDO_RX1_EN;
    rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_RX2_EN = rg_top_a29_i.b.RG_M6_WF_TRX_HF_LDO_RX2_EN;
    rg_top_a29.b.RG_M6_WF_TRX_LF_LDO_RX_EN = rg_top_a29_i.b.RG_M6_WF_TRX_LF_LDO_RX_EN;
    rf_write_register(RG_TOP_A29, rg_top_a29.data);
    // Revert the original value before calibration back
    rg_tx_a5.data = rf_read_register(RG_TX_A5);
    rg_tx_a5.b.RG_WF2G_TX_TSSI_EN = rg_tx_a5_i.b.RG_WF2G_TX_TSSI_EN;
    rf_write_register(RG_TX_A5, rg_tx_a5.data);
    // Revert the original value before calibration back
    rg_tx_a6.data = rf_read_register(RG_TX_A6);
    rg_tx_a6.b.RG_WF2G_TX_MAN_MODE = rg_tx_a6_i.b.RG_WF2G_TX_MAN_MODE;
    rf_write_register(RG_TX_A6, rg_tx_a6.data);
    // Revert the original value before calibration back
    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);
    // Revert the original value before calibration back
    rg_rx_a90.data = rf_read_register(RG_RX_A90);
    rg_rx_a90.b.RG_M6_WF_RTX_ABB_RX_EN = rg_rx_a90_i.b.RG_M6_WF_RTX_ABB_RX_EN;
    rg_rx_a90.b.RG_M6_WF_RX_TIA_BIAS_EN = rg_rx_a90_i.b.RG_M6_WF_RX_TIA_BIAS_EN;
    rg_rx_a90.b.RG_M6_WF_RX_RSSIPGA_BIAS_EN = rg_rx_a90_i.b.RG_M6_WF_RX_RSSIPGA_BIAS_EN;
    rg_rx_a90.b.RG_M6_WF_RX_LPF_BIAS_EN = rg_rx_a90_i.b.RG_M6_WF_RX_LPF_BIAS_EN;
    rg_rx_a90.b.RG_M6_WF_RX_TIA_EN = rg_rx_a90_i.b.RG_M6_WF_RX_TIA_EN;
    rg_rx_a90.b.RG_M6_WF_RX_RSSIPGA_EN = rg_rx_a90_i.b.RG_M6_WF_RX_RSSIPGA_EN;
    rg_rx_a90.b.RG_M6_WF_RX_LPF_EN = rg_rx_a90_i.b.RG_M6_WF_RX_LPF_EN;
    rg_rx_a90.b.RG_M6_WF_RX_LPF_I_DCOC_EN = rg_rx_a90_i.b.RG_M6_WF_RX_LPF_I_DCOC_EN;
    rg_rx_a90.b.RG_M6_WF_RX_LPF_Q_DCOC_EN = rg_rx_a90_i.b.RG_M6_WF_RX_LPF_Q_DCOC_EN;
    rf_write_register(RG_RX_A90, rg_rx_a90.data);
    // Revert the original value before calibration back
    rg_rx_a68.data = rf_read_register(RG_RX_A68);
    rg_rx_a68.b.RG_WF2G_TXLPBK2TIA_EN = rg_rx_a68_i.b.RG_WF2G_TXLPBK2TIA_EN;
    rf_write_register(RG_RX_A68, rg_rx_a68.data);
    // Revert the original value before calibration back
    rg_top_a23.data = rf_read_register(RG_TOP_A23);
    rg_top_a23.b.rg_m6_wf_radc_en = rg_top_a23_i.b.rg_m6_wf_radc_en;
    rg_top_a23.b.rg_m6_wf_wadc_enable_i = rg_top_a23_i.b.rg_m6_wf_wadc_enable_i;
    rg_top_a23.b.rg_m6_wf_wadc_enable_q = rg_top_a23_i.b.rg_m6_wf_wadc_enable_q;
    rf_write_register(RG_TOP_A23, rg_top_a23.data);
    // Revert the original value before calibration back
    rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
    rg_xmit_a46.b.rg_txpwc_comp_man_sel = rg_xmit_a46_i.b.rg_txpwc_comp_man_sel;
    fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);
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

    t9026_cali_mode_exit();
    RF_DPRINTF(RF_DEBUG_INFO, "TX TSSI calib finish \n");
}
#endif //  RF_CALI_TEST
