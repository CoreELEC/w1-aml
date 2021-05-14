
#include "rf_calibration.h"
#include "wifi_hal_com.h"
#include "wifi_mac.h"
#include "dpd_memory_packet.h"
#ifdef RF_CALI_TEST
extern void hal_dpd_memory_download(void);

int t9026_dpd_tx_func(void *param)
{
        struct hal_private *hal_priv = hal_get_priv();
        int len = 0;
        struct NullDataCmd null_data;
        unsigned int pwr_save = 0;
        unsigned int pwr_flag = 0;
        unsigned int ac = 0;
        unsigned char phony_macaddr[] = {0x00,0x22,0x33, 0x44, 0x55,0x66};
        unsigned int   phony_sn = 0;

        memset(&null_data, 0, sizeof(struct NullDataCmd));

        null_data.vid = 0;
        null_data.pwr_save= pwr_save;
        null_data.pwr_flag = pwr_flag;
        null_data.staid =1 ;

        null_data.bw = CHAN_BW_40M;//CHAN_BW_40M;
        null_data.rate = WIFI_11AC_MCS0;//WIFI_11AC_MCS5;
        null_data.qos = 1;
        null_data.tid = WME_AC_TO_TID(ac);
        len = sizeof(struct wifi_qos_frame) + FCS_LEN;
        memcpy(&null_data.dest_addr, phony_macaddr, WIFINET_ADDR_LEN);

        while(hal_priv->hi_dpd_tx_thread_quit == 0 )
        {
            if(phony_sn > 4095)
            {
                phony_sn = 0;
            }
            null_data.sn =phony_sn;
            phony_sn++;

            hal_priv->hal_ops.phy_send_null_data(null_data, len);
            udelay(2);
        }

        RF_DPRINTF(RF_DEBUG_INFO, "t9026_dpd_tx_thread stop, last sn %d\n", null_data.sn);
        complete_and_exit(&hal_priv->hi_dpd_tx_thread_completion, 0);
        return 0;
}

void t9026_ofdm_send_packet(void)
{
#if 0
    //t9023_tx_ofdm(2, bw_idx, 0x80, 1000000, 1000);
     struct hal_private *hal_priv = hal_get_priv();
     hal_priv->hi_dpd_tx_thread_quit = 0;

     RF_DPRINTF(RF_DEBUG_INFO, "start dpd thread...\n");
     hal_priv->hi_dpd_tx_thread = kthread_run(t9026_dpd_tx_func, hal_priv, "t9026_dpd_tx_thread");
#else
    if (0)
    {
        int len = 0;
        unsigned int * d_ptr;
        int i =0;
        unsigned int addr;

        //set ram share
        fi_ahb_write(0x00a0d0e4, 0x8000007f);
        //read date and save
        d_ptr = (unsigned int *)MEMDATA;
	 len = sizeof(MEMDATA)/sizeof(int);
	 ASSERT(d_ptr);
	 ASSERT(len >0);
	 printk("%s(%d): memory pkt len %d\n", __func__, __LINE__, len);

	 for(i = 0; i < len; i++)
	 {
	    addr = 0x00b3F000+ i * sizeof(unsigned int);
	    fi_ahb_write(addr, *d_ptr);
	    d_ptr++;
	 }
	 printk("d_ptr is 0x%x, end addr is 0x%x\n", *(d_ptr-1), addr);
    }else{

        hal_dpd_memory_download();
    }
    //start
    printk("SendPkt start!!\n");
    //fi_ahb_write(0x00a0b000, 0xffffffff);
    fi_ahb_write(0x00a0b010, 0x00000020);
    fi_ahb_write(0x00a0b1a4, 0x0003F000);
    fi_ahb_write(0x00a0b1a0, 0xa013fffc);
    fi_ahb_write(0x00a0b004, 0x10000000);
#endif
}

void t9026_ofdm_stop_packet(void)
{
#if 0
    struct hal_private *hal_priv = hal_get_priv();

    if(hal_priv->hi_dpd_tx_thread )
    {
        init_completion(&hal_priv->hi_dpd_tx_thread_completion);
        hal_priv->hi_dpd_tx_thread_quit = 1 ;
        kthread_stop(hal_priv->hi_dpd_tx_thread);
        wait_for_completion(&hal_priv->hi_dpd_tx_thread_completion);
        hal_priv->hi_dpd_tx_thread = NULL;

        RF_DPRINTF(RF_DEBUG_INFO, "stop dpd thread...\n");
    }
#else
    printk("SendPkt stop!!\n");
    fi_ahb_write(0x00a0b010, 0x00000000);
    fi_ahb_write(0x00a0b004, 0x00000000);
    fi_ahb_write(0x00a0b1a0, 0x0003fffc);
    fi_ahb_write(0x00a0d0e4, 0x0000007f);
#endif
}

#endif //# RF_CALI_TEST
