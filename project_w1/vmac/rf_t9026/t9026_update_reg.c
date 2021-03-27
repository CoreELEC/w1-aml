#include "rf_calibration.h"
#ifdef RF_CALI_TEST
void t9026_update_reg (void){
    RG_TOP_A2_FIELD_T    rg_top_a2;
    RG_TOP_A7_FIELD_T    rg_top_a7;
    RG_TOP_A11_FIELD_T    rg_top_a11;
    RG_TOP_A12_FIELD_T    rg_top_a12;
    RG_TOP_A14_FIELD_T    rg_top_a14;
    RG_TOP_A15_FIELD_T    rg_top_a15;
    RG_TOP_A21_FIELD_T    rg_top_a21;
    RG_TOP_A27_FIELD_T    rg_top_a27;
    RG_TOP_A29_FIELD_T    rg_top_a29;
    RG_SX_A3_FIELD_T    rg_sx_a3;
    RG_SX_A4_FIELD_T      rg_sx_a4;
    RG_SX_A25_FIELD_T    rg_sx_a25;
    RG_SX_A32_FIELD_T    rg_sx_a32;
    RG_SX_A34_FIELD_T    rg_sx_a34;
    RG_SX_A35_FIELD_T    rg_sx_a35;
    RG_SX_A42_FIELD_T    rg_sx_a42;
    RG_SX_A46_FIELD_T    rg_sx_a46;
    RG_TX_A1_FIELD_T    rg_tx_a1;
    RG_TX_A10_FIELD_T    rg_tx_a10;
    RG_RX_A4_FIELD_T    rg_rx_a4;
    RG_RX_A5_FIELD_T    rg_rx_a5;
    RG_RX_A6_FIELD_T    rg_rx_a6;
    RG_RX_A11_FIELD_T    rg_rx_a11;
    RG_RX_A17_FIELD_T    rg_rx_a17;
    RG_RX_A19_FIELD_T    rg_rx_a19;
    RG_RX_A64_FIELD_T    rg_rx_a64;
    RG_RX_A65_FIELD_T    rg_rx_a65;
    RG_RX_A79_FIELD_T    rg_rx_a79;
    RG_RX_A80_FIELD_T    rg_rx_a80;
    RG_RX_A81_FIELD_T    rg_rx_a81;
    RG_RX_A85_FIELD_T    rg_rx_a85;
    RG_RX_A87_FIELD_T    rg_rx_a87;
    RG_RX_A88_FIELD_T    rg_rx_a88;
    RG_RX_A89_FIELD_T    rg_rx_a89;
    RG_RX_A90_FIELD_T    rg_rx_a90;
    RG_RX_A91_FIELD_T    rg_rx_a91;
    RG_RX_A92_FIELD_T    rg_rx_a92;
    RG_RECV_A0_FIELD_T    rg_recv_a0;
    RG_RECV_A2_FIELD_T    rg_recv_a2;
    RG_XMIT_A34_FIELD_T    rg_xmit_a34;

    //fi_ahb_write(0x00f01024, 0x001081a8);  //xosc freq offset
    //fi_ahb_write(0x00f01034, 0x07deba01); // BG Current
    fi_ahb_write(0x00f00078, 0x0000c1e0);  //wifi rf automode value
    fi_ahb_write(0x00f01074, 0x88890c4f);  //CLDO,930mv
    fi_ahb_write(0x00f0104c, 0x52467256);  //pmu pwm

    fi_ahb_write(0x00a0d0a4, 0x00400013);
    fi_ahb_write(0x00a0b224, 0x33000120);

    rg_top_a2.data = rf_read_register(RG_TOP_A2);
    rg_top_a2.b.RG_WF_MODE_MAN_MODE = 0x0;
    rg_top_a2.b.RG_WF_TRSWITCH_EDGE_MASK = 0x3;
    rf_write_register(RG_TOP_A2, rg_top_a2.data);

    rg_top_a7.data = rf_read_register(RG_TOP_A7);
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_TX1_EN = 0x0;
    rf_write_register(RG_TOP_A7, rg_top_a7.data);

    rg_top_a11.data = rf_read_register(RG_TOP_A11);
    rg_top_a11.b.rg_wf_dac_op_ibias = 0x1;
    rf_write_register(RG_TOP_A11, rg_top_a11.data);

    rg_top_a12.data = rf_read_register(RG_TOP_A12);
    rg_top_a12.b.rg_wf_dac_rsv = 0x77;
    rf_write_register(RG_TOP_A12, rg_top_a12.data);

    rg_top_a14.data = rf_read_register(RG_TOP_A14);
    rg_top_a14.b.rg_wf_wadc_clkout_phase = 0x1;
    rf_write_register(RG_TOP_A14, rg_top_a14.data);

    rg_top_a15.data = rf_read_register(RG_TOP_A15);
    rg_top_a15.b.rg_wf_dac_ampctrl_man = 0x2;
    rf_write_register(RG_TOP_A15, rg_top_a15.data);

    rg_top_a21.data = rf_read_register(RG_TOP_A21);
    rg_top_a21.b.rg_m4_wf_adda_ldo_en = 0x0;
    rg_top_a21.b.rg_m4_wf_dac_enable = 0x0;
    rg_top_a21.b.rg_m4_wf_wadc_enable_i = 0x0;
    rg_top_a21.b.rg_m4_wf_wadc_enable_q = 0x0;
    rf_write_register(RG_TOP_A21, rg_top_a21.data);

    rg_top_a27.data = rf_read_register(RG_TOP_A27);
    rg_top_a27.b.RG_M4_WF_TRX_HF_LDO_EN = 0x0;
    rg_top_a27.b.RG_M4_WF_TRX_HF_LDO_TX1_EN = 0x0;
    rg_top_a27.b.RG_M4_WF_TRX_HF_LDO_TX2_EN = 0x0;
    rg_top_a27.b.RG_M4_WF_TRX_HF_LDO_RXB_EN = 0x0;
    rg_top_a27.b.RG_M4_WF_TRX_HF_LDO_RX1_EN = 0x0;
    rg_top_a27.b.RG_M4_WF_TRX_HF_LDO_RX2_EN = 0x0;
    rg_top_a27.b.RG_M4_WF_TRX_HF_LDO_LO_EN = 0x0;
    rg_top_a27.b.RG_M4_WF_TRX_LF_LDO_EN = 0x0;
    rg_top_a27.b.RG_M4_WF_TRX_LF_LDO_TX_EN = 0x0;
    rg_top_a27.b.RG_M4_WF_TRX_LF_LDO_RX_EN = 0x0;
    rf_write_register(RG_TOP_A27, rg_top_a27.data);

    rg_top_a29.data = rf_read_register(RG_TOP_A29);
    rg_top_a29.b.RG_M6_WF_TRX_HF_LDO_TX2_EN = 0x0;
    rf_write_register(RG_TOP_A29, rg_top_a29.data);

    rg_sx_a3.data = rf_read_register(RG_SX_A3);
    rg_sx_a3.b.RG_WF_SX_TR_SWITCH_BND = 0x3;
    rf_write_register(RG_SX_A3, rg_sx_a3.data);

    rg_sx_a4.data = rf_read_register(RG_SX_A4);
    rg_sx_a4.b.RG_WF_SX_VCO_I = 0xa;
    rf_write_register(RG_SX_A4, rg_sx_a4.data);

    rg_sx_a25.data = rf_read_register(RG_SX_A25);
    rg_sx_a25.b.RG_M2_WF_SX_LOG2G_TXLO_EN = 0x0;
    rf_write_register(RG_SX_A25, rg_sx_a25.data);

    rg_sx_a32.data = rf_read_register(RG_SX_A32);
    rg_sx_a32.b.RG_WF_SX_LPF_BW = 0x1;
    rg_sx_a32.b.RG_WF_SX_CP_OFFSET = 0x4;
    rf_write_register(RG_SX_A32, rg_sx_a32.data);

    rg_sx_a34.data = rf_read_register(RG_SX_A34);
    rg_sx_a34.b.RG_WF_SX_VCO_LDO_VREF_ADJ = 0x18;
    rf_write_register(RG_SX_A34, rg_sx_a34.data);

    rg_sx_a35.data = rf_read_register(RG_SX_A35);
    rg_sx_a35.b.RG_WF_SX_VCO_2ND_CAPN = 0x0;
    rg_sx_a35.b.RG_WF_SX_VCO_2ND_CAPP = 0x0;
    rf_write_register(RG_SX_A35, rg_sx_a35.data);

    rg_sx_a42.data = rf_read_register(RG_SX_A42);
    rg_sx_a42.b.RG_WF_SX_RSV0 = 0xfff8;
    rf_write_register(RG_SX_A42, rg_sx_a42.data);

    rg_sx_a46.data = rf_read_register(RG_SX_A46);
    rg_sx_a46.b.RG_M4_WF_SX_VCO_EN = 0x0;
    rg_sx_a46.b.RG_M4_WF_SX_VCO_ACAL_EN = 0x0;
    rg_sx_a46.b.RG_M4_WF_SX_PFD_EN = 0x0;
    rg_sx_a46.b.RG_M4_WF_SX_CP_EN = 0x0;
    rg_sx_a46.b.RG_M4_WF_SX_VCO_LDO_EN = 0x0;
    rg_sx_a46.b.RG_M4_WF_SX_CP_LDO_EN = 0x0;
    rg_sx_a46.b.RG_M4_WF_SX_LPF_COMP_EN = 0x0;
    rg_sx_a46.b.RG_M4_WF_SX_LOG5G_MXR_EN = 0x0;
    rg_sx_a46.b.RG_M4_WF_SX_LOG5G_INV_EN = 0x0;
    rg_sx_a46.b.RG_M4_WF_SX_LOG5G_DIV_EN = 0x0;
    rg_sx_a46.b.RG_M4_WF_SX_LOG5G_IQDIV_EN = 0x0;
    rg_sx_a46.b.RG_M4_WF_SX_LOG5G_DPDLO_EN = 0x0;
    rg_sx_a46.b.RG_M4_WF_SX_LOG5G_TXLO_EN = 0x0;
    rf_write_register(RG_SX_A46, rg_sx_a46.data);

    rg_tx_a1.data = rf_read_register(RG_TX_A1);
    rg_tx_a1.b.RG_WF5G_TX_ADB_ED_EN = 0x1;
    rg_tx_a1.b.RG_WF5G_TX_ADB_ED_GAIN = 0xa;
    rg_tx_a1.b.RG_WF5G_TX_ADB_ICGM = 0x0;
    rg_tx_a1.b.RG_WF5G_TX_ADB_IPTAT = 0x1;
    rg_tx_a1.b.RG_WF5G_TX_IB_ADJ = 0x0;
    rf_write_register(RG_TX_A1, rg_tx_a1.data);

    rg_tx_a10.data = rf_read_register(RG_TX_A10);
    rg_tx_a10.b.RG_M4_WF5G_TX_PA_EN = 0x0;
    rg_tx_a10.b.RG_M4_WF5G_TX_MXR_EN = 0x0;
    rg_tx_a10.b.RG_M4_WF5G_TX_IB_EN = 0x0;
    rf_write_register(RG_TX_A10, rg_tx_a10.data);

    rg_rx_a4.data = rf_read_register(RG_RX_A4);
    rg_rx_a4.b.RG_M1_WF5G_RX_LNA_TRSW_EN = 0x1;
    rg_rx_a4.b.RG_M1_WF2G_RX_LNA_TRSW_EN = 0x1;
    rf_write_register(RG_RX_A4, rg_rx_a4.data);

    rg_rx_a5.data = rf_read_register(RG_RX_A5);
    rg_rx_a5.b.RG_M2_WF2G_RX_LNA_TRSW_EN = 0x1;
    rf_write_register(RG_RX_A5, rg_rx_a5.data);

    rg_rx_a6.data = rf_read_register(RG_RX_A6);
    rg_rx_a6.b.RG_M3_WF2G_RX_LNA_TRSW_EN = 0x1;
    rf_write_register(RG_RX_A6, rg_rx_a6.data);

    rg_rx_a11.data = rf_read_register(RG_RX_A11);
    rg_rx_a11.b.RG_WF_RXFE_GAIN_G1 = 0x22;
    rg_rx_a11.b.RG_WF_RXFE_GAIN_G2 = 0x22;
    rf_write_register(RG_RX_A11, rg_rx_a11.data);

    rg_rx_a17.data = rf_read_register(RG_RX_A17);
    rg_rx_a17.b.RG_WF5G_RX_LNA_RFB_LB_G1 = 0x2;
    rg_rx_a17.b.RG_WF5G_RX_LNA_RFB_LB_G3 = 0x3;
    rg_rx_a17.b.RG_WF5G_RX_LNA_RFB_LB_G5 = 0x4;
    rf_write_register(RG_RX_A17, rg_rx_a17.data);

    rg_rx_a19.data = rf_read_register(RG_RX_A19);
    rg_rx_a19.b.RG_WF5G_RX_LNA_RT_G1 = 0x3;
    rg_rx_a19.b.RG_WF5G_RX_LNA_RT_G3 = 0x7;
    rf_write_register(RG_RX_A19, rg_rx_a19.data);

    rg_rx_a64.data = rf_read_register(RG_RX_A64);
    rg_rx_a64.b.RG_WF_RX_RSSIPGA_VCMSEL = 0x3;
    rf_write_register(RG_RX_A64, rg_rx_a64.data);

    rg_rx_a65.data = rf_read_register(RG_RX_A65);
    rg_rx_a65.b.RG_WF2G_RX_LNA_CAL_ATT = 0x3;
    rf_write_register(RG_RX_A65, rg_rx_a65.data);

    rg_rx_a79.data = rf_read_register(RG_RX_A79);
    rg_rx_a79.b.RG_WF2G_RX_LNA_ICTRL_G1 = 0x1;
    rg_rx_a79.b.RG_WF2G_RX_LNA_ICTRL_G2 = 0x5;
    rg_rx_a79.b.RG_WF2G_RX_LNA_ICTRL_G4 = 0x1;
    rg_rx_a79.b.RG_WF2G_RX_LNA_ICTRL_G5 = 0x1;
    rg_rx_a79.b.RG_WF2G_RX_LNA_ICTRL_G7 = 0x4;
    rf_write_register(RG_RX_A79, rg_rx_a79.data);

    rg_rx_a80.data = rf_read_register(RG_RX_A80);
    rg_rx_a80.b.RG_WF2G_RX_LNA_SLICE_G1 = 0x91;
    rg_rx_a80.b.RG_WF2G_RX_LNA_SLICE_G2 = 0xda;
    rg_rx_a80.b.RG_WF2G_RX_LNA_SLICE_G4 = 0x9;
    rf_write_register(RG_RX_A80, rg_rx_a80.data);

    rg_rx_a81.data = rf_read_register(RG_RX_A81);
    rg_rx_a81.b.RG_WF2G_RX_LNA_SLICE_G5 = 0x19;
    rg_rx_a81.b.RG_WF2G_RX_LNA_SLICE_G7 = 0xfb;
    rf_write_register(RG_RX_A81, rg_rx_a81.data);

    rg_rx_a85.data = rf_read_register(RG_RX_A85);
    rg_rx_a85.b.RG_WF2G_RX_LNA_ATT_G2 = 0x5;
    rg_rx_a85.b.RG_WF2G_RX_LNA_ATT_G3 = 0x2;
    rf_write_register(RG_RX_A85, rg_rx_a85.data);

    rg_rx_a87.data = rf_read_register(RG_RX_A87);
    rg_rx_a87.b.RG_WF2G_RX_LNA_PRIORITY = 0xc9;
    rf_write_register(RG_RX_A87, rg_rx_a87.data);

    rg_rx_a88.data = rf_read_register(RG_RX_A88);
    rg_rx_a88.b.RG_M4_WF_RX_FE_TOPBIAS_EN = 0x0;
    rg_rx_a88.b.RG_M4_WF_RX_LNA_BIAS_EN = 0x0;
    rg_rx_a88.b.RG_M4_WF_RX_GM_BIAS_EN = 0x0;
    rg_rx_a88.b.RG_M4_WF_RX_MXR_BIAS_EN = 0x0;
    rg_rx_a88.b.RG_M4_WF5G_RX_LNA_TRSW_EN = 0x0;
    rg_rx_a88.b.RG_M4_WF5G_RX_DPD_EN = 0x0;
    rg_rx_a88.b.RG_M4_WF_RTX_ABB_TOPBIAS_EN = 0x0;
    rg_rx_a88.b.RG_M4_WF_RTX_ABB_RX_EN = 0x0;
    rg_rx_a88.b.RG_M4_WF_RX_TIA_BIAS_EN = 0x0;
    rg_rx_a88.b.RG_M4_WF_RX_RSSIPGA_BIAS_EN = 0x0;
    rg_rx_a88.b.RG_M4_WF_RX_LPF_BIAS_EN = 0x0;
    rg_rx_a88.b.RG_M4_WF_RX_TIA_EN = 0x0;
    rg_rx_a88.b.RG_M4_WF_RX_RSSIPGA_EN = 0x0;
    rg_rx_a88.b.RG_M4_WF_RX_LPF_EN = 0x0;
    rg_rx_a88.b.RG_M4_WF_RX_LPF_I_DCOC_EN = 0x0;
    rg_rx_a88.b.RG_M4_WF_RX_LPF_Q_DCOC_EN = 0x0;
    rf_write_register(RG_RX_A88, rg_rx_a88.data);

    rg_rx_a89.data = rf_read_register(RG_RX_A89);
    rg_rx_a89.b.RG_M5_WF5G_RX_LNA_TRSW_EN = 0x1;
    rg_rx_a89.b.RG_M5_WF2G_RX_LNA_TRSW_EN = 0x1;
    rf_write_register(RG_RX_A89, rg_rx_a89.data);

    rg_rx_a90.data = rf_read_register(RG_RX_A90);
    rg_rx_a90.b.RG_M6_WF5G_RX_LNA_TRSW_EN = 0x1;
    rf_write_register(RG_RX_A90, rg_rx_a90.data);

    rg_rx_a91.data = rf_read_register(RG_RX_A91);
    rg_rx_a91.b.RG_M7_WF5G_RX_LNA_TRSW_EN = 0x1;
    rf_write_register(RG_RX_A91, rg_rx_a91.data);

    rg_rx_a92.data = rf_read_register(RG_RX_A92);
    rg_rx_a92.b.RG_WF2G_RX_LNA_ICTRL_G67 = 0x5;
    rg_rx_a92.b.RG_WF2G_RX_LNA_ICTRL_G57 = 0x0;
    rg_rx_a92.b.RG_WF2G_RX_LNA_ICTRL_G37 = 0x1;
    rg_rx_a92.b.RG_WF2G_RX_LNA_ICTRL_G56 = 0x4;
    rg_rx_a92.b.RG_WF2G_RX_LNA_ICTRL_G46 = 0x0;
    rg_rx_a92.b.RG_WF2G_RX_LNA_ICTRL_G36 = 0x4;
    rg_rx_a92.b.RG_WF2G_RX_LNA_ICTRL_G45 = 0x4;
    rg_rx_a92.b.RG_WF2G_RX_LNA_ICTRL_G35 = 0x4;
    rf_write_register(RG_RX_A92, rg_rx_a92.data);


    rg_recv_a0.data = fi_ahb_read(RG_RECV_A0);
    rg_recv_a0.b.rg_recv_cfg0 = 0x10;
    fi_ahb_write(RG_RECV_A0, rg_recv_a0.data);

    rg_recv_a2.data = fi_ahb_read(RG_RECV_A2);
    rg_recv_a2.b.rg_rxneg_msb = 0x1;
    fi_ahb_write(RG_RECV_A2, rg_recv_a2.data);

    rg_xmit_a34.data = fi_ahb_read(RG_XMIT_A34);
    rg_xmit_a34.b.rg_tx_iqmm_bypass = 0x1;
    rg_xmit_a34.b.rg_tx_negate_msb = 0x1;
    rg_xmit_a34.b.rg_tx_iq_swap = 0x0;
    rg_xmit_a34.b.rg_tx_dout_sel = 0x0;
    rg_xmit_a34.b.rg_tx_async_wgap = 0xa;
    fi_ahb_write(RG_XMIT_A34, rg_xmit_a34.data);

}


#endif // RF_CALI_TEST

