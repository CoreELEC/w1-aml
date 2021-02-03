
#ifdef RF_CALI_TEST
#include "rf_calibration.h"
void wf5g_txdpd(struct wf5g_txdpd_result* wf5g_txdpd_result,  U8 sync_gap, U8 tx_pa_gain, U8 tx_mxr_gain)
{
    RG_TOP_A7_FIELD_T    rg_top_a7;
    RG_SX_A25_FIELD_T    rg_sx_a25;
    RG_TX_A3_FIELD_T    rg_tx_a3;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_RX_A5_FIELD_T    rg_rx_a5;
    RG_RX_A58_FIELD_T    rg_rx_a58;
    RG_RX_A60_FIELD_T    rg_rx_a60;
    RG_XMIT_A1_FIELD_T    rg_xmit_a1;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4;
    RG_XMIT_A46_FIELD_T    rg_xmit_a46;
    RG_XMIT_A56_FIELD_T    rg_xmit_a56;
    RG_ESTI_A70_FIELD_T    rg_esti_a70;
    RG_ESTI_A87_FIELD_T    rg_esti_a87;
    RG_ESTI_A64_FIELD_T    rg_esti_a64;
    RG_ESTI_A65_FIELD_T    rg_esti_a65;
    RG_ESTI_A66_FIELD_T    rg_esti_a66;
    RG_ESTI_A89_FIELD_T    rg_esti_a89;
    RG_ESTI_A72_FIELD_T    rg_esti_a72;
    RG_ESTI_A73_FIELD_T    rg_esti_a73;
    RG_ESTI_A74_FIELD_T    rg_esti_a74;
    RG_ESTI_A75_FIELD_T    rg_esti_a75;
    RG_ESTI_A76_FIELD_T    rg_esti_a76;
    RG_ESTI_A77_FIELD_T    rg_esti_a77;
    RG_ESTI_A78_FIELD_T    rg_esti_a78;
    RG_ESTI_A79_FIELD_T    rg_esti_a79;
    RG_ESTI_A80_FIELD_T    rg_esti_a80;
    RG_ESTI_A81_FIELD_T    rg_esti_a81;
    RG_ESTI_A82_FIELD_T    rg_esti_a82;
    RG_ESTI_A83_FIELD_T    rg_esti_a83;
    RG_ESTI_A84_FIELD_T    rg_esti_a84;
    RG_ESTI_A85_FIELD_T    rg_esti_a85;
    RG_ESTI_A86_FIELD_T    rg_esti_a86;
    RG_XMIT_A57_FIELD_T    rg_xmit_a57;
    RG_XMIT_A58_FIELD_T    rg_xmit_a58;
    RG_XMIT_A59_FIELD_T    rg_xmit_a59;
    RG_XMIT_A60_FIELD_T    rg_xmit_a60;
    RG_XMIT_A61_FIELD_T    rg_xmit_a61;
    RG_XMIT_A62_FIELD_T    rg_xmit_a62;
    RG_XMIT_A63_FIELD_T    rg_xmit_a63;
    RG_XMIT_A64_FIELD_T    rg_xmit_a64;
    RG_XMIT_A65_FIELD_T    rg_xmit_a65;
    RG_XMIT_A66_FIELD_T    rg_xmit_a66;
    RG_XMIT_A67_FIELD_T    rg_xmit_a67;
    RG_XMIT_A68_FIELD_T    rg_xmit_a68;
    RG_XMIT_A69_FIELD_T    rg_xmit_a69;
    RG_XMIT_A70_FIELD_T    rg_xmit_a70;
    RG_XMIT_A71_FIELD_T    rg_xmit_a71;

    // Internal variables definitions
    bool    RG_TOP_A7_saved = False;
    RG_TOP_A7_FIELD_T    rg_top_a7_i;
    // Internal variables definitions
    bool    RG_SX_A25_saved = False;
    RG_SX_A25_FIELD_T    rg_sx_a25_i;
    // Internal variables definitions
    bool    RG_TX_A3_saved = False;
    RG_TX_A3_FIELD_T    rg_tx_a3_i;
    // Internal variables definitions
    bool    RG_RX_A2_saved = False;
    RG_RX_A2_FIELD_T    rg_rx_a2_i;
    // Internal variables definitions
    bool    RG_RX_A5_saved = False;
    RG_RX_A5_FIELD_T    rg_rx_a5_i;
    // Internal variables definitions
    bool    RG_RX_A58_saved = False;
    RG_RX_A58_FIELD_T    rg_rx_a58_i;
    // Internal variables definitions
    bool    RG_RX_A60_saved = False;
    RG_RX_A60_FIELD_T    rg_rx_a60_i;
    // Internal variables definitions
    bool    RG_XMIT_A4_saved = False;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4_i;

    unsigned int data = 0;
    unsigned int snr = 0;
    unsigned int tx_dpd_ready = 0;
    unsigned int unlock_cnt = 0;
    unsigned int TIME_OUT_CNT = 300;
    unsigned int coeff_I[15];
    unsigned int coeff_Q[15];
    unsigned int ck = 0;
    unsigned int tmp;
    int  temp_I = 0;//0.0000;
    int temp_Q = 0;//0.0000;
    bool tx_dpd_manual = 0;

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
        rg_sx_a25_i.b.RG_M2_WF_SX_LOG5G_DPDLO_EN = rg_sx_a25.b.RG_M2_WF_SX_LOG5G_DPDLO_EN;
    }
    rg_sx_a25.b.RG_M2_WF_SX_LOG5G_DPDLO_EN = 0x1;
    rf_write_register(RG_SX_A25, rg_sx_a25.data);

    rg_tx_a3.data = rf_read_register(RG_TX_A3);
    if(!RG_TX_A3_saved) {
        RG_TX_A3_saved = True;
        rg_tx_a3_i.b.RG_WF5G_TXPA_GAIN_MAN = rg_tx_a3.b.RG_WF5G_TXPA_GAIN_MAN;
        rg_tx_a3_i.b.RG_WF5G_TXM_GAIN_MAN = rg_tx_a3.b.RG_WF5G_TXM_GAIN_MAN;
        rg_tx_a3_i.b.RG_WF5G_TX_GAIN_MAN_MODE = rg_tx_a3.b.RG_WF5G_TX_GAIN_MAN_MODE;
    }
    rg_tx_a3.b.RG_WF5G_TXPA_GAIN_MAN = tx_pa_gain;
    rg_tx_a3.b.RG_WF5G_TXM_GAIN_MAN = tx_mxr_gain;
    rg_tx_a3.b.RG_WF5G_TX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_TX_A3, rg_tx_a3.data);

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    if(!RG_RX_A2_saved) {
        RG_RX_A2_saved = True;
        rg_rx_a2_i.b.RG_WF_RX_BW_SEL = rg_rx_a2.b.RG_WF_RX_BW_SEL;
        rg_rx_a2_i.b.RG_WF_RX_ADCFS_SEL = rg_rx_a2.b.RG_WF_RX_ADCFS_SEL;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE;
    }
    rg_rx_a2.b.RG_WF_RX_BW_SEL = 0x3;
    rg_rx_a2.b.RG_WF_RX_ADCFS_SEL = 0x1;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x8c;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);

    rg_rx_a5.data = rf_read_register(RG_RX_A5);
    if(!RG_RX_A5_saved) {
        RG_RX_A5_saved = True;
        rg_rx_a5_i.b.RG_M2_WF5G_RX_FE_TOPBIAS_EN = rg_rx_a5.b.RG_M2_WF5G_RX_FE_TOPBIAS_EN;
        rg_rx_a5_i.b.RG_M2_WF5G_RX_DPD_GM_BIAS_EN = rg_rx_a5.b.RG_M2_WF5G_RX_DPD_GM_BIAS_EN;
        rg_rx_a5_i.b.RG_M2_WF5G_RX_MXR_BIAS_EN = rg_rx_a5.b.RG_M2_WF5G_RX_MXR_BIAS_EN;
        rg_rx_a5_i.b.RG_M2_WF5G_RX_LNA_TRSW_EN = rg_rx_a5.b.RG_M2_WF5G_RX_LNA_TRSW_EN;
        rg_rx_a5_i.b.RG_M2_WF5G_RX_DPD_GM_EN = rg_rx_a5.b.RG_M2_WF5G_RX_DPD_GM_EN;
        rg_rx_a5_i.b.RG_M2_WF5G_RX_DPD_MXR_EN = rg_rx_a5.b.RG_M2_WF5G_RX_DPD_MXR_EN;
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
    rg_rx_a5.b.RG_M2_WF5G_RX_FE_TOPBIAS_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF5G_RX_DPD_GM_BIAS_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF5G_RX_MXR_BIAS_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF5G_RX_LNA_TRSW_EN = 0x0;
    rg_rx_a5.b.RG_M2_WF5G_RX_DPD_GM_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF5G_RX_DPD_MXR_EN = 0x1;
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

    rg_rx_a58.data = rf_read_register(RG_RX_A58);
    if(!RG_RX_A58_saved) {
        RG_RX_A58_saved = True;
        rg_rx_a58_i.b.RG_WF_RX_LPF_NOTCH_EN = rg_rx_a58.b.RG_WF_RX_LPF_NOTCH_EN;
    }
    rg_rx_a58.b.RG_WF_RX_LPF_NOTCH_EN = 0x0;
    rf_write_register(RG_RX_A58, rg_rx_a58.data);

    rg_rx_a60.data = rf_read_register(RG_RX_A60);
    if(!RG_RX_A60_saved) {
        RG_RX_A60_saved = True;
        rg_rx_a60_i.b.RG_WF_TX_LPF_GAIN = rg_rx_a60.b.RG_WF_TX_LPF_GAIN;
    }
    rg_rx_a60.b.RG_WF_TX_LPF_GAIN = 0x3;
    rf_write_register(RG_RX_A60, rg_rx_a60.data);

    rg_xmit_a1.data = fi_ahb_read(RG_XMIT_A1);
    rg_xmit_a1.b.rg_xmit_cfg1 = 0x3;                         // tx enable manual mode
    fi_ahb_write(RG_XMIT_A1, rg_xmit_a1.data);

    tmp = fi_ahb_read(AGC_AGC_CCA_CTRL);
    fi_ahb_write(AGC_AGC_CCA_CTRL, (tmp | BIT(28)));

    rg_xmit_a4.data = fi_ahb_read(RG_XMIT_A4);
    if(!RG_XMIT_A4_saved) {
        RG_XMIT_A4_saved = True;
        rg_xmit_a4_i.b.rg_tx_signal_sel = rg_xmit_a4.b.rg_tx_signal_sel;
    }
    rg_xmit_a4.b.rg_tx_signal_sel = 0x0;                         // new_set_reg(0xe410, 0x00000000); //0:select OFDM signal;1:signal tone
    fi_ahb_write(RG_XMIT_A4, rg_xmit_a4.data);

    rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
    rg_xmit_a46.b.rg_txpwc_comp_man = 0xb0;                        // i2c_set_reg_fragment(0xe4b8, 31, 24, 0xb0); //set pwtrl gain
    fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);

    rg_xmit_a56.data = fi_ahb_read(RG_XMIT_A56);
    rg_xmit_a56.b.rg_txdpd_comp_in_gain = 0x80;                        // i2c_set_reg_fragment(0xe4f0, 7, 0, 0x80);//dpd in gain
    rg_xmit_a56.b.rg_txdpd_comp_out_gain = 0x80;
    rg_xmit_a56.b.rg_txdpd_comp_type = 0x1;                         // i2c_set_reg_fragment(0xe4f0, 24, 24, 1);//set dpd comp type to 1:coeff
    rg_xmit_a56.b.rg_txdpd_comp_bypass = 0x1;                         // i2c_set_reg_fragment(0xe4f0, 26, 26, 1);//set dpd comp bypass to 1
    rg_xmit_a56.b.rg_txdpd_comp_bypass_man = 0x1;                         // i2c_set_reg_fragment(0xe4f0, 28, 28, 1);//set dpd comp bypass man to 1
    fi_ahb_write(RG_XMIT_A56, rg_xmit_a56.data);

    if (tx_dpd_manual == 0)
    {

        rg_xmit_a56.data = fi_ahb_read(RG_XMIT_A56);
        rg_xmit_a56.b.rg_txdpd_comp_coef_man_sel = 0x0;
        fi_ahb_write(RG_XMIT_A56, rg_xmit_a56.data);

    }    
    else
    {

        rg_xmit_a56.data = fi_ahb_read(RG_XMIT_A56);
        rg_xmit_a56.b.rg_txdpd_comp_coef_man_sel = 0x1;
        fi_ahb_write(RG_XMIT_A56, rg_xmit_a56.data);

    }

    rg_esti_a70.data = fi_ahb_read(RG_ESTI_A70);
    rg_esti_a70.b.rg_txdpd_esti_read_response = 0x0;
    rg_esti_a70.b.rg_txdpd_esti_read_response_bypass = 0x1;
    rg_esti_a70.b.rg_txdpd_esti_alpha = 0x1;
    rg_esti_a70.b.rg_txdpd_esti_sync_gap = sync_gap & 0x7f;
    rg_esti_a70.b.ro_txdpd_esti_ready = 0x0;
    rg_esti_a70.b.rg_txdpd_esti_mp_pow_thresh = 0x6;
    fi_ahb_write(RG_ESTI_A70, rg_esti_a70.data);

    rg_esti_a87.data = fi_ahb_read(RG_ESTI_A87);
    rg_esti_a87.b.rg_txdpd_snr_esti_alpha = 0x4;
    fi_ahb_write(RG_ESTI_A87, rg_esti_a87.data);

    rg_esti_a64.data = fi_ahb_read(RG_ESTI_A64);
    rg_esti_a64.b.rg_adda_wait_count = 0xd2;
    fi_ahb_write(RG_ESTI_A64, rg_esti_a64.data);

    rg_esti_a65.data = fi_ahb_read(RG_ESTI_A65);
    rg_esti_a65.b.rg_adda_calc_count = 0x1000;
    fi_ahb_write(RG_ESTI_A65, rg_esti_a65.data);

    rg_esti_a66.data = fi_ahb_read(RG_ESTI_A66);
    rg_esti_a66.b.rg_adda_esti_count = 0xfffffff;
    fi_ahb_write(RG_ESTI_A66, rg_esti_a66.data);

WF5G_CALI_MODE_T wf5g_cali_mode = WF5G_TXDPD;
wf5g_cali_mode_access(wf5g_cali_mode);

    rg_esti_a70.data = fi_ahb_read(RG_ESTI_A70);
    tx_dpd_ready = rg_esti_a70.b.ro_txdpd_esti_ready;
    while(unlock_cnt < TIME_OUT_CNT)
    {
        if(tx_dpd_ready == 0)
        {

            rg_esti_a70.data = fi_ahb_read(RG_ESTI_A70);
            tx_dpd_ready = rg_esti_a70.b.ro_txdpd_esti_ready;
            unlock_cnt = unlock_cnt + 1;
            st_printf("unlock_cnt:%d, TX DPD NOT DOING...\n", unlock_cnt);
        }
        else
        {
            PUTS("TX DPD FINISH\n");
            break;
        } // if(tx_dpd_ready == 0)
    } // while(unlock_cnt < TIME_OUT_CNT)

    rg_esti_a89.data = fi_ahb_read(RG_ESTI_A89);
    snr = rg_esti_a89.b.ro_txdpd_esti_snr_db;
    if(snr > 256)
    {
           snr = snr - 512;
    }
    st_printf("snr: %d,  sync_gap:%d\n", snr, sync_gap);

    rg_esti_a72.data = fi_ahb_read(RG_ESTI_A72);
    rg_esti_a73.data = fi_ahb_read(RG_ESTI_A73);
    rg_esti_a74.data = fi_ahb_read(RG_ESTI_A74);
    rg_esti_a75.data = fi_ahb_read(RG_ESTI_A75);
    rg_esti_a76.data = fi_ahb_read(RG_ESTI_A76);
    rg_esti_a77.data = fi_ahb_read(RG_ESTI_A77);
    rg_esti_a78.data = fi_ahb_read(RG_ESTI_A78);
    rg_esti_a79.data = fi_ahb_read(RG_ESTI_A79);
    rg_esti_a80.data = fi_ahb_read(RG_ESTI_A80);
    rg_esti_a81.data = fi_ahb_read(RG_ESTI_A81);
    rg_esti_a82.data = fi_ahb_read(RG_ESTI_A82);
    rg_esti_a83.data = fi_ahb_read(RG_ESTI_A83);
    rg_esti_a84.data = fi_ahb_read(RG_ESTI_A84);
    rg_esti_a85.data = fi_ahb_read(RG_ESTI_A85);
    rg_esti_a86.data = fi_ahb_read(RG_ESTI_A86);
   coeff_I[0]  = rg_esti_a72.b.ro_txdpd_esti_coef_i_0;
   coeff_Q[0]  = rg_esti_a72.b.ro_txdpd_esti_coef_q_0;
   coeff_I[1]  = rg_esti_a73.b.ro_txdpd_esti_coef_i_1;
   coeff_Q[1]  = rg_esti_a73.b.ro_txdpd_esti_coef_q_1;
   coeff_I[2]  = rg_esti_a74.b.ro_txdpd_esti_coef_i_2;
   coeff_Q[2]  = rg_esti_a74.b.ro_txdpd_esti_coef_q_2;
   coeff_I[3]  = rg_esti_a75.b.ro_txdpd_esti_coef_i_3;
   coeff_Q[3]  = rg_esti_a75.b.ro_txdpd_esti_coef_q_3;
   coeff_I[4]  = rg_esti_a76.b.ro_txdpd_esti_coef_i_4;
   coeff_Q[4]  = rg_esti_a76.b.ro_txdpd_esti_coef_q_4;
   coeff_I[5]  = rg_esti_a77.b.ro_txdpd_esti_coef_i_5;
   coeff_Q[5]  = rg_esti_a77.b.ro_txdpd_esti_coef_q_5;
   coeff_I[6]  = rg_esti_a78.b.ro_txdpd_esti_coef_i_6;
   coeff_Q[6]  = rg_esti_a78.b.ro_txdpd_esti_coef_q_6;
   coeff_I[7]  = rg_esti_a79.b.ro_txdpd_esti_coef_i_7;
   coeff_Q[7]  = rg_esti_a79.b.ro_txdpd_esti_coef_q_7;
   coeff_I[8]  = rg_esti_a80.b.ro_txdpd_esti_coef_i_8;
   coeff_Q[8]  = rg_esti_a80.b.ro_txdpd_esti_coef_q_8;
   coeff_I[9]  = rg_esti_a81.b.ro_txdpd_esti_coef_i_9;
   coeff_Q[9]  = rg_esti_a81.b.ro_txdpd_esti_coef_q_9;
   coeff_I[10]  = rg_esti_a82.b.ro_txdpd_esti_coef_i_a;
   coeff_Q[10]  = rg_esti_a82.b.ro_txdpd_esti_coef_q_a;
   coeff_I[11]  = rg_esti_a83.b.ro_txdpd_esti_coef_i_b;
   coeff_Q[11]  = rg_esti_a83.b.ro_txdpd_esti_coef_q_b;
   coeff_I[12]  = rg_esti_a84.b.ro_txdpd_esti_coef_i_c;
   coeff_Q[12]  = rg_esti_a84.b.ro_txdpd_esti_coef_q_c;
   coeff_I[13]  = rg_esti_a85.b.ro_txdpd_esti_coef_i_d;
   coeff_Q[13]  = rg_esti_a85.b.ro_txdpd_esti_coef_q_d;
   coeff_I[14]  = rg_esti_a86.b.ro_txdpd_esti_coef_i_e;
   coeff_Q[14]  = rg_esti_a86.b.ro_txdpd_esti_coef_q_e;



    rg_xmit_a57.data = fi_ahb_read(RG_XMIT_A57);
    rg_xmit_a57.b.rg_txdpd_comp_coef_i_0_man = coeff_I[0];
    rg_xmit_a57.b.rg_txdpd_comp_coef_q_0_man = coeff_Q[0];
    fi_ahb_write(RG_XMIT_A57, rg_xmit_a57.data);

    rg_xmit_a58.data = fi_ahb_read(RG_XMIT_A58);
    rg_xmit_a58.b.rg_txdpd_comp_coef_i_1_man = coeff_I[1];
    rg_xmit_a58.b.rg_txdpd_comp_coef_q_1_man = coeff_Q[1];
    fi_ahb_write(RG_XMIT_A58, rg_xmit_a58.data);

    rg_xmit_a59.data = fi_ahb_read(RG_XMIT_A59);
    rg_xmit_a59.b.rg_txdpd_comp_coef_i_2_man = coeff_I[2];
    rg_xmit_a59.b.rg_txdpd_comp_coef_q_2_man = coeff_Q[2];
    fi_ahb_write(RG_XMIT_A59, rg_xmit_a59.data);

    rg_xmit_a60.data = fi_ahb_read(RG_XMIT_A60);
    rg_xmit_a60.b.rg_txdpd_comp_coef_i_3_man = coeff_I[3];
    rg_xmit_a60.b.rg_txdpd_comp_coef_q_3_man = coeff_Q[3];
    fi_ahb_write(RG_XMIT_A60, rg_xmit_a60.data);

    rg_xmit_a61.data = fi_ahb_read(RG_XMIT_A61);
    rg_xmit_a61.b.rg_txdpd_comp_coef_i_4_man = coeff_I[4];
    rg_xmit_a61.b.rg_txdpd_comp_coef_q_4_man = coeff_Q[4];
    fi_ahb_write(RG_XMIT_A61, rg_xmit_a61.data);

    rg_xmit_a62.data = fi_ahb_read(RG_XMIT_A62);
    rg_xmit_a62.b.rg_txdpd_comp_coef_i_5_man = coeff_I[5];
    rg_xmit_a62.b.rg_txdpd_comp_coef_q_5_man = coeff_Q[5];
    fi_ahb_write(RG_XMIT_A62, rg_xmit_a62.data);

    rg_xmit_a63.data = fi_ahb_read(RG_XMIT_A63);
    rg_xmit_a63.b.rg_txdpd_comp_coef_i_6_man = coeff_I[6];
    rg_xmit_a63.b.rg_txdpd_comp_coef_q_6_man = coeff_Q[6];
    fi_ahb_write(RG_XMIT_A63, rg_xmit_a63.data);

    rg_xmit_a64.data = fi_ahb_read(RG_XMIT_A64);
    rg_xmit_a64.b.rg_txdpd_comp_coef_i_7_man = coeff_I[7];
    rg_xmit_a64.b.rg_txdpd_comp_coef_q_7_man = coeff_Q[7];
    fi_ahb_write(RG_XMIT_A64, rg_xmit_a64.data);

    rg_xmit_a65.data = fi_ahb_read(RG_XMIT_A65);
    rg_xmit_a65.b.rg_txdpd_comp_coef_i_8_man = coeff_I[8];
    rg_xmit_a65.b.rg_txdpd_comp_coef_q_8_man = coeff_Q[8];
    fi_ahb_write(RG_XMIT_A65, rg_xmit_a65.data);

    rg_xmit_a66.data = fi_ahb_read(RG_XMIT_A66);
    rg_xmit_a66.b.rg_txdpd_comp_coef_i_9_man = coeff_I[9];
    rg_xmit_a66.b.rg_txdpd_comp_coef_q_9_man = coeff_Q[9];
    fi_ahb_write(RG_XMIT_A66, rg_xmit_a66.data);

    rg_xmit_a67.data = fi_ahb_read(RG_XMIT_A67);
    rg_xmit_a67.b.rg_txdpd_comp_coef_i_a_man = coeff_I[10];
    rg_xmit_a67.b.rg_txdpd_comp_coef_q_a_man = coeff_Q[10];
    fi_ahb_write(RG_XMIT_A67, rg_xmit_a67.data);

    rg_xmit_a68.data = fi_ahb_read(RG_XMIT_A68);
    rg_xmit_a68.b.rg_txdpd_comp_coef_i_b_man = coeff_I[11];
    rg_xmit_a68.b.rg_txdpd_comp_coef_q_b_man = coeff_Q[11];
    fi_ahb_write(RG_XMIT_A68, rg_xmit_a68.data);

    rg_xmit_a69.data = fi_ahb_read(RG_XMIT_A69);
    rg_xmit_a69.b.rg_txdpd_comp_coef_i_c_man = coeff_I[12];
    rg_xmit_a69.b.rg_txdpd_comp_coef_q_c_man = coeff_Q[12];
    fi_ahb_write(RG_XMIT_A69, rg_xmit_a69.data);

    rg_xmit_a70.data = fi_ahb_read(RG_XMIT_A70);
    rg_xmit_a70.b.rg_txdpd_comp_coef_i_d_man = coeff_I[13];
    rg_xmit_a70.b.rg_txdpd_comp_coef_q_d_man = coeff_Q[13];
    fi_ahb_write(RG_XMIT_A70, rg_xmit_a70.data);

    rg_xmit_a71.data = fi_ahb_read(RG_XMIT_A71);
    rg_xmit_a71.b.rg_txdpd_comp_coef_i_e_man = coeff_I[14];
    rg_xmit_a71.b.rg_txdpd_comp_coef_q_e_man = coeff_Q[14];
    fi_ahb_write(RG_XMIT_A71, rg_xmit_a71.data);

    for(ck = 0; ck < 15; ck++)
    {
        wf5g_txdpd_result->cal_tx_dpd.tx_dpd_coeff_i[ck] = coeff_I[ck];
        wf5g_txdpd_result->cal_tx_dpd.tx_dpd_coeff_q[ck] = coeff_Q[ck];
    }
    wf5g_txdpd_result->cal_tx_dpd.snr = snr;

    for(ck = 0; ck < 15; ck++)
    {
        if(coeff_I[ck] > (1<<11)){
            temp_I = ((int)coeff_I[ck] - (int)(1<<12));
        }
        else{ 
            temp_I = coeff_I[ck]; 
        }
        temp_I = temp_I / ( (int)(1 <<10));
        
        if(coeff_Q[ck] > (1<<11)){
            temp_Q = (int)coeff_Q[ck] - (int)(1<<12);
        }
        else{ 
            temp_Q = (int)coeff_Q[ck]; 
        }
        temp_Q = temp_Q / (int)(1 <<10);

    } // for(ck = 0; ck < 15; ck++)
    st_printf("snr: %d\n", snr);

    rg_xmit_a56.data = fi_ahb_read(RG_XMIT_A56);
    rg_xmit_a56.b.rg_txdpd_comp_bypass = 0x0;
    fi_ahb_write(RG_XMIT_A56, rg_xmit_a56.data);

    // Revert the original value before calibration back
    rg_top_a7.data = rf_read_register(RG_TOP_A7);
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX1_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RX1_EN;
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX2_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RX2_EN;
    rg_top_a7.b.RG_M2_WF_TRX_LF_LDO_RX_EN = rg_top_a7_i.b.RG_M2_WF_TRX_LF_LDO_RX_EN;
    rf_write_register(RG_TOP_A7, rg_top_a7.data);
    // Revert the original value before calibration back
    rg_sx_a25.data = rf_read_register(RG_SX_A25);
    rg_sx_a25.b.RG_M2_WF_SX_LOG5G_DPDLO_EN = rg_sx_a25_i.b.RG_M2_WF_SX_LOG5G_DPDLO_EN;
    rf_write_register(RG_SX_A25, rg_sx_a25.data);
    // Revert the original value before calibration back
    rg_tx_a3.data = rf_read_register(RG_TX_A3);
    rg_tx_a3.b.RG_WF5G_TXPA_GAIN_MAN = rg_tx_a3_i.b.RG_WF5G_TXPA_GAIN_MAN;
    rg_tx_a3.b.RG_WF5G_TXM_GAIN_MAN = rg_tx_a3_i.b.RG_WF5G_TXM_GAIN_MAN;
    rg_tx_a3.b.RG_WF5G_TX_GAIN_MAN_MODE = rg_tx_a3_i.b.RG_WF5G_TX_GAIN_MAN_MODE;
    rf_write_register(RG_TX_A3, rg_tx_a3.data);
    // Revert the original value before calibration back
    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    rg_rx_a2.b.RG_WF_RX_BW_SEL = rg_rx_a2_i.b.RG_WF_RX_BW_SEL;
    rg_rx_a2.b.RG_WF_RX_ADCFS_SEL = rg_rx_a2_i.b.RG_WF_RX_ADCFS_SEL;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);
    // Revert the original value before calibration back
    rg_rx_a5.data = rf_read_register(RG_RX_A5);
    rg_rx_a5.b.RG_M2_WF5G_RX_FE_TOPBIAS_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_FE_TOPBIAS_EN;
    rg_rx_a5.b.RG_M2_WF5G_RX_DPD_GM_BIAS_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_DPD_GM_BIAS_EN;
    rg_rx_a5.b.RG_M2_WF5G_RX_MXR_BIAS_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_MXR_BIAS_EN;
    rg_rx_a5.b.RG_M2_WF5G_RX_LNA_TRSW_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_LNA_TRSW_EN;
    rg_rx_a5.b.RG_M2_WF5G_RX_DPD_GM_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_DPD_GM_EN;
    rg_rx_a5.b.RG_M2_WF5G_RX_DPD_MXR_EN = rg_rx_a5_i.b.RG_M2_WF5G_RX_DPD_MXR_EN;
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
    rg_rx_a58.data = rf_read_register(RG_RX_A58);
    rg_rx_a58.b.RG_WF_RX_LPF_NOTCH_EN = rg_rx_a58_i.b.RG_WF_RX_LPF_NOTCH_EN;
    rf_write_register(RG_RX_A58, rg_rx_a58.data);
    // Revert the original value before calibration back
    rg_rx_a60.data = rf_read_register(RG_RX_A60);
    rg_rx_a60.b.RG_WF_TX_LPF_GAIN = rg_rx_a60_i.b.RG_WF_TX_LPF_GAIN;
    rf_write_register(RG_RX_A60, rg_rx_a60.data);
    // Revert the original value before calibration back
    rg_xmit_a4.data = fi_ahb_read(RG_XMIT_A4);
    rg_xmit_a4.b.rg_tx_signal_sel = rg_xmit_a4_i.b.rg_tx_signal_sel;
    fi_ahb_write(RG_XMIT_A4, rg_xmit_a4.data);
    tmp = fi_ahb_read(AGC_AGC_CCA_CTRL);
    fi_ahb_write(AGC_AGC_CCA_CTRL, (tmp & ~BIT(28)));
    PUTS("TX DPD calib finish \n");
}

#endif //RF_CALI_TEST
