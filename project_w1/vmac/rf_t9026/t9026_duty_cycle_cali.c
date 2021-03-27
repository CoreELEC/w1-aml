#ifdef RF_CALI_TEST

#include "rf_calibration.h"

unsigned int t9026_duty_cycle_cali()
{
    unsigned int rg_out;
    unsigned int unlock_cnt = 0;
    unsigned int TIME_OUT_CNT = RF_TIME_OUT_CNT;
    RG_RX_A63_FIELD_T rg_rx_a63;
    RG_SX_A44_FIELD_T    rg_sx_a44;
    RG_SX_A45_FIELD_T    rg_sx_a45;
    RG_SX_A50_FIELD_T    rg_sx_a50;


    rg_sx_a44.data = fi_ahb_read(RG_SX_A44);
    rg_sx_a44.b.RG_WF_SX_DOUBLE_BYPASS_MODE = 0x0;
    rg_sx_a44.b.RG_WF_SX_DOUBLE_DIG_LOOP_EN = 0x1;
    fi_ahb_write(RG_SX_A44, rg_sx_a44.data);

    rg_sx_a45.data = fi_ahb_read(RG_SX_A45);
    rg_sx_a45.b.RG_WF_SX_DOUBLE_EN = 0x1;
    rg_sx_a45.b.RG_WF_SX_DOUBLE_DUTY_FLAG_SEL = 0x1;
    fi_ahb_write(RG_SX_A45, rg_sx_a45.data);

    rg_sx_a45.data = fi_ahb_read(RG_SX_A45);
    rg_sx_a45.b.RG_WF_SX_DOUBLE_CALI_EN = 0x1;
    fi_ahb_write(RG_SX_A45, rg_sx_a45.data);

    rg_sx_a50.data = rf_read_register(RG_SX_A50);
    while (unlock_cnt < TIME_OUT_CNT)
    {
        if(rg_sx_a50.b.RO_WF_SX_DOUBLE_TUNE_READY == 0)
        {

            rg_sx_a50.data = rf_read_register(RG_SX_A50);
            g_fw_funcs.system_wait(1);
            unlock_cnt = unlock_cnt + 1;
            if(unlock_cnt == TIME_OUT_CNT)
            {
                RF_DPRINTF(RF_DEBUG_INFO, "error: DUTY CYCLE CALI TIME OUT\n");
            }
        }
        else
        {
            RF_DPRINTF(RF_DEBUG_INFO, "Duty Cycle Cali READY\n");
            break;
        }
    }
    rg_out = rg_rx_a63.data;
    return rg_out;
}

#endif // RF_CALI_TEST

