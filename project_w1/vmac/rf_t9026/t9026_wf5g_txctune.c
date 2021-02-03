#include "rf_calibration.h"

#ifdef RF_CALI_TEST
void t9026_wf5g_txctune(U8 channel)
{
    RG_TX_A13_FIELD_T    rg_tx_a13;

    if(channel >= 180)
    {
        rg_tx_a13.data = rf_read_register(RG_TX_A13);
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G1 = 0x11;                        // Band0 : 4900-5000MHz
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G2 = 0x11;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G3 = 0x11;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G4 = 0x11;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_MAN = 0x11;
        rf_write_register(RG_TX_A13, rg_tx_a13.data);

    }
    else if(channel < 30)
    {
        rg_tx_a13.data = rf_read_register(RG_TX_A13);
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G1 = 0x11;                        // Band0 : 4900-5000MHz
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G2 = 0x11;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G3 = 0x11;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G4 = 0x11;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_MAN = 0x11;
        rf_write_register(RG_TX_A13, rg_tx_a13.data);

    }
    else if(channel < 50)
    {
        rg_tx_a13.data = rf_read_register(RG_TX_A13);
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G1 = 0xd;                         // Band0 : 4900-5000MHz
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G2 = 0xd;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G3 = 0xd;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G4 = 0xd;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_MAN = 0xd;
        rf_write_register(RG_TX_A13, rg_tx_a13.data);

    }
    else if(channel < 70)
    {
        rg_tx_a13.data = rf_read_register(RG_TX_A13);
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G1 = 0xc;                         // Band0 : 4900-5000MHz
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G2 = 0xc;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G3 = 0xc;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G4 = 0xc;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_MAN = 0xc;
        rf_write_register(RG_TX_A13, rg_tx_a13.data);

    }
    else if(channel < 90)
    {
        rg_tx_a13.data = rf_read_register(RG_TX_A13);
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G1 = 0xa;                         // Band0 : 4900-5000MHz
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G2 = 0xa;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G3 = 0xa;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G4 = 0xa;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_MAN = 0xa;
        rf_write_register(RG_TX_A13, rg_tx_a13.data);

    }
    else if(channel < 110)
    {
        rg_tx_a13.data = rf_read_register(RG_TX_A13);
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G1 = 0x9;                         // Band0 : 4900-5000MHz
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G2 = 0x9;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G3 = 0x9;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G4 = 0x9;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_MAN = 0x9;
        rf_write_register(RG_TX_A13, rg_tx_a13.data);

    }
    else if(channel < 130)
    {
        rg_tx_a13.data = rf_read_register(RG_TX_A13);
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G1 = 0x8;                         // Band0 : 4900-5000MHz
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G2 = 0x8;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G3 = 0x8;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G4 = 0x8;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_MAN = 0x8;
        rf_write_register(RG_TX_A13, rg_tx_a13.data);

    }
    else if(channel < 150)
    {
        rg_tx_a13.data = rf_read_register(RG_TX_A13);
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G1 = 0x7;                         // Band0 : 4900-5000MHz
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G2 = 0x7;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G3 = 0x7;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G4 = 0x7;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_MAN = 0x7;
        rf_write_register(RG_TX_A13, rg_tx_a13.data);

    }
    else if(channel < 180)
    {
        rg_tx_a13.data = rf_read_register(RG_TX_A13);
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G1 = 0x7;                         // Band0 : 4900-5000MHz
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G2 = 0x7;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G3 = 0x7;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_G4 = 0x7;
        rg_tx_a13.b.RG_WF5G_TX_MXR_CBANK_MAN = 0x7;
        rf_write_register(RG_TX_A13, rg_tx_a13.data);
    } // if(channel>=180)
 
    return;
}

#endif // RF_CALI_TEST
