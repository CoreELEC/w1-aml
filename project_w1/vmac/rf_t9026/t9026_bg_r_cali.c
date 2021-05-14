#ifdef RF_CALI_TEST

#include "rf_calibration.h"

void t9026_bg_r_cali(void)
{
    unsigned int unlock_cnt = 0;
    unsigned int TIME_OUT_CNT = RF_TIME_OUT_CNT;
    RG_BG_A12_FIELD_T    rg_bg_a12;
    RG_BG_A13_FIELD_T    rg_bg_a13;


    rg_bg_a12.data = fi_ahb_read(RG_BG_A12);
    rg_bg_a12.b.RG_WF_BG_RCAL_EN = 0x1;
    rg_bg_a12.b.RG_WF_BG_IOUT_SEL = 0x1;
    rg_bg_a12.b.RG_WF_BG_ZT_CAL_I_MAN_SEL = 0x0;
    rg_bg_a12.b.RG_WF_BG_PTAT_CAL_I_MAN_SEL = 0x0;
    fi_ahb_write(RG_BG_A12, rg_bg_a12.data);

    rg_bg_a13.data = fi_ahb_read(RG_BG_A13);
    while (unlock_cnt < TIME_OUT_CNT)
    {
        if(!rg_bg_a13.b.RO_WF_BG_RCAL_FINISH)
        {

            rg_bg_a13.data = fi_ahb_read(RG_BG_A13);
            g_fw_funcs.system_wait(1);
            unlock_cnt = unlock_cnt + 1;
            if(unlock_cnt == TIME_OUT_CNT)
            {
                RF_DPRINTF(RF_DEBUG_INFO, "error: RX RC TIME OUT\n");
            }
        }
        else
        {
            RF_DPRINTF(RF_DEBUG_INFO, "BG R Cali READY\n");
            break;
        }
    }

    rg_bg_a12.data = fi_ahb_read(RG_BG_A12);
    rg_bg_a12.b.RG_WF_BG_RCAL_EN = 0x0;
    rg_bg_a12.b.RG_WF_BG_IOUT_SEL = 0x0;
    rg_bg_a12.b.RG_WF_BG_ZT_CAL_I_MAN_SEL = 0x0;
    rg_bg_a12.b.RG_WF_BG_PTAT_CAL_I_MAN_SEL = 0x0;
    fi_ahb_write(RG_BG_A12, rg_bg_a12.data);

    rg_bg_a12.data = fi_ahb_read(RG_BG_A12);
    rg_bg_a12.b.RG_WF_BG_RCAL_EN = 0x1;
    fi_ahb_write(RG_BG_A12, rg_bg_a12.data);

    rg_bg_a13.data = fi_ahb_read(RG_BG_A13);
    while (unlock_cnt < TIME_OUT_CNT)
    {
        if(!rg_bg_a13.b.RO_WF_BG_RCAL_FINISH)
        {

            rg_bg_a13.data = fi_ahb_read(RG_BG_A13);
            g_fw_funcs.system_wait(1);
            unlock_cnt = unlock_cnt + 1;
            if(unlock_cnt == TIME_OUT_CNT)
            {
                RF_DPRINTF(RF_DEBUG_INFO, "error: BG R CALI TIME OUT\n");
            }
        }
        else
        {
            RF_DPRINTF(RF_DEBUG_INFO, "BG R Cali READY\n");
            break;
        }
    }
}

#endif // RF_CALI_TEST

