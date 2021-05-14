#include "rf_calibration.h"

#ifdef RF_CALI_TEST

int t9026_wf5g_check_loop_rx_irr(struct wf5g_rxirr_result* wf5g_rxirr_result, unsigned int bw_idx)
{
    int ret = 0;
    int i = 0;
    int cnt_max;
    int irr;
    int freq_offset;
    int pwr_list_tone[4];
    int pwr_list_img[4];
    unsigned freq_offset_list[4];
    int pass_idx = 0;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4;
    RG_XMIT_A3_FIELD_T    rg_xmit_a3;
    RG_RECV_A11_FIELD_T    rg_recv_a11;
    RG_RECV_A3_FIELD_T    rg_recv_a3;
    RG_RECV_A4_FIELD_T    rg_recv_a4;
    RG_RECV_A5_FIELD_T    rg_recv_a5;
    RG_RECV_A6_FIELD_T    rg_recv_a6;
    RG_RECV_A7_FIELD_T    rg_recv_a7;
    RG_RECV_A8_FIELD_T    rg_recv_a8;
    RG_RECV_A9_FIELD_T    rg_recv_a9;
    RG_RECV_A10_FIELD_T    rg_recv_a10;

    // Internal variables definitions
    bool    RG_RECV_A11_saved = False;
    RG_RECV_A11_FIELD_T    rg_recv_a11_i;
    // Internal variables definitions
    bool    RG_XMIT_A4_saved = False;
    RG_XMIT_A4_FIELD_T    rg_xmit_a4_i;
    // Internal variables definitions
    bool    RG_XMIT_A3_saved = False;
    RG_XMIT_A3_FIELD_T    rg_xmit_a3_i;

    if (bw_idx == 0) {
        freq_offset_list[0] = 4;
        freq_offset_list[1] = 9;
        cnt_max = 2;

    } else if (bw_idx == 1) {
        freq_offset_list[0] = 6;
        freq_offset_list[1] = 18;
        cnt_max = 2;

    } else {
        freq_offset_list[0] = 12;
        freq_offset_list[1] = 26;
        freq_offset_list[2] = 33;
        freq_offset_list[3] = 39;
        cnt_max = 4;
    }
 
    RF_DPRINTF(RF_DEBUG_INFO, "WF5G Check loop Rx IRR Start........\n");

    rg_recv_a11.data = fi_ahb_read(RG_RECV_A11);
    if(!RG_RECV_A11_saved) {
        RG_RECV_A11_saved = True;
        rg_recv_a11_i.b.rg_rxirr_man_mode = rg_recv_a11.b.rg_rxirr_man_mode;
        rg_recv_a11_i.b.rg_rxirr_bypass = rg_recv_a11.b.rg_rxirr_bypass;
    }
    rg_recv_a11.b.rg_rxirr_man_mode = 0x1;
    rg_recv_a11.b.rg_rxirr_bypass = 0x0;
    fi_ahb_write(RG_RECV_A11, rg_recv_a11.data);

    rg_recv_a3.data = fi_ahb_read(RG_RECV_A3);
    rg_recv_a3.b.rg_rxirr_eup_0 = wf5g_rxirr_result->cal_rx_irr.coeff_eup[0];
    rg_recv_a3.b.rg_rxirr_eup_1 = wf5g_rxirr_result->cal_rx_irr.coeff_eup[1];
    fi_ahb_write(RG_RECV_A3, rg_recv_a3.data);

    rg_recv_a4.data = fi_ahb_read(RG_RECV_A4);
    rg_recv_a4.b.rg_rxirr_eup_2 = wf5g_rxirr_result->cal_rx_irr.coeff_eup[2];
    rg_recv_a4.b.rg_rxirr_eup_3 = wf5g_rxirr_result->cal_rx_irr.coeff_eup[3];
    fi_ahb_write(RG_RECV_A4, rg_recv_a4.data);

    rg_recv_a5.data = fi_ahb_read(RG_RECV_A5);
    rg_recv_a5.b.rg_rxirr_pup_0 = wf5g_rxirr_result->cal_rx_irr.coeff_pup[0];
    rg_recv_a5.b.rg_rxirr_pup_1 = wf5g_rxirr_result->cal_rx_irr.coeff_pup[1];
    fi_ahb_write(RG_RECV_A5, rg_recv_a5.data);

    rg_recv_a6.data = fi_ahb_read(RG_RECV_A6);
    rg_recv_a6.b.rg_rxirr_pup_2 = wf5g_rxirr_result->cal_rx_irr.coeff_pup[2];
    rg_recv_a6.b.rg_rxirr_pup_3 = wf5g_rxirr_result->cal_rx_irr.coeff_pup[3];
    fi_ahb_write(RG_RECV_A6, rg_recv_a6.data);

    rg_recv_a7.data = fi_ahb_read(RG_RECV_A7);
    rg_recv_a7.b.rg_rxirr_elow_0 = wf5g_rxirr_result->cal_rx_irr.coeff_elow[0];
    rg_recv_a7.b.rg_rxirr_elow_1 = wf5g_rxirr_result->cal_rx_irr.coeff_elow[1];
    fi_ahb_write(RG_RECV_A7, rg_recv_a7.data);

    rg_recv_a8.data = fi_ahb_read(RG_RECV_A8);
    rg_recv_a8.b.rg_rxirr_elow_2 = wf5g_rxirr_result->cal_rx_irr.coeff_elow[2];
    rg_recv_a8.b.rg_rxirr_elow_3 = wf5g_rxirr_result->cal_rx_irr.coeff_elow[3];
    fi_ahb_write(RG_RECV_A8, rg_recv_a8.data);

    rg_recv_a9.data = fi_ahb_read(RG_RECV_A9);
    rg_recv_a9.b.rg_rxirr_plow_0 = wf5g_rxirr_result->cal_rx_irr.coeff_plow[0];
    rg_recv_a9.b.rg_rxirr_plow_1 = wf5g_rxirr_result->cal_rx_irr.coeff_plow[1];
    fi_ahb_write(RG_RECV_A9, rg_recv_a9.data);

    rg_recv_a10.data = fi_ahb_read(RG_RECV_A10);
    rg_recv_a10.b.rg_rxirr_plow_2 = wf5g_rxirr_result->cal_rx_irr.coeff_plow[2];
    rg_recv_a10.b.rg_rxirr_plow_3 = wf5g_rxirr_result->cal_rx_irr.coeff_plow[3];
    fi_ahb_write(RG_RECV_A10, rg_recv_a10.data);

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
        rg_xmit_a3_i.b.rg_tg2_tone_gain = rg_xmit_a3.b.rg_tg2_tone_gain;
        rg_xmit_a3_i.b.rg_tg2_enable = rg_xmit_a3.b.rg_tg2_enable;
        rg_xmit_a3_i.b.rg_tg2_tone_40 = rg_xmit_a3.b.rg_tg2_tone_40;
        rg_xmit_a3_i.b.rg_tg2_tone_man_en = rg_xmit_a3.b.rg_tg2_tone_man_en;
    }
    rg_xmit_a3.b.rg_tg2_f_sel = 0x200000;
    rg_xmit_a3.b.rg_tg2_tone_gain = 0x0;
    rg_xmit_a3.b.rg_tg2_enable = 0x1;
    rg_xmit_a3.b.rg_tg2_tone_40 = 0x0;
    rg_xmit_a3.b.rg_tg2_tone_man_en = 0x0;
    fi_ahb_write(RG_XMIT_A3, rg_xmit_a3.data);

    for (i = 0; i < cnt_max; i++)
    {
        freq_offset = freq_offset_list[i];
        pwr_list_tone[i] = t9026_get_spectrum_snr_pwr(-freq_offset, 0);
        pwr_list_img[i] = t9026_get_spectrum_snr_pwr(freq_offset, 1);
        RF_DPRINTF(RF_DEBUG_INFO, "SingleTone freq = %d M, tone power= %.2d, image power= %.2d\n", freq_offset, pwr_list_tone[i], pwr_list_img[i]);
   
        irr = pwr_list_tone[i] - pwr_list_img[i];
        if (irr >= 40 ) {
            ret = 0;
            pass_idx++;
            RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "SingleTone freq = %d M, irr = %.2d, test is pass!\n", freq_offset, irr);

        } else {
            ret = -1;
            RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "SingleTone freq = %d M, irr = %.2d, test is fail!\n", freq_offset, irr);
        }
    }


    for (i = 0; i < cnt_max; i++)
    {
        freq_offset = freq_offset_list[i];
        pwr_list_tone[i] = t9026_get_spectrum_snr_pwr(freq_offset, 0);
        pwr_list_img[i] = t9026_get_spectrum_snr_pwr(-freq_offset, 1);
        RF_DPRINTF(RF_DEBUG_INFO, "SingleTone freq = %d M, tone power= %.2d, image power= %.2d\n", ~freq_offset+1, pwr_list_tone[i], pwr_list_img[i]);
   
        irr = pwr_list_tone[i] - pwr_list_img[i];
        if (irr >= 40 ) {
            ret = 0;
            pass_idx++;
            RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "SingleTone freq = %d M, irr = %.2d, test is pass!\n", ~freq_offset+1, irr);

        } else {
            ret = -1;
            RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "SingleTone freq = %d M, irr = %.2d, test is fail!\n", ~freq_offset+1, irr);
        }
    }

    if(pass_idx == 2 * cnt_max)
        RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "WF5G All freq IRR test is pass!\n");


    // Revert the original value before calibration back
    rg_recv_a11.data = fi_ahb_read(RG_RECV_A11);
    rg_recv_a11.b.rg_rxirr_man_mode = rg_recv_a11_i.b.rg_rxirr_man_mode;
    rg_recv_a11.b.rg_rxirr_bypass = rg_recv_a11_i.b.rg_rxirr_bypass;
    fi_ahb_write(RG_RECV_A11, rg_recv_a11.data);
    // Revert the original value before calibration back
    rg_xmit_a4.data = fi_ahb_read(RG_XMIT_A4);
    rg_xmit_a4.b.rg_tx_signal_sel = rg_xmit_a4_i.b.rg_tx_signal_sel;
    fi_ahb_write(RG_XMIT_A4, rg_xmit_a4.data);
    // Revert the original value before calibration back
    rg_xmit_a3.data = fi_ahb_read(RG_XMIT_A3);
    rg_xmit_a3.b.rg_tg2_f_sel = rg_xmit_a3_i.b.rg_tg2_f_sel;
    rg_xmit_a3.b.rg_tg2_tone_gain = rg_xmit_a3_i.b.rg_tg2_tone_gain;
    rg_xmit_a3.b.rg_tg2_enable = rg_xmit_a3_i.b.rg_tg2_enable;
    rg_xmit_a3.b.rg_tg2_tone_40 = rg_xmit_a3_i.b.rg_tg2_tone_40;
    rg_xmit_a3.b.rg_tg2_tone_man_en = rg_xmit_a3_i.b.rg_tg2_tone_man_en;
    fi_ahb_write(RG_XMIT_A3, rg_xmit_a3.data);
    RF_DPRINTF(RF_DEBUG_INFO, "WF5G Check loop Rx IRR finish \n");
    return ret;   //PASS_RESULT
}


#endif // RF_CALI_TEST
