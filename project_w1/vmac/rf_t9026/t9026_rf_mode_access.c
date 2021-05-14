#include "rf_calibration.h"

#ifdef RF_CALI_TEST

void t9026_rf_mode_access(WF_MODE_T wf_mode)
{
    RG_TOP_A2_FIELD_T    rg_top_a2;
    RG_TOP_A12_FIELD_T    rg_top_a12;
    RG_SX_A42_FIELD_T    rg_sx_a42;

    rg_top_a2.data = rf_read_register(RG_TOP_A2);
    rg_top_a2.b.RG_WF_MODE_MAN = wf_mode;                     // Enter 5GDPD Mode
    rg_top_a2.b.RG_WF_MODE_MAN_MODE = 0x1;
    rf_write_register(RG_TOP_A2, rg_top_a2.data);

    if( wf_mode == WF5G_TX || wf_mode == WF5G_TXDPD || wf_mode == WF5G_SX)
    {
        rg_top_a12.data = rf_read_register(RG_TOP_A12);
        rg_top_a12.b.rg_wf_dac_rsv = 0x67;                        // For WF5G
        rf_write_register(RG_TOP_A12, rg_top_a12.data);

    }
    else if( wf_mode == WF2G_TX || wf_mode == WF2G_SX)
    {
        rg_top_a12.data = rf_read_register(RG_TOP_A12);
        rg_top_a12.b.rg_wf_dac_rsv = 0x3f;                        // For WF2G
        rf_write_register(RG_TOP_A12, rg_top_a12.data);

    } //     if( wf_mode = "0x2" or "0x4")


    if( wf_mode == WF_SLEEP)
    {

        rg_sx_a42.data = rf_read_register(RG_SX_A42);
        rg_sx_a42.b.RG_WF_SX_RSV0 = 0xfe20;
        rf_write_register(RG_SX_A42, rg_sx_a42.data);

    }
    else
    {

        rg_sx_a42.data = rf_read_register(RG_SX_A42);
        rg_sx_a42.b.RG_WF_SX_RSV0 = 0xfff8;
        rf_write_register(RG_SX_A42, rg_sx_a42.data);

    } //      if( wf_mode == WF_SLEEP)

}

#endif//RF_CALI_TEST

