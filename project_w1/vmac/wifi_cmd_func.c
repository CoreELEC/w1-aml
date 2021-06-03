#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include "wifi_mac_com.h"
#include "wifi_cmd_func.h"
#include "wifi_drv_reg_ops.h"
#include "wifi_cfg80211.h"
#include "wifi_mac_if.h"

struct udp_info aml_udp_info[32];
int udp_cnt = 0;
struct udp_timer aml_udp_timer;
extern struct _B2B_Test_Case_Packet gB2BTestCasePacket;

cmd_to_func_table_t cmd_to_func[] =
{
    {"set_chip_id", aml_set_chip_id},
    {"get_chip_id", aml_get_chip_id},
    {"set_mac_addr", aml_set_mac_addr},
    {"get_mac_addr", aml_get_mac_addr},
    {"get_wifi_mac", aml_get_wifi_mac_addr},
    {"set_bt_dev_id", aml_set_bt_device_id},
    {"get_bt_dev_id", aml_get_bt_device_id},
    {"sta_send_bareq", aml_sta_send_addba_req},
    {"send_bss_coex", aml_sta_send_coexist_mgmt},
    {"get_txaggr_sta", aml_get_drv_txaggr_status},
    {"sta_get_session", aml_sta_get_wfd_session},
    {"wmm_ac_addts",  aml_wmm_ac_addts},
    {"wmm_ac_delts",  aml_wmm_ac_delts},
#ifdef CONFIG_P2P
    {"get_p2p_dev_id", aml_get_p2p_device_addr},
#endif
    {"set_amsdu", aml_set_mac_amsdu},
    {"set_ampdu", aml_set_drv_ampdu},
    {"update_wmm_arg", aml_update_wmm_arg},
    {"set_dynamic_bw", aml_set_dynamic_bw},
    {"set_short_gi", aml_set_short_gi},
    {"set_eat_count", aml_set_eat_count_max},
    {"set_aggr_thresh", aml_set_aggr_thresh},
    {"set_hrt_int", aml_set_hrtimer_interval},
    {"get_ap_ip", aml_get_ap_ip},
    {"set_roam_thr_2g", aml_set_roaming_threshold_2g},//wpa_cli driver set_roam_thr_2g -80
    {"set_roam_thr_5g", aml_set_roaming_threshold_5g},
    {"get_roam_chan", aml_get_roaming_candidate_chans},
    {"set_roam_chan", aml_set_roaming_candidate_chans},
    {"set_roam_mode", aml_set_roaming_mode},
    {"set_udp_info", aml_set_udp_info},
    {"get_udp_info", aml_get_udp_info},
    {"mark_dfs_chan",aml_mark_dfs_channel},
    {"unmark_dfs_chan",aml_unmark_dfs_channel},
    {"set_dev_sn", aml_set_device_sn},
    {"get_dev_sn", aml_get_device_sn},
    {"set_weak_thr_nb", aml_set_signal_power_weak_thresh_for_narrow_bandwidth},
    {"set_weak_thr_wb", aml_set_signal_power_weak_thresh_for_wide_bandwidth},
    {"", NULL},
};

//Returns a char * arr [] and size is the length of the returned array
char **aml_cmd_char_prase(char sep, const char *str, int *size)
{
    int count = 0;
    int i;
    char **ret;
    int lastindex = -1;
    int j = 0;

    for (i = 0; i < strlen(str); i++) {
        if (str[i] == sep) {
            count++;
        }
    }

    ret = (char **)kzalloc((++count) * sizeof(char *), GFP_KERNEL);

    for (i = 0; i < strlen(str); i++) {
        if (str[i] == sep) {
            // kzalloc the memory space of substring length + 1
            ret[j] = (char *)kzalloc((i - lastindex) * sizeof(char), GFP_KERNEL);
            memcpy(ret[j], str + lastindex + 1, i - lastindex - 1);
            j++;
            lastindex = i;
        }
    }
    //Processing the last substring
    if (lastindex <= strlen(str) - 1) {
        ret[j] = (char *)kzalloc((strlen(str) - lastindex) * sizeof(char), GFP_KERNEL);
        memcpy(ret[j], str + lastindex + 1, strlen(str) - 1 - lastindex);
        j++;
    }

    *size = j;

    return ret;
}

/*
*  cmd: sta_send_bareq 11:22:33:44:55:66 5
*/
int aml_sta_send_addba_req(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    int skip = 0;
    char mac[18];
    int tid = 0;

    skip = strlen("sta_send_bareq") + 1;
    sscanf(buf + skip, "%s %d", mac, &tid);
    wifi_mac_send_addba_req(buf + skip, tid);
    return 0;
}

int aml_set_chip_id(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    int i,cmd_arg;
    char sep = ' ';

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if (arg) {
        for (i = 0; i < 32; i++) {
            if (simple_strtoul(arg[2],NULL,16) & (1 << i)) {
                efuse_manual_write(i, 8);
            }
        }
        for (i = 0; i < 16; i++) {
            if (simple_strtoul(arg[1],NULL,16) & (1 << i)) {
                efuse_manual_write(i, 9);
            }
        }
        printk("write chip_id is :%04x%08x\n", simple_strtoul(arg[1],NULL,16) & 0xffff,
            simple_strtoul(arg[2],NULL,16));
    }
    kfree(arg);
    return 0;
}

int aml_get_chip_id(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    int cmd_arg;
    char sep = ' ';
    unsigned int efuse_data_l = 0;
    unsigned int efuse_data_h = 0;

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if (arg) {
        efuse_data_l = efuse_manual_read(simple_strtoul(arg[1],NULL,16));
        efuse_data_h = efuse_manual_read(simple_strtoul(arg[2],NULL,16));

        printk("efuse addr:%08x,%08x, chip_id is :%04x%08x\n", simple_strtoul(arg[1],NULL,16),
            simple_strtoul(arg[2],NULL,16), efuse_data_h & 0xffff, efuse_data_l);
        kfree(arg);
    }else {
        printk("efuse addr:%08x, get efuse data error\n", *arg);
        kfree(arg);
    }
    return 0;
}

int aml_set_mac_addr(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char **mac_cmd;
    int i,cmd_arg;
    char sep = ' ';
    unsigned int efuse_data_l = 0;
    unsigned int efuse_data_h = 0;

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if (arg) {
        sep = ':';
        mac_cmd = aml_cmd_char_prase(sep, arg[1], &cmd_arg);
        if (mac_cmd) {
            efuse_data_l = (simple_strtoul(mac_cmd[2],NULL,16) << 24) | (simple_strtoul(mac_cmd[3],NULL,16) << 16)
                           | (simple_strtoul(mac_cmd[4],NULL,16) << 8) | simple_strtoul(mac_cmd[5],NULL,16);
            efuse_data_h = (simple_strtoul(mac_cmd[0],NULL,16) << 8) | (simple_strtoul(mac_cmd[1],NULL,16));
            for (i = 0; i < 32; i++) {
                if (efuse_data_l & (1 << i)) {
                    efuse_manual_write(i, 1);
                }
            }
            for (i = 0; i < 16; i++) {
                if (efuse_data_h & (1 << i)) {
                    efuse_manual_write(i, 2);
                }
            }
            printk("write MAC addr is:  %02x:%02x:%02x:%02x:%02x:%02x\n",
                    (efuse_data_h & 0xff00) >> 8,efuse_data_h & 0x00ff, (efuse_data_l & 0xff000000) >> 24,
                    (efuse_data_l & 0x00ff0000) >> 16,(efuse_data_l & 0xff00) >> 8,efuse_data_l & 0xff);
        }
        kfree(mac_cmd);
    }
    kfree(arg);
    return 0;
}

int aml_get_mac_addr(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    int cmd_arg;
    char sep = ' ';
    unsigned int efuse_data_l = 0;
    unsigned int efuse_data_h = 0;

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if (arg) {
        efuse_data_l = efuse_manual_read(simple_strtoul(arg[1],NULL,16));
        efuse_data_h = efuse_manual_read(simple_strtoul(arg[2],NULL,16));
        printk("efuse addr:%08x,%08x, MAC addr is:  %02x:%02x:%02x:%02x:%02x:%02x\n",
                simple_strtoul(arg[1],NULL,16), simple_strtoul(arg[2],NULL,16),
                (efuse_data_h & 0xff00) >> 8,efuse_data_h & 0x00ff, (efuse_data_l & 0xff000000) >> 24,
                (efuse_data_l & 0x00ff0000) >> 16,(efuse_data_l & 0xff00) >> 8,efuse_data_l & 0xff);
        kfree(arg);
    }else {
        printk("efuse addr:%08x, get mac addr error\n", *arg);
        kfree(arg);
    }

    return 0;
}
int aml_get_wifi_mac_addr(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    int bytes_written = 0;

    bytes_written = snprintf(buf, len, MAC_FMT, MAC_ARG(wnet_vif->vm_myaddr));
    printk("%s,buf:  %s\n", __func__, buf);
    return bytes_written;
}

int aml_set_bt_device_id(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char **mac_cmd;
    int i,cmd_arg;
    char sep = ' ';
    unsigned int efuse_data_l = 0;
    unsigned int efuse_data_h = 0;

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if (arg) {
        sep = ':';
        mac_cmd = aml_cmd_char_prase(sep, arg[1], &cmd_arg);
        if (mac_cmd) {
            efuse_data_h = (simple_strtoul(mac_cmd[0],NULL,16) << 24) | (simple_strtoul(mac_cmd[1],NULL,16) << 16)
                           | (simple_strtoul(mac_cmd[2],NULL,16) << 8) | simple_strtoul(mac_cmd[3],NULL,16);
            efuse_data_l = (simple_strtoul(mac_cmd[4],NULL,16) << 24) | (simple_strtoul(mac_cmd[5],NULL,16) << 16);
            for (i = 0; i < 32; i++) {
                if (efuse_data_h & (1 << i)) {
                    efuse_manual_write(i, 3);
                }
            }
            for (i = 16; i < 32; i++) {
                if (efuse_data_l & (1 << i)) {
                    efuse_manual_write(i, 2);
                }
            }
            printk("write BT device id  is:  %02x:%02x:%02x:%02x:%02x:%02x\n",
                    (efuse_data_h & 0xff000000) >> 24, (efuse_data_h & 0x00ff0000) >> 16,
                    (efuse_data_h & 0xff00) >> 8,efuse_data_h & 0xff,
                    (efuse_data_l & 0xff000000) >> 24,(efuse_data_l & 0x00ff0000) >> 16);
        }
        kfree(mac_cmd);
    }
    kfree(arg);
    return 0;

}

int aml_get_bt_device_id(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    int cmd_arg;
    char sep = ' ';
    unsigned int efuse_data_l = 0;
    unsigned int efuse_data_h = 0;

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if (arg) {
        efuse_data_l = efuse_manual_read(simple_strtoul(arg[1],NULL,16));
        efuse_data_h = efuse_manual_read(simple_strtoul(arg[2],NULL,16));
        printk("efuse addr:%08x,%08x, get BT device id is:  %02x:%02x:%02x:%02x:%02x:%02x\n",
                simple_strtoul(arg[1],NULL,16), simple_strtoul(arg[2],NULL,16),
                (efuse_data_h & 0xff000000) >> 24, (efuse_data_h & 0x00ff0000) >> 16,
                (efuse_data_h & 0xff00) >> 8,efuse_data_h & 0xff,
                (efuse_data_l & 0xff000000) >> 24,(efuse_data_l & 0x00ff0000) >> 16);
        kfree(arg);
    }else {
        printk("efuse addr:%08x, get BT device id error\n", *arg);
        kfree(arg);
    }

    return 0;
}


int aml_sta_send_coexist_mgmt(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    int skip = 0;

    skip = strlen("send_bss_coex") + 1;
    wifi_mac_send_coexist_mgmt(buf + skip);
    return 0;
}

int aml_get_drv_txaggr_status(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char sep = ' ';
    int i, j, cmd_arg;
    unsigned int nframes;
    struct drv_private *drv_priv = drv_get_drv_priv();

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);

    //get queue-0 statistics
    if (likely(!arg[1])) {
        i = 0;
        for (j = 0; j < 16; j++) {
            nframes = drv_priv->drv_txlist_table[i].tx_aggr_status[j];
            printk("Q:%d; aggr %d packets times: %d\n", i, j + 1, nframes);
        }
        printk("\n");
    } else {
        //get all statistics
        if (strnicmp(arg[1], "all", strlen("all")) == 0) {
            for (i = 0; i < HAL_NUM_TX_QUEUES; i++) {
                for (j = 0; j < 16; j++) {
                    nframes = drv_priv->drv_txlist_table[i].tx_aggr_status[j];
                    printk("Q:%d; aggr %d packets times: %d\n", i, j + 1, nframes);
                }
                printk("\n");
            }
        }
        //reset
        if (strnicmp(arg[1], "reset", strlen("reset")) == 0) {
            for (i = 0; i < HAL_NUM_TX_QUEUES; i++) {
                for (j = 0; j < 16; j++) {
                    drv_priv->drv_txlist_table[i].tx_aggr_status[j] = 0;
                }
            }
        }
    }

    return 0;
}

int aml_sta_get_wfd_session(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    printk("%s, wfd_session_id=%s\n", __func__, wnet_vif->vm_p2p->wfd_session_id);
    return 0;
}

int aml_get_p2p_device_addr(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    int bytes_written = 0;

    memcpy(buf, wnet_vif->vm_ndev->dev_addr, ETH_ALEN);
    printk("%s,p2p_device_addr= %02x:%02x:%02x:%02x:%02x:%02x\n", __func__, *buf, *(buf + 1), *(buf + 2),
           *(buf + 3), *(buf + 4), *(buf + 5));

    bytes_written = ETH_ALEN;
    return bytes_written;

}


int aml_wmm_ac_addts(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    int skip = 0, cmd_arg;
    char **arg;
    char sep = ' ';
    skip = strlen("wmm_ac_addts") + 1;
    arg = aml_cmd_char_prase(sep, buf + skip, &cmd_arg);
    wifi_mac_send_wmm_ac_addts(arg);
    return 0;
}

int aml_wmm_ac_delts(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    int skip = 0;
    skip = strlen("wmm_ac_delts") + 1;
    wifi_mac_send_wmm_ac_delts(buf + skip);
    return 0;
}

void aml_set_mac_amsdu_switch_state(char* arg)
{
    struct drv_private *drv_priv = drv_get_drv_priv();

    if (!arg) {
        if (drv_priv->drv_config.cfg_txamsdu) {
            printk("AMSDU is ON.\n");
        }
        else {
            printk("AMSDU is OFF.\n");
        }
    }
    else {
        if (strnicmp(arg, "on", strlen("on")) == 0) {
            drv_priv->drv_config.cfg_txamsdu = 1;
            printk("AMSDU set ON!\n");

        }
        else if (strnicmp(arg, "off", strlen("off")) == 0) {
            drv_priv->drv_config.cfg_txamsdu = 0;
            printk("AMSDU set OFF!\n");

        }
        else {
            if (drv_priv->drv_config.cfg_txamsdu) {
                printk("AMSDU is ON.\n");
            }
            else {
                printk("AMSDU is OFF.\n");
            }
        }
    }
}

void aml_set_drv_ampdu_switch_state(char* arg)
{
    struct drv_private *drv_priv = drv_get_drv_priv();

    if (!arg) {
        if (drv_priv->drv_config.cfg_txaggr) {
            printk("AMPDU is ON.\n");
        }
        else {
            printk("AMPDU is OFF.\n");
        }
    }
    else {
        if (strnicmp(arg, "on", strlen("on")) == 0) {
            drv_priv->drv_config.cfg_txaggr = 1;
            printk("AMPDU set ON!\n");
        }
        else if (strnicmp(arg, "off", strlen("off")) == 0) {
            drv_priv->drv_config.cfg_txaggr = 0;
            printk("AMPDU set OFF!\n");

        }
        else {
            if (drv_priv->drv_config.cfg_txaggr) {
                printk("AMPDU is ON.\n");
            }
            else {
                printk("AMPDU is OFF.\n");
            }
        }
    }
}

int aml_set_mac_amsdu(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char sep = ' ';
    int cmd_arg;

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);

    aml_set_mac_amsdu_switch_state(arg[1]);

    kfree(arg);
    return 0;
}

static struct wifi_channel * wifi_mac_find_chan_unlock( struct wifi_mac *wifimac, unsigned short cur_chan,
    enum wifi_mac_bwc_width cur_bw, unsigned short cur_freq)
{
    struct wifi_channel *c = NULL;
    int i = 0;

    for (i = 0; i < wifimac->wm_nchans; i++) {
        c = &wifimac->wm_channels[i];
        if ((c->chan_pri_num== cur_chan) && (c->chan_bw == cur_bw) && (c->chan_cfreq1 == cur_freq)) {
            DPRINTF(AML_DEBUG_BWC, "%s(%d) !!!: find a chan=%d, bw=%d and freq=%d.\n", __func__, __LINE__, cur_chan, cur_bw, cur_freq);
            return c;
        }
    }

    DPRINTF(AML_DEBUG_BWC, "%s(%d) !!!: NOT find a chan=%d, bw=%d and freq=%d.\n", __func__, __LINE__, cur_chan, cur_bw, cur_freq);
    return NULL;
}

void wifi_mac_set_country_code(char* arg)
{
    struct wifi_mac *wifimac = wifi_mac_get_mac_handle();
    struct drv_private* drv_priv = wifimac->drv_priv;
    struct wlan_net_vif *selected_wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC];
    unsigned short cur_chan = 0;
    enum wifi_mac_bwc_width cur_bw = 0;
    unsigned short cur_freq = 0;

    if ((wifimac->wm_nrunning == 1) && (drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC]->vm_state == WIFINET_S_CONNECTED)) {
            selected_wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];
    }

    if  (wifimac->wm_nrunning > 1) {
        printk("Setting country_code in this mode is not yet supported!\n");
        return ;
    }

    printk("%s,%d, arg=%s\n", __func__, __LINE__, arg);
    if (arg && (arg[0] == wifimac->wm_country.iso[0]) && (arg[1] == wifimac->wm_country.iso[1])) {
        printk("%s no need to set country code due to the same country code\n", __func__);
        return;
    }

    preempt_scan(selected_wnet_vif->vm_ndev, 100, 100);

    WIFI_CHANNEL_LOCK(wifimac);

    if (wifimac->wm_nrunning == 1) {
        cur_chan = selected_wnet_vif->vm_curchan->chan_pri_num;
        cur_bw = selected_wnet_vif->vm_curchan->chan_bw;
        cur_freq = selected_wnet_vif->vm_curchan->chan_cfreq1;
    }

    wifi_mac_set_country(wifimac, arg);

    if (wifimac->wm_nrunning == 1) {
        selected_wnet_vif->vm_curchan = wifi_mac_find_chan_unlock(wifimac,cur_chan, cur_bw, cur_freq);

        if (!selected_wnet_vif->vm_curchan) {
            wifi_mac_top_sm(selected_wnet_vif, WIFINET_S_INIT,0);
        }
    }
    WIFI_CHANNEL_UNLOCK(wifimac);

    return;
}

void wifi_mac_ap_set_11h(unsigned char channel)
{
    struct wifi_mac *wifimac = wifi_mac_get_mac_handle();
    struct drv_private* drv_priv = wifimac->drv_priv;
    struct wlan_net_vif *selected_wnet_vif = NULL;

    if (wifimac->wm_nrunning == 1) {
        if (drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC]->vm_state == WIFINET_S_CONNECTED) {
            selected_wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];
        } else {
            selected_wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC];
        }
    } else {
        printk("%s, no sta connected\n", __func__);
        return;
    }

    wifimac->wm_doth_tbtt = 255; //simple_strtoul(*buf, NULL, 0);
    //wifimac->wm_doth_channel = simple_strtoul(*(buf + 1), NULL, 0);
    wifimac->wm_doth_channel = channel;
    printk("%s, wm_dott_tbtt=%d, wm_doth_channel=%d\n", __func__, wifimac->wm_doth_tbtt, wifimac->wm_doth_channel);
    wifimac->wm_flags |= WIFINET_F_CHANSWITCH;
    wifimac->wm_flags |= WIFINET_F_DOTH;
    selected_wnet_vif->vm_chanchange_count = 0;

    return;
}

void wifi_mac_ap_set_arp_rx(char** buf)
{
    struct wifi_mac *wifimac = wifi_mac_get_mac_handle();
    struct drv_private* drv_priv = wifimac->drv_priv;
    struct wlan_net_vif *selected_wnet_vif = NULL;

    if (wifimac->wm_nrunning == 1) {
        if (drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC]->vm_state == WIFINET_S_CONNECTED) {
            selected_wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];
        } else {
            selected_wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC];
        }
    } else {
        printk("%s, no sta connected\n", __func__);
        return;
    }
    selected_wnet_vif->vm_arp_rx.src_mac_addr[0] = simple_strtoul(*buf, NULL, 0);
    selected_wnet_vif->vm_arp_rx.src_mac_addr[1] = simple_strtoul(*(buf+1), NULL, 0);
    selected_wnet_vif->vm_arp_rx.src_mac_addr[2] = simple_strtoul(*(buf+2), NULL, 0);
    selected_wnet_vif->vm_arp_rx.src_mac_addr[3] = simple_strtoul(*(buf+3), NULL, 0);
    selected_wnet_vif->vm_arp_rx.src_mac_addr[4] = simple_strtoul(*(buf+4), NULL, 0);
    selected_wnet_vif->vm_arp_rx.src_mac_addr[5] = simple_strtoul(*(buf+5), NULL, 0);
    selected_wnet_vif->vm_arp_rx.src_ip_addr[0] = simple_strtoul(*(buf+6), NULL, 0);
    selected_wnet_vif->vm_arp_rx.src_ip_addr[1] = simple_strtoul(*(buf+7), NULL, 0);
    selected_wnet_vif->vm_arp_rx.src_ip_addr[2] = simple_strtoul(*(buf+8), NULL, 0);
    selected_wnet_vif->vm_arp_rx.src_ip_addr[3] = simple_strtoul(*(buf+9), NULL, 0);
    selected_wnet_vif->vm_arp_rx.out = simple_strtoul(*(buf+10), NULL, 0);
    printk("set mac=%02x:%02x:%02x:%02x:%02x:%02x\n", selected_wnet_vif->vm_arp_rx.src_mac_addr[0],selected_wnet_vif->vm_arp_rx.src_mac_addr[1],
           selected_wnet_vif->vm_arp_rx.src_mac_addr[2], selected_wnet_vif->vm_arp_rx.src_mac_addr[3], selected_wnet_vif->vm_arp_rx.src_mac_addr[4], selected_wnet_vif->vm_arp_rx.src_mac_addr[5]);
    printk("set ip=%d.%d.%d.%d\n", selected_wnet_vif->vm_arp_rx.src_ip_addr[0], selected_wnet_vif->vm_arp_rx.src_ip_addr[1],
           selected_wnet_vif->vm_arp_rx.src_ip_addr[2],selected_wnet_vif->vm_arp_rx.src_ip_addr[3]);
    wifi_mac_set_arp_rsp(selected_wnet_vif);
    return;
}


int aml_set_drv_ampdu(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char sep = ' ';
    int cmd_arg;

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);

    aml_set_drv_ampdu_switch_state(arg[1]);

    kfree(arg);
    return 0;
}
int aml_update_wmm_arg(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char **param;
    int cmd_arg, cmd_param;
    char sep = ' ';
    int ac = 0;
    int i = 0;
    struct wifi_mac_wme_state *wme = &wnet_vif->vm_wmac->wm_wme[wnet_vif->wnet_vif_id];

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);

    if ((wme != NULL) && (arg[1] != NULL)) {
        ac = simple_strtoul(arg[1], NULL, 0);
        if ((ac < WME_NUM_AC) && (arg[2] != NULL)) {
            struct wmeParams *wmep = &wme->wme_wmeChanParams.cap_wmeParams[ac];

            for (i = 2; i < cmd_arg; i++) {
                sep = ':';
                param = aml_cmd_char_prase(sep, arg[i], &cmd_param);
                if (strnicmp(arg[i], "acm", strlen("acm")) == 0) {
                    wmep->wmep_acm = simple_strtoul(param[1], NULL, 0);

                } else if (strnicmp(arg[i], "aifsn", strlen("aifsn")) == 0) {
                    wmep->wmep_aifsn = simple_strtoul(param[1], NULL, 0);

                } else if (strnicmp(arg[i], "cwmin", strlen("cwmin")) == 0) {
                    wmep->wmep_logcwmin = simple_strtoul(param[1], NULL, 0);

                } else if (strnicmp(arg[i], "cwmax", strlen("cwmax")) == 0) {
                    wmep->wmep_logcwmax = simple_strtoul(param[1], NULL, 0);

                } else if (strnicmp(arg[i], "limit", strlen("limit")) == 0) {
                    wmep->wmep_txopLimit = simple_strtoul(param[1], NULL, 0);

                } else if (strnicmp(arg[i], "noackPolicy", strlen("noackPolicy")) == 0) {
                    wmep->wmep_noackPolicy = simple_strtoul(param[1], NULL, 0);
                } else {
                    printk("Parameter [%s] is error.\n", arg[i]);
                }
                kfree(param);
            }
        }
        else {
            printk("Parameter is error.\n");
            kfree(arg);
            return 0;
        }

        wifi_mac_wme_updateparams(wnet_vif);
    }
    kfree(arg);
    return 0;
}

int aml_set_dynamic_bw(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char sep = ' ';
    int cmd_arg;
    int data = 0;
    struct drv_private *drv_priv = drv_get_drv_priv();

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if ((drv_priv != NULL) && (arg[1] != NULL)) {
        if (strnicmp(arg[1], "on", strlen("on")) == 0) {
            data = 1;
            printk("dynamic_bw set ON!\n");

        } else if (strnicmp(arg[1], "off", strlen("off")) == 0) {
            data = 0;
            printk("dynamic_bw set OFF!\n");

        } else {
            printk("Parameter is error,now cfg_dynamic_bw=%d.\n", drv_priv->drv_config.cfg_dynamic_bw);
            kfree(arg);
            return 0;
        }
        drv_set_config((void *)drv_priv, CHIP_PARAM_DYNAMIC_BW, data);
    } else {
        printk("Parameter is error.\n");
    }

    kfree(arg);
    return 0;
}

int aml_set_short_gi(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    int usr_data;
    char **arg;
    char sep = ' ';
    int cmd_arg;

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if (!arg[1]) {
        printk("%s--Bad parameter\n", __func__);
        return 0;
    }
    printk("%s,cmd param: %s, %s\n", __func__, arg[0], arg[1]);

    if (strnicmp(arg[1], "on", strlen("on")) == 0) {
        usr_data = 1;
        printk("%s: enable short GI.\n", __func__);
    } else if (strnicmp(arg[1], "off", strlen("off")) == 0) {
        usr_data = 0;
        printk("%s: disable short GI.\n", __func__);
    } else {
        printk("%s--Bad parameter\n", __func__);
        return 0;
    }

#ifdef DRV_PT_SUPPORT
    gB2BTestCasePacket.if_shortGI = usr_data;
#else
    if (1 == usr_data) {
        wnet_vif->vm_wmac->wm_flags_ext |= WIFINET_FEXT_SHORTGI_ENABLE;
        printk("%s: enable short GI done\n", __func__);

    } else {
        wnet_vif->vm_wmac->wm_flags_ext &= ~WIFINET_FEXT_SHORTGI_ENABLE;
        printk("%s: disable short GI done.\n", __func__);
    }
#endif

    return 0;
}

int aml_set_eat_count_max(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char sep = ' ';
    int cmd_arg;
    int data = 0;
    struct drv_private *drv_priv = drv_get_drv_priv();

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if ((drv_priv != NULL) && (arg[1] != NULL)) {
        data = simple_strtoul(arg[1], NULL, 0);
        drv_set_config((void *)drv_priv, CHIP_PARAM_EAT_COUNT, data);

    } else {
        printk("Parameter is error.\n");
    }

    kfree(arg);
    return 0;
}

int aml_set_aggr_thresh(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char sep = ' ';
    int cmd_arg;
    int data = 0;
    struct drv_private *drv_priv = drv_get_drv_priv();

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if ((drv_priv != NULL) && (arg[1] != NULL)) {
        data = simple_strtoul(arg[1], NULL, 0);
        drv_set_config((void *)drv_priv, CHIP_PARAM_AGGR_THRESH, data);

    } else {
        printk("Parameter is error.\n");
    }

    kfree(arg);
    return 0;
}

int aml_set_hrtimer_interval(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char sep = ' ';
    int cmd_arg;
    int data = 0;
    struct drv_private *drv_priv = drv_get_drv_priv();

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if ((drv_priv != NULL) && (arg[1] != NULL)) {
        data = simple_strtoul(arg[1], NULL, 0);
        drv_set_config((void *)drv_priv, CHIP_PARAM_HRTIMER_INTERVAL, data);

    } else {
        printk("Parameter is error.\n");
    }

    kfree(arg);
    return 0;
}

int aml_get_ap_ip(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    if (!(wnet_vif->vm_mainsta->sta_arp_flag & WIFI_ARP_GET_AP_IP)) {
        wifi_mac_send_arp_req(wnet_vif);

    } else {
        printk("ap ip is:%d:%d:%d:%d\n", wnet_vif->vm_mainsta->sta_ap_ip[0],
            wnet_vif->vm_mainsta->sta_ap_ip[1], wnet_vif->vm_mainsta->sta_ap_ip[2], wnet_vif->vm_mainsta->sta_ap_ip[3]);
    }

    return 0;
}


int aml_set_roaming_threshold_2g(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char sep = ' ';
    int cmd_arg;
    int data = 0;

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if ((wnet_vif != NULL)&& (wnet_vif->vm_wmac != NULL) && (arg[1] != NULL)) {
        data = simple_strtol(arg[1], NULL, 0);
        wnet_vif->vm_wmac->roaming_threshold_2g = data;
        printk("vm_wmac->roaming_threshold_2g:%d\n", wnet_vif->vm_wmac->roaming_threshold_2g);

    } else {
        printk("Parameter is error.\n");
    }

    kfree(arg);
    return 0;
}

int aml_set_roaming_threshold_5g(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char sep = ' ';
    int cmd_arg;
    int data = 0;

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if ((wnet_vif != NULL) && (wnet_vif->vm_wmac != NULL) && (arg[1] != NULL)) {
        data = simple_strtol(arg[1], NULL, 0);
        wnet_vif->vm_wmac->roaming_threshold_5g = data;
        printk("vm_wmac->roaming_threshold_5g:%d\n", wnet_vif->vm_wmac->roaming_threshold_5g);

    } else {
        printk("Parameter is error.\n");
    }

    kfree(arg);
    return 0;
}



int aml_get_roaming_candidate_chans(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    int i = 0;
    struct wifi_channel * chan = NULL;
    int chan_cnt = wnet_vif->vm_wmac->wm_scan->roaming_candidate_chans_cnt;

    printk("get roaming candidate chans [%d]:\n", chan_cnt);

    for (i=0; i < chan_cnt; i++) {
        chan = wnet_vif->vm_wmac->wm_scan->roaming_candidate_chans[i].channel;

        if (chan == NULL) {
            printk("erro pointer \n");
            return 0;
        }
        printk("chan_cfreq1:%d chan_flags:%d chan_pri_num:%d chan_maxpower:%d chan_minpower:%d chan_bw:%d global_operating_class %d Rssi:%d\n",
            chan->chan_cfreq1, chan->chan_flags, chan->chan_pri_num, chan->chan_maxpower,
            chan->chan_minpower, chan->chan_bw, chan->global_operating_class,
            (wnet_vif->vm_wmac->wm_scan->roaming_candidate_chans[i].avg_rssi - 256));
    }

    return 0;
}

int aml_set_roaming_candidate_chans(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char sep = ' ';
    int cmd_arg;
    int data = 0;
    int i = 0;
    int j = 0;
    struct wifi_channel *c;
    struct wifi_mac *wifimac = wnet_vif->vm_wmac;
    struct wifi_mac_scan_state *ss = wifimac->wm_scan;

    if ((wnet_vif == NULL) || (wifimac == NULL)) {
        return 0;
    }

    WIFI_ROAMING_CHANNLE_LOCK(wnet_vif->vm_wmac->wm_scan);

    wnet_vif->vm_wmac->wm_scan->roaming_candidate_chans_cnt = 0;
    memset(wnet_vif->vm_wmac->wm_scan->roaming_candidate_chans, 0, sizeof(wnet_vif->vm_wmac->wm_scan->roaming_candidate_chans));

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    for (i = 1; i < ROAMING_CANDIDATE_CHAN_MAX; i++) {
        if (arg[i] != NULL) {
            data = simple_strtoul(arg[i], NULL, 0);
            printk("get channel:%d \n",data);
            WIFI_CHANNEL_LOCK(wifimac);
            for (j = 0; j < wifimac->wm_nchans; j++) {
                c = &wifimac->wm_channels[j];
                if (c->chan_bw == WIFINET_BWC_WIDTH20 && c->chan_pri_num == data) {
                    ss->roaming_candidate_chans[ss->roaming_candidate_chans_cnt].channel = c;
                    ss->roaming_candidate_chans[ss->roaming_candidate_chans_cnt].avg_rssi = 156;
                    ss->roaming_candidate_chans_cnt++;
                }
            }
            WIFI_CHANNEL_UNLOCK(wifimac);
        } else {
           WIFI_ROAMING_CHANNLE_UNLOCK(wnet_vif->vm_wmac->wm_scan);
           return 0;
        }
    }
    WIFI_ROAMING_CHANNLE_UNLOCK(wnet_vif->vm_wmac->wm_scan);

    return 0;
}


int aml_set_roaming_mode(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char sep = ' ';
    int cmd_arg;
    int data = 0;

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if ((wnet_vif != NULL) && (wnet_vif->vm_wmac != NULL) && (arg[1] != NULL)) {
        data = simple_strtoul(arg[1], NULL, 0);
        wnet_vif->vm_wmac->wm_roaming = data;
        printk("vm_wmac->wm_roaming:%d\n", wnet_vif->vm_wmac->wm_roaming);
    } else {
        printk("Parameter is error.\n");
    }

    return 0;
}

int wifi_mac_set_udp_info(char** buf)
{
    struct wifi_mac *wifimac = wifi_mac_get_mac_handle();
    struct drv_private* drv_priv = wifimac->drv_priv;
    struct wlan_net_vif *selected_wnet_vif = NULL;
    unsigned int param;
    if (wifimac->wm_nrunning == 1) {
        if (drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC]->vm_state == WIFINET_S_CONNECTED) {
            selected_wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];
        } else {
            selected_wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC];
        }
    } else {
        printk("%s, no sta connected\n", __func__);
        return -1;
    }

    aml_udp_info[udp_cnt].dst_port = simple_strtoul(*buf, NULL, 0);
    aml_udp_info[udp_cnt].src_port = simple_strtoul(*(buf + 1), NULL, 0);
    aml_udp_info[udp_cnt].dst_ip = simple_strtoul(*(buf + 2), NULL, 0);
    aml_udp_info[udp_cnt].src_ip = simple_strtoul(*(buf + 3), NULL, 0);
    aml_udp_info[udp_cnt].pkt_len = simple_strtoul(*(buf + 4), NULL, 0);
    param = simple_strtoul(*(buf + 5), NULL, 0);
    aml_udp_info[udp_cnt].out = param >> 8;
    aml_udp_info[udp_cnt].streamid = param & 0xff;
    aml_udp_info[udp_cnt].seq = 1000;
    aml_udp_info[udp_cnt].rx = 0;
    aml_udp_info[udp_cnt].tx = 0;
    aml_udp_timer.udp_timer_stop = 0;

    DPRINTF(AML_DEBUG_WARNING,"dst_port=%04x, src_port=%04x, dst_ip=%08x, src_ip:%08x, pkt_len:%d, out:%d, streamid:%d, param=%d\n",
        aml_udp_info[udp_cnt].dst_port, aml_udp_info[udp_cnt].src_port, aml_udp_info[udp_cnt].dst_ip, aml_udp_info[udp_cnt].src_ip,
        aml_udp_info[udp_cnt].pkt_len, aml_udp_info[udp_cnt].out, aml_udp_info[udp_cnt].streamid, param);

    if (aml_udp_info[udp_cnt].out && aml_udp_timer.run_flag == 0) {
        os_timer_ex_initialize(&aml_udp_timer.udp_send_timeout, 0, wifi_mac_udp_send_timeout_ex, selected_wnet_vif);
        os_timer_ex_start_period(&aml_udp_timer.udp_send_timeout, 10);
        aml_udp_timer.run_flag = 1;
    }
    udp_cnt++;
    return 0;
}

//set_udp_info dst_port src_port dst_ip in/out
int aml_set_udp_info(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    int skip = 0, cmd_arg;
    char **arg;
    char sep = ' ';
    skip = strlen("set_udp_info") + 1;
    arg = aml_cmd_char_prase(sep, buf + skip, &cmd_arg);
    wifi_mac_set_udp_info(arg);
    return 0;
}

int aml_get_udp_info(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    int i = 0;
    for (i = 0; i < udp_cnt; i++) {
        printk("%s streamid=%d tx is %d, rx is %d\n", __func__, aml_udp_info[i].streamid, aml_udp_info[i].tx, aml_udp_info[i].rx);
    }
    aml_udp_timer.udp_timer_stop = 1;
    aml_udp_timer.run_flag = 0;
    os_timer_ex_del(&aml_udp_timer.udp_send_timeout, CANCEL_SLEEP);
    return 0;
}

int aml_unmark_dfs_channel(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char sep = ' ';
    int cmd_arg;
    int data = 0;

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if ((wnet_vif != NULL) && (wnet_vif->vm_wmac != NULL)) {
        if (arg[1] != NULL) {
            data = simple_strtoul(arg[1], NULL, 0);
        }

        printk("unmarked dfs channel :%d\n",data);
        wifi_mac_unmark_dfs_channel(wnet_vif, data);

   } else {
        printk("Parameter is error.\n");
   }

    return 0;
}

int aml_mark_dfs_channel(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
     char **arg;
     char sep = ' ';
     int cmd_arg;
     int data = 0;

     arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
     if ((wnet_vif != NULL) && (wnet_vif->vm_wmac != NULL)) {
         if (arg[1] != NULL) {
             data = simple_strtoul(arg[1], NULL, 0);
         }

         printk("marked dfs channel :%d\n",data);
         wifi_mac_mark_dfs_channel(wnet_vif, data);

    } else {
         printk("Parameter is error.\n");
    }

     return 0;
}

int aml_set_device_sn(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char **mac_cmd;
    int i,cmd_arg;
    char sep = ' ';
    unsigned int efuse_data = 0;

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if (arg) {
        sep = ':';
        mac_cmd = aml_cmd_char_prase(sep, arg[1], &cmd_arg);
        if (mac_cmd) {
            efuse_data = (simple_strtoul(mac_cmd[0],NULL,16) << 8) | (simple_strtoul(mac_cmd[1],NULL,16));
            for (i = 0; i < 16; i++) {
                if (efuse_data & (1 << i)) {
                    //printk("set_dev_sn ===>>> efuse_manual_write: %d 0xf\n", i);
                    efuse_manual_write(i, 0xf);
                }
            }
            printk("set_dev_sn ===>>> write SN/ID is: %02x %02x\n", ((efuse_data & 0xff00) >> 8), (efuse_data & 0x00ff));
        }
        kfree(mac_cmd);
    }
    kfree(arg);

    return 0;
}

int aml_get_device_sn(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    int cmd_arg;
    char sep = ' ';
    unsigned int efuse_data = 0;

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if (arg) {
        efuse_data = efuse_manual_read(simple_strtoul(arg[1],NULL,16));
        printk("get_dev_sn ===>>> efuse addr:%08x, get SN/ID is: %02x %02x\n", simple_strtoul(arg[1],NULL,16), ((efuse_data & 0xff00) >> 8), (efuse_data & 0x00ff));
        kfree(arg);
    }else {
        printk("get_dev_sn ===>>> efuse addr:%08x, get SN/ID error\n", *arg);
        kfree(arg);
    }

    return 0;
}

int aml_set_signal_power_weak_thresh_for_narrow_bandwidth(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char sep = ' ';
    int cmd_arg;
    int data = 0;

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if ((wnet_vif != NULL) && (wnet_vif->vm_wmac != NULL) && (arg[1] != NULL)) {
        data = simple_strtol(arg[1], NULL, 0);
        wnet_vif->vm_wmac->wm_signal_power_weak_thresh_narrow = data;
        printk("set signal power weak thresh to: %d\n", wnet_vif->vm_wmac->wm_signal_power_weak_thresh_narrow);

    } else {
        printk("Parameter is error.\n");
    }

    kfree(arg);
    return 0;
}

int aml_set_signal_power_weak_thresh_for_wide_bandwidth(struct wlan_net_vif *wnet_vif, char* buf, int len)
{
    char **arg;
    char sep = ' ';
    int cmd_arg;
    int data = 0;

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    if ((wnet_vif != NULL) && (wnet_vif->vm_wmac != NULL) && (arg[1] != NULL)) {
        data = simple_strtol(arg[1], NULL, 0);
        wnet_vif->vm_wmac->wm_signal_power_weak_thresh_wide = data;
        printk("set signal power weak thresh to: %d\n", wnet_vif->vm_wmac->wm_signal_power_weak_thresh_wide);

    } else {
        printk("Parameter is error.\n");
    }

    kfree(arg);
    return 0;
}


