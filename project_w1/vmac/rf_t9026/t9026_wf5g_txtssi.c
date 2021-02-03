#include "rf_calibration.h"

#ifdef RF_CALI_TEST
extern U8 g_temp_rise_flag;
extern U8 g_temp_fall_flag;
//10*log10(x)
int log_10(int in)
{
    int power_wl;
    int db3; // 3.01;
    int V;
    //int wl;
    int tableSize;
    int tmpW;
    int tmp1;
    int out;
    int out1;
	
    power_wl = 27;
    db3 = 3;

    if(in <= 0){
        out1 = -100;
        return out1;
    }

    out = db3 * (power_wl - 1);
    tmpW = in;
    V = (int)(1 << (power_wl - 1));
    //wl = 32;
    tableSize = 5;//int(log2(wl));
    if(tmpW == 0){
        out = 0;
    }else{
        while(tmpW < V){
        tmpW = tmpW * 2;
        out = out - db3;
    }
    //tmp1 = floor(tmpW * 1.0 / (1 << (power_wl - tableSize - 1)) + 0.5);
    tmp1 = tmpW >> (power_wl - tableSize - 1);

    if(tmp1 < 34)
        out = out*4 + 0;
    else if(tmp1 < 35)
        out = out*4 + 1;
    else if(tmp1 < 37)
        out = out*4 + 2;
    else if(tmp1 < 40)
        out = out*4 + 3;
    else if(tmp1 < 42)
        out = out*4 + 4;
    else if(tmp1 < 44)
        out = out*4 + 5;
    else if(tmp1 < 47)
        out = out*4 + 6;
    else if(tmp1 < 49)
        out = out*4 + 7;
    else if(tmp1 < 53)
        out = out*4 + 8;
    else if(tmp1 < 56)
        out = out*4 + 9;
    else if(tmp1 < 58)
        out = out*4 + 10;
    else if(tmp1 < 62)
        out = out*4 + 11;
    else
        out = out*4 + 12;
    }

	//out = out/4;
    return(out);
}

int exp_10(int in)//10^(x/10)support high to 33db
{
    //in: TC<8.6>
    //out: US<16.10> 
    int range = 8+3;
    int out;
    int tmpW;
    int out1;
    int V;
    int in_scale = 128;

    out1 = (int)(1 << (range-1));
    out1 = out1 * 1024;
    V = 3 * (range - 1);
    V = V * in_scale;

    tmpW = in;
    if(tmpW < -V)
        tmpW = -V;

    while(tmpW < V){
        tmpW = tmpW + 3*in_scale;
        out1 = out1 / 2;
    }

    tmpW = (tmpW - V) * 16;

    if(tmpW <= 1 * in_scale)
        out = out1 * 1024;
    else if(tmpW <= 3 * in_scale)//4
        //out = out + 0.125;
        out = out1 * 1054;
    else if(tmpW <= 5 * in_scale)
        //out = out + 0.25;
        out = out1 * 1085;
    else if(tmpW <= 7 * in_scale)
        //out = out + 0.375;
        out = out1 * 1116;
    else if(tmpW <= 9 * in_scale)
        //out = out + 0.5;
        out = out1 * 1149;
    else if(tmpW <= 11 * in_scale)//4
        //out = out + 0.625;
        out = out1 * 1182;
    else if(tmpW <= 13 * in_scale)
        //out = out + 0.75;
        out = out1 * 1217;
    else if(tmpW <= 15 * in_scale)
        //out = out + 0.875;
        out = out1 * 1253;
    else if(tmpW <= 17 * in_scale)
        //out = out + 1.0;
        out = out1 * 1289;
    else if(tmpW <= 19 * in_scale)
        //out = out + 1.125;
        out = out1 * 1327;
    else if(tmpW <= 21 * in_scale)
        //out = out + 1.25;
        out = out1 * 1366;
    else if(tmpW <= 23 * in_scale)
        //out = out + 1.375;
        out = out1 * 1405;
    else if(tmpW <= 25 * in_scale)
        //out = out + 1.5;
        out = out1 * 1446;
    else if(tmpW <= 27 * in_scale)
        //out = out + 1.625;
        out = out1 * 1489;
    else if(tmpW <= 29 * in_scale)
        //out = out + 1.75;
        out = out1 * 1532;
    else if(tmpW <= 31 * in_scale)
        //out = out + 1.875;
        out = out1 * 1577;
    else if(tmpW <= 33 * in_scale)
        //out = out + 2.0;
        out = out1 * 1623;
    else if(tmpW <= 35 * in_scale)
        //out = out + 2.125;
        out = out1 * 1670;
    else if(tmpW <= 37 * in_scale)
        //out = out + 2.25;
        out = out1 * 1719;
    else if(tmpW <= 39 * in_scale)
        //out = out + 2.375;
        out = out1 * 1769;
    else if(tmpW <= 41 * in_scale)
        //out = out + 2.5;
        out = out1 * 1821;
    else if(tmpW <= 43 * in_scale)
        //out = out + 2.625;
        out = out1 * 1874;
    else if(tmpW <= 45 * in_scale)
        //out = out + 2.75;
        out = out1 * 1929;
    else if(tmpW <= 47 * in_scale)
        //out = out + 2.875;
        out = out1 * 1985;
    else
        //out = out + 3.0;
        //out = out1 * 1.9953;
        out = out1 * 2043;
 
    //out = out /1024;
	
    return(out);
}

void t9026_wf5g_txtssi(struct wf5g_txtssi_result * wf5g_txtssi_result, int ph, int pl, int vh, int vl)
{
    unsigned int txpwc_rdy = 0;
    unsigned int unlock_cnt = 0;
    unsigned int TIME_OUT_CNT = RF_TIME_OUT_CNT;
    int slope_tmp;
    int offset_tmp;
    //float slope;
    //float offset;
    struct wf5g_rxdcoc_in wf5g_rxdcoc_in;
    struct wf5g_rxdcoc_result wf5g_rxdcoc_result;
    CALI_MODE_T cali_mode = TXTSSI;
    RG_TOP_A7_FIELD_T    rg_top_a7;
    RG_TOP_A15_FIELD_T    rg_top_a15;
    RG_TX_A2_FIELD_T    rg_tx_a2;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_RX_A5_FIELD_T    rg_rx_a5;
    RG_RX_A68_FIELD_T    rg_rx_a68;
    RG_TOP_A19_FIELD_T    rg_top_a19;
    RG_ESTI_A64_FIELD_T    rg_esti_a64;
    RG_XMIT_A46_FIELD_T    rg_xmit_a46;
    RG_XMIT_A3_FIELD_T    rg_xmit_a3;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4;
    RG_ESTI_A14_FIELD_T    rg_esti_a14;
    RG_ESTI_A67_FIELD_T    rg_esti_a67;
    RG_ESTI_A68_FIELD_T    rg_esti_a68;
    RG_ESTI_A69_FIELD_T    rg_esti_a69;

    // Internal variables definitions
    bool    RG_TOP_A7_saved = False;
    RG_TOP_A7_FIELD_T    rg_top_a7_i;
	// Internal variables definitions
    bool    RG_TOP_A15_saved = False;
    RG_TOP_A15_FIELD_T    rg_top_a15_i;
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
    bool    RG_RX_A68_saved = False;
    RG_RX_A68_FIELD_T    rg_rx_a68_i;
    // Internal variables definitions
    bool    RG_TOP_A19_saved = False;
    RG_TOP_A19_FIELD_T    rg_top_a19_i;
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

    rg_top_a15.data = rf_read_register(RG_TOP_A15);
    if(!RG_TOP_A15_saved) {
        RG_TOP_A15_saved = True;
        rg_top_a15_i.b.rg_wf_adda_man_mode = rg_top_a15.b.rg_wf_adda_man_mode;
		rg_top_a15_i.b.rg_wf_dac_ampctrl_man = rg_top_a15.b.rg_wf_dac_ampctrl_man;
    }
    rg_top_a15.b.rg_wf_adda_man_mode = 0x1;
	rg_top_a15.b.rg_wf_dac_ampctrl_man = 0x2;
    rf_write_register(RG_TOP_A15, rg_top_a15.data);

    rg_tx_a2.data = rf_read_register(RG_TX_A2);
    if(!RG_TX_A2_saved) {
        RG_TX_A2_saved = True;
        rg_tx_a2_i.b.RG_WF5G_TX_TSSI_EN = rg_tx_a2.b.RG_WF5G_TX_TSSI_EN;
        rg_tx_a2_i.b.RG_WF5G_TX_MAN_MODE = rg_tx_a2.b.RG_WF5G_TX_MAN_MODE;
    }
    rg_tx_a2.b.RG_WF5G_TX_TSSI_EN = 0x1;
    rg_tx_a2.b.RG_WF5G_TX_MAN_MODE = 0x1;
    rf_write_register(RG_TX_A2, rg_tx_a2.data);

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    if(!RG_RX_A2_saved) {
        RG_RX_A2_saved = True;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE;
    }
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x9a;
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

    rg_rx_a68.data = rf_read_register(RG_RX_A68);
    if(!RG_RX_A68_saved) {
        RG_RX_A68_saved = True;
        rg_rx_a68_i.b.RG_WF5G_TXLPBK2TIA_EN = rg_rx_a68.b.RG_WF5G_TXLPBK2TIA_EN;
    }
    rg_rx_a68.b.RG_WF5G_TXLPBK2TIA_EN = 0x1;
    rf_write_register(RG_RX_A68, rg_rx_a68.data);

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

    //do rx dc
    wf5g_rxdcoc_in.bw_idx = 0;
    wf5g_rxdcoc_in.tia_idx = 0;
    wf5g_rxdcoc_in.lpf_idx = 1;
    wf5g_rxdcoc_in.wf_rx_gain = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
    wf5g_rxdcoc_in.manual_mode = 1;
    t9026_wf5g_rxdcoc(&wf5g_rxdcoc_in, &wf5g_rxdcoc_result);

    rg_esti_a64.data = fi_ahb_read(RG_ESTI_A64);
    rg_esti_a64.b.rg_adda_wait_count = 0x1000;
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
            RF_DPRINTF(RF_DEBUG_INFO,"unlock_cnt:%d,TX TSSI NOT READY...\n", unlock_cnt);
            unlock_cnt = unlock_cnt + 1; 
        }
        else
        {
            RF_DPRINTF(RF_DEBUG_INFO,"TX TSSI READY\n");
            break;
        } //if(rx_irr_rdy == 0)
    } // while(unlock_cnt < TIME_OUT_CNT)

    rg_esti_a69.data = fi_ahb_read(RG_ESTI_A69);
    wf5g_txtssi_result->cal_tx_tssi.txpwc_slope = rg_esti_a69.b.ro_txpwc_slope;
    wf5g_txtssi_result->cal_tx_tssi.txpwc_offset = rg_esti_a69.b.ro_txpwc_offset;

    slope_tmp = rg_esti_a69.b.ro_txpwc_slope;  //9.5
    offset_tmp = rg_esti_a69.b.ro_txpwc_offset;  //15.9
    RF_DPRINTF(RF_DEBUG_RESULT, "TX TSSI: txpwc_slope_tmp = 0x%x, txpwc_offset_tmp = 0x%x\n", slope_tmp, offset_tmp);

    // not support in linux kernel
    /*
        if(slope_tmp & BIT(8))
        {
                slope_tmp = slope_tmp - (1 << 9);
         }
         slope = slope_tmp >> 4;

         if(offset_tmp & BIT(14))
        {
                offset_tmp = offset_tmp - (1 << 15);
         }
         offset = offset_tmp >> 6;

         PUTS("TX TSSI: txpwc_slope = %f, txpwc_offset = %f\n", slope, offset);
    */

    // Revert the original value before calibration back
    rg_top_a7.data = rf_read_register(RG_TOP_A7);
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RXB_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RXB_EN;
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX1_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RX1_EN;
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX2_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RX2_EN;
    rg_top_a7.b.RG_M2_WF_TRX_LF_LDO_RX_EN = rg_top_a7_i.b.RG_M2_WF_TRX_LF_LDO_RX_EN;
    rf_write_register(RG_TOP_A7, rg_top_a7.data);
	// Revert the original value before calibration back
    rg_top_a15.data = rf_read_register(RG_TOP_A15);
    rg_top_a15.b.rg_wf_adda_man_mode = rg_top_a15_i.b.rg_wf_adda_man_mode;
	rg_top_a15.b.rg_wf_dac_ampctrl_man = rg_top_a15_i.b.rg_wf_dac_ampctrl_man;
    rf_write_register(RG_TOP_A15, rg_top_a15.data);
    // Revert the original value before calibration back
    rg_tx_a2.data = rf_read_register(RG_TX_A2);
    rg_tx_a2.b.RG_WF5G_TX_TSSI_EN = rg_tx_a2_i.b.RG_WF5G_TX_TSSI_EN;
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
    rg_rx_a68.data = rf_read_register(RG_RX_A68);
    rg_rx_a68.b.RG_WF5G_TXLPBK2TIA_EN = rg_rx_a68_i.b.RG_WF5G_TXLPBK2TIA_EN;
    rf_write_register(RG_RX_A68, rg_rx_a68.data);
    // Revert the original value before calibration back
    rg_top_a19.data = rf_read_register(RG_TOP_A19);
    rg_top_a19.b.rg_m2_wf_radc_en = rg_top_a19_i.b.rg_m2_wf_radc_en;
    rg_top_a19.b.rg_m2_wf_wadc_enable_i = rg_top_a19_i.b.rg_m2_wf_wadc_enable_i;
    rg_top_a19.b.rg_m2_wf_wadc_enable_q = rg_top_a19_i.b.rg_m2_wf_wadc_enable_q;
    rf_write_register(RG_TOP_A19, rg_top_a19.data);
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
    RF_DPRINTF(RF_DEBUG_INFO,"TX TSSI calib finish \n");
}

void t9026_wf5g_txtssi_ratio(struct wf5g_txtssi_result * wf5g_txtssi_result, int slope_tmp, int offset_tmp, U8 tssi_chn_idx, U8 tx_mxr_gain)
{
    int slope;
    int offset;
    int dci;
    int dcq;
    int dci_init;
    int dcq_init;
    int tssi_dc;
    int pre_digital_gain = 58;
    int new_digital_gain = 58;
    int final_digital_gain;
    int target_pout = 1730;
    int real_pout;
    int gain_ratio = 0;
    unsigned int gain_ratio_limit[4];
    int retrynum = 0;
    int v_dc_pow = 0;
    int v_dc_dB = 0;
    unsigned int rx_dc_rdy;
    unsigned int unlock_cnt = 0;
    unsigned int TIME_OUT_CNT = RF_TIME_OUT_CNT;
    CALI_MODE_T cali_mode = RXDCOC;
    struct wf5g_rxdcoc_in wf5g_rxdcoc_in;
    struct wf5g_rxdcoc_result wf5g_rxdcoc_result;

    RG_TOP_A7_FIELD_T    rg_top_a7;
    RG_TOP_A15_FIELD_T    rg_top_a15;
    RG_TX_A2_FIELD_T    rg_tx_a2;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_RX_A5_FIELD_T    rg_rx_a5;
    RG_RX_A68_FIELD_T    rg_rx_a68;
    RG_TOP_A19_FIELD_T    rg_top_a19;
    RG_ESTI_A64_FIELD_T  rg_esti_a64;
    RG_ESTI_A65_FIELD_T  rg_esti_a65;
    RG_ESTI_A17_FIELD_T  rg_esti_a17;
    RG_ESTI_A90_FIELD_T  rg_esti_a90;
    RG_ESTI_A91_FIELD_T  rg_esti_a91;
    RG_XMIT_A4_FIELD_T   rg_xmit_a4;
    RG_XMIT_A3_FIELD_T   rg_xmit_a3;
    RG_XMIT_A46_FIELD_T  rg_xmit_a46;
    RG_XMIT_A56_FIELD_T    rg_xmit_a56;
    RG_RECV_A11_FIELD_T    rg_recv_a11;

    // Internal variables definitions
    bool    RG_TOP_A7_saved = False;
    RG_TOP_A7_FIELD_T    rg_top_a7_i;
    // Internal variables definitions
    bool    RG_TOP_A15_saved = False;
    RG_TOP_A15_FIELD_T    rg_top_a15_i;
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
    bool    RG_RX_A68_saved = False;
    RG_RX_A68_FIELD_T    rg_rx_a68_i;
    // Internal variables definitions
    bool    RG_TOP_A19_saved = False;
    RG_TOP_A19_FIELD_T    rg_top_a19_i;
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
    RG_ESTI_A17_FIELD_T    rg_esti_a17_i;

    gain_ratio_limit[0] = 100; //{100,160}
    gain_ratio_limit[1] = 90; //{90,150}
    gain_ratio_limit[2] = 90; //{90,150}
    gain_ratio_limit[3] = 80; //{80,140}
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

    rg_top_a15.data = rf_read_register(RG_TOP_A15);
    if(!RG_TOP_A15_saved) {
        RG_TOP_A15_saved = True;
        rg_top_a15_i.b.rg_wf_adda_man_mode = rg_top_a15.b.rg_wf_adda_man_mode;
        rg_top_a15_i.b.rg_wf_dac_ampctrl_man = rg_top_a15.b.rg_wf_dac_ampctrl_man;
    }
    rg_top_a15.b.rg_wf_adda_man_mode = 0x1;
    rg_top_a15.b.rg_wf_dac_ampctrl_man = 0x2;
    rf_write_register(RG_TOP_A15, rg_top_a15.data);

    rg_tx_a2.data = rf_read_register(RG_TX_A2);
    if(!RG_TX_A2_saved) {
        RG_TX_A2_saved = True;
        rg_tx_a2_i.b.RG_WF5G_TX_TSSI_EN = rg_tx_a2.b.RG_WF5G_TX_TSSI_EN;
        rg_tx_a2_i.b.RG_WF5G_TX_PA_GAIN_MAN = rg_tx_a2.b.RG_WF5G_TX_PA_GAIN_MAN;
        rg_tx_a2_i.b.RG_WF5G_TX_MXR_GAIN_MAN = rg_tx_a2.b.RG_WF5G_TX_MXR_GAIN_MAN;
        rg_tx_a2_i.b.RG_WF5G_TX_MAN_MODE = rg_tx_a2.b.RG_WF5G_TX_MAN_MODE;
    }
    rg_tx_a2.b.RG_WF5G_TX_TSSI_EN = 0x1;
    rg_tx_a2.b.RG_WF5G_TX_PA_GAIN_MAN = 0x7;
    rg_tx_a2.b.RG_WF5G_TX_MXR_GAIN_MAN = tx_mxr_gain;
    rg_tx_a2.b.RG_WF5G_TX_MAN_MODE = 0x1;
    rf_write_register(RG_TX_A2, rg_tx_a2.data);

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    if(!RG_RX_A2_saved) {
        RG_RX_A2_saved = True;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE;
    }
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x9a;
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

    rg_rx_a68.data = rf_read_register(RG_RX_A68);
    if(!RG_RX_A68_saved) {
        RG_RX_A68_saved = True;
        rg_rx_a68_i.b.RG_WF5G_TXLPBK2TIA_EN = rg_rx_a68.b.RG_WF5G_TXLPBK2TIA_EN;
    }
    rg_rx_a68.b.RG_WF5G_TXLPBK2TIA_EN = 0x1;
    rf_write_register(RG_RX_A68, rg_rx_a68.data);

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
    wf5g_rxdcoc_in.bw_idx = 0;
    wf5g_rxdcoc_in.tia_idx = 0;
    wf5g_rxdcoc_in.lpf_idx = 1;
    wf5g_rxdcoc_in.wf_rx_gain = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
    wf5g_rxdcoc_in.manual_mode = 1;
    t9026_wf5g_rxdcoc(&wf5g_rxdcoc_in, &wf5g_rxdcoc_result);

    dci_init = wf5g_rxdcoc_result.cal_rx_dc.rxdc_i;
    dcq_init = wf5g_rxdcoc_result.cal_rx_dc.rxdc_q;

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
    rg_esti_a64.b.rg_adda_wait_count = 0x10;  //0x1000;
    fi_ahb_write(RG_ESTI_A64, rg_esti_a64.data);

    rg_esti_a65.data = fi_ahb_read(RG_ESTI_A65);
    rg_esti_a65.b.rg_adda_calc_count = 0x4000;//0x4000;
    fi_ahb_write(RG_ESTI_A65, rg_esti_a65.data);

    rg_xmit_a46_i.b.rg_txpwc_comp_man_sel = 0;
    rg_xmit_a46_i.b.rg_txpwc_comp_man = 0;
    rg_esti_a17_i.b.rg_dcoc_read_response = 0;

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
        printk("dci = %d, dcq = %d \r\n",dci, dcq);
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
            new_digital_gain = new_digital_gain * exp_10((target_pout - real_pout) * 128/100/2)/1024/1024;
            if (new_digital_gain > 0xff)
            {
                new_digital_gain = 0xff;
            }
            final_digital_gain = new_digital_gain;
            retrynum++;
            continue;
        }
    }
    gain_ratio = final_digital_gain * 100/pre_digital_gain ;

    if (gain_ratio < gain_ratio_limit[tssi_chn_idx])
    {
        gain_ratio = gain_ratio_limit[tssi_chn_idx];
    }
    else if (gain_ratio > gain_ratio_limit[tssi_chn_idx] + 60)
    {
        gain_ratio = gain_ratio_limit[tssi_chn_idx] + 60;
    }
    wf5g_txtssi_result->cal_tx_tssi.gain_ratio = gain_ratio;
    RF_DPRINTF(RF_DEBUG_RESULT,"final_digital_gain = %d, gain_ratio = %d \r\n",final_digital_gain, gain_ratio);

    // Revert the original value before calibration back
    rg_top_a7.data = rf_read_register(RG_TOP_A7);
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RXB_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RXB_EN;
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX1_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RX1_EN;
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX2_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RX2_EN;
    rg_top_a7.b.RG_M2_WF_TRX_LF_LDO_RX_EN = rg_top_a7_i.b.RG_M2_WF_TRX_LF_LDO_RX_EN;
    rf_write_register(RG_TOP_A7, rg_top_a7.data);
    // Revert the original value before calibration back
    rg_top_a15.data = rf_read_register(RG_TOP_A15);
    rg_top_a15.b.rg_wf_adda_man_mode = rg_top_a15_i.b.rg_wf_adda_man_mode;
    rg_top_a15.b.rg_wf_dac_ampctrl_man = rg_top_a15_i.b.rg_wf_dac_ampctrl_man;
    rf_write_register(RG_TOP_A15, rg_top_a15.data);
    // Revert the original value before calibration back
    rg_tx_a2.data = rf_read_register(RG_TX_A2);
    rg_tx_a2.b.RG_WF5G_TX_TSSI_EN = rg_tx_a2_i.b.RG_WF5G_TX_TSSI_EN;
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
    rg_rx_a68.data = rf_read_register(RG_RX_A68);
    rg_rx_a68.b.RG_WF5G_TXLPBK2TIA_EN = rg_rx_a68_i.b.RG_WF5G_TXLPBK2TIA_EN;
    rf_write_register(RG_RX_A68, rg_rx_a68.data);
    // Revert the original value before calibration back
    rg_top_a19.data = rf_read_register(RG_TOP_A19);
    rg_top_a19.b.rg_m2_wf_radc_en = rg_top_a19_i.b.rg_m2_wf_radc_en;
    rg_top_a19.b.rg_m2_wf_wadc_enable_i = rg_top_a19_i.b.rg_m2_wf_wadc_enable_i;
    rg_top_a19.b.rg_m2_wf_wadc_enable_q = rg_top_a19_i.b.rg_m2_wf_wadc_enable_q;
    rf_write_register(RG_TOP_A19, rg_top_a19.data);
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
    rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
    rg_xmit_a46.b.rg_txpwc_comp_man_sel = rg_xmit_a46_i.b.rg_txpwc_comp_man_sel;
    rg_xmit_a46.b.rg_txpwc_comp_man = rg_xmit_a46_i.b.rg_txpwc_comp_man;
    fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);
    // Revert the original value before calibration back
    rg_esti_a17.data = fi_ahb_read(RG_ESTI_A17);
    rg_esti_a17.b.rg_dcoc_read_response = rg_esti_a17_i.b.rg_dcoc_read_response;
    fi_ahb_write(RG_ESTI_A17, rg_esti_a17.data);
    RF_DPRINTF(RF_DEBUG_INFO, "TX TSSI adjust finish \n");

    return;
}

void t9026_wf5g_txtssi_bias(int slope_tmp, int offset_tmp)
{
    int slope;
    int offset;
    int dci;
    int dcq;
    int dci_init;
    int dcq_init;
    int tssi_dc;
    int pre_digital_gain = 61;
    int target_pout = 1780;
    int real_pout;
    int pout_offset = 50;
    int retrynum = 0;
    int v_dc_pow = 0;
    int v_dc_dB = 0;
    unsigned int rx_dc_rdy;
    unsigned int unlock_cnt = 0;
    unsigned int TIME_OUT_CNT = RF_TIME_OUT_CNT;
    CALI_MODE_T cali_mode = RXDCOC;
    struct wf5g_rxdcoc_in wf5g_rxdcoc_in;
    struct wf5g_rxdcoc_result wf5g_rxdcoc_result;

    RG_TOP_A7_FIELD_T    rg_top_a7;
    RG_TOP_A15_FIELD_T    rg_top_a15;
    RG_TX_A1_FIELD_T    rg_tx_a1;
    RG_TX_A2_FIELD_T    rg_tx_a2;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_RX_A5_FIELD_T    rg_rx_a5;
    RG_RX_A68_FIELD_T    rg_rx_a68;
    RG_TOP_A19_FIELD_T    rg_top_a19;
    RG_ESTI_A64_FIELD_T  rg_esti_a64;
    RG_ESTI_A65_FIELD_T  rg_esti_a65;
    RG_ESTI_A17_FIELD_T  rg_esti_a17;
    RG_ESTI_A90_FIELD_T  rg_esti_a90;
    RG_ESTI_A91_FIELD_T  rg_esti_a91;
    RG_XMIT_A4_FIELD_T   rg_xmit_a4;
    RG_XMIT_A3_FIELD_T   rg_xmit_a3;
    RG_XMIT_A46_FIELD_T  rg_xmit_a46;
    RG_RECV_A11_FIELD_T    rg_recv_a11;

    // Internal variables definitions
    bool    RG_TOP_A7_saved = False;
    RG_TOP_A7_FIELD_T    rg_top_a7_i;
    // Internal variables definitions
    bool    RG_TOP_A15_saved = False;
    RG_TOP_A15_FIELD_T    rg_top_a15_i;
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
    bool    RG_RX_A68_saved = False;
    RG_RX_A68_FIELD_T    rg_rx_a68_i;
    // Internal variables definitions
    bool    RG_TOP_A19_saved = False;
    RG_TOP_A19_FIELD_T    rg_top_a19_i;
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
    RG_ESTI_A17_FIELD_T    rg_esti_a17_i;

    rg_esti_a17_i.b.rg_dcoc_read_response = 0;

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

    rg_top_a15.data = rf_read_register(RG_TOP_A15);
    if(!RG_TOP_A15_saved) {
        RG_TOP_A15_saved = True;
        rg_top_a15_i.b.rg_wf_adda_man_mode = rg_top_a15.b.rg_wf_adda_man_mode;
        rg_top_a15_i.b.rg_wf_dac_ampctrl_man = rg_top_a15.b.rg_wf_dac_ampctrl_man;
    }
    rg_top_a15.b.rg_wf_adda_man_mode = 0x1;
    rg_top_a15.b.rg_wf_dac_ampctrl_man = 0x2;
    rf_write_register(RG_TOP_A15, rg_top_a15.data);

    rg_tx_a2.data = rf_read_register(RG_TX_A2);
    if(!RG_TX_A2_saved) {
        RG_TX_A2_saved = True;
        rg_tx_a2_i.b.RG_WF5G_TX_TSSI_EN = rg_tx_a2.b.RG_WF5G_TX_TSSI_EN;
        rg_tx_a2_i.b.RG_WF5G_TX_MAN_MODE = rg_tx_a2.b.RG_WF5G_TX_MAN_MODE;
    }
    rg_tx_a2.b.RG_WF5G_TX_TSSI_EN = 0x1;
    rg_tx_a2.b.RG_WF5G_TX_MAN_MODE = 0x1;
    rf_write_register(RG_TX_A2, rg_tx_a2.data);

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    if(!RG_RX_A2_saved) {
        RG_RX_A2_saved = True;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE;
    }
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x9a;
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

    rg_rx_a68.data = rf_read_register(RG_RX_A68);
    if(!RG_RX_A68_saved) {
        RG_RX_A68_saved = True;
        rg_rx_a68_i.b.RG_WF5G_TXLPBK2TIA_EN = rg_rx_a68.b.RG_WF5G_TXLPBK2TIA_EN;
    }
    rg_rx_a68.b.RG_WF5G_TXLPBK2TIA_EN = 0x1;
    rf_write_register(RG_RX_A68, rg_rx_a68.data);

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

    //close rx irr compensation
    rg_recv_a11.data = fi_ahb_read(RG_RECV_A11);
    rg_recv_a11.b.rg_rxirr_man_mode = 0x0;
    rg_recv_a11.b.rg_rxirr_bypass = 0x1;
    fi_ahb_write(RG_RECV_A11, rg_recv_a11.data);

    //do rx dc
    wf5g_rxdcoc_in.bw_idx = 0;
    wf5g_rxdcoc_in.tia_idx = 0;
    wf5g_rxdcoc_in.lpf_idx = 1;
    wf5g_rxdcoc_in.wf_rx_gain = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
    wf5g_rxdcoc_in.manual_mode = 1;
    t9026_wf5g_rxdcoc(&wf5g_rxdcoc_in, &wf5g_rxdcoc_result);

    dci_init = wf5g_rxdcoc_result.cal_rx_dc.rxdc_i;
    dcq_init = wf5g_rxdcoc_result.cal_rx_dc.rxdc_q;

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

    rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
    if(!RG_XMIT_A46_saved) {
        RG_XMIT_A46_saved = True;
        rg_xmit_a46_i.b.rg_txpwc_comp_man_sel = rg_xmit_a46.b.rg_txpwc_comp_man_sel;
        rg_xmit_a46_i.b.rg_txpwc_comp_man = rg_xmit_a46.b.rg_txpwc_comp_man;
    }
    rg_xmit_a46.b.rg_txpwc_comp_man_sel = 0x1;
    rg_xmit_a46.b.rg_txpwc_comp_man = pre_digital_gain;
    fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);

    while(retrynum < 3)
    {
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
        printk("dci = %d, dcq = %d \r\n",dci, dcq);
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

	 if(abs(target_pout - real_pout) <= pout_offset)
	 {
            break;
	 }else if (real_pout > target_pout + pout_offset){
            rg_tx_a1.data = rf_read_register(RG_TX_A1);
            rg_tx_a1.b.RG_WF5G_TX_ADB_ED_GAIN = rg_tx_a1.b.RG_WF5G_TX_ADB_ED_GAIN - 1;
            rf_write_register(RG_TX_A1, rg_tx_a1.data);
            retrynum++;
            continue;
        }else if (real_pout < target_pout - pout_offset){
            rg_tx_a1.data = rf_read_register(RG_TX_A1);
            rg_tx_a1.b.RG_WF5G_TX_ADB_ED_GAIN = rg_tx_a1.b.RG_WF5G_TX_ADB_ED_GAIN + 1;
            rf_write_register(RG_TX_A1, rg_tx_a1.data);
            retrynum++;
            continue;
        }
    }
    printk("0xff000804 = 0x%x \r\n", rf_read_register(RG_TX_A1));
    // Revert the original value before calibration back
    rg_top_a7.data = rf_read_register(RG_TOP_A7);
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RXB_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RXB_EN;
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX1_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RX1_EN;
    rg_top_a7.b.RG_M2_WF_TRX_HF_LDO_RX2_EN = rg_top_a7_i.b.RG_M2_WF_TRX_HF_LDO_RX2_EN;
    rg_top_a7.b.RG_M2_WF_TRX_LF_LDO_RX_EN = rg_top_a7_i.b.RG_M2_WF_TRX_LF_LDO_RX_EN;
    rf_write_register(RG_TOP_A7, rg_top_a7.data);
    // Revert the original value before calibration back
    rg_top_a15.data = rf_read_register(RG_TOP_A15);
    rg_top_a15.b.rg_wf_adda_man_mode = rg_top_a15_i.b.rg_wf_adda_man_mode;
    rg_top_a15.b.rg_wf_dac_ampctrl_man = rg_top_a15_i.b.rg_wf_dac_ampctrl_man;
    rf_write_register(RG_TOP_A15, rg_top_a15.data);
    // Revert the original value before calibration back
    rg_tx_a2.data = rf_read_register(RG_TX_A2);
    rg_tx_a2.b.RG_WF5G_TX_TSSI_EN = rg_tx_a2_i.b.RG_WF5G_TX_TSSI_EN;
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
    rg_rx_a68.data = rf_read_register(RG_RX_A68);
    rg_rx_a68.b.RG_WF5G_TXLPBK2TIA_EN = rg_rx_a68_i.b.RG_WF5G_TXLPBK2TIA_EN;
    rf_write_register(RG_RX_A68, rg_rx_a68.data);
    // Revert the original value before calibration back
    rg_top_a19.data = rf_read_register(RG_TOP_A19);
    rg_top_a19.b.rg_m2_wf_radc_en = rg_top_a19_i.b.rg_m2_wf_radc_en;
    rg_top_a19.b.rg_m2_wf_wadc_enable_i = rg_top_a19_i.b.rg_m2_wf_wadc_enable_i;
    rg_top_a19.b.rg_m2_wf_wadc_enable_q = rg_top_a19_i.b.rg_m2_wf_wadc_enable_q;
    rf_write_register(RG_TOP_A19, rg_top_a19.data);
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
    rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
    rg_xmit_a46.b.rg_txpwc_comp_man_sel = rg_xmit_a46_i.b.rg_txpwc_comp_man_sel;
    rg_xmit_a46.b.rg_txpwc_comp_man = rg_xmit_a46_i.b.rg_txpwc_comp_man;
    fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);
    // Revert the original value before calibration back
    rg_esti_a17.data = fi_ahb_read(RG_ESTI_A17);
    rg_esti_a17.b.rg_dcoc_read_response = rg_esti_a17_i.b.rg_dcoc_read_response;
    fi_ahb_write(RG_ESTI_A17, rg_esti_a17.data);
    RF_DPRINTF(RF_DEBUG_INFO, "TX TSSI bias finish \n");
}

void t9026_wf5g_get_efuse_tssi(struct wf5g_txtssi_result * wf5g_txtssi_result, unsigned int channel)
{
    unsigned int slope_tmp = 0;
    unsigned int offset_tmp = 0;
    unsigned int tmp = 0;
    unsigned int tmp1 = 0;
    U8 tssi_chn_idx = 0;
    U32 temp_offset = 0;

    if (g_temp_rise_flag == 1)
    {
        temp_offset = 0xc0;
    }
    else
    {
        temp_offset = 0;
    }
    tmp = efuse_manual_read(0x9);

    if (channel >= 0x24 && channel <= 0x3c)
    {
        if (tmp & BIT(17))
        {
            tmp1 = efuse_manual_read(0xb);
            slope_tmp = tmp1 & 0xff;
            offset_tmp = (tmp1 & 0xfff0000) >> 12;
            offset_tmp = offset_tmp  - temp_offset - 0x140;
        }
        else
        {
            slope_tmp = 31;
            offset_tmp = 29904;
        }
        tssi_chn_idx = 0;
    }
    else if (channel > 0x3c && channel <= 0x64)
    {
        if (tmp & BIT(18))
        {
            tmp1 = efuse_manual_read(0xc);
            slope_tmp = tmp1 & 0xff;
            offset_tmp = (tmp1 & 0xfff0000) >> 12;
            offset_tmp = offset_tmp  - temp_offset -0xf0;
        }
        else
        {
            slope_tmp = 28;
            offset_tmp = 30032;
        }
        tssi_chn_idx = 1;
    }
    else if (channel > 0x64 && channel <= 0x8c)
    {
        if (tmp & BIT(19))
        {
            tmp1 = efuse_manual_read(0xd);
            slope_tmp = tmp1 & 0xff;
            offset_tmp = (tmp1 & 0xfff0000) >> 12;
            offset_tmp = offset_tmp  - temp_offset - 0x40;
        }
        else
        {
            slope_tmp = 28;
            offset_tmp = 29888;
        }
        tssi_chn_idx = 2;
    }
    else if (channel > 0x8c && channel <= 0xa5)
    {
        if (tmp & BIT(20))
        {
            tmp1 = efuse_manual_read(0xe);
            slope_tmp = tmp1 & 0xff;
            offset_tmp = (tmp1 & 0xfff0000) >> 12;
            offset_tmp = offset_tmp  - temp_offset - 0x40;
        }
        else
        {
            slope_tmp = 31;
            offset_tmp = 29776;
        }
        tssi_chn_idx = 3;
    }
    wf5g_txtssi_result->cal_tx_tssi.txpwc_slope = slope_tmp;
    wf5g_txtssi_result->cal_tx_tssi.txpwc_offset = offset_tmp;
    wf5g_txtssi_result->cal_tx_tssi.tssi_chn_idx = tssi_chn_idx;
}
#endif //RF_CALI_TEST
