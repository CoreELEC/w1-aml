#include"t9026_top.h"
#include "rf_calibration.h"

struct t9026_wf5g_calb_rst_out wf5g_calb_result;
struct t9026_wf2g_calb_rst_out wf2g_calb_result;
U8 g_temp_rise_flag;
U8 g_temp_fall_flag;
unsigned char rf_debug = RF_DEBUG_NONE;//RF_DEBUG_RESULT | RF_DEBUG_RESULT_CHECK;//RF_DEBUG_ALL | RF_DEBUG_NONE;
unsigned char rf_2g_calibration = 0;
unsigned char rf_5g_a = 0;

unsigned int agc_9026_tbl[][2] =
{
    {0x00008114, 0x00000005},
    {0x00008300, 0x00000011},
    {0x00008304, 0x00000012},
    {0x00008308, 0x00000013},
    {0x0000830c, 0x00000014},
    {0x00008310, 0x00000015},
    {0x00008314, 0x00000016},
    {0x00008318, 0x00000024},
    {0x0000831c, 0x00000025},
    {0x00008320, 0x00000026},
    {0x00008324, 0x00000034},
    {0x00008328, 0x00000035},
    {0x0000832c, 0x00000036},
    {0x00008330, 0x00000044},
    {0x00008334, 0x00000045},
    {0x00008338, 0x00000046},
    {0x0000833c, 0x00000054},
    {0x00008340, 0x00000055},
    {0x00008344, 0x00000056},
    {0x00008348, 0x00000064},
    {0x0000834c, 0x00000065},
    {0x00008350, 0x00000066},
    {0x00008354, 0x00000074},
    {0x00008358, 0x00000075},
    {0x0000835c, 0x00000076},
    {0x00008360, 0x00000084},
    {0x00008364, 0x00000085},
    {0x00008368, 0x00000086},
    {0x0000836c, 0x00000094},
    {0x00008370, 0x00000095},
    {0x00008374, 0x00000096},
    {0x00008378, 0x00000097},
    {0x0000837c, 0x00000098},
    {0x00008380, 0x00000099},
    {0x00008384, 0x0000009a},
    {0x00008388, 0x0000009b},
    {0x0000838c, 0x0000009c},
    {0x00008390, 0x0000009c},
    {0x00008114, 0x00000105},
    {0x00008050, 0x003ec104},
    {0x00008054, 0x003ec104},
    {0x00008058, 0x00050104},
    {0x0000810c, 0x00001550},//rx agc auto gain

    {0x00008148, 0x07412828},// enable pow sat in det wait
    {0x00009298, 0x58051108},// ldpc iteration
    {0x00009130, 0x0003180c},// increase ltf confirm thr

    {0x0000e808, 0x11100001}, // agc 320M enable
    {0x00008168, 0x04002818}, // adc 320M, bit26 :1

    {0x00008018, 0x0481a00a}, // det wait timeout, 8us.  0xa = 0.5us
    {0x0000801c, 0x121400bc}, // init stable
    {0x00008020, 0x08004ee0}, // ramp down thr
    {0x00008128, 0x00003211}, // rssi det enable [12]
    {0x00008084, 0x00107800}, // rf gain sel [28]
    {0x00008158, 0x3001a016}, // samp sat disable
    {0x00008038, 0x521d1d44}, // 1st ad sat avglen
    {0x00008144, 0x44410000}, // pow avg len
    {0x00008008, 0x01d801d4}, // target gain
    {0x00008178, 0x01a1a22f}, // rssi sat thr -3dB
    {0x0000805c, 0x306a4080}, // (yuntao,20200927)STF defult 306a4020; stf q thr,   20191017 for mc0 sen
    {0x00a08074, 0x002d8328}, //shijie.chen change
    {0x00008060, 0xc3caa9a0}, // fft q thr
    //{0x0000816c, 0x00a96311}, // rssi delta 2g 20M
    {0x0000816c, 0x02296311}, // rssi delta
    {0x00008014, 0x10200414}, //RF stable
    //(shijie, 20201022), extand time of checking cca busy to protect tx ok (especiall fiac queue)
    {0x00a08028, 0x10080000},
    //(shijie, 20201023), reduce tx start delay
    {0x00a000b0, 0x6a281c2c},

    {0x00a09140, 0x000c000c},
    {0x00a09144, 0x00000000},
    {0x00a09148, 0x00000000},
    {0x00a0914c, 0x00000000},

    {0x00a0d0a4, 0x00400013},
    {0x00a0b224, 0x33000120},

    {0x00a0e800, 0x00000010},
    {0x00a0e808, 0x11100001},
    {0x00a0e488, 0x000a0010},

    /* 20210118 fix control frame rate */
    {0x00a00358, 0x33000000},
    {0x00a0035c, 0x33000000},
    {0x00a00360, 0x33020202},
    {0x00a00364, 0x33020202},
    {0x00a00368, 0x00040404},
    {0x00a0036c, 0x00040404},
    {0x00a00370, 0x00060606},
    {0x00a00374, 0x00060606},
    {0x00a00378, 0x00060606},
    {0x00a0037c, 0x00080808},
    {0x00a00380, 0x00080808},
    {0x00a00384, 0x00080808},
    {0x00a00388, 0x00040404},
    {0x00a0038c, 0x00040404},
    {0x00a00390, 0x00040404},
    {0x00a00394, 0x00060606},
    {0x00a00398, 0x00060606},
    {0x00a0039c, 0x00060606},
    {0x00a003a0, 0x00080808},
    {0x00a003a4, 0x00080808},
    {0x00a003a8, 0x00040404},
    {0x00a003ac, 0x00040404},
    {0x00a003b0, 0x00040404},
    {0x00a003b4, 0x00060606},
    {0x00a003b8, 0x00060606},
    {0x00a003bc, 0x00060606},
    {0x00a003c0, 0x00080808},
    {0x00a003c4, 0x00080808},
    {0x00a003c8, 0x00080808},
    {0x00a003cc, 0x00080808},
};

void bat_set_reg(unsigned int reg_cfg[][2], int reg_cfg_len)
{
	int i  = 0;
	for(i = 0; i < reg_cfg_len; i++) {
	new_set_reg(reg_cfg[i][0], reg_cfg[i][1]);
	}
}

void t9026_rx_cw_remove(unsigned char channel)
{
    int lo_freq = 0;
    int freq1 = 0;
    int freq2 = 0;
    int fs = 0;
    U32 tmp1 = 0;
    U32 tmp2 = 0;
    RG_RECV_A19_FIELD_T rg_recv_a19;
    RG_RECV_A20_FIELD_T rg_recv_a20;
    RG_RECV_A17_FIELD_T rg_recv_a17;
    RG_RECV_A16_FIELD_T rg_recv_a16;

    if (channel > 14)
    {
         lo_freq = 5000 + channel * 5;
    }
    else
    {
         lo_freq = 2407 + channel * 5;
    }

    freq1 = lo_freq % 40; //(lo_freq % 40) * (-1);
    freq2 = 40 - freq1; //freq1 + 40;
    fs = 80;

    tmp1 = BIT(25) - 1 - (U32)(freq1 * 2 * BIT(24)) / fs;
    rg_recv_a19.data = fi_ahb_read(RG_RECV_A19);
    rg_recv_a19.b.rg_cw_remove_sub_angle_base3 = tmp1;
    fi_ahb_write(RG_RECV_A19, rg_recv_a19.data);

    tmp2 = (U32)(freq2 * 2 * BIT(24)) / fs;
    rg_recv_a20.data = fi_ahb_read(RG_RECV_A20);
    rg_recv_a20.b.rg_cw_remove_sub_angle_base4 = tmp2;
    fi_ahb_write(RG_RECV_A20, rg_recv_a20.data);

    fi_ahb_write(0x00a0e834, 0x00000062);  //dc thresh
    fi_ahb_write(0x00a0e83c, 0x00034034);  //cw thresh
    fi_ahb_write(0x00a0e854, 0x000f00ff);  //HDMI bw & DC bw
    fi_ahb_write(0x00a0e858, 0x0000000f);  //cw bw

    if (channel == 151 || channel == 153 || channel == 155 || channel == 161)
    {
        /* fi_ahb_write(0x00a0e840, 0xc1000000); */
        rg_recv_a16.data = fi_ahb_read(RG_RECV_A16);
        rg_recv_a16.b.rg_cw_remove_bypass_man = 0x1;
        rg_recv_a16.b.rg_cw_remove_bypass_val = 0x1;
        rg_recv_a16.b.rg_cw_bypass = 0x0;
        rg_recv_a16.b.rg_hdmi_bypass = 0x1;
        rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode4 = 0x0;
        rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode3 = 0x0;
        rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode2 = 0x0;
        rg_recv_a16.b.rg_cw_remove_sub_man_bypass2 = 0x0;
        rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode1 = 0x0;
        rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode0 = 0x0;
        fi_ahb_write(RG_RECV_A16, rg_recv_a16.data);

        /* fi_ahb_write(0x00a0e840, 0x81022002); //dc open, cw open,hdmi bypass */
        rg_recv_a16.data = fi_ahb_read(RG_RECV_A16);
        rg_recv_a16.b.rg_cw_remove_bypass_man = 0x1;
        rg_recv_a16.b.rg_cw_remove_bypass_val = 0x0;
        rg_recv_a16.b.rg_hdmi_bypass = 0x1;
        rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode4 = 0x1;
        rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode3 = 0x1;
        rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode0 = 0x1;
        fi_ahb_write(RG_RECV_A16, rg_recv_a16.data);
    }
    else
    {
        /* fi_ahb_write(0x00a0e840, 0xc1000000); */
        rg_recv_a16.data = fi_ahb_read(RG_RECV_A16);
        rg_recv_a16.b.rg_cw_remove_bypass_man = 0x1;
        rg_recv_a16.b.rg_cw_remove_bypass_val = 0x1;
        rg_recv_a16.b.rg_cw_bypass = 0x0;
        rg_recv_a16.b.rg_hdmi_bypass = 0x1;
        rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode4 = 0x0;
        rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode3 = 0x0;
        rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode2 = 0x0;
        rg_recv_a16.b.rg_cw_remove_sub_man_bypass2 = 0x0;
        rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode1 = 0x0;
        rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode0 = 0x0;
        fi_ahb_write(RG_RECV_A16, rg_recv_a16.data);

        /* fi_ahb_write(0x00a0e840, 0x91000002); //dc open, cw close,hdmi bypass */
        rg_recv_a16.data = fi_ahb_read(RG_RECV_A16);
        rg_recv_a16.b.rg_cw_remove_bypass_man = 0x1;
        rg_recv_a16.b.rg_cw_remove_bypass_val = 0x0;
        rg_recv_a16.b.rg_cw_bypass = 0x1;
        rg_recv_a16.b.rg_hdmi_bypass = 0x1;
        rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode4 = 0x0;
        rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode3 = 0x0;
        rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode0 = 0x1;
        fi_ahb_write(RG_RECV_A16, rg_recv_a16.data);
    }

    if (channel <= 14)
    {
        if (((lo_freq * 10) > (24478 - 200)) && ((lo_freq * 10) < (24478 + 200)))
        {
            freq1 = 24478 - (lo_freq * 10);
            if (freq1 >= 0)
            {
                tmp1 = (U32)(freq1 * 1 * BIT(24)) / (fs * 5);
            }
            else
            {
                freq1 = 0 - freq1;
                tmp1 = BIT(25) - 1 - (U32)(freq1 * 1 * BIT(24)) / (fs * 5);
            }

            rg_recv_a17.data = fi_ahb_read(RG_RECV_A17);
            rg_recv_a17.b.rg_cw_remove_sub_angle_base1 = tmp1;
            fi_ahb_write(RG_RECV_A17, rg_recv_a17.data);

            /* fi_ahb_write(0x00a0e840, 0xc0022322); */
            rg_recv_a16.data = fi_ahb_read(RG_RECV_A16);
            rg_recv_a16.b.rg_cw_remove_bypass_man = 0x1;
            rg_recv_a16.b.rg_cw_remove_bypass_val = 0x1;
            rg_recv_a16.b.rg_cw_bypass = 0x0;
            rg_recv_a16.b.rg_hdmi_bypass = 0x0;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode4 = 0x1;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode3 = 0x1;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode2 = 0x1;
            rg_recv_a16.b.rg_cw_remove_sub_man_bypass2 = 0x1;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode1 = 0x1;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode0 = 0x1;
            fi_ahb_write(RG_RECV_A16, rg_recv_a16.data);

            /* fi_ahb_write(0x00a0e840, 0x80022322); //dc open, cw open,hdmi open */
            rg_recv_a16.data = fi_ahb_read(RG_RECV_A16);
            rg_recv_a16.b.rg_cw_remove_bypass_man = 0x1;
            rg_recv_a16.b.rg_cw_remove_bypass_val = 0x0;
            rg_recv_a16.b.rg_cw_bypass = 0x0;
            rg_recv_a16.b.rg_hdmi_bypass = 0x0;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode4 = 0x1;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode3 = 0x1;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode2 = 0x1;
            rg_recv_a16.b.rg_cw_remove_sub_man_bypass2 = 0x1;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode1 = 0x1;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode0 = 0x1;
            fi_ahb_write(RG_RECV_A16, rg_recv_a16.data);
        }
        else
        {
            /* fi_ahb_write(0x00a0e840, 0xc1000000); */
            rg_recv_a16.data = fi_ahb_read(RG_RECV_A16);
            rg_recv_a16.b.rg_cw_remove_bypass_man = 0x1;
            rg_recv_a16.b.rg_cw_remove_bypass_val = 0x1;
            rg_recv_a16.b.rg_cw_bypass = 0x0;
            rg_recv_a16.b.rg_hdmi_bypass = 0x1;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode4 = 0x0;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode3 = 0x0;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode2 = 0x0;
            rg_recv_a16.b.rg_cw_remove_sub_man_bypass2 = 0x0;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode1 = 0x0;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode0 = 0x0;
            fi_ahb_write(RG_RECV_A16, rg_recv_a16.data);

            /* fi_ahb_write(0x00a0e840, 0x81022002); //dc open, cw open,hdmi bypass */
            rg_recv_a16.data = fi_ahb_read(RG_RECV_A16);
            rg_recv_a16.b.rg_cw_remove_bypass_man = 0x1;
            rg_recv_a16.b.rg_cw_remove_bypass_val = 0x0;
            rg_recv_a16.b.rg_hdmi_bypass = 0x1;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode4 = 0x1;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode3 = 0x1;
            rg_recv_a16.b.rg_cw_remove_sub_bypass_man_mode0 = 0x1;
            fi_ahb_write(RG_RECV_A16, rg_recv_a16.data);
        }
    }
}

void t9026_init_0(void)
{
    bat_set_reg(agc_9026_tbl, sizeof(agc_9026_tbl)/sizeof(agc_9026_tbl[0]));
    t9026_update_reg();
    t9026_xosc_update();
}

void t9026_rx_rc_1(void)
{
    WF_MODE_T wf_mode ;
    U8 osc_freq = 0x3;
    SX_CALI_MODE_T  sx_cali_mode;
    struct wf5g_rxrc_result wf5g_rxrc_result;
    unsigned channel = 0x5a;

    sx_cali_mode = CHANNEL_MAP|PCODE_CALI|TCODE_CALI|MXR_TANK;
    wf_mode = WF5G_SX; // enter SX Mode
    t9026_rf_mode_access(wf_mode);
    t9026_wf5g_sx(channel, osc_freq, sx_cali_mode);

    wf_mode = WF5G_RX; // enter RX Mode
    t9026_rf_mode_access(wf_mode);

    t9026_wf5g_rxrc(&wf5g_rxrc_result);
    RF_DPRINTF(RF_DEBUG_RESULT, "-------------------------->RC OUT 0x%x\n", wf5g_rxrc_result.cal_rx_rc.rxrc_out);
    RF_DPRINTF(RF_DEBUG_INFO, "RX RC calib finish \n");
}

#if 0
// for print all result after calbration
void t9026_wf5g_print_all_calb(struct t9026_wf5g_calb_rst_out* wf5g_calb_rst_out)
{
    unsigned char i, j, k;

    printk("RC OUT 0x%x\n", wf5g_calb_rst_out->rx_rc.rxrc_out);

    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < 6; j++)
        {
            printk("WF5G rxdc[%d][%d]: rxdc_i=%d,\t rxdc_q=%d,\t rxdc_codei=0x%x,\t rxdc_codeq=0x%x\n",
                i, j, wf5g_calb_rst_out->rx_dc[i][j].rxdc_i, wf5g_calb_rst_out->rx_dc[i][j].rxdc_q,
                wf5g_calb_rst_out->rx_dc[i][j].rxdc_codei, wf5g_calb_rst_out->rx_dc[i][j].rxdc_codeq);
        }
    }

    for(i = 0; i < 5; i++)
    {
        for(j = 0; j < 3; j++)
        {
            printk("WF5G tx_dc_irr[%d][%d]:\t tx_dc_i = 0x%x,\t tx_dc_q = 0x%x,\t tx_alpha = 0x%x,\t tx_theta = 0x%x\n",
                i, j, wf5g_calb_rst_out->tx_dc_irr[i][j].tx_dc_i, wf5g_calb_rst_out->tx_dc_irr[i][j].tx_dc_q,
                wf5g_calb_rst_out->tx_dc_irr[i][j].tx_alpha, wf5g_calb_rst_out->tx_dc_irr[i][j].tx_theta);
        }
    }

#if 0
    for(i = 0; i < 5; i++)
    {
        for(j = 0; j < 3; j++)
        {
            printk("WF5G tx_dpd[%d][%d]:\t snr = %d\n", i, j, wf5g_calb_rst_out->tx_dpd[i][j].snr);
            for(int k = 0; k < 15; k++)
            {
                printk("WF5G tx_dpd[%d][%d]:\t tx_dpd_coeff_i[%d] = 0x%x,\t tx_dpd_coeff_q[%d] = 0x%x\n",
                    i, j, k, wf5g_calb_rst_out->tx_dpd[i][j].tx_dpd_coeff_i[k], k, wf5g_calb_rst_out->tx_dpd[i][j].tx_dpd_coeff_q[k]);
            }
        }
    }
#endif

    for(i = 0; i < 10; i++)
    {
        printk("WF5G rx_lna[%d]: rx_lna_code = 0x%x\n", i, wf5g_calb_rst_out->rx_lna[i].rx_lna_code);
    }

    for(i = 0; i < 5; i++)
    {
        for(j = 0; j < 3; j++)
        {
            for(k = 0; k < 4; k++)
            {
                printk("WF5G rx_irr[%d][%d]:\t coeff_eup[%d] = %x,\t coeff_pup[%d]=%x,\t coeff_elow[%d]=%x,\t coeff_plow[%d]=%x\n",
                    i, j, k, wf5g_calb_rst_out->rx_irr[i][j].coeff_eup[k], k, wf5g_calb_rst_out->rx_irr[i][j].coeff_pup[k],	k,
                    wf5g_calb_rst_out->rx_irr[i][j].coeff_elow[k], k, wf5g_calb_rst_out->rx_irr[i][j].coeff_plow[k]);
            }
        }
    }
}

void t9026_wf2g_print_all_calb(struct t9026_wf2g_calb_rst_out* wf2g_calb_rst_out)
{
    unsigned char i, j;

    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < 6; j++)
        {
            printk("WF2G rxdc[%d][%d]: rxdc_i=%d,\t rxdc_q=%d,\t rxdc_codei=0x%x,\t rxdc_codeq=0x%x\n",
                i, j, wf2g_calb_rst_out->rx_dc[i][j].rxdc_i, wf2g_calb_rst_out->rx_dc[i][j].rxdc_q,
                wf2g_calb_rst_out->rx_dc[i][j].rxdc_codei, wf2g_calb_rst_out->rx_dc[i][j].rxdc_codeq);
        }
    }

    for(i = 0; i < 3; i++)
    {
        printk("tx_dc_irr[%d]:\t tx_dc_i = 0x%x,\t tx_dc_q = 0x%x,\t tx_alpha = 0x%x,\t tx_theta = 0x%x\n",
            i, wf2g_calb_rst_out->tx_dc_irr[i].tx_dc_i, wf2g_calb_rst_out->tx_dc_irr[i].tx_dc_q,
            wf2g_calb_rst_out->tx_dc_irr[i].tx_alpha, wf2g_calb_rst_out->tx_dc_irr[i].tx_theta);
    }

#if 0
    for(i = 0; i < 3; i++)
    {
        printk("WF2G tx_dpd[%d]:\t snr = %d\n", i, wf2g_calb_rst_out->tx_dpd[i].snr);
        for(int j = 0; j < 15; j++)
        {
            printk("WF2G tx_dpd[%d]:\t tx_dpd_coeff_i[%d] = 0x%x,\t tx_dpd_coeff_q[%d] = 0x%x\n",
                i, j, wf2g_calb_rst_out->tx_dpd[i].tx_dpd_coeff_i[j], j, wf2g_calb_rst_out->tx_dpd[i].tx_dpd_coeff_q[j]);
        }
    }
#endif

    printk("WF2G rx_lna: rx_lna_code = 0x%x\n", wf2g_calb_rst_out->rx_lna.rx_lna_code);

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 4; j++)
        {
            printk("WF2G rx_irr[%d]:\t coeff_eup[%d] = %x,\t coeff_pup[%d]=%x,\t coeff_elow[%d]=%x,\t coeff_plow[%d]=%x\n",
                i, j, wf2g_calb_rst_out->rx_irr[i].coeff_eup[j], j, wf2g_calb_rst_out->rx_irr[i].coeff_pup[j], j,
                wf2g_calb_rst_out->rx_irr[i].coeff_elow[j], j, wf2g_calb_rst_out->rx_irr[i].coeff_plow[j]);
        }
    }
}
#endif

void t9026_top()
{
    t9026_init_0();

    t9026_wf5g_cal_for_scan(0x64, &wf5g_calb_result, 0);
    t9026_wf2g_cal_for_scan(0x08, &wf2g_calb_result, 0);

    t9026_cali_end();
    // enter sleep Mode + auto mode
    t9026_rf_mode_auto();

    //enable agc
    i2c_set_reg_fragment(0x00a08010, 28, 28, 0);
}

void t9026_top_pt(U8 channel, U8 bw_idx)
{
    unsigned int calibration_channel = 0;
    U32 temp_data = 0;
    U32 temp = 0;

    //disable agc
    i2c_set_reg_fragment(0x00a08010, 28, 28, 1);

    temp_data = fi_ahb_read(0x00a04940);
    temp = (temp_data >> 4) & 0xfff;

    if (temp >= 0x26c)
    {
        /* change tssi offset to decrease tx power */
        g_temp_rise_flag = 1;
        g_temp_fall_flag = 0;
    }
    else if ((temp >= 0x1da) && (temp <= 0x25d))
    {
        /* restore tssi offset to restore tx power */
        g_temp_rise_flag = 0;
        g_temp_fall_flag = 0;
    }
    else if (temp <= 0x1ce)
    {
        g_temp_rise_flag = 0;
        g_temp_fall_flag = 1;
    }


    if(channel <= 14)
    {
        t9026_wf2g_cal_for_link(0x8, bw_idx, &wf2g_calb_result, 0);
    }else
    {
            if ((channel >= 36) && (channel <= 70)) {
                calibration_channel = 50;

            } else if ((channel > 70) && (channel <= 110)) {
                calibration_channel = 90;

            } else if ((channel > 110) && (channel <= 150)) {
                calibration_channel = 130;

            } else if ((channel > 150) && (channel <= 180)) {
                calibration_channel = 155;
            }
            t9026_wf5g_cal_for_linkA(calibration_channel, bw_idx, &wf5g_calb_result, 0);
    }
    //enable agc
    i2c_set_reg_fragment(0x00a08010, 28, 28, 0);
}

void rf_calibration_before_connect(unsigned char channel, unsigned char bw_idx, unsigned char vid)
{
    //printk("%s, p_chn:%d, bw:%d, vid:%d\n", __func__, channel, bw_idx, vid);
    unsigned char calibration_channel = 0x8;

    //disable agc
    i2c_set_reg_fragment(0x00a08010, 28, 28, 1);

    if(channel <= 14) {
        if (!rf_2g_calibration) {
            t9026_wf2g_cal_for_link(calibration_channel, bw_idx, &wf2g_calb_result, 0);
            rf_2g_calibration = 1;
        }

    } else {
        if ((channel >= 36) && (channel <= 70)) {
            calibration_channel = 50;

        } else if ((channel > 70) && (channel <= 110)) {
            calibration_channel = 90;

        } else if ((channel > 110) && (channel <= 150)) {
            calibration_channel = 130;

        } else if ((channel > 150) && (channel <= 180)) {
            calibration_channel = 155;
        }

        if (vid == 0) {
            t9026_wf5g_cal_for_linkA(calibration_channel, bw_idx, &wf5g_calb_result, 0);
            rf_5g_a = channel;

        } else {
            t9026_wf5g_cal_for_linkB(calibration_channel, bw_idx, &wf5g_calb_result, 0);
        }
    }

    //enable agc
    i2c_set_reg_fragment(0x00a08010, 28, 28, 0);
}

void change_rf_5g_param_when_switch_channel(struct t9026_wf5g_calb_rst_out* wf5g_calb_result, U8 bw_idx, unsigned char channel_idx, unsigned char txirr_manual)
{
    unsigned int tmp;
    unsigned int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    unsigned int tmp8, tmp9, tmpa, tmpb, tmpc, tmpd, tmpe, tmpf;
    int gain_ratio = 0;
    U8 dacgain_idx = 0;
    U8 bw_offset = 0;
    U8 mxr_offset = 0;
    RG_XMIT_A45_FIELD_T    rg_xmit_a45;
    RG_XMIT_A46_FIELD_T    rg_xmit_a46;
    RG_XMIT_A6_FIELD_T    rg_xmit_a6;
    RG_XMIT_A7_FIELD_T    rg_xmit_a7;
    RG_XMIT_A8_FIELD_T    rg_xmit_a8;
    RG_XMIT_A9_FIELD_T    rg_xmit_a9;
    RG_XMIT_A10_FIELD_T    rg_xmit_a10;
    RG_XMIT_A11_FIELD_T    rg_xmit_a11;
    RG_XMIT_A12_FIELD_T    rg_xmit_a12;
    RG_XMIT_A13_FIELD_T    rg_xmit_a13;
    RG_XMIT_A18_FIELD_T    rg_xmit_a18;
    RG_XMIT_A19_FIELD_T    rg_xmit_a19;
    RG_XMIT_A20_FIELD_T    rg_xmit_a20;
    RG_XMIT_A21_FIELD_T    rg_xmit_a21;
    RG_XMIT_A22_FIELD_T    rg_xmit_a22;
    RG_XMIT_A23_FIELD_T    rg_xmit_a23;
    RG_XMIT_A24_FIELD_T    rg_xmit_a24;
    RG_XMIT_A25_FIELD_T    rg_xmit_a25;
    RG_XMIT_A26_FIELD_T    rg_xmit_a26;
    RG_XMIT_A27_FIELD_T    rg_xmit_a27;
    RG_XMIT_A28_FIELD_T    rg_xmit_a28;
    RG_XMIT_A29_FIELD_T    rg_xmit_a29;
    RG_XMIT_A30_FIELD_T    rg_xmit_a30;
    RG_XMIT_A31_FIELD_T    rg_xmit_a31;
    RG_XMIT_A32_FIELD_T    rg_xmit_a32;
    RG_XMIT_A33_FIELD_T    rg_xmit_a33;
    RG_XMIT_A47_FIELD_T    rg_xmit_a47;
    RG_XMIT_A48_FIELD_T    rg_xmit_a48;
    RG_XMIT_A49_FIELD_T    rg_xmit_a49;
    RG_XMIT_A50_FIELD_T    rg_xmit_a50;
    RG_XMIT_A51_FIELD_T    rg_xmit_a51;
    RG_XMIT_A52_FIELD_T    rg_xmit_a52;
    RG_XMIT_A53_FIELD_T    rg_xmit_a53;
    RG_XMIT_A54_FIELD_T    rg_xmit_a54;

#ifdef CONFIG_DPD_ENABLE
    RG_XMIT_A56_FIELD_T    rg_xmit_a56;
    RG_XMIT_A57_FIELD_T    rg_xmit_a57;
    RG_XMIT_A58_FIELD_T    rg_xmit_a58;
    RG_XMIT_A59_FIELD_T    rg_xmit_a59;
    RG_XMIT_A60_FIELD_T    rg_xmit_a60;
    RG_XMIT_A61_FIELD_T    rg_xmit_a61;
    RG_XMIT_A62_FIELD_T    rg_xmit_a62;
    RG_XMIT_A63_FIELD_T    rg_xmit_a63;
    RG_XMIT_A64_FIELD_T    rg_xmit_a64;
    RG_XMIT_A65_FIELD_T    rg_xmit_a65;
    RG_XMIT_A66_FIELD_T    rg_xmit_a66;
    RG_XMIT_A67_FIELD_T    rg_xmit_a67;
    RG_XMIT_A68_FIELD_T    rg_xmit_a68;
    RG_XMIT_A69_FIELD_T    rg_xmit_a69;
    RG_XMIT_A70_FIELD_T    rg_xmit_a70;
    RG_XMIT_A71_FIELD_T    rg_xmit_a71;
#endif
    RG_RECV_A11_FIELD_T    rg_recv_a11;
    RG_RECV_A3_FIELD_T    rg_recv_a3;
    RG_RECV_A4_FIELD_T    rg_recv_a4;
    RG_RECV_A5_FIELD_T    rg_recv_a5;
    RG_RECV_A6_FIELD_T    rg_recv_a6;
    RG_RECV_A7_FIELD_T    rg_recv_a7;
    RG_RECV_A8_FIELD_T    rg_recv_a8;
    RG_RECV_A9_FIELD_T    rg_recv_a9;
    RG_RECV_A10_FIELD_T    rg_recv_a10;
    RG_RX_A58_FIELD_T    rg_rx_a58;
    RG_RX_A59_FIELD_T    rg_rx_a59;

    if (bw_idx == 0)
    {
       dacgain_idx = 0;
       bw_offset = 0x01;
    }
    else if (bw_idx == 1)
    {
       dacgain_idx = 0;
       bw_offset = 0x01;
    }
    else if (bw_idx == 2)
    {
       dacgain_idx = 1;
       bw_offset = 0x0;
    }

    if (g_temp_fall_flag == 1)
    {
         mxr_offset = 0x10;
    }
    else
    {
         mxr_offset = 0x00;
    }
    gain_ratio = wf5g_calb_result->tx_tssi[channel_idx].gain_ratio;

    rg_rx_a58.data = rf_read_register(RG_RX_A58);
    rg_rx_a58.b.RG_WF_RX_LPF_CTUNE_ISEL_MAN_MODE = 0x0;    // WF5G BW fixed RC code auto mode
    rf_write_register(RG_RX_A58, rg_rx_a58.data);

    rg_rx_a59.data = rf_read_register(RG_RX_A59);
    rg_rx_a59.b.RG_WF_RX_LPF_CTUNE_QSEL_MAN_MODE = 0x0;    // WF5G BW fixed RC code auto mode
    rf_write_register(RG_RX_A59, rg_rx_a59.data);

    fi_ahb_write(0x0000816c, 0x02296311);

    if (txirr_manual)
    {
        tmp = (0x80<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;

        rg_xmit_a45.data = fi_ahb_read(RG_XMIT_A45);
        rg_xmit_a45.b.rg_man_tx_pwrlvl = tmp;
        rg_xmit_a45.b.rg_tx_pwr_sel_man_en = 0x1;
        fi_ahb_write(RG_XMIT_A45, rg_xmit_a45.data);

        rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
        rg_xmit_a46.b.rg_man_txirr_comp_theta = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        rg_xmit_a46.b.rg_man_txirr_comp_alpha = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);

    }
    else
    {
        rg_xmit_a6.data = fi_ahb_read(RG_XMIT_A6);
        rg_xmit_a6.b.rg_txirr_comp_theta0 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        rg_xmit_a6.b.rg_txirr_comp_theta1 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        rg_xmit_a6.b.rg_txirr_comp_theta2 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        rg_xmit_a6.b.rg_txirr_comp_theta3 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        fi_ahb_write(RG_XMIT_A6, rg_xmit_a6.data);

        rg_xmit_a7.data = fi_ahb_read(RG_XMIT_A7);
        rg_xmit_a7.b.rg_txirr_comp_theta4 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        rg_xmit_a7.b.rg_txirr_comp_theta5 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        rg_xmit_a7.b.rg_txirr_comp_theta6 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        rg_xmit_a7.b.rg_txirr_comp_theta7 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        fi_ahb_write(RG_XMIT_A7, rg_xmit_a7.data);

        rg_xmit_a8.data = fi_ahb_read(RG_XMIT_A8);
        rg_xmit_a8.b.rg_txirr_comp_theta8 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        rg_xmit_a8.b.rg_txirr_comp_theta9 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        rg_xmit_a8.b.rg_txirr_comp_thetaa = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        rg_xmit_a8.b.rg_txirr_comp_thetab = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        fi_ahb_write(RG_XMIT_A8, rg_xmit_a8.data);

        rg_xmit_a9.data = fi_ahb_read(RG_XMIT_A9);
        rg_xmit_a9.b.rg_txirr_comp_thetac = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        rg_xmit_a9.b.rg_txirr_comp_thetad = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        rg_xmit_a9.b.rg_txirr_comp_thetae = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        rg_xmit_a9.b.rg_txirr_comp_thetaf = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_theta;
        fi_ahb_write(RG_XMIT_A9, rg_xmit_a9.data);

        rg_xmit_a10.data = fi_ahb_read(RG_XMIT_A10);
        rg_xmit_a10.b.rg_txirr_comp_alpha0 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        rg_xmit_a10.b.rg_txirr_comp_alpha1 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        rg_xmit_a10.b.rg_txirr_comp_alpha2 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        rg_xmit_a10.b.rg_txirr_comp_alpha3 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        fi_ahb_write(RG_XMIT_A10, rg_xmit_a10.data);

        rg_xmit_a11.data = fi_ahb_read(RG_XMIT_A11);
        rg_xmit_a11.b.rg_txirr_comp_alpha4 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        rg_xmit_a11.b.rg_txirr_comp_alpha5 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        rg_xmit_a11.b.rg_txirr_comp_alpha6 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        rg_xmit_a11.b.rg_txirr_comp_alpha7 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        fi_ahb_write(RG_XMIT_A11, rg_xmit_a11.data);

        rg_xmit_a12.data = fi_ahb_read(RG_XMIT_A12);
        rg_xmit_a12.b.rg_txirr_comp_alpha8 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        rg_xmit_a12.b.rg_txirr_comp_alpha9 = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        rg_xmit_a12.b.rg_txirr_comp_alphaa = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        rg_xmit_a12.b.rg_txirr_comp_alphab = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        fi_ahb_write(RG_XMIT_A12, rg_xmit_a12.data);

        rg_xmit_a13.data = fi_ahb_read(RG_XMIT_A13);
        rg_xmit_a13.b.rg_txirr_comp_alphac = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        rg_xmit_a13.b.rg_txirr_comp_alphad = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        rg_xmit_a13.b.rg_txirr_comp_alphae = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        rg_xmit_a13.b.rg_txirr_comp_alphaf = wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_alpha;
        fi_ahb_write(RG_XMIT_A13, rg_xmit_a13.data);

        tmp0 = ((0x84 * gain_ratio/100)<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;
        tmp1 = ((0x80 * gain_ratio/100)<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;
        tmp2 = ((0x7C * gain_ratio/100)<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;
        tmp3 = ((0x79 * gain_ratio/100)<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;
        tmp4 = ((0x75 * gain_ratio/100)<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;
        tmp5 = ((0x72 * gain_ratio/100)<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;
        tmp6 = ((0x6F * gain_ratio/100)<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;
        tmp7 = ((0x6C * gain_ratio/100)<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;
        tmp8 = ((0x69 * gain_ratio/100)<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;
        tmp9 = ((0x66 * gain_ratio/100)<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;
        tmpa = ((0x63 * gain_ratio/100)<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;
        tmpb = ((0x60 * gain_ratio/100)<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;
        tmpc = ((0x5D * gain_ratio/100)<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;
        tmpd = ((0x5B * gain_ratio/100)<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;
        tmpe = ((0x58 * gain_ratio/100)<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;
        tmpf = ((0x56 * gain_ratio/100)<<16) | (wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_q << 8) | wf5g_calb_result->tx_dc_irr[channel_idx][dacgain_idx].tx_dc_i;

        rg_xmit_a18.data = fi_ahb_read(RG_XMIT_A18);
        rg_xmit_a18.b.rg_tx_pwrlvl0 = tmp0;
        fi_ahb_write(RG_XMIT_A18, rg_xmit_a18.data);

        rg_xmit_a19.data = fi_ahb_read(RG_XMIT_A19);
        rg_xmit_a19.b.rg_tx_pwrlvl1 = tmp1;
        fi_ahb_write(RG_XMIT_A19, rg_xmit_a19.data);

        rg_xmit_a20.data = fi_ahb_read(RG_XMIT_A20);
        rg_xmit_a20.b.rg_tx_pwrlvl2 = tmp2;
        fi_ahb_write(RG_XMIT_A20, rg_xmit_a20.data);

        rg_xmit_a21.data = fi_ahb_read(RG_XMIT_A21);
        rg_xmit_a21.b.rg_tx_pwrlvl3 = tmp3;
        fi_ahb_write(RG_XMIT_A21, rg_xmit_a21.data);

        rg_xmit_a22.data = fi_ahb_read(RG_XMIT_A22);
        rg_xmit_a22.b.rg_tx_pwrlvl4 = tmp4;
        fi_ahb_write(RG_XMIT_A22, rg_xmit_a22.data);

        rg_xmit_a23.data = fi_ahb_read(RG_XMIT_A23);
        rg_xmit_a23.b.rg_tx_pwrlvl5 = tmp5;
        fi_ahb_write(RG_XMIT_A23, rg_xmit_a23.data);

        rg_xmit_a24.data = fi_ahb_read(RG_XMIT_A24);
        rg_xmit_a24.b.rg_tx_pwrlvl6 = tmp6;
        fi_ahb_write(RG_XMIT_A24, rg_xmit_a24.data);

#if 0
        if (bw_idx == 0)
        {
            rg_xmit_a25.data = fi_ahb_read(RG_XMIT_A25);
            rg_xmit_a25.b.rg_tx_pwrlvl7 = tmp7;
            fi_ahb_write(RG_XMIT_A25, rg_xmit_a25.data);

            rg_xmit_a26.data = fi_ahb_read(RG_XMIT_A26);
            rg_xmit_a26.b.rg_tx_pwrlvl8 = tmp6;
            fi_ahb_write(RG_XMIT_A26, rg_xmit_a26.data);

            rg_xmit_a27.data = fi_ahb_read(RG_XMIT_A27);
            rg_xmit_a27.b.rg_tx_pwrlvl9 = tmp9;
            fi_ahb_write(RG_XMIT_A27, rg_xmit_a27.data);

            rg_xmit_a28.data = fi_ahb_read(RG_XMIT_A28);
            rg_xmit_a28.b.rg_tx_pwrlvla = tmp6;
            fi_ahb_write(RG_XMIT_A28, rg_xmit_a28.data);

            rg_xmit_a29.data = fi_ahb_read(RG_XMIT_A29);
            rg_xmit_a29.b.rg_tx_pwrlvlb = tmpa;
            fi_ahb_write(RG_XMIT_A29, rg_xmit_a29.data);

            rg_xmit_a30.data = fi_ahb_read(RG_XMIT_A30);
            rg_xmit_a30.b.rg_tx_pwrlvlc = tmpd;
            fi_ahb_write(RG_XMIT_A30, rg_xmit_a30.data);

            rg_xmit_a31.data = fi_ahb_read(RG_XMIT_A31);
            rg_xmit_a31.b.rg_tx_pwrlvld = tmpa;
            fi_ahb_write(RG_XMIT_A31, rg_xmit_a31.data);

            rg_xmit_a32.data = fi_ahb_read(RG_XMIT_A32);
            rg_xmit_a32.b.rg_tx_pwrlvle = tmpe;
            fi_ahb_write(RG_XMIT_A32, rg_xmit_a32.data);

            rg_xmit_a33.data = fi_ahb_read(RG_XMIT_A33);
            rg_xmit_a33.b.rg_tx_pwrlvlf = tmpf;
            fi_ahb_write(RG_XMIT_A33, rg_xmit_a33.data);
        }
        else if (bw_idx == 1)
        {
            rg_xmit_a25.data = fi_ahb_read(RG_XMIT_A25);
            rg_xmit_a25.b.rg_tx_pwrlvl7 = tmp7;
            fi_ahb_write(RG_XMIT_A25, rg_xmit_a25.data);

            rg_xmit_a26.data = fi_ahb_read(RG_XMIT_A26);
            rg_xmit_a26.b.rg_tx_pwrlvl8 = tmp6;
            fi_ahb_write(RG_XMIT_A26, rg_xmit_a26.data);

            rg_xmit_a27.data = fi_ahb_read(RG_XMIT_A27);
            rg_xmit_a27.b.rg_tx_pwrlvl9 = tmp9;
            fi_ahb_write(RG_XMIT_A27, rg_xmit_a27.data);

            rg_xmit_a28.data = fi_ahb_read(RG_XMIT_A28);
            rg_xmit_a28.b.rg_tx_pwrlvla = tmpa;
            fi_ahb_write(RG_XMIT_A28, rg_xmit_a28.data);

            rg_xmit_a29.data = fi_ahb_read(RG_XMIT_A29);
            rg_xmit_a29.b.rg_tx_pwrlvlb = tmpa;
            fi_ahb_write(RG_XMIT_A29, rg_xmit_a29.data);

            rg_xmit_a30.data = fi_ahb_read(RG_XMIT_A30);
            rg_xmit_a30.b.rg_tx_pwrlvlc = tmpd;
            fi_ahb_write(RG_XMIT_A30, rg_xmit_a30.data);

            rg_xmit_a31.data = fi_ahb_read(RG_XMIT_A31);
            rg_xmit_a31.b.rg_tx_pwrlvld = tmpd;
            fi_ahb_write(RG_XMIT_A31, rg_xmit_a31.data);

            rg_xmit_a32.data = fi_ahb_read(RG_XMIT_A32);
            rg_xmit_a32.b.rg_tx_pwrlvle = tmpf;
            fi_ahb_write(RG_XMIT_A32, rg_xmit_a32.data);

            rg_xmit_a33.data = fi_ahb_read(RG_XMIT_A33);
            rg_xmit_a33.b.rg_tx_pwrlvlf = tmpf;
            fi_ahb_write(RG_XMIT_A33, rg_xmit_a33.data);
        }
        else if (bw_idx == 2)
        {
            rg_xmit_a25.data = fi_ahb_read(RG_XMIT_A25);
            rg_xmit_a25.b.rg_tx_pwrlvl7 = tmp7;
            fi_ahb_write(RG_XMIT_A25, rg_xmit_a25.data);

            rg_xmit_a26.data = fi_ahb_read(RG_XMIT_A26);
            rg_xmit_a26.b.rg_tx_pwrlvl8 = tmp8;
            fi_ahb_write(RG_XMIT_A26, rg_xmit_a26.data);

            rg_xmit_a27.data = fi_ahb_read(RG_XMIT_A27);
            rg_xmit_a27.b.rg_tx_pwrlvl9 = tmp9;
            fi_ahb_write(RG_XMIT_A27, rg_xmit_a27.data);

            rg_xmit_a28.data = fi_ahb_read(RG_XMIT_A28);
            rg_xmit_a28.b.rg_tx_pwrlvla = tmpa;
            fi_ahb_write(RG_XMIT_A28, rg_xmit_a28.data);

            rg_xmit_a29.data = fi_ahb_read(RG_XMIT_A29);
            rg_xmit_a29.b.rg_tx_pwrlvlb = tmpb;
            fi_ahb_write(RG_XMIT_A29, rg_xmit_a29.data);

            rg_xmit_a30.data = fi_ahb_read(RG_XMIT_A30);
            rg_xmit_a30.b.rg_tx_pwrlvlc = tmpc;
            fi_ahb_write(RG_XMIT_A30, rg_xmit_a30.data);

            rg_xmit_a31.data = fi_ahb_read(RG_XMIT_A31);
            rg_xmit_a31.b.rg_tx_pwrlvld = tmpd;
            fi_ahb_write(RG_XMIT_A31, rg_xmit_a31.data);

            rg_xmit_a32.data = fi_ahb_read(RG_XMIT_A32);
            rg_xmit_a32.b.rg_tx_pwrlvle = tmpe;
            fi_ahb_write(RG_XMIT_A32, rg_xmit_a32.data);

            rg_xmit_a33.data = fi_ahb_read(RG_XMIT_A33);
            rg_xmit_a33.b.rg_tx_pwrlvlf = tmpf;
            fi_ahb_write(RG_XMIT_A33, rg_xmit_a33.data);
        }
#else
        {
             unsigned int tmp_val[3][9] =
            {
                {tmp0, tmp2, tmp9, tmpa, tmp7, tmp7, tmpa, tmpe, tmpf,},
                {tmp2, tmp4, tmp9, tmpa, tmp7, tmp9, tmpc, tmpe, tmpf,},
                {tmp0, tmp0, tmp9, tmpa, tmp6, tmp9, tmpb, tmpe, tmpf,},
            };

            rg_xmit_a25.data = fi_ahb_read(RG_XMIT_A25);
            rg_xmit_a25.b.rg_tx_pwrlvl7 = tmp_val[bw_idx][0];
            fi_ahb_write(RG_XMIT_A25, rg_xmit_a25.data);

            rg_xmit_a26.data = fi_ahb_read(RG_XMIT_A26);
            rg_xmit_a26.b.rg_tx_pwrlvl8 = tmp_val[bw_idx][1];
            fi_ahb_write(RG_XMIT_A26, rg_xmit_a26.data);

            rg_xmit_a27.data = fi_ahb_read(RG_XMIT_A27);
            rg_xmit_a27.b.rg_tx_pwrlvl9 = tmp_val[bw_idx][2];
            fi_ahb_write(RG_XMIT_A27, rg_xmit_a27.data);

            rg_xmit_a28.data = fi_ahb_read(RG_XMIT_A28);
            rg_xmit_a28.b.rg_tx_pwrlvla = tmp_val[bw_idx][3];
            fi_ahb_write(RG_XMIT_A28, rg_xmit_a28.data);

            rg_xmit_a29.data = fi_ahb_read(RG_XMIT_A29);
            rg_xmit_a29.b.rg_tx_pwrlvlb = tmp_val[bw_idx][4];
            fi_ahb_write(RG_XMIT_A29, rg_xmit_a29.data);

            rg_xmit_a30.data = fi_ahb_read(RG_XMIT_A30);
            rg_xmit_a30.b.rg_tx_pwrlvlc = tmp_val[bw_idx][5];
            fi_ahb_write(RG_XMIT_A30, rg_xmit_a30.data);

            rg_xmit_a31.data = fi_ahb_read(RG_XMIT_A31);
            rg_xmit_a31.b.rg_tx_pwrlvld = tmp_val[bw_idx][6];
            fi_ahb_write(RG_XMIT_A31, rg_xmit_a31.data);

            rg_xmit_a32.data = fi_ahb_read(RG_XMIT_A32);
            rg_xmit_a32.b.rg_tx_pwrlvle = tmp_val[bw_idx][7];
            fi_ahb_write(RG_XMIT_A32, rg_xmit_a32.data);

            rg_xmit_a33.data = fi_ahb_read(RG_XMIT_A33);
            rg_xmit_a33.b.rg_tx_pwrlvlf = tmp_val[bw_idx][8];
            fi_ahb_write(RG_XMIT_A33, rg_xmit_a33.data);
        }
#endif

        rg_xmit_a47.data = fi_ahb_read(RG_XMIT_A47);
        rg_xmit_a47.b.rg_tx_ex_gain0 = 0x03 + mxr_offset;
        rg_xmit_a47.b.rg_tx_ex_gain1 = 0x03 + mxr_offset;
        fi_ahb_write(RG_XMIT_A47, rg_xmit_a47.data);

        rg_xmit_a48.data = fi_ahb_read(RG_XMIT_A48);
        rg_xmit_a48.b.rg_tx_ex_gain2 = 0x03 + mxr_offset;
        rg_xmit_a48.b.rg_tx_ex_gain3 = 0x03 + mxr_offset;
        fi_ahb_write(RG_XMIT_A48, rg_xmit_a48.data);

        rg_xmit_a49.data = fi_ahb_read(RG_XMIT_A49);
        rg_xmit_a49.b.rg_tx_ex_gain4 = 0x03 + mxr_offset;
        rg_xmit_a49.b.rg_tx_ex_gain5 = 0x03 + mxr_offset;
        fi_ahb_write(RG_XMIT_A49, rg_xmit_a49.data);

        rg_xmit_a50.data = fi_ahb_read(RG_XMIT_A50);
        rg_xmit_a50.b.rg_tx_ex_gain6 = 0x03 + mxr_offset;
        rg_xmit_a50.b.rg_tx_ex_gain7 = 0x02 + bw_offset + mxr_offset;
        fi_ahb_write(RG_XMIT_A50, rg_xmit_a50.data);

        rg_xmit_a51.data = fi_ahb_read(RG_XMIT_A51);
        rg_xmit_a51.b.rg_tx_ex_gain8 = 0x02 + bw_offset + mxr_offset;
        rg_xmit_a51.b.rg_tx_ex_gain9 = 0x03 + mxr_offset;
        fi_ahb_write(RG_XMIT_A51, rg_xmit_a51.data);

        rg_xmit_a52.data = fi_ahb_read(RG_XMIT_A52);
        rg_xmit_a52.b.rg_tx_ex_gaina = 0x03 + mxr_offset;
        rg_xmit_a52.b.rg_tx_ex_gainb = 0x02 + bw_offset + mxr_offset;
        fi_ahb_write(RG_XMIT_A52, rg_xmit_a52.data);

        rg_xmit_a53.data = fi_ahb_read(RG_XMIT_A53);
        rg_xmit_a53.b.rg_tx_ex_gainc = 0x02 + bw_offset + mxr_offset;
        rg_xmit_a53.b.rg_tx_ex_gaind = 0x02 + bw_offset + mxr_offset;
        fi_ahb_write(RG_XMIT_A53, rg_xmit_a53.data);

        rg_xmit_a54.data = fi_ahb_read(RG_XMIT_A54);
        rg_xmit_a54.b.rg_tx_ex_gaine = 0x02 + bw_offset + mxr_offset;
        rg_xmit_a54.b.rg_tx_ex_gainf = 0x03 + mxr_offset;
        fi_ahb_write(RG_XMIT_A54, rg_xmit_a54.data);

    }
#ifdef CONFIG_DPD_ENABLE
    if(bw_idx == 0) {
        rg_xmit_a57.data = fi_ahb_read(RG_XMIT_A57);
        rg_xmit_a57.b.rg_txdpd_comp_coef_i_0_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[0];
        rg_xmit_a57.b.rg_txdpd_comp_coef_q_0_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[0];
        fi_ahb_write(RG_XMIT_A57, rg_xmit_a57.data);

        rg_xmit_a58.data = fi_ahb_read(RG_XMIT_A58);
        rg_xmit_a58.b.rg_txdpd_comp_coef_i_1_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[1];
        rg_xmit_a58.b.rg_txdpd_comp_coef_q_1_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[1];
        fi_ahb_write(RG_XMIT_A58, rg_xmit_a58.data);

        rg_xmit_a59.data = fi_ahb_read(RG_XMIT_A59);
        rg_xmit_a59.b.rg_txdpd_comp_coef_i_2_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[2];
        rg_xmit_a59.b.rg_txdpd_comp_coef_q_2_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[2];
        fi_ahb_write(RG_XMIT_A59, rg_xmit_a59.data);

        rg_xmit_a60.data = fi_ahb_read(RG_XMIT_A60);
        rg_xmit_a60.b.rg_txdpd_comp_coef_i_3_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[3];
        rg_xmit_a60.b.rg_txdpd_comp_coef_q_3_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[3];
        fi_ahb_write(RG_XMIT_A60, rg_xmit_a60.data);

        rg_xmit_a61.data = fi_ahb_read(RG_XMIT_A61);
        rg_xmit_a61.b.rg_txdpd_comp_coef_i_4_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[4];
        rg_xmit_a61.b.rg_txdpd_comp_coef_q_4_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[4];
        fi_ahb_write(RG_XMIT_A61, rg_xmit_a61.data);

        rg_xmit_a62.data = fi_ahb_read(RG_XMIT_A62);
        rg_xmit_a62.b.rg_txdpd_comp_coef_i_5_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[5];
        rg_xmit_a62.b.rg_txdpd_comp_coef_q_5_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[5];
        fi_ahb_write(RG_XMIT_A62, rg_xmit_a62.data);

        rg_xmit_a63.data = fi_ahb_read(RG_XMIT_A63);
        rg_xmit_a63.b.rg_txdpd_comp_coef_i_6_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[6];
        rg_xmit_a63.b.rg_txdpd_comp_coef_q_6_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[6];
        fi_ahb_write(RG_XMIT_A63, rg_xmit_a63.data);

        rg_xmit_a64.data = fi_ahb_read(RG_XMIT_A64);
        rg_xmit_a64.b.rg_txdpd_comp_coef_i_7_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[7];
        rg_xmit_a64.b.rg_txdpd_comp_coef_q_7_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[7];
        fi_ahb_write(RG_XMIT_A64, rg_xmit_a64.data);

        rg_xmit_a65.data = fi_ahb_read(RG_XMIT_A65);
        rg_xmit_a65.b.rg_txdpd_comp_coef_i_8_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[8];
        rg_xmit_a65.b.rg_txdpd_comp_coef_q_8_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[8];
        fi_ahb_write(RG_XMIT_A65, rg_xmit_a65.data);

        rg_xmit_a66.data = fi_ahb_read(RG_XMIT_A66);
        rg_xmit_a66.b.rg_txdpd_comp_coef_i_9_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[9];
        rg_xmit_a66.b.rg_txdpd_comp_coef_q_9_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[9];
        fi_ahb_write(RG_XMIT_A66, rg_xmit_a66.data);

        rg_xmit_a67.data = fi_ahb_read(RG_XMIT_A67);
        rg_xmit_a67.b.rg_txdpd_comp_coef_i_a_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[10];
        rg_xmit_a67.b.rg_txdpd_comp_coef_q_a_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[10];
        fi_ahb_write(RG_XMIT_A67, rg_xmit_a67.data);

        rg_xmit_a68.data = fi_ahb_read(RG_XMIT_A68);
        rg_xmit_a68.b.rg_txdpd_comp_coef_i_b_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[11];
        rg_xmit_a68.b.rg_txdpd_comp_coef_q_b_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[11];
        fi_ahb_write(RG_XMIT_A68, rg_xmit_a68.data);

        rg_xmit_a69.data = fi_ahb_read(RG_XMIT_A69);
        rg_xmit_a69.b.rg_txdpd_comp_coef_i_c_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[12];
        rg_xmit_a69.b.rg_txdpd_comp_coef_q_c_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[12];
        fi_ahb_write(RG_XMIT_A69, rg_xmit_a69.data);

        rg_xmit_a70.data = fi_ahb_read(RG_XMIT_A70);
        rg_xmit_a70.b.rg_txdpd_comp_coef_i_d_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[13];
        rg_xmit_a70.b.rg_txdpd_comp_coef_q_d_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[13];
        fi_ahb_write(RG_XMIT_A70, rg_xmit_a70.data);

        rg_xmit_a71.data = fi_ahb_read(RG_XMIT_A71);
        rg_xmit_a71.b.rg_txdpd_comp_coef_i_e_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[14];
        rg_xmit_a71.b.rg_txdpd_comp_coef_q_e_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[14];
        fi_ahb_write(RG_XMIT_A71, rg_xmit_a71.data);

    } else if(bw_idx == 1) {
        rg_xmit_a57.data = fi_ahb_read(RG_XMIT_A57);
        rg_xmit_a57.b.rg_txdpd_comp_coef_i_0_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[0];
        rg_xmit_a57.b.rg_txdpd_comp_coef_q_0_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[0];
        fi_ahb_write(RG_XMIT_A57, rg_xmit_a57.data);

        rg_xmit_a58.data = fi_ahb_read(RG_XMIT_A58);
        rg_xmit_a58.b.rg_txdpd_comp_coef_i_1_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[1];
        rg_xmit_a58.b.rg_txdpd_comp_coef_q_1_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[1];
        fi_ahb_write(RG_XMIT_A58, rg_xmit_a58.data);

        rg_xmit_a59.data = fi_ahb_read(RG_XMIT_A59);
        rg_xmit_a59.b.rg_txdpd_comp_coef_i_2_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[2];
        rg_xmit_a59.b.rg_txdpd_comp_coef_q_2_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[2];
        fi_ahb_write(RG_XMIT_A59, rg_xmit_a59.data);

        rg_xmit_a60.data = fi_ahb_read(RG_XMIT_A60);
        rg_xmit_a60.b.rg_txdpd_comp_coef_i_3_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[3];
        rg_xmit_a60.b.rg_txdpd_comp_coef_q_3_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[3];
        fi_ahb_write(RG_XMIT_A60, rg_xmit_a60.data);

        rg_xmit_a61.data = fi_ahb_read(RG_XMIT_A61);
        rg_xmit_a61.b.rg_txdpd_comp_coef_i_4_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[4];
        rg_xmit_a61.b.rg_txdpd_comp_coef_q_4_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[4];
        fi_ahb_write(RG_XMIT_A61, rg_xmit_a61.data);

        rg_xmit_a62.data = fi_ahb_read(RG_XMIT_A62);
        rg_xmit_a62.b.rg_txdpd_comp_coef_i_5_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[5];
        rg_xmit_a62.b.rg_txdpd_comp_coef_q_5_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[5];
        fi_ahb_write(RG_XMIT_A62, rg_xmit_a62.data);

        rg_xmit_a63.data = fi_ahb_read(RG_XMIT_A63);
        rg_xmit_a63.b.rg_txdpd_comp_coef_i_6_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[6];
        rg_xmit_a63.b.rg_txdpd_comp_coef_q_6_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[6];
        fi_ahb_write(RG_XMIT_A63, rg_xmit_a63.data);

        rg_xmit_a64.data = fi_ahb_read(RG_XMIT_A64);
        rg_xmit_a64.b.rg_txdpd_comp_coef_i_7_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[7];
        rg_xmit_a64.b.rg_txdpd_comp_coef_q_7_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[7];
        fi_ahb_write(RG_XMIT_A64, rg_xmit_a64.data);

        rg_xmit_a65.data = fi_ahb_read(RG_XMIT_A65);
        rg_xmit_a65.b.rg_txdpd_comp_coef_i_8_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[8];
        rg_xmit_a65.b.rg_txdpd_comp_coef_q_8_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[8];
        fi_ahb_write(RG_XMIT_A65, rg_xmit_a65.data);

        rg_xmit_a66.data = fi_ahb_read(RG_XMIT_A66);
        rg_xmit_a66.b.rg_txdpd_comp_coef_i_9_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[9];
        rg_xmit_a66.b.rg_txdpd_comp_coef_q_9_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[9];
        fi_ahb_write(RG_XMIT_A66, rg_xmit_a66.data);

        rg_xmit_a67.data = fi_ahb_read(RG_XMIT_A67);
        rg_xmit_a67.b.rg_txdpd_comp_coef_i_a_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[10];
        rg_xmit_a67.b.rg_txdpd_comp_coef_q_a_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[10];
        fi_ahb_write(RG_XMIT_A67, rg_xmit_a67.data);

        rg_xmit_a68.data = fi_ahb_read(RG_XMIT_A68);
        rg_xmit_a68.b.rg_txdpd_comp_coef_i_b_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[11];
        rg_xmit_a68.b.rg_txdpd_comp_coef_q_b_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[11];
        fi_ahb_write(RG_XMIT_A68, rg_xmit_a68.data);

        rg_xmit_a69.data = fi_ahb_read(RG_XMIT_A69);
        rg_xmit_a69.b.rg_txdpd_comp_coef_i_c_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[12];
        rg_xmit_a69.b.rg_txdpd_comp_coef_q_c_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[12];
        fi_ahb_write(RG_XMIT_A69, rg_xmit_a69.data);

        rg_xmit_a70.data = fi_ahb_read(RG_XMIT_A70);
        rg_xmit_a70.b.rg_txdpd_comp_coef_i_d_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[13];
        rg_xmit_a70.b.rg_txdpd_comp_coef_q_d_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[13];
        fi_ahb_write(RG_XMIT_A70, rg_xmit_a70.data);

        rg_xmit_a71.data = fi_ahb_read(RG_XMIT_A71);
        rg_xmit_a71.b.rg_txdpd_comp_coef_i_e_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[14];
        rg_xmit_a71.b.rg_txdpd_comp_coef_q_e_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[14];
        fi_ahb_write(RG_XMIT_A71, rg_xmit_a71.data);

    } else if (bw_idx == 2) {
        rg_xmit_a57.data = fi_ahb_read(RG_XMIT_A57);
        rg_xmit_a57.b.rg_txdpd_comp_coef_i_0_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[0];
        rg_xmit_a57.b.rg_txdpd_comp_coef_q_0_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[0];
        fi_ahb_write(RG_XMIT_A57, rg_xmit_a57.data);

        rg_xmit_a58.data = fi_ahb_read(RG_XMIT_A58);
        rg_xmit_a58.b.rg_txdpd_comp_coef_i_1_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[1];
        rg_xmit_a58.b.rg_txdpd_comp_coef_q_1_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[1];
        fi_ahb_write(RG_XMIT_A58, rg_xmit_a58.data);

        rg_xmit_a59.data = fi_ahb_read(RG_XMIT_A59);
        rg_xmit_a59.b.rg_txdpd_comp_coef_i_2_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[2];
        rg_xmit_a59.b.rg_txdpd_comp_coef_q_2_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[2];
        fi_ahb_write(RG_XMIT_A59, rg_xmit_a59.data);

        rg_xmit_a60.data = fi_ahb_read(RG_XMIT_A60);
        rg_xmit_a60.b.rg_txdpd_comp_coef_i_3_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[3];
        rg_xmit_a60.b.rg_txdpd_comp_coef_q_3_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[3];
        fi_ahb_write(RG_XMIT_A60, rg_xmit_a60.data);

        rg_xmit_a61.data = fi_ahb_read(RG_XMIT_A61);
        rg_xmit_a61.b.rg_txdpd_comp_coef_i_4_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[4];
        rg_xmit_a61.b.rg_txdpd_comp_coef_q_4_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[4];
        fi_ahb_write(RG_XMIT_A61, rg_xmit_a61.data);

        rg_xmit_a62.data = fi_ahb_read(RG_XMIT_A62);
        rg_xmit_a62.b.rg_txdpd_comp_coef_i_5_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[5];
        rg_xmit_a62.b.rg_txdpd_comp_coef_q_5_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[5];
        fi_ahb_write(RG_XMIT_A62, rg_xmit_a62.data);

        rg_xmit_a63.data = fi_ahb_read(RG_XMIT_A63);
        rg_xmit_a63.b.rg_txdpd_comp_coef_i_6_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[6];
        rg_xmit_a63.b.rg_txdpd_comp_coef_q_6_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[6];
        fi_ahb_write(RG_XMIT_A63, rg_xmit_a63.data);

        rg_xmit_a64.data = fi_ahb_read(RG_XMIT_A64);
        rg_xmit_a64.b.rg_txdpd_comp_coef_i_7_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[7];
        rg_xmit_a64.b.rg_txdpd_comp_coef_q_7_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[7];
        fi_ahb_write(RG_XMIT_A64, rg_xmit_a64.data);

        rg_xmit_a65.data = fi_ahb_read(RG_XMIT_A65);
        rg_xmit_a65.b.rg_txdpd_comp_coef_i_8_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[8];
        rg_xmit_a65.b.rg_txdpd_comp_coef_q_8_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[8];
        fi_ahb_write(RG_XMIT_A65, rg_xmit_a65.data);

        rg_xmit_a66.data = fi_ahb_read(RG_XMIT_A66);
        rg_xmit_a66.b.rg_txdpd_comp_coef_i_9_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[9];
        rg_xmit_a66.b.rg_txdpd_comp_coef_q_9_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[9];
        fi_ahb_write(RG_XMIT_A66, rg_xmit_a66.data);

        rg_xmit_a67.data = fi_ahb_read(RG_XMIT_A67);
        rg_xmit_a67.b.rg_txdpd_comp_coef_i_a_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[10];
        rg_xmit_a67.b.rg_txdpd_comp_coef_q_a_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[10];
        fi_ahb_write(RG_XMIT_A67, rg_xmit_a67.data);

        rg_xmit_a68.data = fi_ahb_read(RG_XMIT_A68);
        rg_xmit_a68.b.rg_txdpd_comp_coef_i_b_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[11];
        rg_xmit_a68.b.rg_txdpd_comp_coef_q_b_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[11];
        fi_ahb_write(RG_XMIT_A68, rg_xmit_a68.data);

        rg_xmit_a69.data = fi_ahb_read(RG_XMIT_A69);
        rg_xmit_a69.b.rg_txdpd_comp_coef_i_c_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[12];
        rg_xmit_a69.b.rg_txdpd_comp_coef_q_c_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[12];
        fi_ahb_write(RG_XMIT_A69, rg_xmit_a69.data);

        rg_xmit_a70.data = fi_ahb_read(RG_XMIT_A70);
        rg_xmit_a70.b.rg_txdpd_comp_coef_i_d_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[13];
        rg_xmit_a70.b.rg_txdpd_comp_coef_q_d_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[13];
        fi_ahb_write(RG_XMIT_A70, rg_xmit_a70.data);

        rg_xmit_a71.data = fi_ahb_read(RG_XMIT_A71);
        rg_xmit_a71.b.rg_txdpd_comp_coef_i_e_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_i[14];
        rg_xmit_a71.b.rg_txdpd_comp_coef_q_e_man = wf5g_calb_result->tx_dpd[channel_idx][1].tx_dpd_coeff_q[14];
        fi_ahb_write(RG_XMIT_A71, rg_xmit_a71.data);
    }
    rg_xmit_a56.data = fi_ahb_read(RG_XMIT_A56);
    rg_xmit_a56.b.rg_txdpd_comp_type = 0x1;
    rg_xmit_a56.b.rg_txdpd_comp_bypass = 0x0;
    rg_xmit_a56.b.rg_txdpd_comp_bypass_man = 0x0;
    rg_xmit_a56.b.rg_txdpd_comp_coef_man_sel = 0x1;
    fi_ahb_write(RG_XMIT_A56, rg_xmit_a56.data);
#endif

    if(bw_idx == 0) {
        rg_recv_a3.data = fi_ahb_read(RG_RECV_A3);
        rg_recv_a3.b.rg_rxirr_eup_0 = wf5g_calb_result->rx_irr[channel_idx][0].coeff_eup[0];
        rg_recv_a3.b.rg_rxirr_eup_1 = wf5g_calb_result->rx_irr[channel_idx][0].coeff_eup[1];
        fi_ahb_write(RG_RECV_A3, rg_recv_a3.data);

        rg_recv_a4.data = fi_ahb_read(RG_RECV_A4);
        rg_recv_a4.b.rg_rxirr_eup_2 = wf5g_calb_result->rx_irr[channel_idx][0].coeff_eup[2];
        rg_recv_a4.b.rg_rxirr_eup_3 = wf5g_calb_result->rx_irr[channel_idx][0].coeff_eup[3];
        fi_ahb_write(RG_RECV_A4, rg_recv_a4.data);

        rg_recv_a5.data = fi_ahb_read(RG_RECV_A5);
        rg_recv_a5.b.rg_rxirr_pup_0 = wf5g_calb_result->rx_irr[channel_idx][0].coeff_pup[0];
        rg_recv_a5.b.rg_rxirr_pup_1 = wf5g_calb_result->rx_irr[channel_idx][0].coeff_pup[1];
        fi_ahb_write(RG_RECV_A5, rg_recv_a5.data);

        rg_recv_a6.data = fi_ahb_read(RG_RECV_A6);
        rg_recv_a6.b.rg_rxirr_pup_2 = wf5g_calb_result->rx_irr[channel_idx][0].coeff_pup[2];
        rg_recv_a6.b.rg_rxirr_pup_3 = wf5g_calb_result->rx_irr[channel_idx][0].coeff_pup[3];
        fi_ahb_write(RG_RECV_A6, rg_recv_a6.data);

        rg_recv_a7.data = fi_ahb_read(RG_RECV_A7);
        rg_recv_a7.b.rg_rxirr_elow_0 = wf5g_calb_result->rx_irr[channel_idx][0].coeff_elow[0];
        rg_recv_a7.b.rg_rxirr_elow_1 = wf5g_calb_result->rx_irr[channel_idx][0].coeff_elow[1];
        fi_ahb_write(RG_RECV_A7, rg_recv_a7.data);

        rg_recv_a8.data = fi_ahb_read(RG_RECV_A8);
        rg_recv_a8.b.rg_rxirr_elow_2 = wf5g_calb_result->rx_irr[channel_idx][0].coeff_elow[2];
        rg_recv_a8.b.rg_rxirr_elow_3 = wf5g_calb_result->rx_irr[channel_idx][0].coeff_elow[3];
        fi_ahb_write(RG_RECV_A8, rg_recv_a8.data);

        rg_recv_a9.data = fi_ahb_read(RG_RECV_A9);
        rg_recv_a9.b.rg_rxirr_plow_0 = wf5g_calb_result->rx_irr[channel_idx][0].coeff_plow[0];
        rg_recv_a9.b.rg_rxirr_plow_1 = wf5g_calb_result->rx_irr[channel_idx][0].coeff_plow[1];
        fi_ahb_write(RG_RECV_A9, rg_recv_a9.data);

        rg_recv_a10.data = fi_ahb_read(RG_RECV_A10);
        rg_recv_a10.b.rg_rxirr_plow_2 = wf5g_calb_result->rx_irr[channel_idx][0].coeff_plow[2];
        rg_recv_a10.b.rg_rxirr_plow_3 = wf5g_calb_result->rx_irr[channel_idx][0].coeff_plow[3];
        fi_ahb_write(RG_RECV_A10, rg_recv_a10.data);

    } else if (bw_idx == 1) {
        rg_recv_a3.data = fi_ahb_read(RG_RECV_A3);
        rg_recv_a3.b.rg_rxirr_eup_0 = wf5g_calb_result->rx_irr[channel_idx][1].coeff_eup[0];
        rg_recv_a3.b.rg_rxirr_eup_1 = wf5g_calb_result->rx_irr[channel_idx][1].coeff_eup[1];
        fi_ahb_write(RG_RECV_A3, rg_recv_a3.data);

        rg_recv_a4.data = fi_ahb_read(RG_RECV_A4);
        rg_recv_a4.b.rg_rxirr_eup_2 = wf5g_calb_result->rx_irr[channel_idx][1].coeff_eup[2];
        rg_recv_a4.b.rg_rxirr_eup_3 = wf5g_calb_result->rx_irr[channel_idx][1].coeff_eup[3];
        fi_ahb_write(RG_RECV_A4, rg_recv_a4.data);

        rg_recv_a5.data = fi_ahb_read(RG_RECV_A5);
        rg_recv_a5.b.rg_rxirr_pup_0 = wf5g_calb_result->rx_irr[channel_idx][1].coeff_pup[0];
        rg_recv_a5.b.rg_rxirr_pup_1 = wf5g_calb_result->rx_irr[channel_idx][1].coeff_pup[1];
        fi_ahb_write(RG_RECV_A5, rg_recv_a5.data);

        rg_recv_a6.data = fi_ahb_read(RG_RECV_A6);
        rg_recv_a6.b.rg_rxirr_pup_2 = wf5g_calb_result->rx_irr[channel_idx][1].coeff_pup[2];
        rg_recv_a6.b.rg_rxirr_pup_3 = wf5g_calb_result->rx_irr[channel_idx][1].coeff_pup[3];
        fi_ahb_write(RG_RECV_A6, rg_recv_a6.data);

        rg_recv_a7.data = fi_ahb_read(RG_RECV_A7);
        rg_recv_a7.b.rg_rxirr_elow_0 = wf5g_calb_result->rx_irr[channel_idx][1].coeff_elow[0];
        rg_recv_a7.b.rg_rxirr_elow_1 = wf5g_calb_result->rx_irr[channel_idx][1].coeff_elow[1];
        fi_ahb_write(RG_RECV_A7, rg_recv_a7.data);

        rg_recv_a8.data = fi_ahb_read(RG_RECV_A8);
        rg_recv_a8.b.rg_rxirr_elow_2 = wf5g_calb_result->rx_irr[channel_idx][1].coeff_elow[2];
        rg_recv_a8.b.rg_rxirr_elow_3 = wf5g_calb_result->rx_irr[channel_idx][1].coeff_elow[3];
        fi_ahb_write(RG_RECV_A8, rg_recv_a8.data);

        rg_recv_a9.data = fi_ahb_read(RG_RECV_A9);
        rg_recv_a9.b.rg_rxirr_plow_0 = wf5g_calb_result->rx_irr[channel_idx][1].coeff_plow[0];
        rg_recv_a9.b.rg_rxirr_plow_1 = wf5g_calb_result->rx_irr[channel_idx][1].coeff_plow[1];
        fi_ahb_write(RG_RECV_A9, rg_recv_a9.data);

        rg_recv_a10.data = fi_ahb_read(RG_RECV_A10);
        rg_recv_a10.b.rg_rxirr_plow_2 = wf5g_calb_result->rx_irr[channel_idx][1].coeff_plow[2];
        rg_recv_a10.b.rg_rxirr_plow_3 = wf5g_calb_result->rx_irr[channel_idx][1].coeff_plow[3];
        fi_ahb_write(RG_RECV_A10, rg_recv_a10.data);

    } else if (bw_idx == 2) {
        rg_recv_a3.data = fi_ahb_read(RG_RECV_A3);
        rg_recv_a3.b.rg_rxirr_eup_0 = wf5g_calb_result->rx_irr[channel_idx][2].coeff_eup[0];
        rg_recv_a3.b.rg_rxirr_eup_1 = wf5g_calb_result->rx_irr[channel_idx][2].coeff_eup[1];
        fi_ahb_write(RG_RECV_A3, rg_recv_a3.data);

        rg_recv_a4.data = fi_ahb_read(RG_RECV_A4);
        rg_recv_a4.b.rg_rxirr_eup_2 = wf5g_calb_result->rx_irr[channel_idx][2].coeff_eup[2];
        rg_recv_a4.b.rg_rxirr_eup_3 = wf5g_calb_result->rx_irr[channel_idx][2].coeff_eup[3];
        fi_ahb_write(RG_RECV_A4, rg_recv_a4.data);

        rg_recv_a5.data = fi_ahb_read(RG_RECV_A5);
        rg_recv_a5.b.rg_rxirr_pup_0 = wf5g_calb_result->rx_irr[channel_idx][2].coeff_pup[0];
        rg_recv_a5.b.rg_rxirr_pup_1 = wf5g_calb_result->rx_irr[channel_idx][2].coeff_pup[1];
        fi_ahb_write(RG_RECV_A5, rg_recv_a5.data);

        rg_recv_a6.data = fi_ahb_read(RG_RECV_A6);
        rg_recv_a6.b.rg_rxirr_pup_2 = wf5g_calb_result->rx_irr[channel_idx][2].coeff_pup[2];
        rg_recv_a6.b.rg_rxirr_pup_3 = wf5g_calb_result->rx_irr[channel_idx][2].coeff_pup[3];
        fi_ahb_write(RG_RECV_A6, rg_recv_a6.data);

        rg_recv_a7.data = fi_ahb_read(RG_RECV_A7);
        rg_recv_a7.b.rg_rxirr_elow_0 = wf5g_calb_result->rx_irr[channel_idx][2].coeff_elow[0];
        rg_recv_a7.b.rg_rxirr_elow_1 = wf5g_calb_result->rx_irr[channel_idx][2].coeff_elow[1];
        fi_ahb_write(RG_RECV_A7, rg_recv_a7.data);

        rg_recv_a8.data = fi_ahb_read(RG_RECV_A8);
        rg_recv_a8.b.rg_rxirr_elow_2 = wf5g_calb_result->rx_irr[channel_idx][2].coeff_elow[2];
        rg_recv_a8.b.rg_rxirr_elow_3 = wf5g_calb_result->rx_irr[channel_idx][2].coeff_elow[3];
        fi_ahb_write(RG_RECV_A8, rg_recv_a8.data);

        rg_recv_a9.data = fi_ahb_read(RG_RECV_A9);
        rg_recv_a9.b.rg_rxirr_plow_0 = wf5g_calb_result->rx_irr[channel_idx][2].coeff_plow[0];
        rg_recv_a9.b.rg_rxirr_plow_1 = wf5g_calb_result->rx_irr[channel_idx][2].coeff_plow[1];
        fi_ahb_write(RG_RECV_A9, rg_recv_a9.data);

        rg_recv_a10.data = fi_ahb_read(RG_RECV_A10);
        rg_recv_a10.b.rg_rxirr_plow_2 = wf5g_calb_result->rx_irr[channel_idx][2].coeff_plow[2];
        rg_recv_a10.b.rg_rxirr_plow_3 = wf5g_calb_result->rx_irr[channel_idx][2].coeff_plow[3];
        fi_ahb_write(RG_RECV_A10, rg_recv_a10.data);
    }
    rg_recv_a11.data = fi_ahb_read(RG_RECV_A11);
    rg_recv_a11.b.rg_rxirr_man_mode = 0x1;
    rg_recv_a11.b.rg_rxirr_bypass = 0x0;
    fi_ahb_write(RG_RECV_A11, rg_recv_a11.data);
}

void change_rf_2g_param_when_switch_channel(struct t9026_wf2g_calb_rst_out* wf2g_calb_result, unsigned char bw_idx,  unsigned char txirr_manual)
{
    unsigned int tmp;
    unsigned int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    unsigned int tmp8, tmp9, tmpa, tmpb, tmpc, tmpd, tmpe, tmpf;
    int gain_ratio = 0;
    unsigned int dciq_offset = 0;
    RG_XMIT_A45_FIELD_T    rg_xmit_a45;
    RG_XMIT_A46_FIELD_T    rg_xmit_a46;
    RG_XMIT_A6_FIELD_T    rg_xmit_a6;
    RG_XMIT_A7_FIELD_T    rg_xmit_a7;
    RG_XMIT_A8_FIELD_T    rg_xmit_a8;
    RG_XMIT_A9_FIELD_T    rg_xmit_a9;
    RG_XMIT_A10_FIELD_T    rg_xmit_a10;
    RG_XMIT_A11_FIELD_T    rg_xmit_a11;
    RG_XMIT_A12_FIELD_T    rg_xmit_a12;
    RG_XMIT_A13_FIELD_T    rg_xmit_a13;
    RG_XMIT_A18_FIELD_T    rg_xmit_a18;
    RG_XMIT_A19_FIELD_T    rg_xmit_a19;
    RG_XMIT_A20_FIELD_T    rg_xmit_a20;
    RG_XMIT_A21_FIELD_T    rg_xmit_a21;
    RG_XMIT_A22_FIELD_T    rg_xmit_a22;
    RG_XMIT_A23_FIELD_T    rg_xmit_a23;
    RG_XMIT_A24_FIELD_T    rg_xmit_a24;
    RG_XMIT_A25_FIELD_T    rg_xmit_a25;
    RG_XMIT_A26_FIELD_T    rg_xmit_a26;
    RG_XMIT_A27_FIELD_T    rg_xmit_a27;
    RG_XMIT_A28_FIELD_T    rg_xmit_a28;
    RG_XMIT_A29_FIELD_T    rg_xmit_a29;
    RG_XMIT_A30_FIELD_T    rg_xmit_a30;
    RG_XMIT_A31_FIELD_T    rg_xmit_a31;
    RG_XMIT_A32_FIELD_T    rg_xmit_a32;
    RG_XMIT_A33_FIELD_T    rg_xmit_a33;
    RG_XMIT_A47_FIELD_T    rg_xmit_a47;
    RG_XMIT_A48_FIELD_T    rg_xmit_a48;
    RG_XMIT_A49_FIELD_T    rg_xmit_a49;
    RG_XMIT_A50_FIELD_T    rg_xmit_a50;
    RG_XMIT_A51_FIELD_T    rg_xmit_a51;
    RG_XMIT_A52_FIELD_T    rg_xmit_a52;
    RG_XMIT_A53_FIELD_T    rg_xmit_a53;
    RG_XMIT_A54_FIELD_T    rg_xmit_a54;
#ifdef CONFIG_DPD_ENABLE
    RG_XMIT_A56_FIELD_T    rg_xmit_a56;
    RG_XMIT_A57_FIELD_T    rg_xmit_a57;
    RG_XMIT_A58_FIELD_T    rg_xmit_a58;
    RG_XMIT_A59_FIELD_T    rg_xmit_a59;
    RG_XMIT_A60_FIELD_T    rg_xmit_a60;
    RG_XMIT_A61_FIELD_T    rg_xmit_a61;
    RG_XMIT_A62_FIELD_T    rg_xmit_a62;
    RG_XMIT_A63_FIELD_T    rg_xmit_a63;
    RG_XMIT_A64_FIELD_T    rg_xmit_a64;
    RG_XMIT_A65_FIELD_T    rg_xmit_a65;
    RG_XMIT_A66_FIELD_T    rg_xmit_a66;
    RG_XMIT_A67_FIELD_T    rg_xmit_a67;
    RG_XMIT_A68_FIELD_T    rg_xmit_a68;
    RG_XMIT_A69_FIELD_T    rg_xmit_a69;
    RG_XMIT_A70_FIELD_T    rg_xmit_a70;
    RG_XMIT_A71_FIELD_T    rg_xmit_a71;
#endif
    RG_RECV_A11_FIELD_T    rg_recv_a11;
    RG_RECV_A3_FIELD_T    rg_recv_a3;
    RG_RECV_A4_FIELD_T    rg_recv_a4;
    RG_RECV_A5_FIELD_T    rg_recv_a5;
    RG_RECV_A6_FIELD_T    rg_recv_a6;
    RG_RECV_A7_FIELD_T    rg_recv_a7;
    RG_RECV_A8_FIELD_T    rg_recv_a8;
    RG_RECV_A9_FIELD_T    rg_recv_a9;
    RG_RECV_A10_FIELD_T    rg_recv_a10;
    RG_RX_A58_FIELD_T    rg_rx_a58;
    RG_RX_A59_FIELD_T    rg_rx_a59;
    RG_TX_A4_FIELD_T rg_tx_a4;

    gain_ratio = wf2g_calb_result->tx_tssi.gain_ratio;
    dciq_offset = (((-6) * 0xd1) / (1 << 6) + (1 << 8)); //for remove 11b dc

    if(bw_idx == 0)
    {
        rg_rx_a58.data = rf_read_register(RG_RX_A58);
        rg_rx_a58.b.RG_WF_RX_LPF_CTUNE_ISEL_MAN_MODE = 0x1;    // WF2G 20M BW fixed RC code manual mode
        rf_write_register(RG_RX_A58, rg_rx_a58.data);

        rg_rx_a59.data = rf_read_register(RG_RX_A59);
        rg_rx_a59.b.RG_WF_RX_LPF_CTUNE_QSEL_MAN_MODE = 0x1;    // WF2G 20M BW fixed RC code manual mode
        rf_write_register(RG_RX_A59, rg_rx_a59.data);

        rg_tx_a4.data = rf_read_register(RG_TX_A4);
        rg_tx_a4.b.RG_WF2G_TX_MXR_CBANK = 0x0;
        rf_write_register(RG_TX_A4, rg_tx_a4.data);

        fi_ahb_write(0x0000816c, 0x00a96311);
    }
    else  if(bw_idx == 1)
    {
        rg_rx_a58.data = rf_read_register(RG_RX_A58);
        rg_rx_a58.b.RG_WF_RX_LPF_CTUNE_ISEL_MAN_MODE = 0x0;    // WF2G 40M BW fixed RC code auto mode
        rf_write_register(RG_RX_A58, rg_rx_a58.data);

        rg_rx_a59.data = rf_read_register(RG_RX_A59);
        rg_rx_a59.b.RG_WF_RX_LPF_CTUNE_QSEL_MAN_MODE = 0x0;    // WF2G 40M BW fixed RC code auto mode
        rf_write_register(RG_RX_A59, rg_rx_a59.data);

        rg_tx_a4.data = rf_read_register(RG_TX_A4);
        rg_tx_a4.b.RG_WF2G_TX_MXR_CBANK = 0x4;
        rf_write_register(RG_TX_A4, rg_tx_a4.data);

        fi_ahb_write(0x0000816c, 0x02296311);
    }

    if(txirr_manual){
        tmp = (0x80<<16) | (wf2g_calb_result->tx_dc_irr[1].tx_dc_q << 8) | wf2g_calb_result->tx_dc_irr[1].tx_dc_i;

        rg_xmit_a45.data = fi_ahb_read(RG_XMIT_A45);
        rg_xmit_a45.b.rg_man_tx_pwrlvl = tmp;
        rg_xmit_a45.b.rg_tx_pwr_sel_man_en = 0x1;
        fi_ahb_write(RG_XMIT_A45, rg_xmit_a45.data);

        rg_xmit_a46.data = fi_ahb_read(RG_XMIT_A46);
        rg_xmit_a46.b.rg_man_txirr_comp_theta = wf2g_calb_result->tx_dc_irr[1].tx_theta;
        rg_xmit_a46.b.rg_man_txirr_comp_alpha = wf2g_calb_result->tx_dc_irr[1].tx_alpha;
        fi_ahb_write(RG_XMIT_A46, rg_xmit_a46.data);

    } else {

        rg_xmit_a6.data = fi_ahb_read(RG_XMIT_A6);
        rg_xmit_a6.b.rg_txirr_comp_theta0 = wf2g_calb_result->tx_dc_irr[1].tx_theta;
        rg_xmit_a6.b.rg_txirr_comp_theta1 = wf2g_calb_result->tx_dc_irr[1].tx_theta;
        rg_xmit_a6.b.rg_txirr_comp_theta2 = wf2g_calb_result->tx_dc_irr[1].tx_theta;
        rg_xmit_a6.b.rg_txirr_comp_theta3 = wf2g_calb_result->tx_dc_irr[0].tx_theta;
        fi_ahb_write(RG_XMIT_A6, rg_xmit_a6.data);

        rg_xmit_a7.data = fi_ahb_read(RG_XMIT_A7);
        rg_xmit_a7.b.rg_txirr_comp_theta4 = wf2g_calb_result->tx_dc_irr[1].tx_theta;
        rg_xmit_a7.b.rg_txirr_comp_theta5 = wf2g_calb_result->tx_dc_irr[1].tx_theta;
        rg_xmit_a7.b.rg_txirr_comp_theta6 = wf2g_calb_result->tx_dc_irr[1].tx_theta;
        rg_xmit_a7.b.rg_txirr_comp_theta7 = wf2g_calb_result->tx_dc_irr[1].tx_theta;
        fi_ahb_write(RG_XMIT_A7, rg_xmit_a7.data);

        rg_xmit_a8.data = fi_ahb_read(RG_XMIT_A8);
        rg_xmit_a8.b.rg_txirr_comp_theta8 = wf2g_calb_result->tx_dc_irr[1].tx_theta;
        rg_xmit_a8.b.rg_txirr_comp_theta9 = wf2g_calb_result->tx_dc_irr[1].tx_theta;
        rg_xmit_a8.b.rg_txirr_comp_thetaa = wf2g_calb_result->tx_dc_irr[1].tx_theta;
        rg_xmit_a8.b.rg_txirr_comp_thetab = wf2g_calb_result->tx_dc_irr[1].tx_theta;
        fi_ahb_write(RG_XMIT_A8, rg_xmit_a8.data);

        rg_xmit_a9.data = fi_ahb_read(RG_XMIT_A9);
        rg_xmit_a9.b.rg_txirr_comp_thetac = wf2g_calb_result->tx_dc_irr[1].tx_theta;
        rg_xmit_a9.b.rg_txirr_comp_thetad = wf2g_calb_result->tx_dc_irr[1].tx_theta;
        rg_xmit_a9.b.rg_txirr_comp_thetae = wf2g_calb_result->tx_dc_irr[1].tx_theta;
        rg_xmit_a9.b.rg_txirr_comp_thetaf = wf2g_calb_result->tx_dc_irr[1].tx_theta;
        fi_ahb_write(RG_XMIT_A9, rg_xmit_a9.data);

        rg_xmit_a10.data = fi_ahb_read(RG_XMIT_A10);
        rg_xmit_a10.b.rg_txirr_comp_alpha0 = wf2g_calb_result->tx_dc_irr[1].tx_alpha;
        rg_xmit_a10.b.rg_txirr_comp_alpha1 = wf2g_calb_result->tx_dc_irr[1].tx_alpha;
        rg_xmit_a10.b.rg_txirr_comp_alpha2 = wf2g_calb_result->tx_dc_irr[1].tx_alpha;
        rg_xmit_a10.b.rg_txirr_comp_alpha3 = wf2g_calb_result->tx_dc_irr[0].tx_alpha;
        fi_ahb_write(RG_XMIT_A10, rg_xmit_a10.data);

        rg_xmit_a11.data = fi_ahb_read(RG_XMIT_A11);
        rg_xmit_a11.b.rg_txirr_comp_alpha4 = wf2g_calb_result->tx_dc_irr[1].tx_alpha;
        rg_xmit_a11.b.rg_txirr_comp_alpha5 = wf2g_calb_result->tx_dc_irr[1].tx_alpha;
        rg_xmit_a11.b.rg_txirr_comp_alpha6 = wf2g_calb_result->tx_dc_irr[1].tx_alpha;
        rg_xmit_a11.b.rg_txirr_comp_alpha7 = wf2g_calb_result->tx_dc_irr[1].tx_alpha;
        fi_ahb_write(RG_XMIT_A11, rg_xmit_a11.data);

        rg_xmit_a12.data = fi_ahb_read(RG_XMIT_A12);
        rg_xmit_a12.b.rg_txirr_comp_alpha8 = wf2g_calb_result->tx_dc_irr[1].tx_alpha;
        rg_xmit_a12.b.rg_txirr_comp_alpha9 = wf2g_calb_result->tx_dc_irr[1].tx_alpha;
        rg_xmit_a12.b.rg_txirr_comp_alphaa = wf2g_calb_result->tx_dc_irr[1].tx_alpha;
        rg_xmit_a12.b.rg_txirr_comp_alphab = wf2g_calb_result->tx_dc_irr[1].tx_alpha;
        fi_ahb_write(RG_XMIT_A12, rg_xmit_a12.data);

        rg_xmit_a13.data = fi_ahb_read(RG_XMIT_A13);
        rg_xmit_a13.b.rg_txirr_comp_alphac = wf2g_calb_result->tx_dc_irr[1].tx_alpha;
        rg_xmit_a13.b.rg_txirr_comp_alphad = wf2g_calb_result->tx_dc_irr[1].tx_alpha;
        rg_xmit_a13.b.rg_txirr_comp_alphae = wf2g_calb_result->tx_dc_irr[1].tx_alpha;
        rg_xmit_a13.b.rg_txirr_comp_alphaf = wf2g_calb_result->tx_dc_irr[1].tx_alpha;
        fi_ahb_write(RG_XMIT_A13, rg_xmit_a13.data);

        tmp0 = ((0xCB * gain_ratio/100)<<16) | (wf2g_calb_result->tx_dc_irr[1].tx_dc_q << 8) | wf2g_calb_result->tx_dc_irr[1].tx_dc_i;
        tmp1 = ((0xC5 * gain_ratio/100)<<16) | (wf2g_calb_result->tx_dc_irr[1].tx_dc_q << 8) | wf2g_calb_result->tx_dc_irr[1].tx_dc_i;
        tmp2 = ((0xC0 * gain_ratio/100)<<16) | (wf2g_calb_result->tx_dc_irr[1].tx_dc_q << 8) | wf2g_calb_result->tx_dc_irr[1].tx_dc_i;
        tmp3 = ((0xBA * gain_ratio/100)<<16) | ((wf2g_calb_result->tx_dc_irr[0].tx_dc_q + dciq_offset) << 8) | (wf2g_calb_result->tx_dc_irr[0].tx_dc_i + dciq_offset);
        tmp4 = ((0xB5 * gain_ratio/100)<<16) | (wf2g_calb_result->tx_dc_irr[1].tx_dc_q << 8) | wf2g_calb_result->tx_dc_irr[1].tx_dc_i;
        tmp5 = ((0xB0 * gain_ratio/100)<<16) | (wf2g_calb_result->tx_dc_irr[1].tx_dc_q << 8) | wf2g_calb_result->tx_dc_irr[1].tx_dc_i;
        tmp6 = ((0xAB * gain_ratio/100)<<16) | (wf2g_calb_result->tx_dc_irr[1].tx_dc_q << 8) | wf2g_calb_result->tx_dc_irr[1].tx_dc_i;
        tmp7 = ((0xA6 * gain_ratio/100)<<16) | (wf2g_calb_result->tx_dc_irr[1].tx_dc_q << 8) | wf2g_calb_result->tx_dc_irr[1].tx_dc_i;
        tmp8 = ((0xA1 * gain_ratio/100)<<16) | (wf2g_calb_result->tx_dc_irr[1].tx_dc_q << 8) | wf2g_calb_result->tx_dc_irr[1].tx_dc_i;
        tmp9 = ((0x9D * gain_ratio/100)<<16) | (wf2g_calb_result->tx_dc_irr[1].tx_dc_q << 8) | wf2g_calb_result->tx_dc_irr[1].tx_dc_i;
        tmpa = ((0x98 * gain_ratio/100)<<16) | (wf2g_calb_result->tx_dc_irr[1].tx_dc_q << 8) | wf2g_calb_result->tx_dc_irr[1].tx_dc_i;
        tmpb = ((0x94 * gain_ratio/100)<<16) | (wf2g_calb_result->tx_dc_irr[1].tx_dc_q << 8) | wf2g_calb_result->tx_dc_irr[1].tx_dc_i;
        tmpc = ((0x90 * gain_ratio/100)<<16) | (wf2g_calb_result->tx_dc_irr[1].tx_dc_q << 8) | wf2g_calb_result->tx_dc_irr[1].tx_dc_i;
        tmpd = ((0x8C * gain_ratio/100)<<16) | (wf2g_calb_result->tx_dc_irr[1].tx_dc_q << 8) | wf2g_calb_result->tx_dc_irr[1].tx_dc_i;
        tmpe = ((0x88 * gain_ratio/100)<<16) | (wf2g_calb_result->tx_dc_irr[1].tx_dc_q << 8) | wf2g_calb_result->tx_dc_irr[1].tx_dc_i;
        tmpf = ((0x84 * gain_ratio/100)<<16) | (wf2g_calb_result->tx_dc_irr[1].tx_dc_q << 8) | wf2g_calb_result->tx_dc_irr[1].tx_dc_i;

        rg_xmit_a18.data = fi_ahb_read(RG_XMIT_A18);
        rg_xmit_a18.b.rg_tx_pwrlvl0 = tmp0;
        fi_ahb_write(RG_XMIT_A18, rg_xmit_a18.data);

        rg_xmit_a19.data = fi_ahb_read(RG_XMIT_A19);
        rg_xmit_a19.b.rg_tx_pwrlvl1 = tmp1;
        fi_ahb_write(RG_XMIT_A19, rg_xmit_a19.data);

        rg_xmit_a20.data = fi_ahb_read(RG_XMIT_A20);
        rg_xmit_a20.b.rg_tx_pwrlvl2 = tmp2;
        fi_ahb_write(RG_XMIT_A20, rg_xmit_a20.data);

        rg_xmit_a21.data = fi_ahb_read(RG_XMIT_A21);
        rg_xmit_a21.b.rg_tx_pwrlvl3 = tmp3 | BIT(24);
        fi_ahb_write(RG_XMIT_A21, rg_xmit_a21.data);

        rg_xmit_a22.data = fi_ahb_read(RG_XMIT_A22);
        rg_xmit_a22.b.rg_tx_pwrlvl4 = tmp4;
        fi_ahb_write(RG_XMIT_A22, rg_xmit_a22.data);

        rg_xmit_a23.data = fi_ahb_read(RG_XMIT_A23);
        rg_xmit_a23.b.rg_tx_pwrlvl5 = tmp5;
        fi_ahb_write(RG_XMIT_A23, rg_xmit_a23.data);

        rg_xmit_a24.data = fi_ahb_read(RG_XMIT_A24);
        rg_xmit_a24.b.rg_tx_pwrlvl6 = tmp6;
        fi_ahb_write(RG_XMIT_A24, rg_xmit_a24.data);

        if (bw_idx == 0)
        {
            rg_xmit_a25.data = fi_ahb_read(RG_XMIT_A25);
            rg_xmit_a25.b.rg_tx_pwrlvl7 = tmp2;
            fi_ahb_write(RG_XMIT_A25, rg_xmit_a25.data);

            rg_xmit_a26.data = fi_ahb_read(RG_XMIT_A26);
            rg_xmit_a26.b.rg_tx_pwrlvl8 = tmp4;
            fi_ahb_write(RG_XMIT_A26, rg_xmit_a26.data);
        }
        else
        {
            rg_xmit_a25.data = fi_ahb_read(RG_XMIT_A25);
            rg_xmit_a25.b.rg_tx_pwrlvl7 = tmp7;
            fi_ahb_write(RG_XMIT_A25, rg_xmit_a25.data);

            rg_xmit_a26.data = fi_ahb_read(RG_XMIT_A26);
            rg_xmit_a26.b.rg_tx_pwrlvl8 = tmp8;
            fi_ahb_write(RG_XMIT_A26, rg_xmit_a26.data);
        }

        rg_xmit_a27.data = fi_ahb_read(RG_XMIT_A27);
        rg_xmit_a27.b.rg_tx_pwrlvl9 = tmp9;
        fi_ahb_write(RG_XMIT_A27, rg_xmit_a27.data);

        rg_xmit_a28.data = fi_ahb_read(RG_XMIT_A28);
        rg_xmit_a28.b.rg_tx_pwrlvla = tmpa;
        fi_ahb_write(RG_XMIT_A28, rg_xmit_a28.data);

        if (bw_idx == 0)
        {
            rg_xmit_a29.data = fi_ahb_read(RG_XMIT_A29);
            rg_xmit_a29.b.rg_tx_pwrlvlb = tmp8;
            fi_ahb_write(RG_XMIT_A29, rg_xmit_a29.data);
        }
        else
        {
            rg_xmit_a29.data = fi_ahb_read(RG_XMIT_A29);
            rg_xmit_a29.b.rg_tx_pwrlvlb = tmpb;
            fi_ahb_write(RG_XMIT_A29, rg_xmit_a29.data);
        }

        rg_xmit_a30.data = fi_ahb_read(RG_XMIT_A30);
        rg_xmit_a30.b.rg_tx_pwrlvlc = tmpc;
        fi_ahb_write(RG_XMIT_A30, rg_xmit_a30.data);

        rg_xmit_a31.data = fi_ahb_read(RG_XMIT_A31);
        rg_xmit_a31.b.rg_tx_pwrlvld = tmpd;
        fi_ahb_write(RG_XMIT_A31, rg_xmit_a31.data);

        rg_xmit_a32.data = fi_ahb_read(RG_XMIT_A32);
        rg_xmit_a32.b.rg_tx_pwrlvle = tmpe;
        fi_ahb_write(RG_XMIT_A32, rg_xmit_a32.data);

        rg_xmit_a33.data = fi_ahb_read(RG_XMIT_A33);
        rg_xmit_a33.b.rg_tx_pwrlvlf = tmpf;
        fi_ahb_write(RG_XMIT_A33, rg_xmit_a33.data);

        rg_xmit_a45.data = fi_ahb_read(RG_XMIT_A45);
        rg_xmit_a45.b.rg_tx_pwr_sel_man_en = 0x0;
        fi_ahb_write(RG_XMIT_A45, rg_xmit_a45.data);

        rg_xmit_a47.data = fi_ahb_read(RG_XMIT_A47);
        rg_xmit_a47.b.rg_tx_ex_gain0 = 0x02;
        rg_xmit_a47.b.rg_tx_ex_gain1 = 0x02;
        fi_ahb_write(RG_XMIT_A47, rg_xmit_a47.data);

        rg_xmit_a48.data = fi_ahb_read(RG_XMIT_A48);
        rg_xmit_a48.b.rg_tx_ex_gain2 = 0x02;
        rg_xmit_a48.b.rg_tx_ex_gain3 = 0x12;
        fi_ahb_write(RG_XMIT_A48, rg_xmit_a48.data);

        rg_xmit_a49.data = fi_ahb_read(RG_XMIT_A49);
        rg_xmit_a49.b.rg_tx_ex_gain4 = 0x02;
        rg_xmit_a49.b.rg_tx_ex_gain5 = 0x02;
        fi_ahb_write(RG_XMIT_A49, rg_xmit_a49.data);

        rg_xmit_a50.data = fi_ahb_read(RG_XMIT_A50);
        rg_xmit_a50.b.rg_tx_ex_gain6 = 0x02;
        rg_xmit_a50.b.rg_tx_ex_gain7 = 0x02;
        fi_ahb_write(RG_XMIT_A50, rg_xmit_a50.data);

        rg_xmit_a51.data = fi_ahb_read(RG_XMIT_A51);
        rg_xmit_a51.b.rg_tx_ex_gain8 = 0x02;
        rg_xmit_a51.b.rg_tx_ex_gain9 = 0x02;
        fi_ahb_write(RG_XMIT_A51, rg_xmit_a51.data);

        rg_xmit_a52.data = fi_ahb_read(RG_XMIT_A52);
        rg_xmit_a52.b.rg_tx_ex_gaina = 0x02;
        rg_xmit_a52.b.rg_tx_ex_gainb = 0x02;
        fi_ahb_write(RG_XMIT_A52, rg_xmit_a52.data);

        rg_xmit_a53.data = fi_ahb_read(RG_XMIT_A53);
        rg_xmit_a53.b.rg_tx_ex_gainc = 0x02;
        rg_xmit_a53.b.rg_tx_ex_gaind = 0x02;
        fi_ahb_write(RG_XMIT_A53, rg_xmit_a53.data);

        rg_xmit_a54.data = fi_ahb_read(RG_XMIT_A54);
        rg_xmit_a54.b.rg_tx_ex_gaine = 0x02;
        rg_xmit_a54.b.rg_tx_ex_gainf = 0x02;
        fi_ahb_write(RG_XMIT_A54, rg_xmit_a54.data);
    }
#ifdef CONFIG_DPD_ENABLE

    if(bw_idx == 0){
        rg_xmit_a57.data = fi_ahb_read(RG_XMIT_A57);
        rg_xmit_a57.b.rg_txdpd_comp_coef_i_0_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[0];
        rg_xmit_a57.b.rg_txdpd_comp_coef_q_0_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[0];
        fi_ahb_write(RG_XMIT_A57, rg_xmit_a57.data);

        rg_xmit_a58.data = fi_ahb_read(RG_XMIT_A58);
        rg_xmit_a58.b.rg_txdpd_comp_coef_i_1_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[1];
        rg_xmit_a58.b.rg_txdpd_comp_coef_q_1_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[1];
        fi_ahb_write(RG_XMIT_A58, rg_xmit_a58.data);

        rg_xmit_a59.data = fi_ahb_read(RG_XMIT_A59);
        rg_xmit_a59.b.rg_txdpd_comp_coef_i_2_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[2];
        rg_xmit_a59.b.rg_txdpd_comp_coef_q_2_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[2];
        fi_ahb_write(RG_XMIT_A59, rg_xmit_a59.data);

        rg_xmit_a60.data = fi_ahb_read(RG_XMIT_A60);
        rg_xmit_a60.b.rg_txdpd_comp_coef_i_3_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[3];
        rg_xmit_a60.b.rg_txdpd_comp_coef_q_3_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[3];
        fi_ahb_write(RG_XMIT_A60, rg_xmit_a60.data);

        rg_xmit_a61.data = fi_ahb_read(RG_XMIT_A61);
        rg_xmit_a61.b.rg_txdpd_comp_coef_i_4_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[4];
        rg_xmit_a61.b.rg_txdpd_comp_coef_q_4_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[4];
        fi_ahb_write(RG_XMIT_A61, rg_xmit_a61.data);

        rg_xmit_a62.data = fi_ahb_read(RG_XMIT_A62);
        rg_xmit_a62.b.rg_txdpd_comp_coef_i_5_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[5];
        rg_xmit_a62.b.rg_txdpd_comp_coef_q_5_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[5];
        fi_ahb_write(RG_XMIT_A62, rg_xmit_a62.data);

        rg_xmit_a63.data = fi_ahb_read(RG_XMIT_A63);
        rg_xmit_a63.b.rg_txdpd_comp_coef_i_6_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[6];
        rg_xmit_a63.b.rg_txdpd_comp_coef_q_6_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[6];
        fi_ahb_write(RG_XMIT_A63, rg_xmit_a63.data);

        rg_xmit_a64.data = fi_ahb_read(RG_XMIT_A64);
        rg_xmit_a64.b.rg_txdpd_comp_coef_i_7_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[7];
        rg_xmit_a64.b.rg_txdpd_comp_coef_q_7_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[7];
        fi_ahb_write(RG_XMIT_A64, rg_xmit_a64.data);

        rg_xmit_a65.data = fi_ahb_read(RG_XMIT_A65);
        rg_xmit_a65.b.rg_txdpd_comp_coef_i_8_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[8];
        rg_xmit_a65.b.rg_txdpd_comp_coef_q_8_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[8];
        fi_ahb_write(RG_XMIT_A65, rg_xmit_a65.data);

        rg_xmit_a66.data = fi_ahb_read(RG_XMIT_A66);
        rg_xmit_a66.b.rg_txdpd_comp_coef_i_9_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[9];
        rg_xmit_a66.b.rg_txdpd_comp_coef_q_9_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[9];
        fi_ahb_write(RG_XMIT_A66, rg_xmit_a66.data);

        rg_xmit_a67.data = fi_ahb_read(RG_XMIT_A67);
        rg_xmit_a67.b.rg_txdpd_comp_coef_i_a_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[10];
        rg_xmit_a67.b.rg_txdpd_comp_coef_q_a_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[10];
        fi_ahb_write(RG_XMIT_A67, rg_xmit_a67.data);

        rg_xmit_a68.data = fi_ahb_read(RG_XMIT_A68);
        rg_xmit_a68.b.rg_txdpd_comp_coef_i_b_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[11];
        rg_xmit_a68.b.rg_txdpd_comp_coef_q_b_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[11];
        fi_ahb_write(RG_XMIT_A68, rg_xmit_a68.data);

        rg_xmit_a69.data = fi_ahb_read(RG_XMIT_A69);
        rg_xmit_a69.b.rg_txdpd_comp_coef_i_c_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[12];
        rg_xmit_a69.b.rg_txdpd_comp_coef_q_c_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[12];
        fi_ahb_write(RG_XMIT_A69, rg_xmit_a69.data);

        rg_xmit_a70.data = fi_ahb_read(RG_XMIT_A70);
        rg_xmit_a70.b.rg_txdpd_comp_coef_i_d_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[13];
        rg_xmit_a70.b.rg_txdpd_comp_coef_q_d_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[13];
        fi_ahb_write(RG_XMIT_A70, rg_xmit_a70.data);

        rg_xmit_a71.data = fi_ahb_read(RG_XMIT_A71);
        rg_xmit_a71.b.rg_txdpd_comp_coef_i_e_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[14];
        rg_xmit_a71.b.rg_txdpd_comp_coef_q_e_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[14];
        fi_ahb_write(RG_XMIT_A71, rg_xmit_a71.data);

    } else if(bw_idx == 1) {
        rg_xmit_a57.data = fi_ahb_read(RG_XMIT_A57);
        rg_xmit_a57.b.rg_txdpd_comp_coef_i_0_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[0];
        rg_xmit_a57.b.rg_txdpd_comp_coef_q_0_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[0];
        fi_ahb_write(RG_XMIT_A57, rg_xmit_a57.data);

        rg_xmit_a58.data = fi_ahb_read(RG_XMIT_A58);
        rg_xmit_a58.b.rg_txdpd_comp_coef_i_1_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[1];
        rg_xmit_a58.b.rg_txdpd_comp_coef_q_1_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[1];
        fi_ahb_write(RG_XMIT_A58, rg_xmit_a58.data);

        rg_xmit_a59.data = fi_ahb_read(RG_XMIT_A59);
        rg_xmit_a59.b.rg_txdpd_comp_coef_i_2_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[2];
        rg_xmit_a59.b.rg_txdpd_comp_coef_q_2_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[2];
        fi_ahb_write(RG_XMIT_A59, rg_xmit_a59.data);

        rg_xmit_a60.data = fi_ahb_read(RG_XMIT_A60);
        rg_xmit_a60.b.rg_txdpd_comp_coef_i_3_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[3];
        rg_xmit_a60.b.rg_txdpd_comp_coef_q_3_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[3];
        fi_ahb_write(RG_XMIT_A60, rg_xmit_a60.data);

        rg_xmit_a61.data = fi_ahb_read(RG_XMIT_A61);
        rg_xmit_a61.b.rg_txdpd_comp_coef_i_4_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[4];
        rg_xmit_a61.b.rg_txdpd_comp_coef_q_4_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[4];
        fi_ahb_write(RG_XMIT_A61, rg_xmit_a61.data);

        rg_xmit_a62.data = fi_ahb_read(RG_XMIT_A62);
        rg_xmit_a62.b.rg_txdpd_comp_coef_i_5_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[5];
        rg_xmit_a62.b.rg_txdpd_comp_coef_q_5_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[5];
        fi_ahb_write(RG_XMIT_A62, rg_xmit_a62.data);

        rg_xmit_a63.data = fi_ahb_read(RG_XMIT_A63);
        rg_xmit_a63.b.rg_txdpd_comp_coef_i_6_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[6];
        rg_xmit_a63.b.rg_txdpd_comp_coef_q_6_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[6];
        fi_ahb_write(RG_XMIT_A63, rg_xmit_a63.data);

        rg_xmit_a64.data = fi_ahb_read(RG_XMIT_A64);
        rg_xmit_a64.b.rg_txdpd_comp_coef_i_7_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[7];
        rg_xmit_a64.b.rg_txdpd_comp_coef_q_7_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[7];
        fi_ahb_write(RG_XMIT_A64, rg_xmit_a64.data);

        rg_xmit_a65.data = fi_ahb_read(RG_XMIT_A65);
        rg_xmit_a65.b.rg_txdpd_comp_coef_i_8_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[8];
        rg_xmit_a65.b.rg_txdpd_comp_coef_q_8_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[8];
        fi_ahb_write(RG_XMIT_A65, rg_xmit_a65.data);

        rg_xmit_a66.data = fi_ahb_read(RG_XMIT_A66);
        rg_xmit_a66.b.rg_txdpd_comp_coef_i_9_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[9];
        rg_xmit_a66.b.rg_txdpd_comp_coef_q_9_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[9];
        fi_ahb_write(RG_XMIT_A66, rg_xmit_a66.data);

        rg_xmit_a67.data = fi_ahb_read(RG_XMIT_A67);
        rg_xmit_a67.b.rg_txdpd_comp_coef_i_a_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[10];
        rg_xmit_a67.b.rg_txdpd_comp_coef_q_a_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[10];
        fi_ahb_write(RG_XMIT_A67, rg_xmit_a67.data);

        rg_xmit_a68.data = fi_ahb_read(RG_XMIT_A68);
        rg_xmit_a68.b.rg_txdpd_comp_coef_i_b_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[11];
        rg_xmit_a68.b.rg_txdpd_comp_coef_q_b_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[11];
        fi_ahb_write(RG_XMIT_A68, rg_xmit_a68.data);

        rg_xmit_a69.data = fi_ahb_read(RG_XMIT_A69);
        rg_xmit_a69.b.rg_txdpd_comp_coef_i_c_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[12];
        rg_xmit_a69.b.rg_txdpd_comp_coef_q_c_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[12];
        fi_ahb_write(RG_XMIT_A69, rg_xmit_a69.data);

        rg_xmit_a70.data = fi_ahb_read(RG_XMIT_A70);
        rg_xmit_a70.b.rg_txdpd_comp_coef_i_d_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[13];
        rg_xmit_a70.b.rg_txdpd_comp_coef_q_d_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[13];
        fi_ahb_write(RG_XMIT_A70, rg_xmit_a70.data);

        rg_xmit_a71.data = fi_ahb_read(RG_XMIT_A71);
        rg_xmit_a71.b.rg_txdpd_comp_coef_i_e_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_i[14];
        rg_xmit_a71.b.rg_txdpd_comp_coef_q_e_man = wf2g_calb_result->tx_dpd[1].tx_dpd_coeff_q[14];
        fi_ahb_write(RG_XMIT_A71, rg_xmit_a71.data);
    }

    rg_xmit_a56.data = fi_ahb_read(RG_XMIT_A56);
    rg_xmit_a56.b.rg_txdpd_comp_type = 0x1;
    rg_xmit_a56.b.rg_txdpd_comp_bypass = 0x0;
    rg_xmit_a56.b.rg_txdpd_comp_bypass_man = 0x0;
    rg_xmit_a56.b.rg_txdpd_comp_coef_man_sel = 0x1;
    fi_ahb_write(RG_XMIT_A56, rg_xmit_a56.data);

#endif

    if (bw_idx == 0) {
        rg_recv_a3.data = fi_ahb_read(RG_RECV_A3);
        rg_recv_a3.b.rg_rxirr_eup_0 = wf2g_calb_result->rx_irr[0].coeff_eup[0];
        rg_recv_a3.b.rg_rxirr_eup_1 = wf2g_calb_result->rx_irr[0].coeff_eup[1];
        fi_ahb_write(RG_RECV_A3, rg_recv_a3.data);

        rg_recv_a4.data = fi_ahb_read(RG_RECV_A4);
        rg_recv_a4.b.rg_rxirr_eup_2 = wf2g_calb_result->rx_irr[0].coeff_eup[2];
        rg_recv_a4.b.rg_rxirr_eup_3 = wf2g_calb_result->rx_irr[0].coeff_eup[3];
        fi_ahb_write(RG_RECV_A4, rg_recv_a4.data);

        rg_recv_a5.data = fi_ahb_read(RG_RECV_A5);
        rg_recv_a5.b.rg_rxirr_pup_0 = wf2g_calb_result->rx_irr[0].coeff_pup[0];
        rg_recv_a5.b.rg_rxirr_pup_1 = wf2g_calb_result->rx_irr[0].coeff_pup[1];
        fi_ahb_write(RG_RECV_A5, rg_recv_a5.data);

        rg_recv_a6.data = fi_ahb_read(RG_RECV_A6);
        rg_recv_a6.b.rg_rxirr_pup_2 = wf2g_calb_result->rx_irr[0].coeff_pup[2];
        rg_recv_a6.b.rg_rxirr_pup_3 = wf2g_calb_result->rx_irr[0].coeff_pup[3];
        fi_ahb_write(RG_RECV_A6, rg_recv_a6.data);

        rg_recv_a7.data = fi_ahb_read(RG_RECV_A7);
        rg_recv_a7.b.rg_rxirr_elow_0 = wf2g_calb_result->rx_irr[0].coeff_elow[0];
        rg_recv_a7.b.rg_rxirr_elow_1 = wf2g_calb_result->rx_irr[0].coeff_elow[1];
        fi_ahb_write(RG_RECV_A7, rg_recv_a7.data);

        rg_recv_a8.data = fi_ahb_read(RG_RECV_A8);
        rg_recv_a8.b.rg_rxirr_elow_2 = wf2g_calb_result->rx_irr[0].coeff_elow[2];
        rg_recv_a8.b.rg_rxirr_elow_3 = wf2g_calb_result->rx_irr[0].coeff_elow[3];
        fi_ahb_write(RG_RECV_A8, rg_recv_a8.data);

        rg_recv_a9.data = fi_ahb_read(RG_RECV_A9);
        rg_recv_a9.b.rg_rxirr_plow_0 = wf2g_calb_result->rx_irr[0].coeff_plow[0];
        rg_recv_a9.b.rg_rxirr_plow_1 = wf2g_calb_result->rx_irr[0].coeff_plow[1];
        fi_ahb_write(RG_RECV_A9, rg_recv_a9.data);

        rg_recv_a10.data = fi_ahb_read(RG_RECV_A10);
        rg_recv_a10.b.rg_rxirr_plow_2 = wf2g_calb_result->rx_irr[0].coeff_plow[2];
        rg_recv_a10.b.rg_rxirr_plow_3 = wf2g_calb_result->rx_irr[0].coeff_plow[3];
        fi_ahb_write(RG_RECV_A10, rg_recv_a10.data);

    } else if(bw_idx == 1) {
        rg_recv_a3.data = fi_ahb_read(RG_RECV_A3);
        rg_recv_a3.b.rg_rxirr_eup_0 = wf2g_calb_result->rx_irr[1].coeff_eup[0];
        rg_recv_a3.b.rg_rxirr_eup_1 = wf2g_calb_result->rx_irr[1].coeff_eup[1];
        fi_ahb_write(RG_RECV_A3, rg_recv_a3.data);

        rg_recv_a4.data = fi_ahb_read(RG_RECV_A4);
        rg_recv_a4.b.rg_rxirr_eup_2 = wf2g_calb_result->rx_irr[1].coeff_eup[2];
        rg_recv_a4.b.rg_rxirr_eup_3 = wf2g_calb_result->rx_irr[1].coeff_eup[3];
        fi_ahb_write(RG_RECV_A4, rg_recv_a4.data);

        rg_recv_a5.data = fi_ahb_read(RG_RECV_A5);
        rg_recv_a5.b.rg_rxirr_pup_0 = wf2g_calb_result->rx_irr[1].coeff_pup[0];
        rg_recv_a5.b.rg_rxirr_pup_1 = wf2g_calb_result->rx_irr[1].coeff_pup[1];
        fi_ahb_write(RG_RECV_A5, rg_recv_a5.data);

        rg_recv_a6.data = fi_ahb_read(RG_RECV_A6);
        rg_recv_a6.b.rg_rxirr_pup_2 = wf2g_calb_result->rx_irr[1].coeff_pup[2];
        rg_recv_a6.b.rg_rxirr_pup_3 = wf2g_calb_result->rx_irr[1].coeff_pup[3];
        fi_ahb_write(RG_RECV_A6, rg_recv_a6.data);

        rg_recv_a7.data = fi_ahb_read(RG_RECV_A7);
        rg_recv_a7.b.rg_rxirr_elow_0 = wf2g_calb_result->rx_irr[1].coeff_elow[0];
        rg_recv_a7.b.rg_rxirr_elow_1 = wf2g_calb_result->rx_irr[1].coeff_elow[1];
        fi_ahb_write(RG_RECV_A7, rg_recv_a7.data);

        rg_recv_a8.data = fi_ahb_read(RG_RECV_A8);
        rg_recv_a8.b.rg_rxirr_elow_2 = wf2g_calb_result->rx_irr[1].coeff_elow[2];
        rg_recv_a8.b.rg_rxirr_elow_3 = wf2g_calb_result->rx_irr[1].coeff_elow[3];
        fi_ahb_write(RG_RECV_A8, rg_recv_a8.data);

        rg_recv_a9.data = fi_ahb_read(RG_RECV_A9);
        rg_recv_a9.b.rg_rxirr_plow_0 = wf2g_calb_result->rx_irr[1].coeff_plow[0];
        rg_recv_a9.b.rg_rxirr_plow_1 = wf2g_calb_result->rx_irr[1].coeff_plow[1];
        fi_ahb_write(RG_RECV_A9, rg_recv_a9.data);

        rg_recv_a10.data = fi_ahb_read(RG_RECV_A10);
        rg_recv_a10.b.rg_rxirr_plow_2 = wf2g_calb_result->rx_irr[1].coeff_plow[2];
        rg_recv_a10.b.rg_rxirr_plow_3 = wf2g_calb_result->rx_irr[1].coeff_plow[3];
        fi_ahb_write(RG_RECV_A10, rg_recv_a10.data);
    }

    rg_recv_a11.data = fi_ahb_read(RG_RECV_A11);
    rg_recv_a11.b.rg_rxirr_man_mode = 0x1;
    rg_recv_a11.b.rg_rxirr_bypass = 0x0;
    fi_ahb_write(RG_RECV_A11, rg_recv_a11.data);

}

void t9026_bw_switch(int bw_idx) {
    RG_RX_A2_FIELD_T    rg_rx_a2;

    rg_rx_a2.data = rf_read_register(RG_RX_A2);
    rg_rx_a2.b.RG_WF_RX_BBBM_MAN_MODE = 1;
    rg_rx_a2.b.RG_WF_RX_BW_SEL = bw_idx & 0x03;                         // 80MHz BW

    if ((bw_idx & 0x03) >= 2) {
        rg_rx_a2.b.RG_WF_RX_ADCFS_SEL = 0x1;                         // adcfs 320MHz
    } else {
        rg_rx_a2.b.RG_WF_RX_ADCFS_SEL = 0x0;                         // adcfs 160MHz,jinliang
    }

    rf_write_register(RG_RX_A2, rg_rx_a2.data);
#ifdef DRV_PT_SUPPORT
    if (bw_idx == RF_BW_20M)
    {
        fi_ahb_write(PHY_PRIMARY_CHANNEL, 0x80000000);//80M:a1; 40M;90; 20M:80
    }
    else if (bw_idx == RF_BW_40M)
    {
        fi_ahb_write(PHY_PRIMARY_CHANNEL,  0x91000000);//80M:a1; 40M;90; 20M:80
    }
    else if (bw_idx == RF_BW_80M)
    {
        fi_ahb_write(PHY_PRIMARY_CHANNEL, 0xa1000000);//80M:a1; 40M;90; 20M:80
    } //if(bw_idx == RF_BW_20M)
#endif
}

void rf_channel_restore(unsigned char p_chn, unsigned char bw) {
    //printk("%s, p_chn:%d, bw:%d\n", __func__, p_chn, bw);

    if ((p_chn >= 1) && (p_chn <= 14)) {
        change_rf_2g_param_when_switch_channel(&wf2g_calb_result, bw, 0);

    } else {
        if (rf_5g_a == p_chn) {
            change_rf_5g_param_when_switch_channel(&wf5g_calb_result, bw, 0, 0);

        } else {
            change_rf_5g_param_when_switch_channel(&wf5g_calb_result, bw, 1, 0);
        }
    }
}
#ifndef DRV_PT_SUPPORT
void t9026_channel_switch(int p_chn, unsigned char bw, unsigned char restore) {
    RG_TOP_A2_FIELD_T    rg_top_a2;
    RG_RECV_A11_FIELD_T    rg_recv_a11;
#ifdef CONFIG_DPD_ENABLE
    RG_XMIT_A56_FIELD_T    rg_xmit_a56;

    rg_xmit_a56.data = fi_ahb_read(RG_XMIT_A56);
    rg_xmit_a56.b.rg_txdpd_comp_type = 0x1;
    rg_xmit_a56.b.rg_txdpd_comp_bypass = 0x1;
    rg_xmit_a56.b.rg_txdpd_comp_bypass_man = 0x1;
    rg_xmit_a56.b.rg_txdpd_comp_coef_man_sel = 0x1;
    fi_ahb_write(RG_XMIT_A56, rg_xmit_a56.data);
#endif

    rg_recv_a11.data = fi_ahb_read(RG_RECV_A11);
    rg_recv_a11.b.rg_rxirr_man_mode = 0x1;
    rg_recv_a11.b.rg_rxirr_bypass = 0x1;
    fi_ahb_write(RG_RECV_A11, rg_recv_a11.data);

    if ((p_chn >= 1) && (p_chn <= 14))
    {
        //switch to 2.4G
        /* rf_write_register(0xff000008, 0x01393900); */
        rg_top_a2.data = rf_read_register(RG_TOP_A2);
        rg_top_a2.b.RG_WF_RF_MODE = 0x1;
        rf_write_register(RG_TOP_A2, rg_top_a2.data);

        t9026_rf_mode_access(WF2G_SX);
        t9026_wf2g_sx(p_chn, 0x3, CHANNEL_MAP | PCODE_CALI | TCODE_CALI | MXR_TANK);
        t9026_rx_cw_remove(p_chn);
    } else {
        //switch to 5G
        /* rf_write_register(0xff000008, 0x00393900); */
        rg_top_a2.data = rf_read_register(RG_TOP_A2);
        rg_top_a2.b.RG_WF_RF_MODE = 0x0;
        rf_write_register(RG_TOP_A2, rg_top_a2.data);

        t9026_rf_mode_access(WF5G_SX);
        t9026_wf5g_sx(p_chn, 0x3, CHANNEL_MAP | PCODE_CALI | TCODE_CALI | MXR_TANK);
        t9026_rx_cw_remove(p_chn);
    }

    if (restore) {
        rf_channel_restore(p_chn, bw);
    }
    t9026_bw_switch(bw);
    t9026_rf_mode_auto();
}

#else

void t9026_channel_switch(int p_chn, unsigned char bw, unsigned char restore)
{
    RG_TOP_A2_FIELD_T    rg_top_a2;
    if ((p_chn >= 1) && (p_chn <= 14)) {
        //switch to 2.4G
        /* rf_write_register(0xff000008, 0x01393900); */
        rg_top_a2.data = rf_read_register(RG_TOP_A2);
        rg_top_a2.b.RG_WF_RF_MODE = 0x1;
        rf_write_register(RG_TOP_A2, rg_top_a2.data);

        t9026_rf_mode_access(WF2G_SX);
        t9026_wf2g_sx(p_chn, 0x3, CHANNEL_MAP|PCODE_CALI|TCODE_CALI|MXR_TANK);
        change_rf_2g_param_when_switch_channel(&wf2g_calb_result, bw, 0);
        t9026_rx_cw_remove(p_chn);
    } else {
        //switch to 5G
        /* rf_write_register(0xff000008, 0x00393900); */
        rg_top_a2.data = rf_read_register(RG_TOP_A2);
        rg_top_a2.b.RG_WF_RF_MODE = 0x0;
        rf_write_register(RG_TOP_A2, rg_top_a2.data);

        t9026_rf_mode_access(WF5G_SX);
        t9026_wf5g_sx(p_chn, 0x3, CHANNEL_MAP|PCODE_CALI|TCODE_CALI|MXR_TANK);
        change_rf_5g_param_when_switch_channel(&wf5g_calb_result, bw, 0, 0);
        t9026_rx_cw_remove(p_chn);
    }

    t9026_bw_switch(bw);
    t9026_rf_mode_auto();
}

#endif
