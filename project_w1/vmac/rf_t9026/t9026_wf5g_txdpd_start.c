
#include "rf_calibration.h"
#ifdef RF_CALI_TEST
void t9026_wf5g_txdpd_start(U8 sync_gap, U8 bw_idx, U8 gain)
{
    RG_XMIT_A1_FIELD_T    rg_xmit_a1;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4;
    RG_XMIT_A46_FIELD_T    rg_xmit_a46;
    RG_XMIT_A56_FIELD_T    rg_xmit_a56;
    RG_ESTI_A70_FIELD_T    rg_esti_a70;
    RG_ESTI_A87_FIELD_T    rg_esti_a87;
    RG_ESTI_A64_FIELD_T    rg_esti_a64;
    RG_ESTI_A65_FIELD_T    rg_esti_a65;
    RG_ESTI_A66_FIELD_T    rg_esti_a66;

    RG_CORE_A6_FIELD_T   rg_core_a6;
    RG_CORE_A2_FIELD_T   rg_core_a2;

    bool tx_dpd_manual = 0;
    CALI_MODE_T cali_mode = TXDPD;

    printk("SendPkt start!!\n");
    /* start send ofdm pkt from memory */
    {
        fi_ahb_write(0x00a0b010, 0x00000020);
        fi_ahb_write(0x00a0b1a4, 0x0003F000);
        fi_ahb_write(0x00a0b1a0, 0xa013fffc);
        fi_ahb_write(0x00a0b004, 0x10000000);
    }

    rg_xmit_a1.data = fi_ahb_read(RG_XMIT_A1);
    rg_xmit_a1.b.rg_xmit_cfg1 = 0x3;                         // tx enable manual mode
    fi_ahb_write(RG_XMIT_A1, rg_xmit_a1.data);

    rg_xmit_a4.data = fi_ahb_read(RG_XMIT_A4);
    rg_xmit_a4.b.rg_tx_signal_sel = 0x0;                         // new_set_reg(0xe410, 0x00000000); //0:select OFDM signal;1:signal tone
    fi_ahb_write(RG_XMIT_A4, rg_xmit_a4.data);

    rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
    rg_xmit_a46.b.rg_txpwc_comp_man_sel = 0x1;
    if (bw_idx == 0)
    {
        rg_xmit_a46.b.rg_txpwc_comp_man = 0x80;                        // i2c_set_reg_fragment(0xe4b8, 31, 24, 0xb0); //set pwtrl gain
        //fi_ahb_write(0x0000b22c, 0x80000000);
    }
    else if (bw_idx == 1)
    {
        rg_xmit_a46.b.rg_txpwc_comp_man = gain;//0x80;                        // i2c_set_reg_fragment(0xe4b8, 31, 24, 0xb0); //set pwtrl gain
        //fi_ahb_write(0x0000b22c, 0x90000000);
    }
    else if (bw_idx == 2)
    {
        rg_xmit_a46.b.rg_txpwc_comp_man = 0x70;                        // i2c_set_reg_fragment(0xe4b8, 31, 24, 0xb0); //set pwtrl gain
        //fi_ahb_write(0x0000b22c, 0xa0000000);
    }
    fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);

    rg_xmit_a56.data = fi_ahb_read(RG_XMIT_A56);
    rg_xmit_a56.b.rg_txdpd_comp_in_gain = 0x80;                        // i2c_set_reg_fragment(0xe4f0, 7, 0, 0x80);//dpd in gain
    rg_xmit_a56.b.rg_txdpd_comp_out_gain = 0x80;
    rg_xmit_a56.b.rg_txdpd_comp_type = 0x1;                         // i2c_set_reg_fragment(0xe4f0, 24, 24, 1);//set dpd comp type to 1:coeff
    rg_xmit_a56.b.rg_txdpd_comp_bypass = 0x1;                         // i2c_set_reg_fragment(0xe4f0, 26, 26, 1);//set dpd comp bypass to 1
    rg_xmit_a56.b.rg_txdpd_comp_bypass_man = 0x1;                         // i2c_set_reg_fragment(0xe4f0, 28, 28, 1);//set dpd comp bypass man to 1
    fi_ahb_write(RG_XMIT_A56, rg_xmit_a56.data);

    fi_ahb_write(PHY_TX_LPF_BYPASS, 0x1);  //bypass spectrum mask

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
    rg_esti_a70.b.rg_txdpd_esti_alpha = 0x20;
    rg_esti_a70.b.rg_txdpd_esti_sync_gap = sync_gap & 0x7f;
    rg_esti_a70.b.ro_txdpd_esti_ready = 0x0;
    rg_esti_a70.b.rg_txdpd_esti_mp_pow_thresh = 0x06;
    fi_ahb_write(RG_ESTI_A70, rg_esti_a70.data);

    rg_esti_a87.data = fi_ahb_read(RG_ESTI_A87);
    rg_esti_a87.b.rg_txdpd_snr_esti_alpha = 0x4;
    fi_ahb_write(RG_ESTI_A87, rg_esti_a87.data);

    rg_esti_a64.data = fi_ahb_read(RG_ESTI_A64);
    rg_esti_a64.b.rg_adda_wait_count = 0xd2;
    fi_ahb_write(RG_ESTI_A64, rg_esti_a64.data);

    //rg_esti_a65.data = fi_ahb_read(RG_ESTI_A65);
    //rg_esti_a65.b.rg_adda_calc_count = 0x1000;
    //fi_ahb_write(RG_ESTI_A65, rg_esti_a65.data);

    rg_esti_a66.data = fi_ahb_read(RG_ESTI_A66);
    rg_esti_a66.b.rg_adda_esti_count = 0x186a00;   //0x186a00;   //0x7a1200;
    fi_ahb_write(RG_ESTI_A66, rg_esti_a66.data);

    t9026_dcoc_dig_comp();

    rg_esti_a65.data = fi_ahb_read(RG_ESTI_A65);
    rg_esti_a65.b.rg_adda_calc_count = 0x1000;
    fi_ahb_write(RG_ESTI_A65, rg_esti_a65.data);

    rg_core_a6.data = fi_ahb_read(RG_CORE_A6);
    rg_core_a6.b.rg_cali_mode_man = cali_mode;
    rg_core_a6.b.rg_cali_mode_man_en = 0x1;
    fi_ahb_write(RG_CORE_A6, rg_core_a6.data);

    rg_core_a2.data = fi_ahb_read(RG_CORE_A2);
    rg_core_a2.b.rg_core_enb_man = 0x1;
    rg_core_a2.b.rg_core_enb_man_en = 0x1;
    rg_core_a2.b.rg_xmit_enb_man = 0x1;
    rg_core_a2.b.rg_xmit_enb_man_en = 0x1;
    rg_core_a2.b.rg_recv_enb_man = 0x1;
    rg_core_a2.b.rg_recv_enb_man_en = 0x1;
    rg_core_a2.b.rg_esti_enb_man = 0x0;
    rg_core_a2.b.rg_esti_enb_man_en = 0x0;
    fi_ahb_write(RG_CORE_A2, rg_core_a2.data);

    rg_core_a2.data = fi_ahb_read(RG_CORE_A2);
    rg_core_a2.b.rg_core_enb_man = 0x1;
    rg_core_a2.b.rg_core_enb_man_en = 0x1;
    rg_core_a2.b.rg_xmit_enb_man = 0x1;
    rg_core_a2.b.rg_xmit_enb_man_en = 0x1;
    rg_core_a2.b.rg_recv_enb_man = 0x1;
    rg_core_a2.b.rg_recv_enb_man_en = 0x1;
    rg_core_a2.b.rg_esti_enb_man = 0x1;
    rg_core_a2.b.rg_esti_enb_man_en = 0x1;
    fi_ahb_write(RG_CORE_A2, rg_core_a2.data);
}

#endif //# RF_CALI_TEST
