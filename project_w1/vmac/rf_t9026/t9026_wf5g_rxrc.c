#include "rf_calibration.h"

#ifdef RF_CALI_TEST

unsigned int t9026_wf5g_rxrc(struct wf5g_rxrc_result* wf5g_rxrc_result)
{
    unsigned int rg_out = 0;
    unsigned int unlock_cnt = 0;
    unsigned int TIME_OUT_CNT = RF_TIME_OUT_CNT;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_RX_A57_FIELD_T    rg_rx_a57;
    RG_RX_A63_FIELD_T    rg_rx_a63;

    // Internal variables definitions
    bool    RG_RX_A2_saved = False;
    RG_RX_A2_FIELD_T    rg_rx_a2_i;
    // Internal variables definitions
    bool    RG_RX_A57_saved = False;
    RG_RX_A57_FIELD_T    rg_rx_a57_i;

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    if(!RG_RX_A2_saved) {
        RG_RX_A2_saved = True;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE;
    }
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);

    rg_rx_a57.data = rf_read_register(RG_RX_A57);
    if(!RG_RX_A57_saved) {
        RG_RX_A57_saved = True;
        rg_rx_a57_i.b.RG_WF_RX_RCK_CALI_EN = rg_rx_a57.b.RG_WF_RX_RCK_CALI_EN;
    }
    rg_rx_a57.b.RG_WF_RX_RCK_CALI_EN = 0x1;                         // Trigger Rx RC Calibration
    rf_write_register(RG_RX_A57, rg_rx_a57.data);

    rg_rx_a63.data = rf_read_register(RG_RX_A63);
    while (unlock_cnt < TIME_OUT_CNT)
    {
        if(rg_rx_a63.b.RO_WF_RX_RCK_VLD == 0)
        {

            rg_rx_a63.data = rf_read_register(RG_RX_A63);
            //g_fw_funcs.system_wait(1);
            unlock_cnt = unlock_cnt + 1;
            if(unlock_cnt == TIME_OUT_CNT)
            {
                RF_DPRINTF(RF_DEBUG_INFO, "error: RX RC TIME OUT\n");
            }
        }
        else
        {
            RF_DPRINTF(RF_DEBUG_INFO, "RX RC  READY\n");
            break;
        }
    }

    rg_rx_a63.data = rf_read_register(RG_RX_A63);
    rg_out = rg_rx_a63.data;
    wf5g_rxrc_result->cal_rx_rc.rxrc_out = rg_out;
    RF_DPRINTF(RF_DEBUG_RESULT, "unlock_cnt:%d, RX RC output: rxrc_out = 0x%x\n", unlock_cnt, rg_out);

    // Revert the original value before calibration back
    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);
    // Revert the original value before calibration back
    rg_rx_a57.data = rf_read_register(RG_RX_A57);
    rg_rx_a57.b.RG_WF_RX_RCK_CALI_EN = rg_rx_a57_i.b.RG_WF_RX_RCK_CALI_EN;
    rf_write_register(RG_RX_A57, rg_rx_a57.data);

    return rg_out;
}

#endif // RF_CALI_TEST

