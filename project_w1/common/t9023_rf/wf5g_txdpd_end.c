
#ifdef RF_CALI_TEST
#include "rf_calibration.h"
void wf5g_txdpd_end(struct wf5g_txdpd_result* wf5g_txdpd_result, U8 sync_gap)
{
    RG_ESTI_A70_FIELD_T    rg_esti_a70;
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
    RG_XMIT_A56_FIELD_T    rg_xmit_a56;
    RG_TOP_A7_FIELD_T    rg_top_a7;
    RG_SX_A25_FIELD_T    rg_sx_a25;
    RG_TX_A3_FIELD_T    rg_tx_a3;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_RX_A5_FIELD_T    rg_rx_a5;
    RG_RX_A58_FIELD_T    rg_rx_a58;
    RG_RX_A60_FIELD_T    rg_rx_a60;


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

    tmp = fi_ahb_read(AGC_AGC_CCA_CTRL);
    fi_ahb_write(AGC_AGC_CCA_CTRL, (tmp & ~BIT(28)));

    rg_top_a7.data = rf_read_register(RG_TOP_A7);
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX1_EN = 0x0;                         // Manually Reverted in DPD END
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX2_EN = 0x0;                         // Manually Reverted in DPD END
    rg_top_a7.b.RG_M2_WF_TRX_LF_LDO_RX_EN = 0x0;                         // Manually Reverted in DPD END
    rf_write_register(RG_TOP_A7, rg_top_a7.data);

    rg_sx_a25.data = rf_read_register(RG_SX_A25);
    rg_sx_a25.b.RG_M2_WF_SX_LOG5G_DPDLO_EN = 0x0;                         // Manually Reverted in DPD END
    rf_write_register(RG_SX_A25, rg_sx_a25.data);

    rg_tx_a3.data = rf_read_register(RG_TX_A3);
    rg_tx_a3.b.RG_WF5G_TXPA_GAIN_MAN = 0x7;                         // Manually Reverted in DPD END
    rg_tx_a3.b.RG_WF5G_TXM_GAIN_MAN = 0x7;                         // Manually Reverted in DPD END
    rg_tx_a3.b.RG_WF5G_TX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_TX_A3, rg_tx_a3.data);

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    rg_rx_a2.b.RG_WF_RX_BW_SEL = 0x0;                         // Manually Reverted in DPD END
    rg_rx_a2.b.RG_WF_RX_ADCFS_SEL = 0x0;                         // Manually Reverted in DPD END
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x9c;                        // Manually Reverted in DPD END
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);

    rg_rx_a5.data = rf_read_register(RG_RX_A5);
    rg_rx_a5.b.RG_M2_WF5G_RX_FE_TOPBIAS_EN = 0x0;                         // Manually Reverted in DPD END
    rg_rx_a5.b.RG_M2_WF5G_RX_DPD_GM_BIAS_EN = 0x0;                         // Manually Reverted in DPD END
    rg_rx_a5.b.RG_M2_WF5G_RX_MXR_BIAS_EN = 0x0;                         // Manually Reverted in DPD END
    rg_rx_a5.b.RG_M2_WF5G_RX_LNA_TRSW_EN = 0x1;                         // Manually Reverted in DPD END
    rg_rx_a5.b.RG_M2_WF5G_RX_DPD_GM_EN = 0x0;                         // Manually Reverted in DPD END
    rg_rx_a5.b.RG_M2_WF5G_RX_DPD_MXR_EN = 0x0;                         // Manually Reverted in DPD END
    rg_rx_a5.b.RG_M2_WF_RTX_ABB_RX_EN = 0x0;                         // Manually Reverted in DPD END
    rg_rx_a5.b.RG_M2_WF_RX_TIA_BIAS_EN = 0x0;                         // Manually Reverted in DPD END
    rg_rx_a5.b.RG_M2_WF_RX_RSSIPGA_BIAS_EN = 0x0;                         // Manually Reverted in DPD END
    rg_rx_a5.b.RG_M2_WF_RX_LPF_BIAS_EN = 0x0;                         // Manually Reverted in DPD END
    rg_rx_a5.b.RG_M2_WF_RX_TIA_EN = 0x0;                         // Manually Reverted in DPD END
    rg_rx_a5.b.RG_M2_WF_RX_RSSIPGA_EN = 0x0;                         // Manually Reverted in DPD END
    rg_rx_a5.b.RG_M2_WF_RX_LPF_EN = 0x0;                         // Manually Reverted in DPD END
    rg_rx_a5.b.RG_M2_WF_RX_LPF_I_DCOC_EN = 0x0;                         // Manually Reverted in DPD END
    rg_rx_a5.b.RG_M2_WF_RX_LPF_Q_DCOC_EN = 0x0;                         // Manually Reverted in DPD END
    rf_write_register(RG_RX_A5, rg_rx_a5.data);

    rg_rx_a58.data = rf_read_register(RG_RX_A58);
    rg_rx_a58.b.RG_WF_RX_LPF_NOTCH_EN = 0x1;                         // Manually Reverted in DPD END
    rf_write_register(RG_RX_A58, rg_rx_a58.data);

    rg_rx_a60.data = rf_read_register(RG_RX_A60);
    rg_rx_a60.b.RG_WF_TX_LPF_GAIN = 0x3;
    rf_write_register(RG_RX_A60, rg_rx_a60.data);

    cali_mode_exit();
    PUTS("TX DPD calib finish \n");
}

#endif //RF_CALI_TEST
