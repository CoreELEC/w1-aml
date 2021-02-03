
#ifdef RF_CALI_TEST
#include "rf_calibration.h"
void wf5g_cali_mode_exit()
{
    RG_CORE_A6_FIELD_T    rg_core_a6;
    RG_CORE_A2_FIELD_T    rg_core_a2;


    rg_core_a6.data = fi_ahb_read(RG_CORE_A6);
    rg_core_a6.b.rg_cali_mode_man = 0xf;
    rg_core_a6.b.rg_cali_mode_man_en = 0x0;
    fi_ahb_write(RG_CORE_A6, rg_core_a6.data);

    rg_core_a2.data = fi_ahb_read(RG_CORE_A2);
    rg_core_a2.b.rg_core_enb_man = 0x0;
    rg_core_a2.b.rg_core_enb_man_en = 0x0;
    rg_core_a2.b.rg_xmit_enb_man = 0x0;
    rg_core_a2.b.rg_xmit_enb_man_en = 0x0;
    rg_core_a2.b.rg_recv_enb_man = 0x0;
    rg_core_a2.b.rg_recv_enb_man_en = 0x0;
    rg_core_a2.b.rg_esti_enb_man = 0x0;
    rg_core_a2.b.rg_esti_enb_man_en = 0x0;
    fi_ahb_write(RG_CORE_A2, rg_core_a2.data);

}

#endif //RF_CALI_TEST
