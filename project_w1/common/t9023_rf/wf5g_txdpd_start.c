
#ifdef RF_CALI_TEST
#include "rf_calibration.h"
void wf5g_txdpd_start(U8 sync_gap, U8 tx_pa_gain, U8 tx_mxr_gain)
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
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX1_EN = 0x1;
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX2_EN = 0x1;
    rg_top_a7.b.RG_M2_WF_TRX_LF_LDO_RX_EN = 0x1;
    rf_write_register(RG_TOP_A7, rg_top_a7.data);

    rg_sx_a25.data = rf_read_register(RG_SX_A25);
    rg_sx_a25.b.RG_M2_WF_SX_LOG5G_DPDLO_EN = 0x1;
    rf_write_register(RG_SX_A25, rg_sx_a25.data);

    rg_tx_a3.data = rf_read_register(RG_TX_A3);
    rg_tx_a3.b.RG_WF5G_TXPA_GAIN_MAN = tx_pa_gain;
    rg_tx_a3.b.RG_WF5G_TXM_GAIN_MAN = tx_mxr_gain;
    rg_tx_a3.b.RG_WF5G_TX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_TX_A3, rg_tx_a3.data);

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    rg_rx_a2.b.RG_WF_RX_BW_SEL = 0x3;
    rg_rx_a2.b.RG_WF_RX_ADCFS_SEL = 0x1;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x8c;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);

    rg_rx_a5.data = rf_read_register(RG_RX_A5);
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
    rg_rx_a58.b.RG_WF_RX_LPF_NOTCH_EN = 0x0;
    rf_write_register(RG_RX_A58, rg_rx_a58.data);

    rg_rx_a60.data = rf_read_register(RG_RX_A60);
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
    rg_esti_a70.b.rg_txdpd_esti_read_response_bypass = 0x0;
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

    CALI_MODE_T cali_mode = TXDPD;
    cali_mode_access(cali_mode);

    // Revert the original value before calibration back
    rg_xmit_a4.data = fi_ahb_read(RG_XMIT_A4);
    rg_xmit_a4.b.rg_tx_signal_sel = rg_xmit_a4_i.b.rg_tx_signal_sel;
    fi_ahb_write(RG_XMIT_A4, rg_xmit_a4.data);
}

#endif //RF_CALI_TEST
