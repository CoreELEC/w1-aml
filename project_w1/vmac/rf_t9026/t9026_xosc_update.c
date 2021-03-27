#include "rf_calibration.h"
#include "chip_ana_reg.h"

#ifdef RF_CALI_TEST

void t9026_xosc_update(void) {
    RG_XOSC_A7_FIELD_T    rg_xosc_a7;
    RG_XOSC_A8_FIELD_T    rg_xosc_a8;
    RG_BG_A12_FIELD_T      rg_bg_a12; 
    unsigned int tmp = 0;
    unsigned int tmp1 = 0;
    unsigned int tmp2 = 0;

    tmp = efuse_manual_read(0x4);

    if(tmp & BIT(3))
    {
        tmp2 = (tmp>>24) & 0xff;
        rg_xosc_a8.data = fi_ahb_read(RG_XOSC_A8);
        rg_xosc_a8.b.RG_XO_CORE_CTUNE_CTRL = tmp2;   //i2c_set_reg_fragment(0x00f01024, 11, 4, tmp2);   //xosc freq offset
        fi_ahb_write(RG_XOSC_A8, rg_xosc_a8.data);
    }else{
        rg_xosc_a8.data = fi_ahb_read(RG_XOSC_A8);
        rg_xosc_a8.b.RG_XO_CORE_CTUNE_CTRL = 0x29;
        fi_ahb_write(RG_XOSC_A8, rg_xosc_a8.data);
    }

    if(tmp & BIT(2))
    {
        tmp2 = (tmp>>16) & 0x007f;
        rg_bg_a12.data = fi_ahb_read(RG_BG_A12);
        rg_bg_a12.b.RG_WF_BG_ZT_CAL_I_MAN = tmp2;   //i2c_set_reg_fragment(0x00f01034, 21, 15, tmp2);  // BG Current 
        fi_ahb_write(RG_BG_A12, rg_bg_a12.data);

        tmp2 = ((tmp>>16) & 0x007c)>>2;
        rg_xosc_a7.data = fi_ahb_read(RG_XOSC_A7);
        rg_xosc_a7.b.RG_XO_CORE_BIAS_RCAL_CTRL = tmp2+3;   //i2c_set_reg_fragment(0x00f01020, 21, 17, tmp2+3); 
        fi_ahb_write(RG_XOSC_A7, rg_xosc_a7.data);
    }else{
        rg_bg_a12.data = fi_ahb_read(RG_BG_A12);
        rg_bg_a12.b.RG_WF_BG_ZT_CAL_I_MAN = 0x3f;
        fi_ahb_write(RG_BG_A12, rg_bg_a12.data);

        rg_xosc_a7.data = fi_ahb_read(RG_XOSC_A7);
        rg_xosc_a7.b.RG_XO_CORE_BIAS_RCAL_CTRL = 0x12;
        fi_ahb_write(RG_XOSC_A7, rg_xosc_a7.data);
    }

    if(tmp & BIT(1))
    {
        tmp1 = efuse_manual_read(0x9);

        tmp2 = (tmp1>>24) & 0x7f;  
        rg_bg_a12.data = fi_ahb_read(RG_BG_A12);
        rg_bg_a12.b.RG_WF_BG_PTAT_CAL_I_MAN = tmp2;   //i2c_set_reg_fragment(0x00f01034, 14, 8, tmp2);   // BG Current
        fi_ahb_write(RG_BG_A12, rg_bg_a12.data);    
    }else{
        rg_bg_a12.data = fi_ahb_read(RG_BG_A12);
        rg_bg_a12.b.RG_WF_BG_PTAT_CAL_I_MAN = 0x3c;
        fi_ahb_write(RG_BG_A12, rg_bg_a12.data);    
    }
}

#endif // RF_CALI_TEST
