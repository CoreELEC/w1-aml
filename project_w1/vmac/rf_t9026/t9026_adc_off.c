#include "rf_calibration.h"

#ifdef RF_CALI_TEST

void t9026_adc_off(void)
{
    RG_TOP_A17_FIELD_T    rg_top_a17;
    RG_TOP_A18_FIELD_T    rg_top_a18;
    RG_TOP_A19_FIELD_T    rg_top_a19;
    RG_TOP_A20_FIELD_T    rg_top_a20;
    RG_TOP_A21_FIELD_T    rg_top_a21;
    RG_TOP_A22_FIELD_T    rg_top_a22;
    RG_TOP_A23_FIELD_T    rg_top_a23;
    RG_TOP_A24_FIELD_T    rg_top_a24;



    rg_top_a17.data = rf_read_register(RG_TOP_A17);
    rg_top_a17.b.rg_m0_wf_adda_ldo_en = 0x1;
    rg_top_a17.b.rg_m0_wf_wadc_enable_i = 0x0;
    rg_top_a17.b.rg_m0_wf_wadc_enable_q = 0x0;
    rf_write_register(RG_TOP_A17, rg_top_a17.data);

    rg_top_a18.data = rf_read_register(RG_TOP_A18);
    rg_top_a18.b.rg_m1_wf_adda_ldo_en = 0x1;
    rg_top_a18.b.rg_m1_wf_wadc_enable_i = 0x0;
    rg_top_a18.b.rg_m1_wf_wadc_enable_q = 0x0;
    rf_write_register(RG_TOP_A18, rg_top_a18.data);

    rg_top_a19.data = rf_read_register(RG_TOP_A19);
    rg_top_a19.b.rg_m2_wf_adda_ldo_en = 0x1;
    rg_top_a19.b.rg_m2_wf_wadc_enable_i = 0x0;
    rg_top_a19.b.rg_m2_wf_wadc_enable_q = 0x0;
    rf_write_register(RG_TOP_A19, rg_top_a19.data);

    rg_top_a20.data = rf_read_register(RG_TOP_A20);
    rg_top_a20.b.rg_m3_wf_adda_ldo_en = 0x1;
    rg_top_a20.b.rg_m3_wf_wadc_enable_i = 0x0;
    rg_top_a20.b.rg_m3_wf_wadc_enable_q = 0x0;
    rf_write_register(RG_TOP_A20, rg_top_a20.data);

    rg_top_a21.data = rf_read_register(RG_TOP_A21);
    rg_top_a21.b.rg_m4_wf_adda_ldo_en = 0x1;
    rg_top_a21.b.rg_m4_wf_wadc_enable_i = 0x0;
    rg_top_a21.b.rg_m4_wf_wadc_enable_q = 0x0;
    rf_write_register(RG_TOP_A21, rg_top_a21.data);

    rg_top_a22.data = rf_read_register(RG_TOP_A22);
    rg_top_a22.b.rg_m5_wf_adda_ldo_en = 0x1;
    rg_top_a22.b.rg_m5_wf_wadc_enable_i = 0x0;
    rg_top_a22.b.rg_m5_wf_wadc_enable_q = 0x0;
    rf_write_register(RG_TOP_A22, rg_top_a22.data);

    rg_top_a23.data = rf_read_register(RG_TOP_A23);
    rg_top_a23.b.rg_m6_wf_adda_ldo_en = 0x1;
    rg_top_a23.b.rg_m6_wf_wadc_enable_i = 0x0;
    rg_top_a23.b.rg_m6_wf_wadc_enable_q = 0x0;
    rf_write_register(RG_TOP_A23, rg_top_a23.data);

    rg_top_a24.data = rf_read_register(RG_TOP_A24);
    rg_top_a24.b.rg_m7_wf_adda_ldo_en = 0x1;
    rg_top_a24.b.rg_m7_wf_wadc_enable_i = 0x0;
    rg_top_a24.b.rg_m7_wf_wadc_enable_q = 0x0;
    rf_write_register(RG_TOP_A24, rg_top_a24.data);
}

#endif // RF_CALI_TEST
