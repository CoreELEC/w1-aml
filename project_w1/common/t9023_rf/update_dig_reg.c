
#ifdef RF_CALI_TEST
#include "rf_calibration.h"
void update_dig_reg()
{
    RG_XMIT_A4_FIELD_T    rg_xmit_a4;
    RG_XMIT_A2_FIELD_T    rg_xmit_a2;
    RG_XMIT_A3_FIELD_T    rg_xmit_a3;
    RG_XMIT_A34_FIELD_T    rg_xmit_a34;


{0x0000b234, 0x00000000};

{0x0000b078, 0x00680701};

{0x0000b100, 0x01900008};

    rg_xmit_a4.data = fi_ahb_read(RG_XMIT_A4);
    rg_xmit_a4.b.rg_tx_signal_sel = 0x0;
    fi_ahb_write(RG_XMIT_A4, rg_xmit_a4.data);

    rg_xmit_a2.data = fi_ahb_read(RG_XMIT_A2);
    rg_xmit_a2.b.rg_tg1_f_sel = 0x200000;
    rg_xmit_a2.b.rg_tg1_tone_gain = 0x0;
    rg_xmit_a2.b.rg_tg1_enable = 0x0;
    rg_xmit_a2.b.rg_tg1_tone_40 = 0x0;
    rg_xmit_a2.b.rg_tg1_tone_man_en = 0x1;
    fi_ahb_write(RG_XMIT_A2, rg_xmit_a2.data);

    rg_xmit_a3.data = fi_ahb_read(RG_XMIT_A3);
    rg_xmit_a3.b.rg_tg2_f_sel = 0x200000;
    rg_xmit_a3.b.rg_tg2_tone_gain = 0x0;
    rg_xmit_a3.b.rg_tg2_enable = 0x1;
    rg_xmit_a3.b.rg_tg2_tone_40 = 0x0;
    rg_xmit_a3.b.rg_tg2_tone_man_en = 0x1;
    fi_ahb_write(RG_XMIT_A3, rg_xmit_a3.data);

    rg_xmit_a34.data = fi_ahb_read(RG_XMIT_A34);
    rg_xmit_a34.b.rg_tx_iqmm_bypass = 0x1;
    rg_xmit_a34.b.rg_tx_negate_msb = 0x1;
    rg_xmit_a34.b.rg_tx_iq_swap = 0x1;
    rg_xmit_a34.b.rg_tx_dout_sel = 0x0;
    fi_ahb_write(RG_XMIT_A34, rg_xmit_a34.data);

}

#endif //RF_CALI_TEST
