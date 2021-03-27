
#ifdef RF_CALI_TEST
#include "rf_calibration.h"
void wf5g_rxdcoc(struct wf5g_rxdcoc_in* wf5g_rxdcoc_in, struct wf5g_rxdcoc_result* wf5g_rxdcoc_result)
{
    RG_RX_A6_FIELD_T    rg_rx_a6;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_ESTI_A65_FIELD_T    rg_esti_a65;
    RG_RX_A44_FIELD_T    rg_rx_a44;
    RG_RX_A32_FIELD_T    rg_rx_a32;
    RG_RX_A38_FIELD_T    rg_rx_a38;
    RG_RX_A33_FIELD_T    rg_rx_a33;
    RG_RX_A39_FIELD_T    rg_rx_a39;
    RG_RX_A34_FIELD_T    rg_rx_a34;
    RG_RX_A40_FIELD_T    rg_rx_a40;
    RG_RX_A35_FIELD_T    rg_rx_a35;
    RG_RX_A41_FIELD_T    rg_rx_a41;
    RG_RX_A36_FIELD_T    rg_rx_a36;
    RG_RX_A42_FIELD_T    rg_rx_a42;
    RG_RX_A37_FIELD_T    rg_rx_a37;
    RG_RX_A43_FIELD_T    rg_rx_a43;
    RG_ESTI_A17_FIELD_T    rg_esti_a17;
    RG_ESTI_A90_FIELD_T    rg_esti_a90;
    RG_ESTI_A91_FIELD_T    rg_esti_a91;

    // Internal variables definitions
    bool    RG_RX_A6_saved = False;
    RG_RX_A6_FIELD_T    rg_rx_a6_i;
    // Internal variables definitions
    bool    RG_RX_A2_saved = False;
    RG_RX_A2_FIELD_T    rg_rx_a2_i;
    // Internal variables definitions
    bool    RG_ESTI_A17_saved = False;
    RG_ESTI_A17_FIELD_T    rg_esti_a17_i;

    unsigned int bw_idx = wf5g_rxdcoc_in->bw_idx;
    unsigned int tia_idx = wf5g_rxdcoc_in->tia_idx;
    unsigned int lpf_idx = wf5g_rxdcoc_in->lpf_idx;
    bool manual_mode = wf5g_rxdcoc_in->manual_mode;

    unsigned int tmp;
    unsigned int rx_dc_rdy;
    unsigned int unlock_cnt = 0;
    unsigned int TIME_OUT_CNT = 300;

    int num = 0;
    unsigned int code_i = 0;
    unsigned int code_q = 0;  
    int dci;
    int dcq;

    rg_rx_a6.data = rf_read_register(RG_RX_A6);
    if(!RG_RX_A6_saved) {
        RG_RX_A6_saved = True;
        rg_rx_a6_i.b.RG_M3_WF5G_RX_LNA_EN = rg_rx_a6.b.RG_M3_WF5G_RX_LNA_EN;
    }
    rg_rx_a6.b.RG_M3_WF5G_RX_LNA_EN = 0x0;                         // Turn off LNA
    rf_write_register(RG_RX_A6, rg_rx_a6.data);

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    if(!RG_RX_A2_saved) {
        RG_RX_A2_saved = True;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN = rg_rx_a2.b.RG_WF_RX_GAIN_MAN;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE;
    }
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = wf5g_rxdcoc_in->wf_rx_gain;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);

    rg_esti_a65.data = fi_ahb_read(RG_ESTI_A65);
    rg_esti_a65.b.rg_adda_calc_count = 0x4000;
    fi_ahb_write(RG_ESTI_A65, rg_esti_a65.data);

    rg_rx_a44.data = rf_read_register(RG_RX_A44);
    rg_rx_a44.b.RG_WF_RX_LPF_I_DCOC_SEL_MAN_MODE = manual_mode;
    rg_rx_a44.b.RG_WF_RX_LPF_Q_DCOC_SEL_MAN_MODE = manual_mode;
    rf_write_register(RG_RX_A44, rg_rx_a44.data);

    CALI_MODE_T cali_mode = RXDCOC;
    cali_mode_access(cali_mode);

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

            rg_rx_a44.data = rf_read_register(RG_RX_A44);
            rg_rx_a44.b.RG_WF_RX_LPF_I_DCOC_SEL_MAN = code_i;
            rg_rx_a44.b.RG_WF_RX_LPF_Q_DCOC_SEL_MAN = code_q;
            rf_write_register(RG_RX_A44, rg_rx_a44.data);

        }
        else{    
            if (tia_idx == 0)
            {
                if((lpf_idx == 1))
                {

                    rg_rx_a32.data = rf_read_register(RG_RX_A32);
                    rg_rx_a32.b.RG_WF_RX_LPF_I_DCOC_SEL_TIALGLPFG1 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A32, rg_rx_a32.data);

                    rg_rx_a38.data = rf_read_register(RG_RX_A38);
                    rg_rx_a38.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIALGLPFG1 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A38, rg_rx_a38.data);

                }
                else if((lpf_idx == 2))
                {

                    rg_rx_a32.data = rf_read_register(RG_RX_A32);
                    rg_rx_a32.b.RG_WF_RX_LPF_I_DCOC_SEL_TIALGLPFG2 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A32, rg_rx_a32.data);

                    rg_rx_a38.data = rf_read_register(RG_RX_A38);
                    rg_rx_a38.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIALGLPFG2 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A38, rg_rx_a38.data);

                }
                else if((lpf_idx == 3))
                {

                    rg_rx_a32.data = rf_read_register(RG_RX_A32);
                    rg_rx_a32.b.RG_WF_RX_LPF_I_DCOC_SEL_TIALGLPFG3 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A32, rg_rx_a32.data);

                    rg_rx_a38.data = rf_read_register(RG_RX_A38);
                    rg_rx_a38.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIALGLPFG3 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A38, rg_rx_a38.data);

                }
                else if((lpf_idx == 4))
                {

                    rg_rx_a32.data = rf_read_register(RG_RX_A32);
                    rg_rx_a32.b.RG_WF_RX_LPF_I_DCOC_SEL_TIALGLPFG4 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A32, rg_rx_a32.data);

                    rg_rx_a38.data = rf_read_register(RG_RX_A38);
                    rg_rx_a38.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIALGLPFG4 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A38, rg_rx_a38.data);

                }
                else if((lpf_idx == 5))
                {

                    rg_rx_a33.data = rf_read_register(RG_RX_A33);
                    rg_rx_a33.b.RG_WF_RX_LPF_I_DCOC_SEL_TIALGLPFG5 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A33, rg_rx_a33.data);

                    rg_rx_a39.data = rf_read_register(RG_RX_A39);
                    rg_rx_a39.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIALGLPFG5 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A39, rg_rx_a39.data);

                }
                else if((lpf_idx == 6))
                {

                    rg_rx_a33.data = rf_read_register(RG_RX_A33);
                    rg_rx_a33.b.RG_WF_RX_LPF_I_DCOC_SEL_TIALGLPFG6 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A33, rg_rx_a33.data);

                    rg_rx_a39.data = rf_read_register(RG_RX_A39);
                    rg_rx_a39.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIALGLPFG6 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A39, rg_rx_a39.data);

                }
                else if((lpf_idx == 7))
                {

                    rg_rx_a33.data = rf_read_register(RG_RX_A33);
                    rg_rx_a33.b.RG_WF_RX_LPF_I_DCOC_SEL_TIALGLPFG7 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A33, rg_rx_a33.data);

                    rg_rx_a39.data = rf_read_register(RG_RX_A39);
                    rg_rx_a39.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIALGLPFG7 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A39, rg_rx_a39.data);

                }
                else if((lpf_idx == 8))
                {

                    rg_rx_a33.data = rf_read_register(RG_RX_A33);
                    rg_rx_a33.b.RG_WF_RX_LPF_I_DCOC_SEL_TIALGLPFG8 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A33, rg_rx_a33.data);

                    rg_rx_a39.data = rf_read_register(RG_RX_A39);
                    rg_rx_a39.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIALGLPFG8 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A39, rg_rx_a39.data);

            }
            else if((lpf_idx == 9))
            {

                    rg_rx_a34.data = rf_read_register(RG_RX_A34);
                    rg_rx_a34.b.RG_WF_RX_LPF_I_DCOC_SEL_TIALGLPFG9 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A34, rg_rx_a34.data);

                    rg_rx_a40.data = rf_read_register(RG_RX_A40);
                    rg_rx_a40.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIALGLPFG9 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A40, rg_rx_a40.data);

                }
                else if((lpf_idx == 10))
                {

                    rg_rx_a34.data = rf_read_register(RG_RX_A34);
                    rg_rx_a34.b.RG_WF_RX_LPF_I_DCOC_SEL_TIALGLPFG10 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A34, rg_rx_a34.data);

                    rg_rx_a40.data = rf_read_register(RG_RX_A40);
                    rg_rx_a40.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIALGLPFG10 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A40, rg_rx_a40.data);

                }
                else if((lpf_idx == 11))
                {

                    rg_rx_a34.data = rf_read_register(RG_RX_A34);
                    rg_rx_a34.b.RG_WF_RX_LPF_I_DCOC_SEL_TIALGLPFG11 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A34, rg_rx_a34.data);

                    rg_rx_a40.data = rf_read_register(RG_RX_A40);
                    rg_rx_a40.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIALGLPFG11 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A40, rg_rx_a40.data);

                }
                else if((lpf_idx == 12))
                {

                    rg_rx_a34.data = rf_read_register(RG_RX_A34);
                    rg_rx_a34.b.RG_WF_RX_LPF_I_DCOC_SEL_TIALGLPFG12 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A34, rg_rx_a34.data);

                    rg_rx_a40.data = rf_read_register(RG_RX_A40);
                    rg_rx_a40.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIALGLPFG12 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A40, rg_rx_a40.data);

                }
            }
            else
            {
                if((lpf_idx == 1))
                {

                    rg_rx_a35.data = rf_read_register(RG_RX_A35);
                    rg_rx_a35.b.RG_WF_RX_LPF_I_DCOC_SEL_TIAHGLPFG1 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A35, rg_rx_a35.data);

                    rg_rx_a41.data = rf_read_register(RG_RX_A41);
                    rg_rx_a41.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIAHGLPFG1 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A41, rg_rx_a41.data);

                }
                else if((lpf_idx == 2))
                {

                    rg_rx_a35.data = rf_read_register(RG_RX_A35);
                    rg_rx_a35.b.RG_WF_RX_LPF_I_DCOC_SEL_TIAHGLPFG2 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A35, rg_rx_a35.data);

                    rg_rx_a41.data = rf_read_register(RG_RX_A41);
                    rg_rx_a41.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIAHGLPFG2 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A41, rg_rx_a41.data);

                }
                else if((lpf_idx == 3))
                {

                    rg_rx_a35.data = rf_read_register(RG_RX_A35);
                    rg_rx_a35.b.RG_WF_RX_LPF_I_DCOC_SEL_TIAHGLPFG3 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A35, rg_rx_a35.data);

                    rg_rx_a41.data = rf_read_register(RG_RX_A41);
                    rg_rx_a41.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIAHGLPFG3 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A41, rg_rx_a41.data);

                }
                else if((lpf_idx == 4))
                {

                    rg_rx_a35.data = rf_read_register(RG_RX_A35);
                    rg_rx_a35.b.RG_WF_RX_LPF_I_DCOC_SEL_TIAHGLPFG4 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A35, rg_rx_a35.data);

                    rg_rx_a41.data = rf_read_register(RG_RX_A41);
                    rg_rx_a41.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIAHGLPFG4 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A41, rg_rx_a41.data);

                }
                else if((lpf_idx == 5))
                {

                    rg_rx_a36.data = rf_read_register(RG_RX_A36);
                    rg_rx_a36.b.RG_WF_RX_LPF_I_DCOC_SEL_TIAHGLPFG5 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A36, rg_rx_a36.data);

                    rg_rx_a42.data = rf_read_register(RG_RX_A42);
                    rg_rx_a42.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIAHGLPFG5 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A42, rg_rx_a42.data);

                }
                else if((lpf_idx == 6))
                {

                    rg_rx_a36.data = rf_read_register(RG_RX_A36);
                    rg_rx_a36.b.RG_WF_RX_LPF_I_DCOC_SEL_TIAHGLPFG6 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A36, rg_rx_a36.data);

                    rg_rx_a42.data = rf_read_register(RG_RX_A42);
                    rg_rx_a42.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIAHGLPFG6 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A42, rg_rx_a42.data);

                }
                else if((lpf_idx == 7))
                {

                    rg_rx_a36.data = rf_read_register(RG_RX_A36);
                    rg_rx_a36.b.RG_WF_RX_LPF_I_DCOC_SEL_TIAHGLPFG7 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A36, rg_rx_a36.data);

                    rg_rx_a42.data = rf_read_register(RG_RX_A42);
                    rg_rx_a42.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIAHGLPFG7 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A42, rg_rx_a42.data);

                }
                else if((lpf_idx == 8))
                {

                    rg_rx_a36.data = rf_read_register(RG_RX_A36);
                    rg_rx_a36.b.RG_WF_RX_LPF_I_DCOC_SEL_TIAHGLPFG8 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A36, rg_rx_a36.data);

                    rg_rx_a42.data = rf_read_register(RG_RX_A42);
                    rg_rx_a42.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIAHGLPFG8 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A42, rg_rx_a42.data);

                }
                else if((lpf_idx == 9))
                {

                    rg_rx_a37.data = rf_read_register(RG_RX_A37);
                    rg_rx_a37.b.RG_WF_RX_LPF_I_DCOC_SEL_TIAHGLPFG9 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A37, rg_rx_a37.data);

                    rg_rx_a43.data = rf_read_register(RG_RX_A43);
                    rg_rx_a43.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIAHGLPFG9 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A43, rg_rx_a43.data);

                }
                else if((lpf_idx == 10))
                {

                    rg_rx_a37.data = rf_read_register(RG_RX_A37);
                    rg_rx_a37.b.RG_WF_RX_LPF_I_DCOC_SEL_TIAHGLPFG10 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A37, rg_rx_a37.data);

                    rg_rx_a43.data = rf_read_register(RG_RX_A43);
                    rg_rx_a43.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIAHGLPFG10 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A43, rg_rx_a43.data);

                }
                else if((lpf_idx == 11))
                {

                    rg_rx_a37.data = rf_read_register(RG_RX_A37);
                    rg_rx_a37.b.RG_WF_RX_LPF_I_DCOC_SEL_TIAHGLPFG11 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A37, rg_rx_a37.data);

                    rg_rx_a43.data = rf_read_register(RG_RX_A43);
                    rg_rx_a43.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIAHGLPFG11 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A43, rg_rx_a43.data);

                }
                else if((lpf_idx == 12))
                {

                    rg_rx_a37.data = rf_read_register(RG_RX_A37);
                    rg_rx_a37.b.RG_WF_RX_LPF_I_DCOC_SEL_TIAHGLPFG12 = code_i;                      // DCOC code (I)
                    rf_write_register(RG_RX_A37, rg_rx_a37.data);

                    rg_rx_a43.data = rf_read_register(RG_RX_A43);
                    rg_rx_a43.b.RG_WF_RX_LPF_Q_DCOC_SEL_TIAHGLPFG12 = code_q;                      // DCOC code (Q)
                    rf_write_register(RG_RX_A43, rg_rx_a43.data);

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
            unlock_cnt = unlock_cnt + 1;
            }
            else
            {
                PUTS("RX DC ready\n");
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

        st_printf("rx dc out: num = %d, dci= %d, dcq = %d, codei = %d, codeq = %d\n", num, dci, dcq, code_i, code_q);
    }//end of for(num = 6;num >= -1;num--) 

    cali_mode_exit();

    // Revert the original value before calibration back
    rg_rx_a6.data = rf_read_register(RG_RX_A6);
    rg_rx_a6.b.RG_M3_WF5G_RX_LNA_EN = rg_rx_a6_i.b.RG_M3_WF5G_RX_LNA_EN;
    rf_write_register(RG_RX_A6, rg_rx_a6.data);
    // Revert the original value before calibration back
    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);
    // Revert the original value before calibration back
    rg_esti_a17.data = fi_ahb_read(RG_ESTI_A17);
    rg_esti_a17.b.rg_dcoc_read_response = rg_esti_a17_i.b.rg_dcoc_read_response;
    fi_ahb_write(RG_ESTI_A17, rg_esti_a17.data);
    PUTS("RX DC calib finish \n"); 
    wf5g_rxdcoc_resultl->cal_rx_dc.rxdc_i = dci;
    wf5g_rxdcoc_result->cal_rx_dc.rxdc_q = dcq;
    wf5g_rxdcoc_result->cal_rx_dc.rxdc_codei = code_i;
    wf5g_rxdcoc_result->cal_rx_dc.rxdc_codeq = code_q;
}

#endif //RF_CALI_TEST
