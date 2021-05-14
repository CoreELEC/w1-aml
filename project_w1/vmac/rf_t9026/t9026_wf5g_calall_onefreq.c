#include "rf_calibration.h"

#ifdef RF_CALI_TEST
extern U8 g_temp_rise_flag;
extern U8 g_temp_fall_flag;
void t9026_wf5g_calall_onefreq(int chn_idx, struct t9026_wf5g_calb_rst_out* wf5g_calb_rst_out, bool check_flg)
{
    unsigned all_chn[10] = {0xbe, 0xa, 0x1e, 0x32, 0x46, 0x5a, 0x6e, 0x82, 0x96, 0xa5};
    WF_MODE_T wf_mode ;
    U8 osc_freq = 0x3;
    SX_CALI_MODE_T sx_cali_mode;
    int bw_idx = 0;
    unsigned int tia_idx = 0;
    unsigned int lpf_idx =0;
    U8 tx_pa_gain = 0x7;
    U8 tx_mxr_gain = 0x7;
    bool manual = 0;
    U8 for_dpd_flg = 0;
    int ph = 0;
    int pl = 0;
    int vh = 0;
    int vl = 0;
    int i = 0;
    int rxdc_pass_num = 0;

    //struct wf5g_rxrc_result wf5g_rxrc_result;
    struct wf5g_rxdcoc_in wf5g_rxdcoc_in;
    struct wf5g_rxdcoc_result wf5g_rxdcoc_result;
    struct wf5g_txdciq_result wf5g_txdciq_result;
    //struct wf5g_txtssi_result wf5g_txtssi_result;
#ifdef CONFIG_DPD_ENABLE
    int j = 0;
    U8 sync_gap = 0x0;
    unsigned dpd_gain[5] = {0xb0, 0xa0, 0x90, 0x80, 0x70};
    struct wf_txdpd_result wf5g_txdpd_result[3]; //arrary for BW=20,40,80MHz
#endif

    struct wf5g_rxlnatank_result wf5g_rxlnatank_result;
    struct wf5g_rxirr_result wf5g_rxirr_result[3]; //arrary for BW=20,40,80MHz

    ph = 0x4c;//(19.02dbm)
    pl = 0x2d;//(11.27dbm)
    vh = 0x40;
    vl = 0x15;

    RF_DPRINTF(RF_DEBUG_MSG, "t9026_wf5g_calall_onefreq channel:%02x\n", all_chn[chn_idx]);
    sx_cali_mode = (CHANNEL_MAP|PCODE_CALI|TCODE_CALI|MXR_TANK);
    wf_mode = WF5G_SX; // enter SX Mode
    t9026_rf_mode_access(wf_mode);
    t9026_wf5g_sx(all_chn[chn_idx], osc_freq, sx_cali_mode);

    if(all_chn[chn_idx] == 0x5a){  //5a
        wf_mode = WF5G_RX; // enter RX Mode
        t9026_rf_mode_access(wf_mode);
#if 0
        t9026_wf5g_rxrc(&wf5g_rxrc_result);
        wf5g_calb_rst_out->rx_rc.rxrc_out = wf5g_rxrc_result.cal_rx_rc.rxrc_out;
        if ((wf5g_calb_rst_out->rx_rc.rxrc_out >= 0x110) && (wf5g_calb_rst_out->rx_rc.rxrc_out <= 0x140))
        {
            RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "RX RC calb pass! \n");
        } else {
            RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "RX RC calb fail! \n");
        }
#endif
        for(tia_idx = 0; tia_idx < 2; tia_idx++)
        {
           for(lpf_idx = 1; lpf_idx < 13; lpf_idx = lpf_idx+1)
            {
                wf5g_rxdcoc_in.bw_idx = bw_idx;
                wf5g_rxdcoc_in.tia_idx = tia_idx;
                wf5g_rxdcoc_in.lpf_idx = lpf_idx;
                wf5g_rxdcoc_in.manual_mode = 0x0;
                wf5g_rxdcoc_in.wf_rx_gain = (((tia_idx+8) <<4) + lpf_idx) & 0xff;
                t9026_wf5g_rxdcoc(&wf5g_rxdcoc_in, &wf5g_rxdcoc_result);

                wf5g_calb_rst_out->rx_dc[tia_idx][lpf_idx - 1].rxdc_i = wf5g_rxdcoc_result.cal_rx_dc.rxdc_i;
                wf5g_calb_rst_out->rx_dc[tia_idx][lpf_idx - 1].rxdc_q = wf5g_rxdcoc_result.cal_rx_dc.rxdc_q;
                wf5g_calb_rst_out->rx_dc[tia_idx][lpf_idx - 1].rxdc_codei = wf5g_rxdcoc_result.cal_rx_dc.rxdc_codei;
                wf5g_calb_rst_out->rx_dc[tia_idx][lpf_idx - 1].rxdc_codeq = wf5g_rxdcoc_result.cal_rx_dc.rxdc_codeq;
                if(abs(wf5g_rxdcoc_result.cal_rx_dc.rxdc_i) < 15 && abs(wf5g_rxdcoc_result.cal_rx_dc.rxdc_q) < 15)
                {
                    rxdc_pass_num++;
                }
            }  //  for(lpf_idx = 0; lpf_idx < 12; lpf_idx++)
        }  // for(tia_idx = 0; tia_idx < 2; tia_idx++)

      if(rxdc_pass_num == 24)
          RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "WF5G RX DC calb pass! \n");
      else
          RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "WF5G RX DC calb fail! \n");
    }

    wf_mode = WF5G_TX; // enter TX Mode
    t9026_rf_mode_access(wf_mode);

    if(all_chn[chn_idx] == 0x0a || all_chn[chn_idx] == 0x32 || all_chn[chn_idx] == 0x5a || all_chn[chn_idx] == 0x82 || all_chn[chn_idx] == 0xa5)
    {
        i = (chn_idx - 1)/2;

        t9026_wf5g_txiqdc(&wf5g_txdciq_result, tx_pa_gain, tx_mxr_gain, check_flg);

        wf5g_calb_rst_out->tx_dc_irr[i][0].tx_dc_i = wf5g_txdciq_result.cal_tx_dciq.tx_dc_i;
        wf5g_calb_rst_out->tx_dc_irr[i][0].tx_dc_q = wf5g_txdciq_result.cal_tx_dciq.tx_dc_q;
        wf5g_calb_rst_out->tx_dc_irr[i][0].tx_alpha = wf5g_txdciq_result.cal_tx_dciq.tx_alpha;
        wf5g_calb_rst_out->tx_dc_irr[i][0].tx_theta = wf5g_txdciq_result.cal_tx_dciq.tx_theta;

        //t9026_wf5g_txtssi(&wf5g_txtssi_result, ph, pl, vh, vl);
    //wf5g_calb_rst_out->tx_tssi.txpwc_slope = wf5g_txtssi_result.cal_tx_tssi.txpwc_slope;
    //wf5g_calb_rst_out->tx_tssi.txpwc_offset = wf5g_txtssi_result.cal_tx_tssi.txpwc_offset;
    }

#ifdef CONFIG_DPD_ENABLE
    if(all_chn[chn_idx] == 0x0a || all_chn[chn_idx] == 0x32 || all_chn[chn_idx] == 0x5a || all_chn[chn_idx] == 0x82 || all_chn[chn_idx] == 0xa5)
    {
        for_dpd_flg = 1;
        bw_idx = 1;
        sync_gap = 64-1-2;
        i = (chn_idx - 1)/2;

        //wf_mode = WF5G_TX; // enter TX Mode
        //t9026_rf_mode_access(wf_mode);
        t9026_wf5g_txdpd_rf_init(tx_pa_gain, tx_mxr_gain);

        t9026_wf5g_rxirr(&wf5g_rxirr_result[1], 1, for_dpd_flg, 0);//bw_idx

        t9026_ofdm_send_packet();

        t9026_wf5g_txdpd_start(sync_gap, bw_idx, dpd_gain[i]);
        t9026_wf5g_txdpd_end(&wf5g_txdpd_result[bw_idx], sync_gap);

        //t9026_ofdm_stop_packet();

        wf5g_calb_rst_out->tx_dpd[i][bw_idx].snr = wf5g_txdpd_result[bw_idx].cal_tx_dpd.snr;
        for(j = 0; j < 15; j++){
            wf5g_calb_rst_out->tx_dpd[i][bw_idx].tx_dpd_coeff_i[j] = wf5g_txdpd_result[bw_idx].cal_tx_dpd.tx_dpd_coeff_i[j];
            wf5g_calb_rst_out->tx_dpd[i][bw_idx].tx_dpd_coeff_q[j] = wf5g_txdpd_result[bw_idx].cal_tx_dpd.tx_dpd_coeff_q[j];
        }

        t9026_wf5g_txdpd_rf_revert();
        RF_DPRINTF(RF_DEBUG_RESULT, "wf5g channel %d TX DPD calib finish \n", all_chn[chn_idx]);
    }
#endif

    t9026_wf5g_rxlnatank(&wf5g_rxlnatank_result,  manual, all_chn[chn_idx]);
    wf5g_calb_rst_out->rx_lna[chn_idx].rx_lna_code = wf5g_rxlnatank_result.cal_rx_lna.rx_lna_code;
    if(chn_idx == 0)
    {
        if((chn_idx + wf5g_calb_rst_out->rx_lna[chn_idx].rx_lna_code) > 7 && (chn_idx + wf5g_calb_rst_out->rx_lna[chn_idx].rx_lna_code) < 13)
       {
            RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "WF5G rx_lna[%d] calb pass!\n", chn_idx);
       } else {
            RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "WF5G rx_lna[%d] calb fail!\n", chn_idx);
       }
    } else {
        if((wf5g_calb_rst_out->rx_lna[chn_idx].rx_lna_code <= wf5g_calb_rst_out->rx_lna[chn_idx-1].rx_lna_code))
        {
            if((chn_idx + wf5g_calb_rst_out->rx_lna[chn_idx].rx_lna_code) > 7 && (chn_idx + wf5g_calb_rst_out->rx_lna[chn_idx].rx_lna_code) < 13)
            {
                RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "WF5G rx_lna[%d] calb pass!\n", chn_idx);
            } else {
                RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "WF5G rx_lna[%d] calb fail!\n", chn_idx);
            }
        } else {
            RF_DPRINTF(RF_DEBUG_RESULT_CHECK, "WF5G rx_lna[%d] calb fail!\n", chn_idx);
        }
    }

    if(all_chn[chn_idx] == 0x0a || all_chn[chn_idx] == 0x32 || all_chn[chn_idx] == 0x5a || all_chn[chn_idx] == 0x82 || all_chn[chn_idx] == 0xa5)
    {
        for_dpd_flg = 0;
        i = (chn_idx - 1)/2;
        for(bw_idx = 2; bw_idx >=0; bw_idx--)
        {
            t9026_wf5g_rxirr(&wf5g_rxirr_result[bw_idx], bw_idx, for_dpd_flg, check_flg);

            wf5g_calb_rst_out->rx_irr[i][bw_idx].coeff_eup[0] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[0];
            wf5g_calb_rst_out->rx_irr[i][bw_idx].coeff_eup[1] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[1];
            wf5g_calb_rst_out->rx_irr[i][bw_idx].coeff_eup[2] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[2];
            wf5g_calb_rst_out->rx_irr[i][bw_idx].coeff_eup[3] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[3];
            wf5g_calb_rst_out->rx_irr[i][bw_idx].coeff_pup[0] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[0];
            wf5g_calb_rst_out->rx_irr[i][bw_idx].coeff_pup[1] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[1];
            wf5g_calb_rst_out->rx_irr[i][bw_idx].coeff_pup[2] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[2];
            wf5g_calb_rst_out->rx_irr[i][bw_idx].coeff_pup[3] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[3];
            wf5g_calb_rst_out->rx_irr[i][bw_idx].coeff_elow[0] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[0];
            wf5g_calb_rst_out->rx_irr[i][bw_idx].coeff_elow[1] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[1];
            wf5g_calb_rst_out->rx_irr[i][bw_idx].coeff_elow[2] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[2];
            wf5g_calb_rst_out->rx_irr[i][bw_idx].coeff_elow[3] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[3];
            wf5g_calb_rst_out->rx_irr[i][bw_idx].coeff_plow[0] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[0];
            wf5g_calb_rst_out->rx_irr[i][bw_idx].coeff_plow[1] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[1];
            wf5g_calb_rst_out->rx_irr[i][bw_idx].coeff_plow[2] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[2];
            wf5g_calb_rst_out->rx_irr[i][bw_idx].coeff_plow[3] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[3];
        }  //  for(bw_idx = 0; bw_idx < 3; bw_idx++)
    }
    //t9026_cali_end();
    // enter sleep Mode + auto mode
    //t9026_rf_mode_auto();
}


void t9026_wf5g_cal_for_scan(U8 channel, struct t9026_wf5g_calb_rst_out* wf5g_calb_rst_out, bool check_flg)
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
    //int slope_tmp = 33;
    //int offset_tmp = 29591;

    struct wf5g_rxrc_result wf5g_rxrc_result;
    struct wf5g_rxdcoc_in wf5g_rxdcoc_in;
    struct wf5g_rxdcoc_result wf5g_rxdcoc_result;
    struct wf5g_txdciq_result wf5g_txdciq_result;
    struct wf5g_rxlnatank_result wf5g_rxlnatank_result;
    struct wf5g_rxirr_result wf5g_rxirr_result[3]; //arrary for BW=20,40,80MHz

    RF_DPRINTF(RF_DEBUG_MSG, "t9026_wf5g_cal_for_scan channel:%02x\n", channel);
    sx_cali_mode = (CHANNEL_MAP|PCODE_CALI|TCODE_CALI|MXR_TANK);
    wf_mode = WF5G_SX; // enter SX Mode
    t9026_rf_mode_access(wf_mode);
    t9026_wf5g_sx(channel, osc_freq, sx_cali_mode);

    wf_mode = WF5G_RX; // enter RX Mode
    t9026_rf_mode_access(wf_mode);

    t9026_wf5g_rxrc(&wf5g_rxrc_result);

    for(tia_idx = 0; tia_idx < 2; tia_idx++)
    {
       for(lpf_idx = 1; lpf_idx < 13; lpf_idx = lpf_idx+1)
        {
            wf5g_rxdcoc_in.bw_idx = bw_idx;
            wf5g_rxdcoc_in.tia_idx = tia_idx;
            wf5g_rxdcoc_in.lpf_idx = lpf_idx;
            wf5g_rxdcoc_in.manual_mode = 0x0;
            wf5g_rxdcoc_in.wf_rx_gain = (((tia_idx+8) <<4) + lpf_idx) & 0xff;
            t9026_wf5g_rxdcoc(&wf5g_rxdcoc_in, &wf5g_rxdcoc_result);
        }  //  for(lpf_idx = 0; lpf_idx < 12; lpf_idx++)
    }  // for(tia_idx = 0; tia_idx < 2; tia_idx++)

    wf_mode = WF5G_TX; // enter TX Mode
    t9026_rf_mode_access(wf_mode);

    //t9026_wf5g_txtssi_bias(slope_tmp, offset_tmp);

    t9026_rf_tx_modulation_access(0x3);
    t9026_wf5g_txiqdc(&wf5g_txdciq_result, tx_pa_gain, tx_mxr_gain, check_flg);
    wf5g_calb_rst_out->tx_dc_irr[2][0].tx_dc_i = wf5g_txdciq_result.cal_tx_dciq.tx_dc_i;
    wf5g_calb_rst_out->tx_dc_irr[2][0].tx_dc_q = wf5g_txdciq_result.cal_tx_dciq.tx_dc_q;
    wf5g_calb_rst_out->tx_dc_irr[2][0].tx_alpha = wf5g_txdciq_result.cal_tx_dciq.tx_alpha;
    wf5g_calb_rst_out->tx_dc_irr[2][0].tx_theta = wf5g_txdciq_result.cal_tx_dciq.tx_theta;

    t9026_wf5g_rxlnatank(&wf5g_rxlnatank_result,  manual, channel);


    bw_idx = 0;
    t9026_wf5g_rxirr(&wf5g_rxirr_result[bw_idx], bw_idx, 0, check_flg);


    //t9026_cali_end();
    // enter sleep Mode + auto mode
    //t9026_rf_mode_auto();
}

extern void hal_dpd_memory_download(void);

void t9026_wf5g_cal_for_linkA(U8 channel, U8 bw_idx, struct t9026_wf5g_calb_rst_out* wf5g_calb_rst_out, bool check_flg)
{
    WF_MODE_T wf_mode ;
    U8 osc_freq = 0x3;
    SX_CALI_MODE_T sx_cali_mode;
    U8 tx_pa_gain = 0x7;
    U8 tx_mxr_gain = 0x7;
    bool manual = 0;
    U8 for_dpd_flg = 0;
    int slope_tmp = 0;
    int offset_tmp = 0;
    U8 tssi_chn_idx = 0;
    U8 tx_modulation[2] = {0x3, 0x2};
    U8 dacgain_idx = 0;
    struct wf5g_txdciq_result wf5g_txdciq_result;
    struct wf5g_rxlnatank_result wf5g_rxlnatank_result;
    struct wf5g_rxirr_result wf5g_rxirr_result[3]; //arrary for BW=20,40,80MHz
    struct wf5g_txtssi_result wf5g_txtssi_result;
#ifdef CONFIG_DPD_ENABLE
    int j = 0;
    U8 sync_gap = 0x0;
    U8 dpd_gain = 0;
    struct wf_txdpd_result wf5g_txdpd_result[3]; //arrary for BW=20,40,80MHz
#endif
#if 0
    int ph = 0;
    int pl = 0;
    int vh = 0;
    int vl = 0;
    if (channel >= 0x24 && channel <= 0x3c)
    {
        //slope_tmp = 32;
        //offset_tmp = 29600;
        ph = 0x4e;//(19.5dbm)
        pl = 0x38;//(14.1dbm)
        vh = 0x5e;
        vl = 0x30;
    }
    else if (channel > 0x3c && channel <= 0x64)
    {
        //slope_tmp = 32;
        //offset_tmp = 29591;
        ph = 0x4e;//(19.6dbm)
        pl = 0x3a;//(14.2dbm)
        vh = 0x4a;
        vl = 0x2a;
    }
    else if (channel > 0x64 && channel <= 0x8c)
    {
        //slope_tmp = 32;
        //offset_tmp = 29760;
        ph = 0x50;//(19.9dbm)
        pl = 0x3a;//(13.9dbm)
        vh = 0x48;
        vl = 0x24;
    }
    else if (channel > 0x8c && channel <= 0xa5)
    {
        //slope_tmp = 33;
        //offset_tmp = 29743;
        ph = 0x4c;//(19.0dbm)
        pl = 0x32;//(12.6dbm)
        vh = 0x40;
        vl = 0x1c;
    }
#endif

    RF_DPRINTF(RF_DEBUG_MSG, "t9026_wf5g_cal_for_linkA channel:%02x\n", channel);
    sx_cali_mode = (CHANNEL_MAP|PCODE_CALI|TCODE_CALI|MXR_TANK);
    wf_mode = WF5G_SX; // enter SX Mode
    t9026_rf_mode_access(wf_mode);
    t9026_wf5g_sx(channel, osc_freq, sx_cali_mode);

    wf_mode = WF5G_TX; // enter TX Mode
    t9026_rf_mode_access(wf_mode);

    if (g_temp_fall_flag == 1)
    {
         tx_mxr_gain = 0x3;
    }

    if (bw_idx == 0)
    {
       dacgain_idx = 0;
    }
    else if (bw_idx == 1)
    {
       dacgain_idx = 0;
    }
    else if (bw_idx == 2)
    {
       dacgain_idx = 1;
    }

    t9026_rf_tx_modulation_access( tx_modulation[dacgain_idx] );
    t9026_wf5g_txiqdc(&wf5g_txdciq_result, tx_pa_gain, tx_mxr_gain, check_flg);

    wf5g_calb_rst_out->tx_dc_irr[0][dacgain_idx].tx_dc_i = wf5g_txdciq_result.cal_tx_dciq.tx_dc_i;
    wf5g_calb_rst_out->tx_dc_irr[0][dacgain_idx].tx_dc_q = wf5g_txdciq_result.cal_tx_dciq.tx_dc_q;
    wf5g_calb_rst_out->tx_dc_irr[0][dacgain_idx].tx_alpha = wf5g_txdciq_result.cal_tx_dciq.tx_alpha;
    wf5g_calb_rst_out->tx_dc_irr[0][dacgain_idx].tx_theta = wf5g_txdciq_result.cal_tx_dciq.tx_theta;

    t9026_wf5g_get_efuse_tssi(&wf5g_txtssi_result, channel);
    slope_tmp = wf5g_txtssi_result.cal_tx_tssi.txpwc_slope;
    offset_tmp = wf5g_txtssi_result.cal_tx_tssi.txpwc_offset;
    tssi_chn_idx = wf5g_txtssi_result.cal_tx_tssi.tssi_chn_idx;
    t9026_wf5g_txtssi_ratio(&wf5g_txtssi_result, slope_tmp, offset_tmp, tssi_chn_idx, tx_mxr_gain);
    wf5g_calb_rst_out->tx_tssi[0].gain_ratio = wf5g_txtssi_result.cal_tx_tssi.gain_ratio;

#ifdef CONFIG_DPD_ENABLE
    for_dpd_flg = 1;
    sync_gap = 64-1-2-3;
#if 0
    if (channel >=180 || channel < 30)
    {
         dpd_gain = 0xb0;
    }
    else if (channel >=30 && channel < 70)
    {
         dpd_gain = 0xa0;
    }
    else if (channel >=70 && channel < 110)
    {
         dpd_gain = 0x90;
    }
    else if (channel >=110 && channel < 150)
    {
         dpd_gain = 0x80;
    }
    else if (channel >=150 && channel < 180)
    {
         dpd_gain = 0x70;
    }
#endif
    dpd_gain = 0x90;

    hal_dpd_memory_download();

    t9026_rf_tx_modulation_access(tx_modulation[dacgain_idx]);
    t9026_wf5g_txdpd_rf_init(tx_pa_gain, tx_mxr_gain);

    t9026_wf5g_rxirr(&wf5g_rxirr_result[1], 1, for_dpd_flg, 0);//bw_idx

    //t9026_ofdm_send_packet();

    t9026_wf5g_txdpd_start(sync_gap, 1, dpd_gain);
    t9026_wf5g_txdpd_end(&wf5g_txdpd_result[1], sync_gap);

    //t9026_ofdm_stop_packet();

    wf5g_calb_rst_out->tx_dpd[0][1].snr = wf5g_txdpd_result[1].cal_tx_dpd.snr;
    for(j = 0; j < 15; j++){
        wf5g_calb_rst_out->tx_dpd[0][1].tx_dpd_coeff_i[j] = wf5g_txdpd_result[1].cal_tx_dpd.tx_dpd_coeff_i[j];
        wf5g_calb_rst_out->tx_dpd[0][1].tx_dpd_coeff_q[j] = wf5g_txdpd_result[1].cal_tx_dpd.tx_dpd_coeff_q[j];
    }

    t9026_wf5g_txdpd_rf_revert();
    RF_DPRINTF(RF_DEBUG_RESULT, "wf5g channel %d TX DPD calib finish \n", channel);
#endif

    t9026_wf5g_rxlnatank(&wf5g_rxlnatank_result,  manual, channel);
    wf5g_calb_rst_out->rx_lna[0].rx_lna_code = wf5g_rxlnatank_result.cal_rx_lna.rx_lna_code;

    for_dpd_flg = 0;
    t9026_wf5g_rxirr(&wf5g_rxirr_result[bw_idx], bw_idx, for_dpd_flg, check_flg);

    wf5g_calb_rst_out->rx_irr[0][bw_idx].coeff_eup[0] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[0];
    wf5g_calb_rst_out->rx_irr[0][bw_idx].coeff_eup[1] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[1];
    wf5g_calb_rst_out->rx_irr[0][bw_idx].coeff_eup[2] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[2];
    wf5g_calb_rst_out->rx_irr[0][bw_idx].coeff_eup[3] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[3];
    wf5g_calb_rst_out->rx_irr[0][bw_idx].coeff_pup[0] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[0];
    wf5g_calb_rst_out->rx_irr[0][bw_idx].coeff_pup[1] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[1];
    wf5g_calb_rst_out->rx_irr[0][bw_idx].coeff_pup[2] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[2];
    wf5g_calb_rst_out->rx_irr[0][bw_idx].coeff_pup[3] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[3];
    wf5g_calb_rst_out->rx_irr[0][bw_idx].coeff_elow[0] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[0];
    wf5g_calb_rst_out->rx_irr[0][bw_idx].coeff_elow[1] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[1];
    wf5g_calb_rst_out->rx_irr[0][bw_idx].coeff_elow[2] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[2];
    wf5g_calb_rst_out->rx_irr[0][bw_idx].coeff_elow[3] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[3];
    wf5g_calb_rst_out->rx_irr[0][bw_idx].coeff_plow[0] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[0];
    wf5g_calb_rst_out->rx_irr[0][bw_idx].coeff_plow[1] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[1];
    wf5g_calb_rst_out->rx_irr[0][bw_idx].coeff_plow[2] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[2];
    wf5g_calb_rst_out->rx_irr[0][bw_idx].coeff_plow[3] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[3];

    t9026_cali_end();
    // enter sleep Mode + auto mode
    t9026_rf_mode_auto();
}

void t9026_wf5g_cal_for_linkB(U8 channel, U8 bw_idx, struct t9026_wf5g_calb_rst_out* wf5g_calb_rst_out, bool check_flg)
{
    WF_MODE_T wf_mode ;
    U8 osc_freq = 0x3;
    SX_CALI_MODE_T sx_cali_mode;
    U8 tx_pa_gain = 0x7;
    U8 tx_mxr_gain = 0x7;
    bool manual = 0;
    U8 for_dpd_flg = 0;
    int slope_tmp = 0;
    int offset_tmp = 0;
    U8 tssi_chn_idx = 0;
    U8 tx_modulation[2] = {0x3, 0x2};
    U8 dacgain_idx = 0;
    struct wf5g_txdciq_result wf5g_txdciq_result;
    struct wf5g_rxlnatank_result wf5g_rxlnatank_result;
    struct wf5g_rxirr_result wf5g_rxirr_result[3]; //arrary for BW=20,40,80MHz
    struct wf5g_txtssi_result wf5g_txtssi_result;
#ifdef CONFIG_DPD_ENABLE
    int j = 0;
    U8 sync_gap = 0x0;
    U8 dpd_gain = 0;
    struct wf_txdpd_result wf5g_txdpd_result[3]; //arrary for BW=20,40,80MHz
#endif
#if 0
    int ph = 0;
    int pl = 0;
    int vh = 0;
    int vl = 0;
    if (channel >= 0x24 && channel <= 0x3c)
    {
        //slope_tmp = 32;
        //offset_tmp = 29600;
        ph = 0x4e;//(19.5dbm)
        pl = 0x38;//(14.1dbm)
        vh = 0x5e;
        vl = 0x30;
     }
     else if (channel > 0x3c && channel <= 0x64)
     {
        //slope_tmp = 32;
        //offset_tmp = 29591;
        ph = 0x4e;//(19.6dbm)
        pl = 0x3a;//(14.2dbm)
        vh = 0x4a;
        vl = 0x2a;
     }
     else if (channel > 0x64 && channel <= 0x8c)
     {
        //slope_tmp = 32;
        //offset_tmp = 29760;
        ph = 0x50;//(19.9dbm)
        pl = 0x3a;//(13.9dbm)
        vh = 0x48;
        vl = 0x24;
     }
     else if (channel > 0x8c && channel <= 0xa5)
     {
        //slope_tmp = 33;
        //offset_tmp = 29743;
        ph = 0x4c;//(19.0dbm)
        pl = 0x32;//(12.6dbm)
        vh = 0x40;
        vl = 0x1c;
    }
#endif

    RF_DPRINTF(RF_DEBUG_MSG, "t9026_wf5g_cal_for_linkB channel:%02x\n", channel);
    sx_cali_mode = (CHANNEL_MAP|PCODE_CALI|TCODE_CALI|MXR_TANK);
    wf_mode = WF5G_SX; // enter SX Mode
    t9026_rf_mode_access(wf_mode);
    t9026_wf5g_sx(channel, osc_freq, sx_cali_mode);

    wf_mode = WF5G_TX; // enter TX Mode
    t9026_rf_mode_access(wf_mode);

    if (g_temp_fall_flag == 1)
    {
         tx_mxr_gain = 0x3;
    }

    if(bw_idx == 0){
       dacgain_idx = 0;
    }else if(bw_idx == 1){
       dacgain_idx = 0;
    }else if(bw_idx == 2){
       dacgain_idx = 1;
    }
    t9026_rf_tx_modulation_access( tx_modulation[dacgain_idx] );
    t9026_wf5g_txiqdc(&wf5g_txdciq_result, tx_pa_gain, tx_mxr_gain, check_flg);

    wf5g_calb_rst_out->tx_dc_irr[1][dacgain_idx].tx_dc_i = wf5g_txdciq_result.cal_tx_dciq.tx_dc_i;
    wf5g_calb_rst_out->tx_dc_irr[1][dacgain_idx].tx_dc_q = wf5g_txdciq_result.cal_tx_dciq.tx_dc_q;
    wf5g_calb_rst_out->tx_dc_irr[1][dacgain_idx].tx_alpha = wf5g_txdciq_result.cal_tx_dciq.tx_alpha;
    wf5g_calb_rst_out->tx_dc_irr[1][dacgain_idx].tx_theta = wf5g_txdciq_result.cal_tx_dciq.tx_theta;

    t9026_wf5g_get_efuse_tssi(&wf5g_txtssi_result, channel);
    //t9026_wf5g_txtssi(&wf5g_txtssi_result, ph, pl, vh, vl);
    slope_tmp = wf5g_txtssi_result.cal_tx_tssi.txpwc_slope;
    offset_tmp = wf5g_txtssi_result.cal_tx_tssi.txpwc_offset;
    tssi_chn_idx = wf5g_txtssi_result.cal_tx_tssi.tssi_chn_idx;
    t9026_wf5g_txtssi_ratio(&wf5g_txtssi_result, slope_tmp, offset_tmp, tssi_chn_idx, tx_mxr_gain);
    wf5g_calb_rst_out->tx_tssi[1].gain_ratio = wf5g_txtssi_result.cal_tx_tssi.gain_ratio;

#ifdef CONFIG_DPD_ENABLE
    for_dpd_flg = 1;
    sync_gap = 64-1-2-3;
#if 0
    if(channel >=180 || channel < 30){
         dpd_gain = 0xb0;
    }else if(channel >=30 || channel < 70){
         dpd_gain = 0xa0;
    }else if(channel >=70 || channel < 110){
         dpd_gain = 0x90;
    }else if(channel >=110 || channel < 150){
         dpd_gain = 0x80;
    }else if(channel >=150 || channel < 180){
         dpd_gain = 0x70;
    }
#endif
    dpd_gain = 0x90;
    hal_dpd_memory_download();
    t9026_rf_tx_modulation_access( tx_modulation[dacgain_idx] );
    t9026_wf5g_txdpd_rf_init(tx_pa_gain, tx_mxr_gain);

    t9026_wf5g_rxirr(&wf5g_rxirr_result[1], 1, for_dpd_flg, 0);//bw_idx

    t9026_wf5g_txdpd_start(sync_gap, 1, dpd_gain);
    t9026_wf5g_txdpd_end(&wf5g_txdpd_result[1], sync_gap);

    wf5g_calb_rst_out->tx_dpd[1][1].snr = wf5g_txdpd_result[1].cal_tx_dpd.snr;
    for(j = 0; j < 15; j++){
        wf5g_calb_rst_out->tx_dpd[1][1].tx_dpd_coeff_i[j] = wf5g_txdpd_result[1].cal_tx_dpd.tx_dpd_coeff_i[j];
        wf5g_calb_rst_out->tx_dpd[1][1].tx_dpd_coeff_q[j] = wf5g_txdpd_result[1].cal_tx_dpd.tx_dpd_coeff_q[j];
    }

    t9026_wf5g_txdpd_rf_revert();
    RF_DPRINTF(RF_DEBUG_RESULT, "wf5g channel %d TX DPD calib finish \n", channel);
#endif

    t9026_wf5g_rxlnatank(&wf5g_rxlnatank_result,  manual, channel);
    wf5g_calb_rst_out->rx_lna[1].rx_lna_code = wf5g_rxlnatank_result.cal_rx_lna.rx_lna_code;

    for_dpd_flg = 0;
    t9026_wf5g_rxirr(&wf5g_rxirr_result[bw_idx], bw_idx, for_dpd_flg, check_flg);

    wf5g_calb_rst_out->rx_irr[1][bw_idx].coeff_eup[0] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[0];
    wf5g_calb_rst_out->rx_irr[1][bw_idx].coeff_eup[1] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[1];
    wf5g_calb_rst_out->rx_irr[1][bw_idx].coeff_eup[2] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[2];
    wf5g_calb_rst_out->rx_irr[1][bw_idx].coeff_eup[3] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_eup[3];
    wf5g_calb_rst_out->rx_irr[1][bw_idx].coeff_pup[0] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[0];
    wf5g_calb_rst_out->rx_irr[1][bw_idx].coeff_pup[1] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[1];
    wf5g_calb_rst_out->rx_irr[1][bw_idx].coeff_pup[2] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[2];
    wf5g_calb_rst_out->rx_irr[1][bw_idx].coeff_pup[3] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_pup[3];
    wf5g_calb_rst_out->rx_irr[1][bw_idx].coeff_elow[0] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[0];
    wf5g_calb_rst_out->rx_irr[1][bw_idx].coeff_elow[1] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[1];
    wf5g_calb_rst_out->rx_irr[1][bw_idx].coeff_elow[2] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[2];
    wf5g_calb_rst_out->rx_irr[1][bw_idx].coeff_elow[3] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_elow[3];
    wf5g_calb_rst_out->rx_irr[1][bw_idx].coeff_plow[0] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[0];
    wf5g_calb_rst_out->rx_irr[1][bw_idx].coeff_plow[1] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[1];
    wf5g_calb_rst_out->rx_irr[1][bw_idx].coeff_plow[2] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[2];
    wf5g_calb_rst_out->rx_irr[1][bw_idx].coeff_plow[3] = wf5g_rxirr_result[bw_idx].cal_rx_irr.coeff_plow[3];
    
    t9026_cali_end();
    // enter sleep Mode + auto mode
    t9026_rf_mode_auto();
}

#endif // RF_CALI_TEST
