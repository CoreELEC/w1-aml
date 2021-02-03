#include "rf_calibration.h"

#ifdef RF_CALI_TEST

void t9026_wf2g_calall_onefreq(U8 channel, struct t9026_wf2g_calb_rst_out* wf2g_calb_rst_out, bool check_flg)
{
    WF_MODE_T wf_mode ;
    U8 osc_freq = 0x3;
    SX_CALI_MODE_T sx_cali_mode;
    int bw_idx = 0;
    unsigned int tia_idx = 0;
    unsigned int lpf_idx =0;
    unsigned int txgain_idx =0;
    U8 tx_pa_gain = 0x7;
    U8 tx_mxr_gain[2] = {0x3, 0x7};
    U8 tx_modulation[2] = {0x0, 0x2};
    bool manual = 0;
    U8 for_dpd_flg = 0;
    int rxdc_pass_num = 0;
    int slope_tmp = 0;
    int offset_tmp = 0;
    struct wf2g_rxdcoc_in wf2g_rxdcoc_in;
    struct wf2g_rxdcoc_result wf2g_rxdcoc_result;
    struct wf2g_txdciq_result wf2g_txdciq_result[2]; //arrary for MXR gain=7,3,1
    struct wf2g_txtssi_result wf2g_txtssi_result;
    struct wf5g_rxrc_result wf2g_rxrc_result;
#ifdef CONFIG_DPD_ENABLE
    int j = 0;
    U8 sync_gap = 0x0;
    struct wf_txdpd_result wf2g_txdpd_result[3]; //arrary for BW=20,40,80MHz
#endif

    struct wf2g_rxlnatank_result wf2g_rxlnatank_result;
    struct wf2g_rxirr_result wf2g_rxirr_result[2]; //arrary for BW=20,40,80MHz
    
    slope_tmp = 32;
    offset_tmp = 29328;

    sx_cali_mode = (CHANNEL_MAP|PCODE_CALI|TCODE_CALI|MXR_TANK);
    wf_mode = WF2G_SX; // enter SX Mode
    t9026_rf_mode_access(wf_mode);
    t9026_wf2g_sx(channel, osc_freq, sx_cali_mode);

    wf_mode = WF2G_RX; // enter RX Mode
    t9026_rf_mode_access(wf_mode);

    t9026_wf2g_rxrc_for_20m(&wf2g_rxrc_result);
    wf2g_calb_rst_out->rx_rc.rxrc_2g20M_out= wf2g_rxrc_result.cal_rx_rc.rxrc_2g20M_out;

     for(tia_idx = 0; tia_idx < 2; tia_idx++)
    {
        for(lpf_idx = 1; lpf_idx < 13; lpf_idx = lpf_idx+1)
        {

            wf2g_rxdcoc_in.bw_idx = bw_idx;
            wf2g_rxdcoc_in.tia_idx = tia_idx;
            wf2g_rxdcoc_in.lpf_idx = lpf_idx;
            wf2g_rxdcoc_in.manual_mode = 0x0;
            wf2g_rxdcoc_in.wf_rx_gain = (((tia_idx+8) <<4) + lpf_idx) & 0xff;
            t9026_wf2g_rxdcoc(&wf2g_rxdcoc_in, &wf2g_rxdcoc_result);

            wf2g_calb_rst_out->rx_dc[tia_idx][lpf_idx - 1].rxdc_i = wf2g_rxdcoc_result.cal_rx_dc.rxdc_i;
            wf2g_calb_rst_out->rx_dc[tia_idx][lpf_idx - 1].rxdc_q = wf2g_rxdcoc_result.cal_rx_dc.rxdc_q;
            wf2g_calb_rst_out->rx_dc[tia_idx][lpf_idx - 1].rxdc_codei = wf2g_rxdcoc_result.cal_rx_dc.rxdc_codei;
            wf2g_calb_rst_out->rx_dc[tia_idx][lpf_idx - 1].rxdc_codeq = wf2g_rxdcoc_result.cal_rx_dc.rxdc_codeq;

           if(abs(wf2g_rxdcoc_result.cal_rx_dc.rxdc_i) < 15 && abs(wf2g_rxdcoc_result.cal_rx_dc.rxdc_q) < 15)
           {
               rxdc_pass_num++;
           }
        }// for(tia_idx = 0; tia_idx < 2; tia_idx++)
    }//  for(b_idx = 0; bw_idx < 3; bw_idx++)

    if(rxdc_pass_num == 24)
        RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "WF2G RX DC calb pass! \n");
    else
        RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "WF2G RX DC calb fail! \n");

    wf_mode = WF2G_TX; // enter TX Mode
    t9026_rf_mode_access(wf_mode);

    for(txgain_idx = 0; txgain_idx < 2; txgain_idx++)
    {
        t9026_rf_tx_modulation_access( tx_modulation[txgain_idx] );
        t9026_wf2g_txiqdc(&wf2g_txdciq_result[txgain_idx], tx_pa_gain, tx_mxr_gain[txgain_idx], check_flg);

        wf2g_calb_rst_out->tx_dc_irr[txgain_idx].tx_dc_i = wf2g_txdciq_result[txgain_idx].cal_tx_dciq.tx_dc_i;
        wf2g_calb_rst_out->tx_dc_irr[txgain_idx].tx_dc_q = wf2g_txdciq_result[txgain_idx].cal_tx_dciq.tx_dc_q;
        wf2g_calb_rst_out->tx_dc_irr[txgain_idx].tx_alpha = wf2g_txdciq_result[txgain_idx].cal_tx_dciq.tx_alpha;
        wf2g_calb_rst_out->tx_dc_irr[txgain_idx].tx_theta = wf2g_txdciq_result[txgain_idx].cal_tx_dciq.tx_theta;
    } //     for(txgain_idx = 0; txgain_idx < 3; txgain_idx++)

    //t9026_wf2g_txtssi(&wf2g_txtssi_result, ph, pl, vh, vl);
    //wf2g_calb_rst_out->tx_tssi.txpwc_slope = wf2g_txtssi_result.cal_tx_tssi.txpwc_slope;
    //wf2g_calb_rst_out->tx_tssi.txpwc_offset = wf2g_txtssi_result.cal_tx_tssi.txpwc_offset;
    t9026_wf2g_txtssi_ratio(&wf2g_txtssi_result, slope_tmp, offset_tmp);
    wf2g_calb_rst_out->tx_tssi.gain_ratio = wf2g_txtssi_result.cal_tx_tssi.gain_ratio;

#ifdef CONFIG_DPD_ENABLE
    for_dpd_flg = 1;
    bw_idx = 1;
    t9026_wf2g_txdpd_rf_init(tx_pa_gain, tx_mxr_gain[1]);
    t9026_wf2g_rxirr(&wf2g_rxirr_result[bw_idx], bw_idx, for_dpd_flg, 0);

    sync_gap = 64-1-2;
    t9026_ofdm_send_packet();
    t9026_wf2g_txdpd_start(sync_gap, bw_idx);
    t9026_wf2g_txdpd_end(&wf2g_txdpd_result[bw_idx], sync_gap);
    //t9026_ofdm_stop_packet();
    wf2g_calb_rst_out->tx_dpd[bw_idx].snr = wf2g_txdpd_result[bw_idx].cal_tx_dpd.snr;
    for(j = 0; j < 15; j++){
        wf2g_calb_rst_out->tx_dpd[bw_idx].tx_dpd_coeff_i[j] = wf2g_txdpd_result[bw_idx].cal_tx_dpd.tx_dpd_coeff_i[j];
        wf2g_calb_rst_out->tx_dpd[bw_idx].tx_dpd_coeff_q[j] = wf2g_txdpd_result[bw_idx].cal_tx_dpd.tx_dpd_coeff_q[j];
    }

    t9026_wf2g_txdpd_rf_revert();
    RF_DPRINTF(RF_DEBUG_RESULT, "wf2g channel %d TX DPD calib finish \n", channel);
#endif
    t9026_wf2g_rxlnatank(&wf2g_rxlnatank_result,  manual, channel);
    wf2g_calb_rst_out->rx_lna.rx_lna_code = wf2g_rxlnatank_result.cal_rx_lna.rx_lna_code;
     if((wf2g_calb_rst_out->rx_lna.rx_lna_code >= 0x3 && (wf2g_calb_rst_out->rx_lna.rx_lna_code) <= 0x5)
     || (wf2g_calb_rst_out->rx_lna.rx_lna_code >= 0xb && (wf2g_calb_rst_out->rx_lna.rx_lna_code) <= 0xd))
    {
        RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "WF2G rx_lna calb pass!\n");
    }else{
        RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "WF2G rx_lna calb fail!\n");
    }

    for(bw_idx = 1; bw_idx >= 0; bw_idx--)
    {
        for_dpd_flg = 0;
        t9026_wf2g_rxirr(&wf2g_rxirr_result[bw_idx], bw_idx, for_dpd_flg, check_flg);

        wf2g_calb_rst_out->rx_irr[bw_idx].coeff_eup[0] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[0];
        wf2g_calb_rst_out->rx_irr[bw_idx].coeff_eup[1] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[1];
        wf2g_calb_rst_out->rx_irr[bw_idx].coeff_eup[2] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[2];
        wf2g_calb_rst_out->rx_irr[bw_idx].coeff_eup[3] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[3];
        wf2g_calb_rst_out->rx_irr[bw_idx].coeff_pup[0] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[0];
        wf2g_calb_rst_out->rx_irr[bw_idx].coeff_pup[1] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[1];
        wf2g_calb_rst_out->rx_irr[bw_idx].coeff_pup[2] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[2];
        wf2g_calb_rst_out->rx_irr[bw_idx].coeff_pup[3] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[3];
        wf2g_calb_rst_out->rx_irr[bw_idx].coeff_elow[0] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[0];
        wf2g_calb_rst_out->rx_irr[bw_idx].coeff_elow[1] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[1];
        wf2g_calb_rst_out->rx_irr[bw_idx].coeff_elow[2] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[2];
        wf2g_calb_rst_out->rx_irr[bw_idx].coeff_elow[3] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[3];
        wf2g_calb_rst_out->rx_irr[bw_idx].coeff_plow[0] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[0];
        wf2g_calb_rst_out->rx_irr[bw_idx].coeff_plow[1] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[1];
        wf2g_calb_rst_out->rx_irr[bw_idx].coeff_plow[2] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[2];
        wf2g_calb_rst_out->rx_irr[bw_idx].coeff_plow[3] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[3];
    }//for(bw_idx = 0; bw_idx < 3; bw_idx++)

    //t9026_cali_end();
    // enter sleep Mode + auto mode
    //t9026_rf_mode_auto();
}

void t9026_wf2g_cal_for_scan(U8 channel, struct t9026_wf2g_calb_rst_out* wf2g_calb_rst_out, bool check_flg)
{
    WF_MODE_T wf_mode ;
    U8 osc_freq = 0x3;
    SX_CALI_MODE_T sx_cali_mode;
    int bw_idx = 0;
    unsigned int tia_idx = 0;
    unsigned int lpf_idx =0;
    bool manual = 0;
    U8 tx_pa_gain = 0x7;
    U8 tx_mxr_gain = 0x7;
    struct wf5g_rxrc_result wf2g_rxrc_result;
    struct wf2g_rxdcoc_in wf2g_rxdcoc_in;
    struct wf2g_rxdcoc_result wf2g_rxdcoc_result;
    struct wf2g_txdciq_result wf2g_txdciq_result;
    struct wf2g_rxlnatank_result wf2g_rxlnatank_result;
    struct wf2g_rxirr_result wf2g_rxirr_result[2]; //arrary for BW=20,40,80MHz
    
    sx_cali_mode = (CHANNEL_MAP|PCODE_CALI|TCODE_CALI|MXR_TANK);
    wf_mode = WF2G_SX; // enter SX Mode
    t9026_rf_mode_access(wf_mode);
    t9026_wf2g_sx(channel, osc_freq, sx_cali_mode);

    wf_mode = WF2G_RX; // enter RX Mode
    t9026_rf_mode_access(wf_mode);

    t9026_wf2g_rxrc_for_20m(&wf2g_rxrc_result);

     for(tia_idx = 0; tia_idx < 2; tia_idx++)
    {
        for(lpf_idx = 1; lpf_idx < 13; lpf_idx = lpf_idx+1)
        {

            wf2g_rxdcoc_in.bw_idx = bw_idx;
            wf2g_rxdcoc_in.tia_idx = tia_idx;
            wf2g_rxdcoc_in.lpf_idx = lpf_idx;
            wf2g_rxdcoc_in.manual_mode = 0x0;
            wf2g_rxdcoc_in.wf_rx_gain = (((tia_idx+8) <<4) + lpf_idx) & 0xff;
            t9026_wf2g_rxdcoc(&wf2g_rxdcoc_in, &wf2g_rxdcoc_result);
        }
    }//  for(b_idx = 0; bw_idx < 3; bw_idx++)
return;
    wf_mode = WF2G_TX; // enter TX Mode
    t9026_rf_mode_access(wf_mode);

    t9026_rf_tx_modulation_access(0x2);
    t9026_wf2g_txiqdc(&wf2g_txdciq_result, tx_pa_gain, tx_mxr_gain, check_flg);

    t9026_wf2g_rxlnatank(&wf2g_rxlnatank_result,  manual, channel);


    bw_idx = 0;
    t9026_wf2g_rxirr(&wf2g_rxirr_result[bw_idx], bw_idx, 0, check_flg);


    //t9026_cali_end();
    // enter sleep Mode + auto mode
    //t9026_rf_mode_auto();
}

extern void hal_dpd_memory_download(void);

void t9026_wf2g_cal_for_link(U8 channel, U8 bw_idx, struct t9026_wf2g_calb_rst_out* wf2g_calb_rst_out, bool check_flg)
{
    WF_MODE_T wf_mode ;
    U8 osc_freq = 0x3;
    SX_CALI_MODE_T sx_cali_mode;
    unsigned int txgain_idx =0;
    U8 tx_pa_gain = 0x7;
    U8 tx_mxr_gain[2] = {0x3, 0x7};
    U8 tx_modulation[2] = {0x1, 0x2};
    bool manual = 0;
    U8 for_dpd_flg = 0;
    int slope_tmp = 0;
    int offset_tmp = 0;
    unsigned int tmp = 0;
    unsigned int tmp1 = 0;
    struct wf2g_txdciq_result wf2g_txdciq_result[2]; //arrary for MXR gain=7,3,1
    struct wf2g_txtssi_result wf2g_txtssi_result;
    struct wf2g_rxlnatank_result wf2g_rxlnatank_result;
    struct wf2g_rxirr_result wf2g_rxirr_result[2]; //arrary for BW=20,40,80MHz
#ifdef CONFIG_DPD_ENABLE
    int j = 0;
    U8 sync_gap = 0x0;
    struct wf_txdpd_result wf2g_txdpd_result; //arrary for BW=20,40,80MHz
#endif

#if 1
    tmp = efuse_manual_read(0x9);
    if(tmp & BIT(16))
    {
        tmp1 = efuse_manual_read(0xa);
        slope_tmp = tmp1 & 0xff;
        offset_tmp = (tmp1 & 0xfff0000) >> 12;
        offset_tmp = offset_tmp - 0x40;
    }else{
        slope_tmp = 30;
        offset_tmp = 29216;
    }
    printk("tmp = 0x%x, slope_tmp = 0x%x, offset_tmp = 0x%x \n", tmp, slope_tmp, offset_tmp);
#else
    slope_tmp = 32;
    offset_tmp = 29328;
#endif
    sx_cali_mode = (CHANNEL_MAP|PCODE_CALI|TCODE_CALI|MXR_TANK);
    wf_mode = WF2G_SX; // enter SX Mode
    t9026_rf_mode_access(wf_mode);
    t9026_wf2g_sx(channel, osc_freq, sx_cali_mode);

    wf_mode = WF2G_TX; // enter TX Mode
    t9026_rf_mode_access(wf_mode);

    for(txgain_idx = 0; txgain_idx < 2; txgain_idx++)
    {
        t9026_rf_tx_modulation_access( tx_modulation[txgain_idx] );
        t9026_wf2g_txiqdc(&wf2g_txdciq_result[txgain_idx], tx_pa_gain, tx_mxr_gain[txgain_idx], check_flg);

        wf2g_calb_rst_out->tx_dc_irr[txgain_idx].tx_dc_i = wf2g_txdciq_result[txgain_idx].cal_tx_dciq.tx_dc_i;
        wf2g_calb_rst_out->tx_dc_irr[txgain_idx].tx_dc_q = wf2g_txdciq_result[txgain_idx].cal_tx_dciq.tx_dc_q;
        wf2g_calb_rst_out->tx_dc_irr[txgain_idx].tx_alpha = wf2g_txdciq_result[txgain_idx].cal_tx_dciq.tx_alpha;
        wf2g_calb_rst_out->tx_dc_irr[txgain_idx].tx_theta = wf2g_txdciq_result[txgain_idx].cal_tx_dciq.tx_theta;
    } //     for(txgain_idx = 0; txgain_idx < 3; txgain_idx++)

    //t9026_wf2g_txtssi(&wf2g_txtssi_result, ph, pl, vh, vl);
    //slope_tmp = wf2g_txtssi_result.cal_tx_tssi.txpwc_slope;
    //offset_tmp = wf2g_txtssi_result.cal_tx_tssi.txpwc_offset;
    t9026_wf2g_txtssi_ratio(&wf2g_txtssi_result, slope_tmp, offset_tmp);
    wf2g_calb_rst_out->tx_tssi.gain_ratio = wf2g_txtssi_result.cal_tx_tssi.gain_ratio;

#ifdef CONFIG_DPD_ENABLE
    for_dpd_flg = 1;
    sync_gap = 64-1-2-3;

    hal_dpd_memory_download();

    t9026_wf2g_txdpd_rf_init(tx_pa_gain, tx_mxr_gain[1]);
    t9026_wf2g_rxirr(&wf2g_rxirr_result[1], 1, for_dpd_flg, 0);

    //t9026_ofdm_send_packet();
    t9026_wf2g_txdpd_start(sync_gap, 1);
    t9026_wf2g_txdpd_end(&wf2g_txdpd_result, sync_gap);
    //t9026_ofdm_stop_packet();
    wf2g_calb_rst_out->tx_dpd[1].snr = wf2g_txdpd_result.cal_tx_dpd.snr;
    for(j = 0; j < 15; j++){
        wf2g_calb_rst_out->tx_dpd[1].tx_dpd_coeff_i[j] = wf2g_txdpd_result.cal_tx_dpd.tx_dpd_coeff_i[j];
        wf2g_calb_rst_out->tx_dpd[1].tx_dpd_coeff_q[j] = wf2g_txdpd_result.cal_tx_dpd.tx_dpd_coeff_q[j];
    }

    t9026_wf2g_txdpd_rf_revert();
    RF_DPRINTF(RF_DEBUG_RESULT, "wf2g channel %d TX DPD calib finish \n", channel);
#endif
    t9026_wf2g_rxlnatank(&wf2g_rxlnatank_result,  manual, channel);
    wf2g_calb_rst_out->rx_lna.rx_lna_code = wf2g_rxlnatank_result.cal_rx_lna.rx_lna_code;

    for_dpd_flg = 0;
    t9026_wf2g_rxirr(&wf2g_rxirr_result[bw_idx], bw_idx, for_dpd_flg, check_flg);

    wf2g_calb_rst_out->rx_irr[bw_idx].coeff_eup[0] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[0];
    wf2g_calb_rst_out->rx_irr[bw_idx].coeff_eup[1] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[1];
    wf2g_calb_rst_out->rx_irr[bw_idx].coeff_eup[2] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[2];
    wf2g_calb_rst_out->rx_irr[bw_idx].coeff_eup[3] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[3];
    wf2g_calb_rst_out->rx_irr[bw_idx].coeff_pup[0] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[0];
    wf2g_calb_rst_out->rx_irr[bw_idx].coeff_pup[1] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[1];
    wf2g_calb_rst_out->rx_irr[bw_idx].coeff_pup[2] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[2];
    wf2g_calb_rst_out->rx_irr[bw_idx].coeff_pup[3] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[3];
    wf2g_calb_rst_out->rx_irr[bw_idx].coeff_elow[0] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[0];
    wf2g_calb_rst_out->rx_irr[bw_idx].coeff_elow[1] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[1];
    wf2g_calb_rst_out->rx_irr[bw_idx].coeff_elow[2] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[2];
    wf2g_calb_rst_out->rx_irr[bw_idx].coeff_elow[3] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[3];
    wf2g_calb_rst_out->rx_irr[bw_idx].coeff_plow[0] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[0];
    wf2g_calb_rst_out->rx_irr[bw_idx].coeff_plow[1] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[1];
    wf2g_calb_rst_out->rx_irr[bw_idx].coeff_plow[2] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[2];
    wf2g_calb_rst_out->rx_irr[bw_idx].coeff_plow[3] = wf2g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[3];


    t9026_cali_end();
    // enter sleep Mode + auto mode
    t9026_rf_mode_auto();
}
#endif//RF_CALI_TEST
