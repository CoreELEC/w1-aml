#include "rf_calibration.h"

#ifdef RF_CALI_TEST

void t9026_rf_mode_auto()
{
    RG_TOP_A2_FIELD_T    rg_top_a2;
    RG_TOP_A15_FIELD_T    rg_top_a15;

    rg_top_a2.data = rf_read_register(RG_TOP_A2);
    rg_top_a2.b.RG_WF_MODE_MAN = 0x0;                         // Enter 5GDPD Mode
    rg_top_a2.b.RG_WF_MODE_MAN_MODE = 0x0;
    rf_write_register(RG_TOP_A2, rg_top_a2.data);

    rg_top_a15.data = rf_read_register(RG_TOP_A15);
    rg_top_a15.b.rg_wf_adda_man_mode = 0x0;
    rf_write_register(RG_TOP_A15, rg_top_a15.data);
}

#endif//RF_CALI_TEST
