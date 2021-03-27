#include "rf_calibration.h"

#ifdef RF_CALI_TEST

void t9026_wf2g_rxdcoc(struct wf2g_rxdcoc_in* wf2g_rxdcoc_in, struct wf2g_rxdcoc_result* wf2g_rxdcoc_result)
{
    RG_RX_A91_FIELD_T    rg_rx_a91;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_ESTI_A64_FIELD_T    rg_esti_a64;
    RG_ESTI_A65_FIELD_T    rg_esti_a65;
    RG_RX_A56_FIELD_T    rg_rx_a56;
    RG_RX_A44_FIELD_T    rg_rx_a44;
    RG_RX_A50_FIELD_T    rg_rx_a50;
    RG_RX_A45_FIELD_T    rg_rx_a45;
    RG_RX_A51_FIELD_T    rg_rx_a51;
    RG_RX_A46_FIELD_T    rg_rx_a46;
    RG_RX_A52_FIELD_T    rg_rx_a52;
    RG_RX_A47_FIELD_T    rg_rx_a47;
    RG_RX_A53_FIELD_T    rg_rx_a53;
    RG_RX_A48_FIELD_T    rg_rx_a48;
    RG_RX_A54_FIELD_T    rg_rx_a54;
    RG_RX_A49_FIELD_T    rg_rx_a49;
    RG_RX_A55_FIELD_T    rg_rx_a55;
    RG_ESTI_A17_FIELD_T    rg_esti_a17;
    RG_ESTI_A90_FIELD_T    rg_esti_a90;
    RG_ESTI_A91_FIELD_T    rg_esti_a91;

    RG_XMIT_A4_FIELD_T   rg_xmit_a4;
    RG_XMIT_A3_FIELD_T   rg_xmit_a3;
    RG_XMIT_A46_FIELD_T  rg_xmit_a46;

    RG_RECV_A2_FIELD_T   rg_recv_a2;

    // Internal variables definitions
    bool    RG_RX_A91_saved = False;
    RG_RX_A91_FIELD_T    rg_rx_a91_i;
    // Internal variables definitions
    bool    RG_RX_A2_saved = False;
    RG_RX_A2_FIELD_T    rg_rx_a2_i;
    // Internal variables definitions
    bool    RG_ESTI_A17_saved = False;
    RG_ESTI_A17_FIELD_T    rg_esti_a17_i;
	bool    RG_XMIT_A4_saved = False;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4_i;
    bool    RG_XMIT_A3_saved = False;
    RG_XMIT_A3_FIELD_T    rg_xmit_a3_i;
	bool    RG_XMIT_A46_saved = False;
    RG_XMIT_A46_FIELD_T    rg_xmit_a46_i;

    unsigned int tia_idx = wf2g_rxdcoc_in->tia_idx;
    unsigned int lpf_idx = wf2g_rxdcoc_in->lpf_idx;
    bool manual_mode = wf2g_rxdcoc_in->manual_mode;


    unsigned int rx_dc_rdy;
    unsigned int unlock_cnt = 0;
    unsigned int TIME_OUT_CNT = RF_TIME_OUT_CNT;

    int num = 0;
    unsigned int code_i = 0;
    unsigned int code_q = 0;
    int dci;
    int dcq;
    int comp_dci;
    int comp_dcq;
    unsigned int dig_comp_dci;
    unsigned int dig_comp_dcq;

    CALI_MODE_T cali_mode = RXDCOC;
    rg_esti_a17.b.rg_dcoc_read_response = 0;
    rg_esti_a17_i.b.rg_dcoc_read_response = 0;

    if(manual_mode == 0){

        rg_rx_a91.data = rf_read_register(RG_RX_A91);
        if(!RG_RX_A91_saved) {
            RG_RX_A91_saved = True;
            rg_rx_a91_i.b.RG_M7_WF2G_RX_LNA_EN = rg_rx_a91.b.RG_M7_WF2G_RX_LNA_EN;
        }
        rg_rx_a91.b.RG_M7_WF2G_RX_LNA_EN = 0x0;                         // Turn off LNA
        rf_write_register(RG_RX_A91, rg_rx_a91.data);

   }

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    if(!RG_RX_A2_saved) {
        RG_RX_A2_saved = True;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE;
    }
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = wf2g_rxdcoc_in->wf_rx_gain;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);

    if(manual_mode == 1){
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

        rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
        if(!RG_XMIT_A46_saved) {
            RG_XMIT_A46_saved = True;
            rg_xmit_a46_i.b.rg_txpwc_comp_man_sel = rg_xmit_a46.b.rg_txpwc_comp_man_sel;
            rg_xmit_a46_i.b.rg_txpwc_comp_man = rg_xmit_a46.b.rg_txpwc_comp_man;
        }
        rg_xmit_a46.b.rg_txpwc_comp_man_sel = 0x1;
		rg_xmit_a46.b.rg_txpwc_comp_man = 0x00;//0x10;
        fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);
    }

    rg_esti_a64.data = fi_ahb_read(RG_ESTI_A64);
    rg_esti_a64.b.rg_adda_wait_count = 0x1000;
    fi_ahb_write(RG_ESTI_A64, rg_esti_a64.data);

    rg_esti_a65.data = fi_ahb_read(RG_ESTI_A65);
    rg_esti_a65.b.rg_adda_calc_count = 0x4000;
    fi_ahb_write(RG_ESTI_A65, rg_esti_a65.data);

    rg_rx_a56.data = rf_read_register(RG_RX_A56);
    rg_rx_a56.b.RG_WF_RX_LPF_I_DCOC_SEL_MAN_MODE = manual_mode;
    rg_rx_a56.b.RG_WF_RX_LPF_Q_DCOC_SEL_MAN_MODE = manual_mode;
    rf_write_register(RG_RX_A56, rg_rx_a56.data);

    t9026_cali_mode_access(cali_mode);

    code_i = 0x40;
    code_q = 0x40;
    for(num = 6; num >= -1; num--)
    {
        if(num != -1)
        {
            //code_i = code_i + (1 << num);
            //code_q = code_q + (1 << num);
        }
        else
        {
            code_i &= ~BIT(0);
            code_q &= ~BIT(0);
        }
        if(manual_mode == 1){

            rg_rx_a56.data = rf_read_register(RG_RX_A56);
            rg_rx_a56.b.RG_WF_RX_LPF_I_DCOC_SEL_MAN = code_i;
            rg_rx_a56.b.RG_WF_RX_LPF_Q_DCOC_SEL_MAN = code_q;
            rf_write_register(RG_RX_A56, rg_rx_a56.data);

        }
        else{
            if (tia_idx == 0)
            {
                if (lpf_idx == 1)
                {

                    rg_rx_a44.data = rf_read_register(RG_RX_A44);
                    rg_rx_a44.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIALGLPFG1 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A44, rg_rx_a44.data);

                    rg_rx_a50.data = rf_read_register(RG_RX_A50);
                    rg_rx_a50.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIALGLPFG1 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A50, rg_rx_a50.data);

                }
                else if (lpf_idx == 2)
                {

                    rg_rx_a44.data = rf_read_register(RG_RX_A44);
                    rg_rx_a44.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIALGLPFG2 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A44, rg_rx_a44.data);

                    rg_rx_a50.data = rf_read_register(RG_RX_A50);
                    rg_rx_a50.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIALGLPFG2 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A50, rg_rx_a50.data);

                }
                else if (lpf_idx == 3)
                {

                    rg_rx_a44.data = rf_read_register(RG_RX_A44);
                    rg_rx_a44.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIALGLPFG3 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A44, rg_rx_a44.data);

                    rg_rx_a50.data = rf_read_register(RG_RX_A50);
                    rg_rx_a50.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIALGLPFG3 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A50, rg_rx_a50.data);

                }
                else if (lpf_idx == 4)
                {

                    rg_rx_a44.data = rf_read_register(RG_RX_A44);
                    rg_rx_a44.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIALGLPFG4 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A44, rg_rx_a44.data);

                    rg_rx_a50.data = rf_read_register(RG_RX_A50);
                    rg_rx_a50.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIALGLPFG4 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A50, rg_rx_a50.data);

                }
                else if (lpf_idx == 5)
                {

                    rg_rx_a45.data = rf_read_register(RG_RX_A45);
                    rg_rx_a45.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIALGLPFG5 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A45, rg_rx_a45.data);

                    rg_rx_a51.data = rf_read_register(RG_RX_A51);
                    rg_rx_a51.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIALGLPFG5 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A51, rg_rx_a51.data);

                }
                else if (lpf_idx == 6)
                {

                    rg_rx_a45.data = rf_read_register(RG_RX_A45);
                    rg_rx_a45.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIALGLPFG6 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A45, rg_rx_a45.data);

                    rg_rx_a51.data = rf_read_register(RG_RX_A51);
                    rg_rx_a51.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIALGLPFG6 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A51, rg_rx_a51.data);

                }
                else if (lpf_idx == 7)
                {

                    rg_rx_a45.data = rf_read_register(RG_RX_A45);
                    rg_rx_a45.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIALGLPFG7 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A45, rg_rx_a45.data);

                    rg_rx_a51.data = rf_read_register(RG_RX_A51);
                    rg_rx_a51.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIALGLPFG7 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A51, rg_rx_a51.data);

                }
                else if (lpf_idx == 8)
                {

                    rg_rx_a45.data = rf_read_register(RG_RX_A45);
                    rg_rx_a45.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIALGLPFG8 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A45, rg_rx_a45.data);

                    rg_rx_a51.data = rf_read_register(RG_RX_A51);
                    rg_rx_a51.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIALGLPFG8 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A51, rg_rx_a51.data);

                }
                else if(lpf_idx == 9)
                {

                    rg_rx_a46.data = rf_read_register(RG_RX_A46);
                    rg_rx_a46.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIALGLPFG9 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A46, rg_rx_a46.data);

                    rg_rx_a52.data = rf_read_register(RG_RX_A52);
                    rg_rx_a52.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIALGLPFG9 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A52, rg_rx_a52.data);

                }
                else if(lpf_idx == 10)
                {

                    rg_rx_a46.data = rf_read_register(RG_RX_A46);
                    rg_rx_a46.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIALGLPFG10 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A46, rg_rx_a46.data);

                    rg_rx_a52.data = rf_read_register(RG_RX_A52);
                    rg_rx_a52.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIALGLPFG10 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A52, rg_rx_a52.data);

                }
                else if(lpf_idx == 11)
                {

                    rg_rx_a46.data = rf_read_register(RG_RX_A46);
                    rg_rx_a46.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIALGLPFG11 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A46, rg_rx_a46.data);

                    rg_rx_a52.data = rf_read_register(RG_RX_A52);
                    rg_rx_a52.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIALGLPFG11 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A52, rg_rx_a52.data);

                }
                else if(lpf_idx == 12)
                {

                    rg_rx_a46.data = rf_read_register(RG_RX_A46);
                    rg_rx_a46.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIALGLPFG12 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A46, rg_rx_a46.data);

                    rg_rx_a52.data = rf_read_register(RG_RX_A52);
                    rg_rx_a52.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIALGLPFG12 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A52, rg_rx_a52.data);

                }
            }
            else
            {
                if(lpf_idx == 1)
                {

                    rg_rx_a47.data = rf_read_register(RG_RX_A47);
                    rg_rx_a47.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIAHGLPFG1 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A47, rg_rx_a47.data);

                    rg_rx_a53.data = rf_read_register(RG_RX_A53);
                    rg_rx_a53.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIAHGLPFG1 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A53, rg_rx_a53.data);

                }
                else if(lpf_idx == 2)
                {

                    rg_rx_a47.data = rf_read_register(RG_RX_A47);
                    rg_rx_a47.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIAHGLPFG2 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A47, rg_rx_a47.data);

                    rg_rx_a53.data = rf_read_register(RG_RX_A53);
                    rg_rx_a53.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIAHGLPFG2 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A53, rg_rx_a53.data);

                }
                else if(lpf_idx == 3)
                {

                    rg_rx_a47.data = rf_read_register(RG_RX_A47);
                    rg_rx_a47.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIAHGLPFG3 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A47, rg_rx_a47.data);

                    rg_rx_a53.data = rf_read_register(RG_RX_A53);
                    rg_rx_a53.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIAHGLPFG3 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A53, rg_rx_a53.data);

                }
                else if(lpf_idx == 4)
                {

                    rg_rx_a47.data = rf_read_register(RG_RX_A47);
                    rg_rx_a47.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIAHGLPFG4 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A47, rg_rx_a47.data);

                    rg_rx_a53.data = rf_read_register(RG_RX_A53);
                    rg_rx_a53.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIAHGLPFG4 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A53, rg_rx_a53.data);

                }
                else if(lpf_idx == 5)
                {

                    rg_rx_a48.data = rf_read_register(RG_RX_A48);
                    rg_rx_a48.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIAHGLPFG5 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A48, rg_rx_a48.data);

                    rg_rx_a54.data = rf_read_register(RG_RX_A54);
                    rg_rx_a54.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIAHGLPFG5 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A54, rg_rx_a54.data);

                }
                else if(lpf_idx == 6)
                {

                    rg_rx_a48.data = rf_read_register(RG_RX_A48);
                    rg_rx_a48.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIAHGLPFG6 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A48, rg_rx_a48.data);

                    rg_rx_a54.data = rf_read_register(RG_RX_A54);
                    rg_rx_a54.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIAHGLPFG6 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A54, rg_rx_a54.data);

                }
                else if(lpf_idx == 7)
                {

                    rg_rx_a48.data = rf_read_register(RG_RX_A48);
                    rg_rx_a48.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIAHGLPFG7 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A48, rg_rx_a48.data);

                    rg_rx_a54.data = rf_read_register(RG_RX_A54);
                    rg_rx_a54.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIAHGLPFG7 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A54, rg_rx_a54.data);

                }
                else if(lpf_idx == 8)
                {

                    rg_rx_a48.data = rf_read_register(RG_RX_A48);
                    rg_rx_a48.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIAHGLPFG8 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A48, rg_rx_a48.data);

                    rg_rx_a54.data = rf_read_register(RG_RX_A54);
                    rg_rx_a54.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIAHGLPFG8 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A54, rg_rx_a54.data);

            }
            else if(lpf_idx == 9)
            {

                    rg_rx_a49.data = rf_read_register(RG_RX_A49);
                    rg_rx_a49.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIAHGLPFG9 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A49, rg_rx_a49.data);

                    rg_rx_a55.data = rf_read_register(RG_RX_A55);
                    rg_rx_a55.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIAHGLPFG9 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A55, rg_rx_a55.data);

            }
            else if(lpf_idx == 10)
            {

                    rg_rx_a49.data = rf_read_register(RG_RX_A49);
                    rg_rx_a49.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIAHGLPFG10 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A49, rg_rx_a49.data);

                    rg_rx_a55.data = rf_read_register(RG_RX_A55);
                    rg_rx_a55.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIAHGLPFG10 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A55, rg_rx_a55.data);

                }
                else if(lpf_idx == 11)
                {

                    rg_rx_a49.data = rf_read_register(RG_RX_A49);
                    rg_rx_a49.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIAHGLPFG11 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A49, rg_rx_a49.data);

                    rg_rx_a55.data = rf_read_register(RG_RX_A55);
                    rg_rx_a55.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIAHGLPFG11 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A55, rg_rx_a55.data);

                }
                else if(lpf_idx == 12)
                {

                    rg_rx_a49.data = rf_read_register(RG_RX_A49);
                    rg_rx_a49.b.RG_WF2G_RX_LPF_I_DCOC_SEL_TIAHGLPFG12 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A49, rg_rx_a49.data);

                    rg_rx_a55.data = rf_read_register(RG_RX_A55);
                    rg_rx_a55.b.RG_WF2G_RX_LPF_Q_DCOC_SEL_TIAHGLPFG12 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A55, rg_rx_a55.data);

                }
            }
        }

        rg_esti_a17.data = fi_ahb_read(RG_ESTI_A17);
        if(!RG_ESTI_A17_saved) {
            RG_ESTI_A17_saved = True;
            rg_esti_a17_i.b.rg_dcoc_read_response = rg_esti_a17.b.rg_dcoc_read_response;
        }
        rg_esti_a17.b.rg_dcoc_read_response = 0x1;                         // new_set_reg(0xec44, 0x80000000);//read response bypass to 0, soft read ready response to 1
        fi_ahb_write(RG_ESTI_A17, rg_esti_a17.data);

        rg_esti_a17.data = fi_ahb_read(RG_ESTI_A17);
        if (num != -1)
        {
            rx_dc_rdy = rg_esti_a17.b.ro_dcoc_code_ready;
        }
        else
        {
            rx_dc_rdy = rg_esti_a17.b.ro_dcoc_finish;
        }
        while (unlock_cnt < TIME_OUT_CNT)
        {
            if (rx_dc_rdy == 0)
            {

                rg_esti_a17.data = fi_ahb_read(RG_ESTI_A17);
                if (num != -1)
                {
                    rx_dc_rdy = rg_esti_a17.b.ro_dcoc_code_ready;
                }
                else
                {
                     rx_dc_rdy = rg_esti_a17.b.ro_dcoc_finish;
                }
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

        code_i = rg_esti_a17.b.ro_dcoc_code_i;
        code_q = rg_esti_a17.b.ro_dcoc_code_q;

        rg_esti_a17.data = fi_ahb_read(RG_ESTI_A17);
        rg_esti_a17.b.rg_dcoc_read_response = 0x0;                         // new_set_reg(0xec44, 0x80000000);//read response bypass to 0, soft read ready response to 1
        fi_ahb_write(RG_ESTI_A17, rg_esti_a17.data);

        //RF_DPRINTF(RF_DEBUG_RESULT, "2g unlock_count:%d, rx dc out: num = %d, dci= %d, dcq = %d, codei = %d, codeq = %d\n",
            //unlock_cnt, num, dci, dcq, code_i, code_q);
    }//end of for(num = 6;num >= -1;num--)

    wf2g_rxdcoc_result->cal_rx_dc.rxdc_i = dci;
    wf2g_rxdcoc_result->cal_rx_dc.rxdc_q = dcq;
    wf2g_rxdcoc_result->cal_rx_dc.rxdc_codei = code_i;
    wf2g_rxdcoc_result->cal_rx_dc.rxdc_codeq = code_q;
    //printk( "2g rx dc out: tia_idx = %d, lpf_idx = %d, dci= %d, dcq = %d, codei = %d, codeq = %d\n", tia_idx, lpf_idx, dci, dcq, code_i, code_q);
    t9026_cali_mode_exit();

    if(manual_mode == 1 && 0){
        rg_recv_a2.data = fi_ahb_read(RG_RECV_A2);
        rg_recv_a2.b.rg_rxi_dcoft = 0;
        rg_recv_a2.b.rg_rxq_dcoft = 0;
        fi_ahb_write(RG_RECV_A2, rg_recv_a2.data);

        t9026_cali_mode_access(cali_mode);

        rg_esti_a17.data = fi_ahb_read(RG_ESTI_A17);
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

        comp_dci = -1*dci;
        comp_dcq = -1*dcq;

        if(comp_dci > 0){
            dig_comp_dci = comp_dci;
        }else{
            dig_comp_dci = comp_dci + (1<<8);
        }

        if(comp_dcq > 0){
            dig_comp_dcq = comp_dcq;
        }else{
            dig_comp_dcq = comp_dcq + (1<<8);
        }

        rg_recv_a2.data = fi_ahb_read(RG_RECV_A2);
        rg_recv_a2.b.rg_rxi_dcoft = dig_comp_dci&0xff;
        rg_recv_a2.b.rg_rxq_dcoft = dig_comp_dcq&0xff;
        fi_ahb_write(RG_RECV_A2, rg_recv_a2.data);

        rg_esti_a17.data = fi_ahb_read(RG_ESTI_A17);
        rg_esti_a17.b.rg_dcoc_read_response = 0x0;                         // new_set_reg(0xec44, 0x80000000);//read response bypass to 0, soft read ready response to 1
        fi_ahb_write(RG_ESTI_A17, rg_esti_a17.data);

        t9026_cali_mode_exit();
    }

    if(manual_mode == 0){
        // Revert the original value before calibration back
        rg_rx_a91.data = rf_read_register(RG_RX_A91);
        rg_rx_a91.b.RG_M7_WF2G_RX_LNA_EN = rg_rx_a91_i.b.RG_M7_WF2G_RX_LNA_EN;
        rf_write_register(RG_RX_A91, rg_rx_a91.data);
    }
    // Revert the original value before calibration back
    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);
    // Revert the original value before calibration back
    rg_esti_a17.data = fi_ahb_read(RG_ESTI_A17);
    rg_esti_a17.b.rg_dcoc_read_response = rg_esti_a17_i.b.rg_dcoc_read_response;
    fi_ahb_write(RG_ESTI_A17, rg_esti_a17.data);
    if(manual_mode == 1){
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
    }
    RF_DPRINTF(RF_DEBUG_INFO, "RX DC calib finish \n");
}

#endif // RF_CALI_TEST
