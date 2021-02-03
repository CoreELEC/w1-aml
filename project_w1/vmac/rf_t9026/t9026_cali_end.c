#include "rf_calibration.h"

#ifdef RF_CALI_TEST

void t9026_cali_end()
{
    RG_RX_A56_FIELD_T    rg_rx_a56;
    RG_XMIT_A46_FIELD_T    rg_xmit_a46;
    RG_TX_A2_FIELD_T    rg_tx_a2;
    RG_TX_A6_FIELD_T    rg_tx_a6;
    RG_XMIT_A45_FIELD_T    rg_xmit_a45;
    RG_RECV_A2_FIELD_T   rg_recv_a2;

    rg_rx_a56.data = rf_read_register(RG_RX_A56);
    rg_rx_a56.b.RG_WF_RX_LPF_I_DCOC_SEL_MAN_MODE = 0x0;
    rg_rx_a56.b.RG_WF_RX_LPF_Q_DCOC_SEL_MAN_MODE = 0x0;
    rf_write_register(RG_RX_A56, rg_rx_a56.data);

    rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
    rg_xmit_a46.b.rg_txpwc_comp_man_sel = 0x0;
    fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);

    fi_ahb_write(PHY_TX_LPF_BYPASS, 0x0);  //open spectrum mask

    rg_tx_a2.data = rf_read_register(RG_TX_A2);
    rg_tx_a2.b.RG_WF5G_TX_MAN_MODE = 0x0;
    rf_write_register(RG_TX_A2, rg_tx_a2.data);

    rg_tx_a6.data = rf_read_register(RG_TX_A6);
    rg_tx_a6.b.RG_WF2G_TX_MAN_MODE = 0x0;
    rf_write_register(RG_TX_A6, rg_tx_a6.data);

    rg_xmit_a45.data = fi_ahb_read(RG_XMIT_A45);
    rg_xmit_a45.b.rg_tx_pwr_sel_man_en = 0x0;
    fi_ahb_write(RG_XMIT_A45, rg_xmit_a45.data);

    rg_recv_a2.data = fi_ahb_read(RG_RECV_A2);
    rg_recv_a2.b.rg_rxi_dcoft = 0;
    rg_recv_a2.b.rg_rxq_dcoft = 0;
    fi_ahb_write(RG_RECV_A2, rg_recv_a2.data);

}

#endif//RF_CALI_TEST
