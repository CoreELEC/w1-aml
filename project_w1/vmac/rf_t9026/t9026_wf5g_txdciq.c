#include "rf_calibration.h"

#ifdef RF_CALI_TEST

void t9026_wf5g_txiqdc(struct wf5g_txdciq_result* wf5g_txdciq_result, U8 tx_pa_gain, U8 tx_mxr_gain, bool check_flg)
{
    RG_TOP_A7_FIELD_T    rg_top_a7;
    RG_TX_A2_FIELD_T    rg_tx_a2;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_RX_A5_FIELD_T    rg_rx_a5;
    RG_TOP_A15_FIELD_T    rg_top_a15;
    RG_TOP_A19_FIELD_T    rg_top_a19;
    RG_RX_A68_FIELD_T    rg_rx_a68;
    RG_XMIT_A34_FIELD_T    rg_xmit_a34;
    RG_XMIT_A45_FIELD_T    rg_xmit_a45;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4;
    RG_XMIT_A3_FIELD_T    rg_xmit_a3;
    RG_ESTI_A20_FIELD_T    rg_esti_a20;
    RG_XMIT_A56_FIELD_T    rg_xmit_a56;
    RG_RECV_A11_FIELD_T    rg_recv_a11;
    RG_XMIT_A46_FIELD_T    rg_xmit_a46;
    RG_XMIT_A106_FIELD_T    rg_xmit_a106;
    RG_ESTI_A21_FIELD_T    rg_esti_a21;
    RG_ESTI_A22_FIELD_T    rg_esti_a22;
    RG_ESTI_A23_FIELD_T    rg_esti_a23;
    RG_ESTI_A64_FIELD_T    rg_esti_a64;
    RG_ESTI_A65_FIELD_T    rg_esti_a65;
    RG_ESTI_A14_FIELD_T    rg_esti_a14;
    RG_ESTI_A19_FIELD_T    rg_esti_a19;


   // Internal variables definitions
    bool    RG_TOP_A7_saved = False;
    RG_TOP_A7_FIELD_T    rg_top_a7_i;
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
    bool    RG_TOP_A15_saved = False;
    RG_TOP_A15_FIELD_T    rg_top_a15_i;
    // Internal variables definitions
    bool    RG_TOP_A19_saved = False;
    RG_TOP_A19_FIELD_T    rg_top_a19_i;
    // Internal variables definitions
    bool    RG_RX_A68_saved = False;
    RG_RX_A68_FIELD_T    rg_rx_a68_i;
    // Internal variables definitions
    bool    RG_XMIT_A4_saved = False;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4_i;
    // Internal variables definitions
    bool    RG_XMIT_A3_saved = False;
    RG_XMIT_A3_FIELD_T    rg_xmit_a3_i;
    // Internal variables definitions
    bool    RG_XMIT_A56_saved = False;
    RG_XMIT_A56_FIELD_T    rg_xmit_a56_i;
    // Internal variables definitions
    bool    RG_RECV_A11_saved = False;
    RG_RECV_A11_FIELD_T    rg_recv_a11_i;
    // Internal variables definitions
    bool    RG_XMIT_A46_saved = False;
    RG_XMIT_A46_FIELD_T    rg_xmit_a46_i;
    // Internal variables definitions
    bool    RG_XMIT_A106_saved = False;
    RG_XMIT_A106_FIELD_T    rg_xmit_a106_i;
    // Internal variables definitions
    bool    RG_ESTI_A14_saved = False;
    RG_ESTI_A14_FIELD_T    rg_esti_a14_i;



    unsigned int dc_rdy = 0;
    unsigned int irr_rdy = 0;
    unsigned int unlock_cnt = 0;
    unsigned int TIME_OUT_CNT = RF_TIME_OUT_CNT;

    unsigned int dci = 0;
    unsigned int dcq = 0;
    unsigned int dc_tmp = 0;
    unsigned int alpha = 0;
    unsigned int theta = 0;

    struct wf5g_rxdcoc_in wf5g_rxdcoc_in;
    struct wf5g_rxdcoc_result wf5g_rxdcoc_result;
    CALI_MODE_T cali_mode = TXDCIQ;

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

    rg_tx_a2.data = rf_read_register(RG_TX_A2);
    if(!RG_TX_A2_saved) {
        RG_TX_A2_saved = True;
        rg_tx_a2_i.b.RG_WF5G_TX_IQCAL_EN = rg_tx_a2.b.RG_WF5G_TX_IQCAL_EN;
        rg_tx_a2_i.b.RG_WF5G_TX_PA_GAIN_MAN = rg_tx_a2.b.RG_WF5G_TX_PA_GAIN_MAN;
        rg_tx_a2_i.b.RG_WF5G_TX_MXR_GAIN_MAN = rg_tx_a2.b.RG_WF5G_TX_MXR_GAIN_MAN;
        rg_tx_a2_i.b.RG_WF5G_TX_MAN_MODE = rg_tx_a2.b.RG_WF5G_TX_MAN_MODE;
    }
    rg_tx_a2.b.RG_WF5G_TX_IQCAL_EN = 0x1;
    rg_tx_a2.b.RG_WF5G_TX_PA_GAIN_MAN = tx_pa_gain;
    rg_tx_a2.b.RG_WF5G_TX_MXR_GAIN_MAN = tx_mxr_gain;
    rg_tx_a2.b.RG_WF5G_TX_MAN_MODE = 0x1;
    rf_write_register(RG_TX_A2, rg_tx_a2.data);

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    if(!RG_RX_A2_saved) {
        RG_RX_A2_saved = True;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE;
    }
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x83;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);

    rg_rx_a5.data = rf_read_register(RG_RX_A5);
    if(!RG_RX_A5_saved) {
        RG_RX_A5_saved = True;
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

    rg_top_a15.data = rf_read_register(RG_TOP_A15);
    if(!RG_TOP_A15_saved) {
        RG_TOP_A15_saved = True;
        rg_top_a15_i.b.rg_wf_adda_man_mode = rg_top_a15.b.rg_wf_adda_man_mode;
    }
    rg_top_a15.b.rg_wf_adda_man_mode = 0x1;
    rf_write_register(RG_TOP_A15, rg_top_a15.data);

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

    rg_rx_a68.data = rf_read_register(RG_RX_A68);
    if(!RG_RX_A68_saved) {
        RG_RX_A68_saved = True;
        rg_rx_a68_i.b.RG_WF5G_TXLPBK2TIA_EN = rg_rx_a68.b.RG_WF5G_TXLPBK2TIA_EN;
    }
    rg_rx_a68.b.RG_WF5G_TXLPBK2TIA_EN = 0x1;
    rf_write_register(RG_RX_A68, rg_rx_a68.data);

    wf5g_rxdcoc_in.bw_idx = 0;
    wf5g_rxdcoc_in.tia_idx = 0;
    wf5g_rxdcoc_in.lpf_idx = 1;
    wf5g_rxdcoc_in.wf_rx_gain = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
    wf5g_rxdcoc_in.manual_mode = 1;
    t9026_wf5g_rxdcoc(&wf5g_rxdcoc_in, &wf5g_rxdcoc_result);

    fi_ahb_write(PHY_TX_LPF_BYPASS, 0x1);

    rg_xmit_a34.data = fi_ahb_read(RG_XMIT_A34);
    rg_xmit_a34.b.rg_tx_iqmm_bypass = 0x0;
    fi_ahb_write(RG_XMIT_A34, rg_xmit_a34.data);

    rg_xmit_a45.data = fi_ahb_read(RG_XMIT_A45);
    rg_xmit_a45.b.rg_tx_pwr_sel_man_en = 0x0;
    fi_ahb_write(RG_XMIT_A45, rg_xmit_a45.data);

    rg_xmit_a4.data = fi_ahb_read(RG_XMIT_A4);
    if(!RG_XMIT_A4_saved) {
        RG_XMIT_A4_saved = True;
        rg_xmit_a4_i.b.rg_tx_signal_sel = rg_xmit_a4.b.rg_tx_signal_sel;
    }
    rg_xmit_a4.b.rg_tx_signal_sel = 0x1;                         // new_set_reg(0xe410, 0x00000001);//1: select single tone
    fi_ahb_write(RG_XMIT_A4, rg_xmit_a4.data);

    rg_xmit_a3.data = fi_ahb_read(RG_XMIT_A3);
    if(!RG_XMIT_A3_saved) {
        RG_XMIT_A3_saved = True;
        rg_xmit_a3_i.b.rg_tg2_f_sel = rg_xmit_a3.b.rg_tg2_f_sel;
        rg_xmit_a3_i.b.rg_tg2_enable = rg_xmit_a3.b.rg_tg2_enable;
        rg_xmit_a3_i.b.rg_tg2_tone_man_en = rg_xmit_a3.b.rg_tg2_tone_man_en;
    }
    rg_xmit_a3.b.rg_tg2_f_sel = 0x200000;
    rg_xmit_a3.b.rg_tg2_enable = 0x1;
    rg_xmit_a3.b.rg_tg2_tone_man_en = 0x1;
    fi_ahb_write(RG_XMIT_A3, rg_xmit_a3.data);

    rg_esti_a20.data = fi_ahb_read(RG_ESTI_A20);
    rg_esti_a20.b.rg_txirr_read_response_bypass = 0x1;
    rg_esti_a20.b.rg_txirr_read_response = 0x0;
    fi_ahb_write(RG_ESTI_A20, rg_esti_a20.data);

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

    rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
    if(!RG_XMIT_A46_saved) {
        RG_XMIT_A46_saved = True;
        rg_xmit_a46_i.b.rg_txpwc_comp_man_sel = rg_xmit_a46.b.rg_txpwc_comp_man_sel;
        rg_xmit_a46_i.b.rg_txpwc_comp_man = rg_xmit_a46.b.rg_txpwc_comp_man;
    }
    rg_xmit_a46.b.rg_txpwc_comp_man_sel = 0x1;
    rg_xmit_a46.b.rg_txpwc_comp_man = 0x20;//0x10;
    fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);

    rg_xmit_a106.data = fi_ahb_read(RG_XMIT_A106);
    if(!RG_XMIT_A106_saved) {
        RG_XMIT_A106_saved = True;
        rg_xmit_a106_i.b.rg_tx_atten_factor = rg_xmit_a106.b.rg_tx_atten_factor;
    }
    rg_xmit_a106.b.rg_tx_atten_factor = 0x8;
    fi_ahb_write(RG_XMIT_A106, rg_xmit_a106.data);

    rg_esti_a21.data = fi_ahb_read(RG_ESTI_A21);
    rg_esti_a21.b.rg_tx_dcmax_i = 0x3f;
    rg_esti_a21.b.rg_tx_dcmax_q = 0x3f;
    rg_esti_a21.b.rg_tx_alpha_max = 0x7f;
    rg_esti_a21.b.rg_tx_theta_max = 0x7f;
    fi_ahb_write(RG_ESTI_A21, rg_esti_a21.data);

    rg_esti_a22.data = fi_ahb_read(RG_ESTI_A22);
    rg_esti_a22.b.rg_tx_dcmin_i = 0xc0;
    rg_esti_a22.b.rg_tx_dcmin_q = 0xc0;
    rg_esti_a22.b.rg_tx_alpha_min = 0x80;
    rg_esti_a22.b.rg_tx_theta_min = 0x80;
    fi_ahb_write(RG_ESTI_A22, rg_esti_a22.data);

    rg_esti_a23.data = fi_ahb_read(RG_ESTI_A23);
    rg_esti_a23.b.rg_tx_dc_i_step = 0x4;
    rg_esti_a23.b.rg_tx_dc_q_step = 0x4;
    rg_esti_a23.b.rg_tx_alpha_step = 0x4;
    rg_esti_a23.b.rg_tx_theta_step = 0x4;
    rg_esti_a23.b.rg_txirr_oper_bnd = 0x2;    //0x4
    fi_ahb_write(RG_ESTI_A23, rg_esti_a23.data);

    rg_esti_a64.data = fi_ahb_read(RG_ESTI_A64);
    rg_esti_a64.b.rg_adda_wait_count = 0x1ff;
    fi_ahb_write(RG_ESTI_A64, rg_esti_a64.data);

    rg_esti_a65.data = fi_ahb_read(RG_ESTI_A65);
    rg_esti_a65.b.rg_adda_calc_count = 0x1000;
    fi_ahb_write(RG_ESTI_A65, rg_esti_a65.data);

    rg_esti_a14.data = fi_ahb_read(RG_ESTI_A14);
    if(!RG_ESTI_A14_saved) {
        RG_ESTI_A14_saved = True;
        rg_esti_a14_i.b.rg_dc_rm_leaky_factor = rg_esti_a14.b.rg_dc_rm_leaky_factor;
    }
    rg_esti_a14.b.rg_dc_rm_leaky_factor = 0x6;
    fi_ahb_write(RG_ESTI_A14, rg_esti_a14.data);


    t9026_cali_mode_access(cali_mode);

    rg_esti_a19.data = fi_ahb_read(RG_ESTI_A19);
    rg_esti_a20.data = fi_ahb_read(RG_ESTI_A20);
    dc_rdy = rg_esti_a19.b.ro_txirr_dc_ready;
    irr_rdy = rg_esti_a20.b.ro_txirr_irr_ready;
    while(unlock_cnt < TIME_OUT_CNT)
    {
        if(dc_rdy == 0 || irr_rdy == 0)
        {
            //msleep(1);

            rg_esti_a19.data = fi_ahb_read(RG_ESTI_A19);
            rg_esti_a20.data = fi_ahb_read(RG_ESTI_A20);

            dc_rdy = rg_esti_a19.b.ro_txirr_dc_ready;
            irr_rdy = rg_esti_a20.b.ro_txirr_irr_ready;
            unlock_cnt = unlock_cnt + 1;
            RF_DPRINTF(RF_DEBUG_INFO, "unlock_cnt:%d, TX DCIQ  NOT READY...\n", unlock_cnt);
        }
        else
        {
            RF_DPRINTF(RF_DEBUG_INFO, "TX DCIQ READY\n");
            break;
        } //  if(dc_rdy == 0|| irr_rdy == 0)
    } // while(unlock_cnt < TIME_OUT_CNT)

    rg_esti_a19.data = fi_ahb_read(RG_ESTI_A19);
    rg_esti_a20.data = fi_ahb_read(RG_ESTI_A20);
    dci = rg_esti_a19.b.ro_txirr_dc_i;
    dcq =  rg_esti_a19.b.ro_txirr_dc_q;
    alpha = rg_esti_a20.b.ro_txirr_alpha;
    theta = rg_esti_a20.b.ro_txirr_theta;
    dc_tmp = (0x80 << 16) | (dcq << 8) | dci;

    rg_xmit_a45.data = fi_ahb_read(RG_XMIT_A45);
    rg_xmit_a45.b.rg_man_tx_pwrlvl = dc_tmp;                      // i2c_set_reg_fragment(0xe4b4,  7, 0, dci);  //set esti DC_I
//i2c_set_reg_fragment(0xe4b4, 15, 8, dcq);  //set esti DC_Q
    rg_xmit_a45.b.rg_tx_pwr_sel_man_en = 0x1;                         // i2c_set_reg_fragment(0xe4b4, 31, 31, 1);
//set TX DC/IRR comp manual mode to 1
    fi_ahb_write(RG_XMIT_A45, rg_xmit_a45.data);

    rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
    rg_xmit_a46.b.rg_man_txirr_comp_theta = theta;                       // i2c_set_reg_fragment(0xe4b8,  7, 0, theta);//set esti theta
    rg_xmit_a46.b.rg_man_txirr_comp_alpha = alpha;                       // i2c_set_reg_fragment(0xe4b8, 15, 8, alpha);//set esti alpha
    fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);

    if(check_flg == 1)
    {
        t9026_wf5g_txiqdc_check();
    }

    // Revert the original value before calibration back
    rg_top_a7.data = rf_read_register(RG_TOP_A7);
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RXB_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RXB_EN;
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX1_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RX1_EN;
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX2_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RX2_EN;
    rg_top_a7.b.RG_M2_WF_TRX_LF_LDO_RX_EN = rg_top_a7_i.b.RG_M2_WF_TRX_LF_LDO_RX_EN;
    rf_write_register(RG_TOP_A7, rg_top_a7.data);
    // Revert the original value before calibration back
    rg_tx_a2.data = rf_read_register(RG_TX_A2);
    rg_tx_a2.b.RG_WF5G_TX_IQCAL_EN = rg_tx_a2_i.b.RG_WF5G_TX_IQCAL_EN;
    rg_tx_a2.b.RG_WF5G_TX_PA_GAIN_MAN = rg_tx_a2_i.b.RG_WF5G_TX_PA_GAIN_MAN;
    rg_tx_a2.b.RG_WF5G_TX_MXR_GAIN_MAN = rg_tx_a2_i.b.RG_WF5G_TX_MXR_GAIN_MAN;
    rg_tx_a2.b.RG_WF5G_TX_MAN_MODE = rg_tx_a2_i.b.RG_WF5G_TX_MAN_MODE;
    rf_write_register(RG_TX_A2, rg_tx_a2.data);
    // Revert the original value before calibration back
    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);
    // Revert the original value before calibration back
    rg_rx_a5.data = rf_read_register(RG_RX_A5);
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
    rg_top_a15.data = rf_read_register(RG_TOP_A15);
    rg_top_a15.b.rg_wf_adda_man_mode = rg_top_a15_i.b.rg_wf_adda_man_mode;
    rf_write_register(RG_TOP_A15, rg_top_a15.data);
    // Revert the original value before calibration back
    rg_top_a19.data = rf_read_register(RG_TOP_A19);
    rg_top_a19.b.rg_m2_wf_radc_en = rg_top_a19_i.b.rg_m2_wf_radc_en;
    rg_top_a19.b.rg_m2_wf_wadc_enable_i = rg_top_a19_i.b.rg_m2_wf_wadc_enable_i;
    rg_top_a19.b.rg_m2_wf_wadc_enable_q = rg_top_a19_i.b.rg_m2_wf_wadc_enable_q;
    rf_write_register(RG_TOP_A19, rg_top_a19.data);
    // Revert the original value before calibration back
    rg_rx_a68.data = rf_read_register(RG_RX_A68);
    rg_rx_a68.b.RG_WF5G_TXLPBK2TIA_EN = rg_rx_a68_i.b.RG_WF5G_TXLPBK2TIA_EN;
    rf_write_register(RG_RX_A68, rg_rx_a68.data);
    // Revert the original value before calibration back
    rg_xmit_a4.data = fi_ahb_read(RG_XMIT_A4);
    rg_xmit_a4.b.rg_tx_signal_sel = rg_xmit_a4_i.b.rg_tx_signal_sel;
    fi_ahb_write(RG_XMIT_A4, rg_xmit_a4.data);
    // Revert the original value before calibration back
    rg_xmit_a3.data = fi_ahb_read(RG_XMIT_A3);
    rg_xmit_a3.b.rg_tg2_f_sel = rg_xmit_a3_i.b.rg_tg2_f_sel;
    rg_xmit_a3.b.rg_tg2_enable = rg_xmit_a3_i.b.rg_tg2_enable;
    rg_xmit_a3.b.rg_tg2_tone_man_en = rg_xmit_a3_i.b.rg_tg2_tone_man_en;
    fi_ahb_write(RG_XMIT_A3, rg_xmit_a3.data);
    // Revert the original value before calibration back
    rg_xmit_a56.data = fi_ahb_read(RG_XMIT_A56);
    rg_xmit_a56.b.rg_txdpd_comp_bypass = rg_xmit_a56_i.b.rg_txdpd_comp_bypass;
    fi_ahb_write(RG_XMIT_A56, rg_xmit_a56.data);
    // Revert the original value before calibration back
    rg_recv_a11.data = fi_ahb_read(RG_RECV_A11);
    rg_recv_a11.b.rg_rxirr_bypass = rg_recv_a11_i.b.rg_rxirr_bypass;
    fi_ahb_write(RG_RECV_A11, rg_recv_a11.data);
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
    rg_esti_a14.data = fi_ahb_read(RG_ESTI_A14);
    rg_esti_a14.b.rg_dc_rm_leaky_factor = rg_esti_a14_i.b.rg_dc_rm_leaky_factor;
    fi_ahb_write(RG_ESTI_A14, rg_esti_a14.data);
    wf5g_txdciq_result->cal_tx_dciq.tx_dc_i = dci;
    wf5g_txdciq_result->cal_tx_dciq.tx_dc_q = dcq;
    wf5g_txdciq_result->cal_tx_dciq.tx_alpha = alpha;
    wf5g_txdciq_result->cal_tx_dciq.tx_theta = theta;
    RF_DPRINTF(RF_DEBUG_INFO, "----------------------> TX DC&IRR out\n");
    RF_DPRINTF(RF_DEBUG_RESULT, "5g unlock_cnt:%d, dci: 0x%x dcq: 0x%x alpha:0x%x  theta:0x%x\n", unlock_cnt, dci, dcq, alpha, theta);
    t9026_cali_mode_exit();
    RF_DPRINTF(RF_DEBUG_INFO, "TX DC&IRR calib finish \n");
}

#endif // RF_CALI_TEST

