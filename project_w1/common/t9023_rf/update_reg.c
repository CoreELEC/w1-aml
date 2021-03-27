
#ifdef RF_CALI_TEST
#include "rf_calibration.h"
void update_reg()
{
    RG_TOP_A2_FIELD_T    rg_top_a2;
    RG_TOP_A7_FIELD_T    rg_top_a7;
    RG_TOP_A8_FIELD_T    rg_top_a8;
    RG_SX_A6_FIELD_T    rg_sx_a6;
    RG_SX_A7_FIELD_T    rg_sx_a7;
    RG_SX_A15_FIELD_T    rg_sx_a15;
    RG_SX_A16_FIELD_T    rg_sx_a16;
    RG_SX_A17_FIELD_T    rg_sx_a17;
    RG_SX_A18_FIELD_T    rg_sx_a18;
    RG_SX_A23_FIELD_T    rg_sx_a23;
    RG_SX_A24_FIELD_T    rg_sx_a24;
    RG_SX_A25_FIELD_T    rg_sx_a25;
    RG_SX_A26_FIELD_T    rg_sx_a26;
    RG_SX_A32_FIELD_T    rg_sx_a32;
    RG_SX_A35_FIELD_T    rg_sx_a35;
    RG_SX_A36_FIELD_T    rg_sx_a36;
    RG_SX_A37_FIELD_T    rg_sx_a37;
    RG_SX_A40_FIELD_T    rg_sx_a40;
    RG_TX_A0_FIELD_T    rg_tx_a0;
    RG_TX_A1_FIELD_T    rg_tx_a1;
    RG_TX_A2_FIELD_T    rg_tx_a2;
    RG_TX_A3_FIELD_T    rg_tx_a3;
    RG_TX_A6_FIELD_T    rg_tx_a6;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_RX_A5_FIELD_T    rg_rx_a5;
    RG_RX_A6_FIELD_T    rg_rx_a6;
    RG_RX_A7_FIELD_T    rg_rx_a7;
    RG_RX_A8_FIELD_T    rg_rx_a8;
    RG_RX_A9_FIELD_T    rg_rx_a9;
    RG_RX_A14_FIELD_T    rg_rx_a14;
    RG_RX_A15_FIELD_T    rg_rx_a15;
    RG_RX_A16_FIELD_T    rg_rx_a16;
    RG_RX_A18_FIELD_T    rg_rx_a18;
    RG_RX_A19_FIELD_T    rg_rx_a19;
    RG_RX_A20_FIELD_T    rg_rx_a20;
    RG_RX_A23_FIELD_T    rg_rx_a23;
    RG_RX_A27_FIELD_T    rg_rx_a27;
    RG_RX_A28_FIELD_T    rg_rx_a28;
    RG_RX_A30_FIELD_T    rg_rx_a30;
    RG_RX_A46_FIELD_T    rg_rx_a46;
    RG_RX_A47_FIELD_T    rg_rx_a47;
    RG_RX_A48_FIELD_T    rg_rx_a48;
    RG_RX_A49_FIELD_T    rg_rx_a49;
    RG_RX_A50_FIELD_T    rg_rx_a50;
    RG_RX_A54_FIELD_T    rg_rx_a54;
    RG_RX_A55_FIELD_T    rg_rx_a55;
    RG_RX_A56_FIELD_T    rg_rx_a56;
    RG_RX_A60_FIELD_T    rg_rx_a60;


    rg_top_a2.data = rf_read_register(RG_TOP_A2);
    rg_top_a2.b.RG_WF_TX_MODE = 0x2;
    rg_top_a2.b.RG_WF_RX_MODE = 0x3;
    rf_write_register(RG_TOP_A2, rg_top_a2.data);

    rg_top_a7.data = rf_read_register(RG_TOP_A7);
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_EN = 0x1;
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_TX_EN = 0x1;
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_LO_EN = 0x1;
    rg_top_a7.b.RG_M2_WF_TRX_LF_LDO_EN = 0x1;
    rg_top_a7.b.RG_M2_WF_TRX_LF_LDO_TX_EN = 0x1;
    rf_write_register(RG_TOP_A7, rg_top_a7.data);

    rg_top_a8.data = rf_read_register(RG_TOP_A8);
    rg_top_a8.b.RG_M3_WF_TRX_HF_LDO_EN = 0x1;
    rg_top_a8.b.RG_M3_WF_TRX_HF_LDO_RX1_EN = 0x1;
    rg_top_a8.b.RG_M3_WF_TRX_HF_LDO_RX2_EN = 0x1;
    rg_top_a8.b.RG_M3_WF_TRX_HF_LDO_LO_EN = 0x1;
    rg_top_a8.b.RG_M3_WF_TRX_LF_LDO_EN = 0x1;
    rg_top_a8.b.RG_M3_WF_TRX_LF_LDO_RX_EN = 0x1;
    rf_write_register(RG_TOP_A8, rg_top_a8.data);

    rg_sx_a6.data = rf_read_register(RG_SX_A6);
    rg_sx_a6.b.RG_SX_OSC_FREQ = 0x5;
    rg_sx_a6.b.RG_5GB_SX_CHANNEL = 0xa;
    rf_write_register(RG_SX_A6, rg_sx_a6.data);

    rg_sx_a7.data = rf_read_register(RG_SX_A7);
    rg_sx_a7.b.RG_WF_SX_RFCTRL_INT = 0x14f;
    rf_write_register(RG_SX_A7, rg_sx_a7.data);

    rg_sx_a15.data = rf_read_register(RG_SX_A15);
    rg_sx_a15.b.RG_TR_SWITCH_MAN = 0x1;
    rf_write_register(RG_SX_A15, rg_sx_a15.data);

    rg_sx_a16.data = rf_read_register(RG_SX_A16);
    rg_sx_a16.b.RG_WF_SX_LOG5G_PDTHRESHOLD = 0x7;
    rg_sx_a16.b.RG_WF_SX_LOG2G_PDTHRESHOLD = 0x7;
    rf_write_register(RG_SX_A16, rg_sx_a16.data);

    rg_sx_a17.data = rf_read_register(RG_SX_A17);
    rg_sx_a17.b.RG_WF_SX_LOG5G_MXR_BANKSEL_MAN0 = 0xe;
    rg_sx_a17.b.RG_WF_SX_LOG5G_MXR_BANKSEL_MAN1 = 0xd;
    rg_sx_a17.b.RG_WF_SX_LOG5G_MXR_BANKSEL_MAN2 = 0xc;
    rg_sx_a17.b.RG_WF_SX_LOG5G_MXR_BANKSEL_MAN3 = 0xb;
    rg_sx_a17.b.RG_WF_SX_LOG5G_MXR_BANKSEL_MAN4 = 0xa;
    rg_sx_a17.b.RG_WF_SX_LOG5G_MXR_BANKSEL_MAN5 = 0x9;
    rg_sx_a17.b.RG_WF_SX_LOG5G_MXR_BANKSEL_MAN6 = 0x8;
    rg_sx_a17.b.RG_WF_SX_LOG5G_MXR_BANKSEL_MAN7 = 0x7;
    rf_write_register(RG_SX_A17, rg_sx_a17.data);

    rg_sx_a18.data = rf_read_register(RG_SX_A18);
    rg_sx_a18.b.RG_WF_SX_LOG5G_MXR_BANKSEL_MAN8 = 0x6;
    rg_sx_a18.b.RG_WF_SX_LOG5G_MXR_BANKSEL_MAN9 = 0x4;
    rf_write_register(RG_SX_A18, rg_sx_a18.data);

    rg_sx_a23.data = rf_read_register(RG_SX_A23);
    rg_sx_a23.b.RG_M0_WF_SX_VCO_EN = 0x0;
    rg_sx_a23.b.RG_M0_WF_SX_VCO_ACAL_EN = 0x0;
    rg_sx_a23.b.RG_M0_WF_SX_PFD_EN = 0x0;
    rg_sx_a23.b.RG_M0_WF_SX_CP_EN = 0x0;
    rg_sx_a23.b.RG_M0_WF_SX_VCO_LDO_EN = 0x0;
    rg_sx_a23.b.RG_M0_WF_SX_CP_LDO_EN = 0x0;
    rg_sx_a23.b.RG_M0_WF_SX_LPF_COMP_EN = 0x0;
    rg_sx_a23.b.RG_M0_WF_SX_LOG5G_MXR_EN = 0x0;
    rg_sx_a23.b.RG_M0_WF_SX_LOG5G_DIV_EN = 0x0;
    rg_sx_a23.b.RG_M0_WF_SX_LOG5G_IQDIV_EN = 0x0;
    rg_sx_a23.b.RG_M0_WF_SX_LOG5G_RXLO_EN = 0x0;
    rg_sx_a23.b.RG_M0_WF_SX_LOG5G_DPDLO_EN = 0x0;
    rg_sx_a23.b.RG_M0_WF_SX_LOG5G_TXLO_EN = 0x0;
    rf_write_register(RG_SX_A23, rg_sx_a23.data);

    rg_sx_a24.data = rf_read_register(RG_SX_A24);
    rg_sx_a24.b.RG_M1_WF_SX_LOG5G_RXLO_EN = 0x0;
    rg_sx_a24.b.RG_M1_WF_SX_LOG5G_DPDLO_EN = 0x0;
    rg_sx_a24.b.RG_M1_WF_SX_LOG5G_TXLO_EN = 0x0;
    rf_write_register(RG_SX_A24, rg_sx_a24.data);

    rg_sx_a25.data = rf_read_register(RG_SX_A25);
    rg_sx_a25.b.RG_M2_WF_SX_LOG5G_RXLO_EN = 0x0;
    rg_sx_a25.b.RG_M2_WF_SX_LOG5G_DPDLO_EN = 0x0;
    rf_write_register(RG_SX_A25, rg_sx_a25.data);

    rg_sx_a26.data = rf_read_register(RG_SX_A26);
    rg_sx_a26.b.RG_M3_WF_SX_LOG5G_DPDLO_EN = 0x0;
    rg_sx_a26.b.RG_M3_WF_SX_LOG5G_TXLO_EN = 0x0;
    rf_write_register(RG_SX_A26, rg_sx_a26.data);

    rg_sx_a32.data = rf_read_register(RG_SX_A32);
    rg_sx_a32.b.RG_WF_SX_LPF_BW = 0x7;
    rg_sx_a32.b.RG_WF_SX_CP_OFFSET = 0x7;
    rf_write_register(RG_SX_A32, rg_sx_a32.data);

    rg_sx_a35.data = rf_read_register(RG_SX_A35);
    rg_sx_a35.b.RG_WF_SX_CP_I = 0x0;
    rg_sx_a35.b.RG_WF_SX_PFD_PH_EN = 0x0;
    rg_sx_a35.b.RG_WF_SX_CP_RC_CTRL = 0x0;
    rf_write_register(RG_SX_A35, rg_sx_a35.data);

    rg_sx_a36.data = rf_read_register(RG_SX_A36);
    rg_sx_a36.b.RG_WF_SX_LOG5G_PD_EN = 0x0;
    rg_sx_a36.b.RG_WF_SX_LOG5G_MXR_ICTRL = 0xa;
    rg_sx_a36.b.RG_WF_SX_LOG5G_DIV_VDD = 0x1;
    rf_write_register(RG_SX_A36, rg_sx_a36.data);

    rg_sx_a37.data = rf_read_register(RG_SX_A37);
    rg_sx_a37.b.RG_WF_SX_LOG5G_IQDIV_RCTRL = 0x0;
    rg_sx_a37.b.RG_WF_SX_LOG5G_IQDIV_ICTRL = 0x6;
    rf_write_register(RG_SX_A37, rg_sx_a37.data);

    rg_sx_a40.data = rf_read_register(RG_SX_A40);
    rg_sx_a40.b.RG_WF_SX_DOUBLE_BYPASS_MODE = 0x1;
    rf_write_register(RG_SX_A40, rg_sx_a40.data);

    rg_tx_a0.data = rf_read_register(RG_TX_A0);
    rg_tx_a0.b.RG_WF5G_TXM_LOBUF_ICT = 0x1;
    rg_tx_a0.b.RG_WF5G_TXM_V2I_VCM_GAIN = 0x7;
    rg_tx_a0.b.RG_WF5G_TXM_VB3 = 0x3;
    rg_tx_a0.b.RG_WF5G_TXM_VB_LO = 0x3;
    rf_write_register(RG_TX_A0, rg_tx_a0.data);

    rg_tx_a1.data = rf_read_register(RG_TX_A1);
    rg_tx_a1.b.RG_WF5G_TXPA_VB2 = 0x4;
    rg_tx_a1.b.RG_WF5G_ADB_CBANK_TUNE = 0x3;
    rg_tx_a1.b.RG_WF5G_ADB_CORE_DCIB_TUNE = 0x4;
    rg_tx_a1.b.RG_WF5G_ADB_CORE_ED_GAIN = 0x2;
    rg_tx_a1.b.RG_WF5G_ADB_EN = 0x1;
    rg_tx_a1.b.RG_WF5G_ADB_IBIAS_CGM = 0x4;
    rg_tx_a1.b.RG_WF5G_ADB_IBIAS_PTAT = 0x1;
    rf_write_register(RG_TX_A1, rg_tx_a1.data);

    rg_tx_a2.data = rf_read_register(RG_TX_A2);
    rg_tx_a2.b.RG_WF5G_TXM_DECT_VBLO = 0x3;
    rg_tx_a2.b.RG_WF5G_TX_IB_I = 0x3;
    rg_tx_a2.b.RG_WF5G_TX_VRCT_D = 0x8;
    rg_tx_a2.b.RG_WF5G_TX_VRCT_U = 0x8;
    rf_write_register(RG_TX_A2, rg_tx_a2.data);

    rg_tx_a3.data = rf_read_register(RG_TX_A3);
    rg_tx_a3.b.RG_WF5G_TXPA_GAIN_MAN = 0x7;
    rg_tx_a3.b.RG_WF5G_TXM_GAIN_MAN = 0x7;
    rg_tx_a3.b.RG_WF5G_TX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_TX_A3, rg_tx_a3.data);

    rg_tx_a6.data = rf_read_register(RG_TX_A6);
    rg_tx_a6.b.RG_M2_WF5G_TXM_EN = 0x1;
    rg_tx_a6.b.RG_M2_WF5G_TXM_V2I_EN = 0x1;
    rg_tx_a6.b.RG_M2_WF5G_TXPA_EN = 0x1;
    rg_tx_a6.b.RG_M2_WF5G_TX_IB_EN = 0x1;
    rg_tx_a6.b.RG_M2_WF5G_TX_VRCT_EN = 0x1;
    rf_write_register(RG_TX_A6, rg_tx_a6.data);

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE = 0x1;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x9c;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);

    rg_rx_a5.data = rf_read_register(RG_RX_A5);
    rg_rx_a5.b.RG_M2_WF5G_RX_LNA_TRSW_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF_RTX_ABB_TOPBIAS_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF_RTX_ABB_TX_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF_TX_LPF_BIAS_EN = 0x1;
    rg_rx_a5.b.RG_M2_WF_TX_LPF_EN = 0x1;
    rf_write_register(RG_RX_A5, rg_rx_a5.data);

    rg_rx_a6.data = rf_read_register(RG_RX_A6);
    rg_rx_a6.b.RG_M3_WF5G_RX_FE_TOPBIAS_EN = 0x1;
    rg_rx_a6.b.RG_M3_WF5G_RX_LNA_BIAS_EN = 0x1;
    rg_rx_a6.b.RG_M3_WF5G_RX_GM_BIAS_EN = 0x1;
    rg_rx_a6.b.RG_M3_WF5G_RX_MXR_BIAS_EN = 0x1;
    rg_rx_a6.b.RG_M3_WF5G_RX_LNA_EN = 0x1;
    rg_rx_a6.b.RG_M3_WF5G_RX_GM_EN = 0x1;
    rg_rx_a6.b.RG_M3_WF5G_RX_MXR_EN = 0x1;
    rg_rx_a6.b.RG_M3_WF_RTX_ABB_TOPBIAS_EN = 0x1;
    rg_rx_a6.b.RG_M3_WF_RTX_ABB_RX_EN = 0x1;
    rg_rx_a6.b.RG_M3_WF_RX_TIA_BIAS_EN = 0x1;
    rg_rx_a6.b.RG_M3_WF_RX_RSSIPGA_BIAS_EN = 0x1;
    rg_rx_a6.b.RG_M3_WF_RX_LPF_BIAS_EN = 0x1;
    rg_rx_a6.b.RG_M3_WF_RX_TIA_EN = 0x1;
    rg_rx_a6.b.RG_M3_WF_RX_RSSIPGA_EN = 0x1;
    rg_rx_a6.b.RG_M3_WF_RX_LPF_EN = 0x1;
    rg_rx_a6.b.RG_M3_WF_RX_LPF_I_DCOC_EN = 0x1;
    rg_rx_a6.b.RG_M3_WF_RX_LPF_Q_DCOC_EN = 0x1;
    rf_write_register(RG_RX_A6, rg_rx_a6.data);

    rg_rx_a7.data = rf_read_register(RG_RX_A7);
    rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND0 = 0xd;
    rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND1 = 0xc;
    rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND2 = 0xa;
    rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND3 = 0x9;
    rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND4 = 0x8;
    rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND5 = 0x7;
    rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND6 = 0x5;
    rg_rx_a7.b.RG_WF5G_RX_LNA_TANK_SEL_BAND7 = 0x4;
    rf_write_register(RG_RX_A7, rg_rx_a7.data);

    rg_rx_a8.data = rf_read_register(RG_RX_A8);
    rg_rx_a8.b.RG_WF5G_RX_LNA_TANK_SEL_MAN = 0x7;
    rf_write_register(RG_RX_A8, rg_rx_a8.data);

    rg_rx_a9.data = rf_read_register(RG_RX_A9);
    rg_rx_a9.b.RG_WF5G_RX_LNA_TANK_OFFSET_G5 = 0x2;
    rg_rx_a9.b.RG_WF5G_RX_LNA_TANK_OFFSET_G6 = 0x2;
    rg_rx_a9.b.RG_WF5G_RX_LNA_TANK_OFFSET_G7 = 0x3;
    rf_write_register(RG_RX_A9, rg_rx_a9.data);

    rg_rx_a14.data = rf_read_register(RG_RX_A14);
    rg_rx_a14.b.RG_WF5G_RX_LNA_ICTRL_G1 = 0x7;
    rg_rx_a14.b.RG_WF5G_RX_LNA_ICTRL_G3 = 0x7;
    rg_rx_a14.b.RG_WF5G_RX_LNA_ICTRL_G5 = 0x4;
    rf_write_register(RG_RX_A14, rg_rx_a14.data);

    rg_rx_a15.data = rf_read_register(RG_RX_A15);
    rg_rx_a15.b.RG_WF5G_RX_LNA_SLICE_G2 = 0x2;
    rg_rx_a15.b.RG_WF5G_RX_LNA_SLICE_G3 = 0x3;
    rg_rx_a15.b.RG_WF5G_RX_LNA_SLICE_G4 = 0xc;
    rf_write_register(RG_RX_A15, rg_rx_a15.data);

    rg_rx_a16.data = rf_read_register(RG_RX_A16);
    rg_rx_a16.b.RG_WF5G_RX_LNA_SLICE_G5 = 0x14;
    rf_write_register(RG_RX_A16, rg_rx_a16.data);

    rg_rx_a18.data = rf_read_register(RG_RX_A18);
    rg_rx_a18.b.RG_WF5G_RX_LNA_RFB_HB_G6 = 0x1;
    rf_write_register(RG_RX_A18, rg_rx_a18.data);

    rg_rx_a19.data = rf_read_register(RG_RX_A19);
    rg_rx_a19.b.RG_WF5G_RX_LNA_RT_G1 = 0x0;
    rg_rx_a19.b.RG_WF5G_RX_LNA_RT_G3 = 0x3;
    rg_rx_a19.b.RG_WF5G_RX_LNA_RT_G6 = 0x0;
    rf_write_register(RG_RX_A19, rg_rx_a19.data);

    rg_rx_a20.data = rf_read_register(RG_RX_A20);
    rg_rx_a20.b.RG_WF5G_RX_LNA_ATT_LB_G1 = 0x0;
    rg_rx_a20.b.RG_WF5G_RX_LNA_ATT_LB_G2 = 0x0;
    rg_rx_a20.b.RG_WF5G_RX_LNA_ATT_HB_G1 = 0x0;
    rg_rx_a20.b.RG_WF5G_RX_LNA_ATT_HB_G2 = 0x0;
    rg_rx_a20.b.RG_WF5G_RX_LNA_ATT_HB_G3 = 0x0;
    rf_write_register(RG_RX_A20, rg_rx_a20.data);

    rg_rx_a23.data = rf_read_register(RG_RX_A23);
    rg_rx_a23.b.RG_WF_RX_TIA_CUNIT_SEL_G1 = 0x7;
    rg_rx_a23.b.RG_WF_RX_TIA_CUNIT_SEL_G2 = 0x5;
    rg_rx_a23.b.RG_WF_RX_TIA_CUNIT_SEL_G3 = 0x3;
    rg_rx_a23.b.RG_WF_RX_TIA_CUNIT_SEL_G4 = 0x2;
    rg_rx_a23.b.RG_WF_RX_TIA_CUNIT_SEL_G5 = 0x2;
    rg_rx_a23.b.RG_WF_RX_TIA_CUNIT_SEL_G6 = 0x1;
    rf_write_register(RG_RX_A23, rg_rx_a23.data);

    rg_rx_a27.data = rf_read_register(RG_RX_A27);
    rg_rx_a27.b.RG_WF_RX_LPF_C1UNIT_SEL_BW20M = 0x5;
    rg_rx_a27.b.RG_WF_RX_LPF_C1UNIT_SEL_BW40M = 0x2;
    rg_rx_a27.b.RG_WF_RX_LPF_C1UNIT_SEL_BW80M = 0x2;
    rg_rx_a27.b.RG_WF_RX_LPF_C1UNIT_SEL_BW160M = 0x0;
    rf_write_register(RG_RX_A27, rg_rx_a27.data);

    rg_rx_a28.data = rf_read_register(RG_RX_A28);
    rg_rx_a28.b.RG_WF_RX_LPF_C2UNIT_SEL_BW20M = 0x5;
    rg_rx_a28.b.RG_WF_RX_LPF_C2UNIT_SEL_BW40M = 0x2;
    rg_rx_a28.b.RG_WF_RX_LPF_C2UNIT_SEL_BW80M = 0x2;
    rg_rx_a28.b.RG_WF_RX_LPF_C2UNIT_SEL_BW160M = 0x0;
    rf_write_register(RG_RX_A28, rg_rx_a28.data);

    rg_rx_a30.data = rf_read_register(RG_RX_A30);
    rg_rx_a30.b.RG_WF_RX_LPF_CN_SEL_BW20MADC160M = 0x2;
    rg_rx_a30.b.RG_WF_RX_LPF_CN_SEL_BW40MADC160M = 0x2;
    rg_rx_a30.b.RG_WF_RX_LPF_CN_SEL_BW80MADC160M = 0x5;
    rg_rx_a30.b.RG_WF_RX_LPF_CN_SEL_BW80MADC320M = 0x2;
    rg_rx_a30.b.RG_WF_RX_LPF_CN_SEL_BW160MADC320M = 0x2;
    rf_write_register(RG_RX_A30, rg_rx_a30.data);

    rg_rx_a46.data = rf_read_register(RG_RX_A46);
    rg_rx_a46.b.RG_WF_RX_LPF_CTUNE_ISEL_MAN = 0x28;
    rg_rx_a46.b.RG_WF_RX_LPF_CTUNE_ISEL_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A46, rg_rx_a46.data);

    rg_rx_a47.data = rf_read_register(RG_RX_A47);
    rg_rx_a47.b.RG_WF_RX_LPF_CTUNE_QSEL_MAN = 0x28;
    rg_rx_a47.b.RG_WF_RX_LPF_CTUNE_QSEL_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A47, rg_rx_a47.data);

    rg_rx_a48.data = rf_read_register(RG_RX_A48);
    rg_rx_a48.b.RG_WF_RX_TIA_CTUNE_ISEL_MAN = 0x28;
    rg_rx_a48.b.RG_WF_RX_TIA_CTUNE_ISEL_MAN_MODE = 0x1;
    rg_rx_a48.b.RG_WF_RX_TIA_CTUNE_QSEL_MAN = 0x28;
    rg_rx_a48.b.RG_WF_RX_TIA_CTUNE_QSEL_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A48, rg_rx_a48.data);

    rg_rx_a49.data = rf_read_register(RG_RX_A49);
    rg_rx_a49.b.RG_WF_RX_LPF_NOTCH_CTUNE_SEL_MAN = 0x28;
    rg_rx_a49.b.RG_WF_RX_LPF_NOTCH_CTUNE_SEL_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A49, rg_rx_a49.data);

    rg_rx_a50.data = rf_read_register(RG_RX_A50);
    rg_rx_a50.b.RG_WF_RX_RSSIPGA_CTUNE_ISEL_MAN = 0x28;
    rg_rx_a50.b.RG_WF_RX_RSSIPGA_CTUNE_ISEL_MAN_MODE = 0x1;
    rg_rx_a50.b.RG_WF_RX_RSSIPGA_CTUNE_QSEL_MAN = 0x28;
    rg_rx_a50.b.RG_WF_RX_RSSIPGA_CTUNE_QSEL_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A50, rg_rx_a50.data);

    rg_rx_a54.data = rf_read_register(RG_RX_A54);
    rg_rx_a54.b.RG_WF_RX_RSSIPGA_GAIN = 0x0;
    rf_write_register(RG_RX_A54, rg_rx_a54.data);

    rg_rx_a55.data = rf_read_register(RG_RX_A55);
    rg_rx_a55.b.RG_WF_RX_LPF_DCOC_RSEL = 0x5;
    rg_rx_a55.b.RG_WF_RX_LPF_DCOC_RGSEL = 0x5;
    rf_write_register(RG_RX_A55, rg_rx_a55.data);

    rg_rx_a56.data = rf_read_register(RG_RX_A56);
    rg_rx_a56.b.RG_WF5G_RX_DPD_GM_GAIN = 0x7;
    rg_rx_a56.b.RG_WF5G_RX_DPD_GM_ICTRL = 0x7;
    rg_rx_a56.b.RG_WF5G_RX_DPD_MXR_LODRV_ICTRL = 0x7;
    rg_rx_a56.b.RG_WF5G_RX_MXR_LODRV_ICTRL = 0x7;
    rg_rx_a56.b.RG_WF5G_RX_MXR_VBSEL = 0x2;
    rg_rx_a56.b.RG_WF5G_RX_MXR_VCMSEL = 0x3;
    rf_write_register(RG_RX_A56, rg_rx_a56.data);

    rg_rx_a60.data = rf_read_register(RG_RX_A60);
    rg_rx_a60.b.RG_WF_TX_LPF_GAIN = 0x3;
    rf_write_register(RG_RX_A60, rg_rx_a60.data);

new_set_reg(0xe488, 0x000a0010);

new_set_reg(0xb078, 0x00680701);

new_set_reg(0xb100, 0x01900008);

}

#endif //RF_CALI_TEST
