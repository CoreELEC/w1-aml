
#ifdef RF_CALI_TEST
#include "rf_calibration.h"
unsigned int wf5g_rxrc(struct wf5g_rxrc_result* wf5g_rxrc_result)
{
    unsigned int rg_out = 0;
    unsigned int unlock_cnt = 0;
    unsigned int TIME_OUT_CNT = 300;
    RG_RX_A2_FIELD_T    rg_rx_a2;
    RG_RX_A46_FIELD_T    rg_rx_a46;
    RG_RX_A47_FIELD_T    rg_rx_a47;
    RG_RX_A48_FIELD_T    rg_rx_a48;
    RG_RX_A49_FIELD_T    rg_rx_a49;
    RG_RX_A50_FIELD_T    rg_rx_a50;
    RG_RX_A45_FIELD_T    rg_rx_a45;
    RG_RX_A53_FIELD_T    rg_rx_a53;

    // Internal variables definitions
    bool    RG_RX_A2_saved = False;
    RG_RX_A2_FIELD_T    rg_rx_a2_i;
    // Internal variables definitions
    bool    RG_RX_A46_saved = False;
    RG_RX_A46_FIELD_T    rg_rx_a46_i;
    // Internal variables definitions
    bool    RG_RX_A47_saved = False;
    RG_RX_A47_FIELD_T    rg_rx_a47_i;
    // Internal variables definitions
    bool    RG_RX_A48_saved = False;
    RG_RX_A48_FIELD_T    rg_rx_a48_i;
    // Internal variables definitions
    bool    RG_RX_A49_saved = False;
    RG_RX_A49_FIELD_T    rg_rx_a49_i;
    // Internal variables definitions
    bool    RG_RX_A50_saved = False;
    RG_RX_A50_FIELD_T    rg_rx_a50_i;
    // Internal variables definitions
    bool    RG_RX_A45_saved = False;
    RG_RX_A45_FIELD_T    rg_rx_a45_i;

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    if(!RG_RX_A2_saved) {
        RG_RX_A2_saved = True;
        rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE;
    }
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);

    rg_rx_a46.data = rf_read_register(RG_RX_A46);
    if(!RG_RX_A46_saved) {
        RG_RX_A46_saved = True;
        rg_rx_a46_i.b.RG_WF_RX_LPF_CTUNE_ISEL_MAN_MODE = rg_rx_a46.b.RG_WF_RX_LPF_CTUNE_ISEL_MAN_MODE;
    }
    rg_rx_a46.b.RG_WF_RX_LPF_CTUNE_ISEL_MAN_MODE = 0x0;
    rf_write_register(RG_RX_A46, rg_rx_a46.data);

    rg_rx_a47.data = rf_read_register(RG_RX_A47);
    if(!RG_RX_A47_saved) {
        RG_RX_A47_saved = True;
        rg_rx_a47_i.b.RG_WF_RX_LPF_CTUNE_QSEL_MAN_MODE = rg_rx_a47.b.RG_WF_RX_LPF_CTUNE_QSEL_MAN_MODE;
    }
    rg_rx_a47.b.RG_WF_RX_LPF_CTUNE_QSEL_MAN_MODE = 0x0;
    rf_write_register(RG_RX_A47, rg_rx_a47.data);

    rg_rx_a48.data = rf_read_register(RG_RX_A48);
    if(!RG_RX_A48_saved) {
        RG_RX_A48_saved = True;
        rg_rx_a48_i.b.RG_WF_RX_TIA_CTUNE_ISEL_MAN_MODE = rg_rx_a48.b.RG_WF_RX_TIA_CTUNE_ISEL_MAN_MODE;
        rg_rx_a48_i.b.RG_WF_RX_TIA_CTUNE_QSEL_MAN_MODE = rg_rx_a48.b.RG_WF_RX_TIA_CTUNE_QSEL_MAN_MODE;
    }
    rg_rx_a48.b.RG_WF_RX_TIA_CTUNE_ISEL_MAN_MODE = 0x0;
    rg_rx_a48.b.RG_WF_RX_TIA_CTUNE_QSEL_MAN_MODE = 0x0;
    rf_write_register(RG_RX_A48, rg_rx_a48.data);

    rg_rx_a49.data = rf_read_register(RG_RX_A49);
    if(!RG_RX_A49_saved) {
        RG_RX_A49_saved = True;
        rg_rx_a49_i.b.RG_WF_RX_LPF_NOTCH_CTUNE_SEL_MAN_MODE = rg_rx_a49.b.RG_WF_RX_LPF_NOTCH_CTUNE_SEL_MAN_MODE;
    }
    rg_rx_a49.b.RG_WF_RX_LPF_NOTCH_CTUNE_SEL_MAN_MODE = 0x0;
    rf_write_register(RG_RX_A49, rg_rx_a49.data);

    rg_rx_a50.data = rf_read_register(RG_RX_A50);
    if(!RG_RX_A50_saved) {
        RG_RX_A50_saved = True;
        rg_rx_a50_i.b.RG_WF_RX_RSSIPGA_CTUNE_ISEL_MAN_MODE = rg_rx_a50.b.RG_WF_RX_RSSIPGA_CTUNE_ISEL_MAN_MODE;
        rg_rx_a50_i.b.RG_WF_RX_RSSIPGA_CTUNE_QSEL_MAN_MODE = rg_rx_a50.b.RG_WF_RX_RSSIPGA_CTUNE_QSEL_MAN_MODE;
    }
    rg_rx_a50.b.RG_WF_RX_RSSIPGA_CTUNE_ISEL_MAN_MODE = 0x0;
    rg_rx_a50.b.RG_WF_RX_RSSIPGA_CTUNE_QSEL_MAN_MODE = 0x0;
    rf_write_register(RG_RX_A50, rg_rx_a50.data);

    rg_rx_a45.data = rf_read_register(RG_RX_A45);
    if(!RG_RX_A45_saved) {
        RG_RX_A45_saved = True;
        rg_rx_a45_i.b.RG_WF_RX_RCK_CALI_EN = rg_rx_a45.b.RG_WF_RX_RCK_CALI_EN;
    }
    rg_rx_a45.b.RG_WF_RX_RCK_CALI_EN = 0x1;                         // Trigger Rx RC Calibration
    rf_write_register(RG_RX_A45, rg_rx_a45.data);

    rg_rx_a53.data = rf_read_register(RG_RX_A53);
    while (unlock_cnt < TIME_OUT_CNT)
    {    
        if(rg_rx_a63.b.RO_WF_RX_RCK_VLD == 0)
        {   

            rg_rx_a53.data = rf_read_register(RG_RX_A53);
            g_fw_funcs.system_wait(1);
            unlock_cnt = unlock_cnt + 1;
            if(unlock_cnt == TIME_OUT_CNT)
            {
                PUTS("error: RX RC TIME OUT\n");
            }
        }
        else
        {    
            PUTS("RX RC  READY\n");
            break;
        }        
    }

    rg_rx_a53.data = rf_read_register(RG_RX_A53);
    // Revert the original value before calibration back
    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = rg_rx_a2_i.b.RG_WF_RX_GAIN_MAN_MODE;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);
    // Revert the original value before calibration back
    rg_rx_a46.data = rf_read_register(RG_RX_A46);
    rg_rx_a46.b.RG_WF_RX_LPF_CTUNE_ISEL_MAN_MODE = rg_rx_a46_i.b.RG_WF_RX_LPF_CTUNE_ISEL_MAN_MODE;
    rf_write_register(RG_RX_A46, rg_rx_a46.data);
    // Revert the original value before calibration back
    rg_rx_a47.data = rf_read_register(RG_RX_A47);
    rg_rx_a47.b.RG_WF_RX_LPF_CTUNE_QSEL_MAN_MODE = rg_rx_a47_i.b.RG_WF_RX_LPF_CTUNE_QSEL_MAN_MODE;
    rf_write_register(RG_RX_A47, rg_rx_a47.data);
    // Revert the original value before calibration back
    rg_rx_a48.data = rf_read_register(RG_RX_A48);
    rg_rx_a48.b.RG_WF_RX_TIA_CTUNE_ISEL_MAN_MODE = rg_rx_a48_i.b.RG_WF_RX_TIA_CTUNE_ISEL_MAN_MODE;
    rg_rx_a48.b.RG_WF_RX_TIA_CTUNE_QSEL_MAN_MODE = rg_rx_a48_i.b.RG_WF_RX_TIA_CTUNE_QSEL_MAN_MODE;
    rf_write_register(RG_RX_A48, rg_rx_a48.data);
    // Revert the original value before calibration back
    rg_rx_a49.data = rf_read_register(RG_RX_A49);
    rg_rx_a49.b.RG_WF_RX_LPF_NOTCH_CTUNE_SEL_MAN_MODE = rg_rx_a49_i.b.RG_WF_RX_LPF_NOTCH_CTUNE_SEL_MAN_MODE;
    rf_write_register(RG_RX_A49, rg_rx_a49.data);
    // Revert the original value before calibration back
    rg_rx_a50.data = rf_read_register(RG_RX_A50);
    rg_rx_a50.b.RG_WF_RX_RSSIPGA_CTUNE_ISEL_MAN_MODE = rg_rx_a50_i.b.RG_WF_RX_RSSIPGA_CTUNE_ISEL_MAN_MODE;
    rg_rx_a50.b.RG_WF_RX_RSSIPGA_CTUNE_QSEL_MAN_MODE = rg_rx_a50_i.b.RG_WF_RX_RSSIPGA_CTUNE_QSEL_MAN_MODE;
    rf_write_register(RG_RX_A50, rg_rx_a50.data);
    // Revert the original value before calibration back
    rg_rx_a45.data = rf_read_register(RG_RX_A45);
    rg_rx_a45.b.RG_WF_RX_RCK_CALI_EN = rg_rx_a45_i.b.RG_WF_RX_RCK_CALI_EN;
    rf_write_register(RG_RX_A45, rg_rx_a45.data);
    rg_out = rg_rx_a53.data;
    wf5g_rxrc_result->rx_rc.rxrc_out = rg_out;
    return rg_out;
}

#endif //RF_CALI_TEST
