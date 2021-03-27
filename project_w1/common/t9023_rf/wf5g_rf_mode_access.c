
#ifdef RF_CALI_TEST
#include "rf_calibration.h"
void wf5g_rf_mode_access(WF_MODE_T wf_mode)
{
    RG_TOP_A2_FIELD_T    rg_top_a2;


    rg_top_a2.data = rf_read_register(RG_TOP_A2);
    rg_top_a2.b.RG_WF_MODE_MAN = wf_mode;                     // Enter 5GDPD Mode
    rg_top_a2.b.RG_WF_MODE_MAN_MODE = 0x1;
    rf_write_register(RG_TOP_A2, rg_top_a2.data);

}

#endif //RF_CALI_TEST
