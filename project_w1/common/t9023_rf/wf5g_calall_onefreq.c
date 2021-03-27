
#ifdef RF_CALI_TEST
#include "rf_calibration.h"
void wf5g_calall_onefreq(U8 channel)
{


    WF_MODE_T wf_mode ;
    U8 osc_freq = 0x5;
    SX_CALI_MODE_T sx_cali_mode;

    unsigned int bw_idx = 0;
    unsigned int tia_idx = 0;
    unsigned int lpf_idx =0;

    unsigned int txgain_idx =0;
    U8 tx_pa_gain = 0x7; 
    U8 tx_mxr_gain[3] = {0x7, 0x3, 0x1};
    U8 sync_gap = 0x57;  
    bool manual = 0;


    struct wf5g_rxrc_result* wf5g_rxrc_result;
    struct wf5g_rxdcoc_in* wf5g_rxdcoc_in;
    struct wf5g_rxdcoc_result* wf5g_rxdcoc_result;
    struct wf5g_txdciq_result* wf5g_txdciq_result[3]; //arrary for MXR gain=7,3,1
    struct wf5g_txdpd_result* wf5g_txdpd_result[3]; //arrary for BW=20,40,80MHz
    struct wf5g_rxlnatank_result * wf5g_rxlnatank_result;
    struct wf5g_rxirr_result* wf5g_rxirr_result[3]; //arrary for BW=20,40,80MHz 


    sx_cali_mode = SX_CALI_MODE_T(0x3f);
    wf_mode = WF5G_SX; // enter SX Mode
    wf5g_rf_mode_access(wf_mode); 
    wf5g_sx(channel, osc_freq, sx_cali_mode); 

    wf_mode = WF5G_RX; // enter RX Mode
    wf5g_rf_mode_access(wf_mode); 


    wf5g_rxrc(&wf5g_rxrc_result);  


    for(bw_idx = 0; bw_idx < 1; bw_idx++)
    {
        for(tia_idx = 0; tia_idx < 2; tia_idx++)
       {
           for(lpf_idx = 0; lpf_idx < 12; lpf_idx++)
            { 

                wf5g_rxdcoc_in.bw_idx = bw_idx;
                wf5g_rxdcoc_in.tia_idx = tia_idx;
                wf5g_rxdcoc_in.lpf_idx = lpf_idx;
                wf5g_rxdcoc_in.wf_rf_gain = (((tia_idx+8) <<4) + lpf_idx) & 0xff;
                wf5g_rxdcoc(&wf5g_rxdcoc_in, &wf5g_rxdcoc_result);


            }  //  for(lpf_idx = 0; lpf_idx < 12; lpf_idx++)
        }  // for(tia_idx = 0; tia_idx < 2; tia_idx++)
    }  //  for(bw_idx = 0; bw_idx < 3; bw_idx++)


    wf_mode = WF5G_TX; // enter TX Mode
    wf5g_rf_mode_access(wf_mode);
 

    for(txgain_idx = 0; txgain_idx < 3; txgain_idx++)
    {

        wf5g_txiqdc(wf5g_txdciq_result[txgain_idx], tx_pa_gain, tx_mxr_gain[txgain_idx]);


    } //     for(txgain_idx = 0; txgain_idx < 3; txgain_idx++)


    for(bw_idx = 0; bw_idx < 3; bw_idx++)
    {

        wf5g_txdpd_start(sync_gap, tx_pa_gain, tx_mxr_gain[0]); 
        wf5g_txdpd_end(wf5g_txdpd_result[bw_idx], sync_gap);

    }  //  for(bw_idx = 0; bw_idx < 3; bw_idx++)


// write back RX DCOC for BW=20MHz (skip)

    wf5g_rxlnatank(&wf5g_rxlnatank_result,  manual, channel);


    for(bw_idx = 0; bw_idx < 3; bw_idx++)
    {

    wf5g_rxirr(wf5g_rxirr_result[bw_idx], bw_idx);


    }  //  for(bw_idx = 0; bw_idx < 3; bw_idx++)


    wf_mode = WF5G_SLEEP; // enter sleep Mode
    wf5g_rf_mode_access(wf_mode);
 

}

#endif //RF_CALI_TEST
