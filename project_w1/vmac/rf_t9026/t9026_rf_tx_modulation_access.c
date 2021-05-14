
#include "rf_calibration.h"
#ifdef RF_CALI_TEST
void t9026_rf_tx_modulation_access(U8 wf_modulation)
{
    RG_TOP_A15_FIELD_T    rg_top_a15;

    if( wf_modulation == 1)
    {
        rg_top_a15.data = rf_read_register(RG_TOP_A15);
        rg_top_a15.b.rg_wf_dac_ampctrl_man = 0x1;                         // For 2g 11b
        rf_write_register(RG_TOP_A15, rg_top_a15.data);

    }
    else if( wf_modulation == 2)
    {
        rg_top_a15.data = rf_read_register(RG_TOP_A15);
        rg_top_a15.b.rg_wf_dac_ampctrl_man = 0x2;                         // For 2g tx and 5g 80M
        rf_write_register(RG_TOP_A15, rg_top_a15.data);

    }
    else if( wf_modulation == 3)
    {
        rg_top_a15.data = rf_read_register(RG_TOP_A15);
        rg_top_a15.b.rg_wf_dac_ampctrl_man = 0x3;                         // For 5g tx others
        rf_write_register(RG_TOP_A15, rg_top_a15.data);

    }

}

#endif // RF_CALI_TEST
