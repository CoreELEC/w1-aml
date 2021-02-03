#include "rf_calibration.h"

#ifdef RF_CALI_TEST

#if 1
long pow_a(unsigned int a, unsigned char b) {
    unsigned char i;
    unsigned int result;

    result = a;
    for (i = 1; i < b; ++i) {
        result *= a;
    }

    return (long)result;
}
#endif

int t9026_get_spectrum_snr_pwr(int freq_offset, U8 tone_mode)
{
    int pwr_db;
    int power;
    int fs;
    int tmp;
    unsigned int reg_data;
    unsigned int unlock_cnt = 0;
    unsigned int TIME_OUT_CNT = RF_TIME_OUT_CNT;
    RG_CORE_A2_FIELD_T    rg_core_a2;
    RG_RECV_A11_FIELD_T    rg_recv_a11;
    RG_ESTI_A14_FIELD_T    rg_esti_a14;
    RG_CORE_A6_FIELD_T    rg_core_a6;
    RG_ESTI_A64_FIELD_T    rg_esti_a64;
    RG_ESTI_A87_FIELD_T    rg_esti_a87;
    RG_ESTI_A88_FIELD_T    rg_esti_a88;

    // Internal variables definitions
    bool    RG_RECV_A11_saved = False;
    RG_RECV_A11_FIELD_T    rg_recv_a11_i;
    // Internal variables definitions
    bool    RG_ESTI_A14_saved = False;
    RG_ESTI_A14_FIELD_T    rg_esti_a14_i;
    // Internal variables definitions
    bool    RG_CORE_A6_saved = False;
    RG_CORE_A6_FIELD_T    rg_core_a6_i;
    // Internal variables definitions
    bool    RG_ESTI_A64_saved = False;
    RG_ESTI_A64_FIELD_T    rg_esti_a64_i;
    // Internal variables definitions
    bool    RG_ESTI_A87_saved = False;
    RG_ESTI_A87_FIELD_T    rg_esti_a87_i;

    fs = 80;
    if (freq_offset >= 0) {
        tmp = (long)(freq_offset * 2 * pow_a(2, 24)) / fs;
    } else {
        tmp = (long)(freq_offset * 2 * pow_a(2, 24)) / fs + pow_a(2, 25) - 1;
    }

    reg_data = tmp;

    rg_core_a2.data = fi_ahb_read(RG_CORE_A2);
    rg_core_a2.b.rg_core_enb_man = 0x0;
    rg_core_a2.b.rg_core_enb_man_en = 0x1;
    rg_core_a2.b.rg_xmit_enb_man = 0x0;
    rg_core_a2.b.rg_xmit_enb_man_en = 0x1;
    rg_core_a2.b.rg_recv_enb_man = 0x0;
    rg_core_a2.b.rg_recv_enb_man_en = 0x1;
    rg_core_a2.b.rg_esti_enb_man = 0x0;
    rg_core_a2.b.rg_esti_enb_man_en = 0x1;
    fi_ahb_write(RG_CORE_A2, rg_core_a2.data);

    rg_recv_a11.data = fi_ahb_read(RG_RECV_A11);
    if(!RG_RECV_A11_saved) {
        RG_RECV_A11_saved = True;
        rg_recv_a11_i.b.rg_rxirr_man_mode = rg_recv_a11.b.rg_rxirr_man_mode;
        rg_recv_a11_i.b.rg_rxirr_bypass = rg_recv_a11.b.rg_rxirr_bypass;
    }
    rg_recv_a11.b.rg_rxirr_man_mode = 0x1;
    rg_recv_a11.b.rg_rxirr_bypass = 0x0;
    fi_ahb_write(RG_RECV_A11, rg_recv_a11.data);

    rg_esti_a14.data = fi_ahb_read(RG_ESTI_A14);
    if(!RG_ESTI_A14_saved) {
        RG_ESTI_A14_saved = True;
        rg_esti_a14_i.b.rg_dc_rm_leaky_factor = rg_esti_a14.b.rg_dc_rm_leaky_factor;
        rg_esti_a14_i.b.rg_spectrum_ana_angle_man = rg_esti_a14.b.rg_spectrum_ana_angle_man;
        rg_esti_a14_i.b.rg_spectrum_ana_angle_man_en = rg_esti_a14.b.rg_spectrum_ana_angle_man_en;
    }
    rg_esti_a14.b.rg_dc_rm_leaky_factor = 0x7;
    rg_esti_a14.b.rg_spectrum_ana_angle_man = reg_data;
    rg_esti_a14.b.rg_spectrum_ana_angle_man_en = 0x1;
    fi_ahb_write(RG_ESTI_A14, rg_esti_a14.data);

    if(tone_mode == 1) {
        rg_core_a6.data = fi_ahb_read(RG_CORE_A6);
        if(!RG_CORE_A6_saved) {
            RG_CORE_A6_saved = True;
            rg_core_a6_i.b.rg_cali_mode_man = rg_core_a6.b.rg_cali_mode_man;
            rg_core_a6_i.b.rg_cali_mode_man_en = rg_core_a6.b.rg_cali_mode_man_en;
        }
        rg_core_a6.b.rg_cali_mode_man = 0x5;
        rg_core_a6.b.rg_cali_mode_man_en = 0x1;
        fi_ahb_write(RG_CORE_A6, rg_core_a6.data);

    } else {
        rg_core_a6.data = fi_ahb_read(RG_CORE_A6);
        if(!RG_CORE_A6_saved) {
            RG_CORE_A6_saved = True;
            rg_core_a6_i.b.rg_cali_mode_man = rg_core_a6.b.rg_cali_mode_man;
            rg_core_a6_i.b.rg_cali_mode_man_en = rg_core_a6.b.rg_cali_mode_man_en;
        }
        rg_core_a6.b.rg_cali_mode_man = 0x6;
        rg_core_a6.b.rg_cali_mode_man_en = 0x1;
        fi_ahb_write(RG_CORE_A6, rg_core_a6.data);
    }

    rg_esti_a64.data = fi_ahb_read(RG_ESTI_A64);
    if(!RG_ESTI_A64_saved) {
        RG_ESTI_A64_saved = True;
        rg_esti_a64_i.b.rg_adda_wait_count = rg_esti_a64.b.rg_adda_wait_count;
    }
    rg_esti_a64.b.rg_adda_wait_count = 0x1000;
    fi_ahb_write(RG_ESTI_A64, rg_esti_a64.data);

    rg_esti_a87.data = fi_ahb_read(RG_ESTI_A87);
    if(!RG_ESTI_A87_saved) {
        RG_ESTI_A87_saved = True;
        rg_esti_a87_i.b.rg_snr_esti_en = rg_esti_a87.b.rg_snr_esti_en;
        rg_esti_a87_i.b.rg_snr_esti_calctime = rg_esti_a87.b.rg_snr_esti_calctime;
        rg_esti_a87_i.b.rg_snr_alpha = rg_esti_a87.b.rg_snr_alpha;
        rg_esti_a87_i.b.rg_txdpd_snr_esti_alpha = rg_esti_a87.b.rg_txdpd_snr_esti_alpha;
    }
    rg_esti_a87.b.rg_snr_esti_en = 0x1;
    rg_esti_a87.b.rg_snr_esti_calctime = 0x7;
    rg_esti_a87.b.rg_snr_alpha = 0x0;
    rg_esti_a87.b.rg_txdpd_snr_esti_alpha = 0x0;
    fi_ahb_write(RG_ESTI_A87, rg_esti_a87.data);

    rg_core_a2.data = fi_ahb_read(RG_CORE_A2);
    rg_core_a2.b.rg_core_enb_man = 0x1;
    rg_core_a2.b.rg_core_enb_man_en = 0x1;
    rg_core_a2.b.rg_xmit_enb_man = 0x1;
    rg_core_a2.b.rg_xmit_enb_man_en = 0x1;
    rg_core_a2.b.rg_recv_enb_man = 0x1;
    rg_core_a2.b.rg_recv_enb_man_en = 0x1;
    rg_core_a2.b.rg_esti_enb_man = 0x1;
    rg_core_a2.b.rg_esti_enb_man_en = 0x1;
    fi_ahb_write(RG_CORE_A2, rg_core_a2.data);

    rg_esti_a88.data = fi_ahb_read(RG_ESTI_A88);

    msleep(300);
    while (unlock_cnt < TIME_OUT_CNT)
    {
        if(rg_esti_a88.b.ro_adda_db_vld == 0)
        {
            rg_esti_a88.data = fi_ahb_read(RG_ESTI_A88);
            //g_fw_funcs.system_wait(1);
            unlock_cnt = unlock_cnt + 1;
            if(unlock_cnt == TIME_OUT_CNT)
            {
                RF_DPRINTF(RF_DEBUG_INFO, "error: SNR Pwr TIME OUT\n");
            }
        }
        else
        {
            RF_DPRINTF(RF_DEBUG_INFO, "SNR Pwr ready\n");
            break;
        }
    }

    rg_esti_a88.data = fi_ahb_read(RG_ESTI_A88);
    pwr_db = rg_esti_a88.b.ro_adda_pwr_db;
    RF_DPRINTF(RF_DEBUG_RESULT, "spectrum snr pwr_db: 0x%x\n", pwr_db);
    if(pwr_db & BIT(8)) {
        power = (pwr_db - pow_a(2, 9))/4;
    } else {
        power = pwr_db/4;
    }
    RF_DPRINTF(RF_DEBUG_RESULT, "unlock_cnt:%d, spectrum snr pwr: %d\n", unlock_cnt, power);

    rg_esti_a87.data = fi_ahb_read(RG_ESTI_A87);
    rg_esti_a87.b.rg_snr_esti_en = 0x0;
    fi_ahb_write(RG_ESTI_A87, rg_esti_a87.data);

    // Revert the original value before calibration back
    rg_recv_a11.data = fi_ahb_read(RG_RECV_A11);
    rg_recv_a11.b.rg_rxirr_man_mode = rg_recv_a11_i.b.rg_rxirr_man_mode;
    rg_recv_a11.b.rg_rxirr_bypass = rg_recv_a11_i.b.rg_rxirr_bypass;
    fi_ahb_write(RG_RECV_A11, rg_recv_a11.data);
    // Revert the original value before calibration back
    rg_esti_a14.data = fi_ahb_read(RG_ESTI_A14);
    rg_esti_a14.b.rg_dc_rm_leaky_factor = rg_esti_a14_i.b.rg_dc_rm_leaky_factor;
    rg_esti_a14.b.rg_spectrum_ana_angle_man = rg_esti_a14_i.b.rg_spectrum_ana_angle_man;
    rg_esti_a14.b.rg_spectrum_ana_angle_man_en = rg_esti_a14_i.b.rg_spectrum_ana_angle_man_en;
    fi_ahb_write(RG_ESTI_A14, rg_esti_a14.data);
    // Revert the original value before calibration back
    rg_core_a6.data = fi_ahb_read(RG_CORE_A6);
    rg_core_a6.b.rg_cali_mode_man = rg_core_a6_i.b.rg_cali_mode_man;
    rg_core_a6.b.rg_cali_mode_man_en = rg_core_a6_i.b.rg_cali_mode_man_en;
    fi_ahb_write(RG_CORE_A6, rg_core_a6.data);
    // Revert the original value before calibration back
    rg_core_a6.data = fi_ahb_read(RG_CORE_A6);
    rg_core_a6.b.rg_cali_mode_man = rg_core_a6_i.b.rg_cali_mode_man;
    rg_core_a6.b.rg_cali_mode_man_en = rg_core_a6_i.b.rg_cali_mode_man_en;
    fi_ahb_write(RG_CORE_A6, rg_core_a6.data);
    // Revert the original value before calibration back
    rg_esti_a64.data = fi_ahb_read(RG_ESTI_A64);
    rg_esti_a64.b.rg_adda_wait_count = rg_esti_a64_i.b.rg_adda_wait_count;
    fi_ahb_write(RG_ESTI_A64, rg_esti_a64.data);
    // Revert the original value before calibration back
    rg_esti_a87.data = fi_ahb_read(RG_ESTI_A87);
    rg_esti_a87.b.rg_snr_esti_en = rg_esti_a87_i.b.rg_snr_esti_en;
    rg_esti_a87.b.rg_snr_esti_calctime = rg_esti_a87_i.b.rg_snr_esti_calctime;
    rg_esti_a87.b.rg_snr_alpha = rg_esti_a87_i.b.rg_snr_alpha;
    rg_esti_a87.b.rg_txdpd_snr_esti_alpha = rg_esti_a87_i.b.rg_txdpd_snr_esti_alpha;
    fi_ahb_write(RG_ESTI_A87, rg_esti_a87.data);
    t9026_cali_mode_exit();
    return power;
}

void check_spectrum_snr(unsigned int channel, unsigned int bw_idx)
{
    WF_MODE_T wf_mode;
    U8 osc_freq = 0x3;
    SX_CALI_MODE_T sx_cali_mode;
    int pwr_list_tone[2048]={0};
    int cnt_max = 40;
    int freq_idx = 0;
    int freq_step = 1;
    int freqoffset;

    RG_RX_A2_FIELD_T    rg_rx_a2;

    sx_cali_mode = CHANNEL_MAP|PCODE_CALI|TCODE_CALI|MXR_TANK;
    wf_mode = WF5G_SX; // enter SX Mode
    t9026_rf_mode_access(wf_mode);
    t9026_wf5g_sx(channel, osc_freq, sx_cali_mode);

    wf_mode = WF5G_RX; // enter RX Mode
    t9026_rf_mode_access(wf_mode);

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN = 0x9c;
    rg_rx_a2.b.RG_WF_RX_GAIN_MAN_MODE = 0x1;
    rf_write_register(RG_RX_A2, rg_rx_a2.data);

    if(bw_idx == RF_BW_20M)
    {
        rg_rx_a2.data = rf_read_register(RG_RX_A2);
        rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE = 0x1;
        rg_rx_a2.b.RG_WF_RX_BW_SEL = 0x0;                         // 20MHGz BW
        rg_rx_a2.b.RG_WF_RX_ADCFS_SEL = 0x0;                         // adc 160MHz
        rf_write_register(RG_RX_A2, rg_rx_a2.data);
    }
    else if(bw_idx == RF_BW_40M)
    {
        rg_rx_a2.data = rf_read_register(RG_RX_A2);
        rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE = 0x1;
        rg_rx_a2.b.RG_WF_RX_BW_SEL = 0x1;                         // 40MHz BW
        rg_rx_a2.b.RG_WF_RX_ADCFS_SEL = 0x0;                         // adc 160MHz
        rf_write_register(RG_RX_A2, rg_rx_a2.data);
    }
    else if(bw_idx == RF_BW_80M)
    {
        rg_rx_a2.data = rf_read_register(RG_RX_A2);
        rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE = 0x1;
        rg_rx_a2.b.RG_WF_RX_BW_SEL = 0x2;                         // 80MHz BW
        rg_rx_a2.b.RG_WF_RX_ADCFS_SEL = 0x1;                         // adc 320MHz
        rf_write_register(RG_RX_A2, rg_rx_a2.data);
    }

    for(freqoffset = 0; freqoffset <= cnt_max; freqoffset = freqoffset + freq_step)
    {
        pwr_list_tone[freq_idx] = t9026_get_spectrum_snr_pwr(-freqoffset, 0);
        printk("%x  %d  %6d  %6d\n", channel, bw_idx, freqoffset, pwr_list_tone[freq_idx]);
        freq_idx ++;
    }

    for(freqoffset = 1; freqoffset <= cnt_max; freqoffset = freqoffset + freq_step)
    {
        pwr_list_tone[freq_idx] = t9026_get_spectrum_snr_pwr(freqoffset, 0);
        printk("%x  %d  %6d  %6d\n", channel, bw_idx, -freqoffset, pwr_list_tone[freq_idx]);
        freq_idx ++;
    }
    t9026_rf_mode_auto();
    return;
}

#endif // RF_CALI_TEST
