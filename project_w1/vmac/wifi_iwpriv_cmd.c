#include "wifi_mac_com.h"
#include "wifi_iwpriv_cmd.h"
#include "wifi_cmd_func.h"


extern char **aml_cmd_char_prase(char sep, const char *str, int *size);
extern struct udp_info aml_udp_info[];
extern struct udp_timer aml_udp_timer;
extern int udp_cnt;

static int aml_ap_set_amsdu_state(struct net_device *dev,
    struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    printk("%s, %s\n", __func__,extra);
    aml_set_mac_amsdu_switch_state(extra);

    return 0;
}

static int aml_ap_set_ampdu_state(struct net_device *dev,
    struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    printk("%s, %s\n", __func__,extra);
    aml_set_drv_ampdu_switch_state(extra);

    return 0;
}


static int aml_ap_get_amsdu_state(void)
{
    printk("%s\n", __func__);
    aml_set_mac_amsdu_switch_state(NULL);
    return 0;
}

static int aml_ap_get_ampdu_state(void)
{
    printk("%s\n", __func__);
    aml_set_drv_ampdu_switch_state(NULL);
    return 0;
}

static int aml_ap_set_11h(unsigned char channel)
{
    wifi_mac_ap_set_11h(channel);

    return 0;
}


static int aml_ap_send_addba_req(struct net_device *dev,
    struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{

    int *param = (int *)extra;
    int val = 0;
    char addr[MAX_MAC_BUF_LEN]={0};

    printk("%s(%d), %d, %d, %d, %d, %d, %d, %d\n", __func__, __LINE__,
            param[0], param[1], param[2],param[3],param[4],param[5],param[6]);

    snprintf(addr, MAX_MAC_BUF_LEN, "%02d:%02d:%02d:%02d:%02d:%02d",param[0],param[1],param[2],param[3],param[4],param[5]);
    val = param[6];
    wifi_mac_send_addba_req(addr, val);

    return 0;

}

static int aml_iwpriv_send(struct net_device *dev,
    struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{

    struct wlan_net_vif *wnet_vif = NULL;
    struct drv_private *drv_priv = NULL;
    struct wifi_mac *wifimac = NULL;

    int *param = (int *)extra;
    char* str[2] = {0};
    int sub_cmd = param[0];
    int set = param[1];
    char buf[30] = {0};
    int len = 0;

    printk("%s, su_cmd %d, value %d\n", __func__,param[0], param[1]);

    wifimac = wifi_mac_get_mac_handle();
    drv_priv = wifimac->drv_priv;
    if (strncmp("wlan0", dev->name, strlen("wlan0")) == 0) {
        wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC];
    } else {
        wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];
    }

    switch (sub_cmd) {
        case AML_IWP_11H:
            aml_ap_set_11h(set);
            break;

        case AML_IWP_ARP_RX:
            break;

        case AML_IWP_ROAM_THRESH_2G:
            /*e.g '-80' need 3 char in string, added '\0', so need + 4 */
            len = strlen("set_roam_thr_2g ") + 4;
            snprintf(buf, len, "set_roam_thr_2g %d", set);
            printk("%s: buf %s\n", __func__, buf);
            aml_set_roaming_threshold_2g(wnet_vif, buf, len);
            break;

        case AML_IWP_ROAM_THRESH_5G:
            /*e.g '-80' need 3 char in string, added '\0', so need + 4 */
            len = strlen("set_roam_thr_5g ") + 4;
            snprintf(buf, len, "set_roam_thr_5g %d", set);
            printk("%s: buf %s\n", __func__, buf);
            aml_set_roaming_threshold_5g(wnet_vif, buf, len);
            break;

        case AML_IWP_ROAM_MODE:
            len = strlen("set_roam_mode ") + 4;
            snprintf(buf, len, "set_roam_mode %d", set);
            printk("%s: buf %s\n", __func__, buf);
            aml_set_roaming_mode(wnet_vif, buf, len);
            break;

        case AML_IWP_MARK_DFS_CHAN:
            len = strlen("mark_dfs_chan ") + 4;
            snprintf(buf, len, "mark_dfs_chan %d", set);
            printk("%s: buf %s\n", __func__, buf);
            aml_mark_dfs_channel(wnet_vif, buf, len);
            break;

        case AML_IWP_UNMARK_DFS_CHAN:
            len = strlen("unmark_dfs_chan ") + 4;
            snprintf(buf, len, "unmark_dfs_chan %d", set);
            printk("%s: buf %s\n", __func__, buf);
            aml_unmark_dfs_channel(wnet_vif, buf, len);
            break;

        case AML_IWP_WEAK_THRESH:
            len = strlen("set_weak_thresh ") + 4;
            snprintf(buf, len, "set_weak_thresh %d", set);
            printk("%s: buf %s\n", __func__, buf);
            aml_set_signal_power_weak_thresh(wnet_vif, buf, len);
            break;

        case AML_IWP_EAT_COUNT:
            len = strlen("set_eat_count ") + 4;
            snprintf(buf, len, "set_eat_count %d", set);
            printk("%s: buf %s\n", __func__, buf);
            aml_set_eat_count_max(wnet_vif, buf, len);
            break;

        case AML_IWP_AGGR_THRESH:
            len = strlen("set_aggr_thresh ") + 4;
            snprintf(buf, len, "set_aggr_thresh %d", set);
            printk("%s: buf %s\n", __func__, buf);
            aml_set_aggr_thresh(wnet_vif, buf, len);
            break;

        case AML_IWP_HEART_INTERVAL:
            len = strlen("set_hrt_int ") + 4;
            snprintf(buf, len, "set_hrt_int %d", set);
            printk("%s: buf %s\n", __func__, buf);
            aml_set_hrtimer_interval(wnet_vif, buf, len);
            break;

        case AML_IWP_BSS_COEX:
            len = strlen("send_bss_coex");
            snprintf(buf, len, "send_bss_coex");
            printk("%s: buf %s\n", __func__, buf);
            aml_sta_send_coexist_mgmt(wnet_vif, buf, len);
            break;

        case AML_IWP_WMM_AC_DELTS:
            len = strlen("wmm_ac_delts ") + 4;
            snprintf(buf, len, "wmm_ac_delts %d", set);
            printk("%s: buf %s\n", __func__, buf);
            aml_wmm_ac_delts(wnet_vif, buf, len);
            break;

        case AML_IWP_SHORT_GI:
            str[0] = "off";
            str[1] = "on";
            len = strlen("set_short_gi ") + strlen(str[set]) + 1;
            snprintf(buf, len, "set_short_gi %s", str[set]);
            printk("%s: buf %s\n", __func__, buf);
            aml_set_short_gi(wnet_vif, buf, len);
            break;

        case AML_IWP_DYNAMIC_BW:
            str[0] = "off";
            str[1] = "on";
            len = strlen("set_dynamic_bw ") + strlen(str[set]) + 1;
            snprintf(buf, len, "set_dynamic_bw %s", str[set]);
            printk("%s: buf %s\n", __func__, buf);
            aml_set_dynamic_bw(wnet_vif, buf, len);
            break;

        case AML_IWP_TXAGGR_STA:
            str[0] = "all";
            str[1] = "reset";
            len = strlen("get_txaggr_sta ") + strlen(str[set]) + 1;
            snprintf(buf, len, "get_txaggr_sta %s", str[set]);
            printk("%s: buf %s\n", __func__, buf);
            aml_get_drv_txaggr_status(wnet_vif, buf, len);
            break;

    }

    return 0;
}


static int aml_iwpriv_get(struct net_device *dev,
    struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{

    struct wlan_net_vif *wnet_vif = NULL;
    struct drv_private *drv_priv = NULL;
    struct wifi_mac *wifimac = NULL;
    struct wifi_channel *c = NULL;
    int *param = (int *)extra;
    int sub_cmd = param[0];
    int i = 0;
    char buf[30] = {0};

    printk("%s, sub cmd %d\n", __func__, param[0]);

    wifimac = wifi_mac_get_mac_handle();
    drv_priv = wifimac->drv_priv;
    if (strncmp("wlan0", dev->name, strlen("wlan0")) == 0) {
        wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC];
    } else {
        wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];
    }

    /*if we need feed back the value to user space, we need these 2 lines code, this is a sample*/
    //wrqu->data.length = sizeof(int);
    //*param = 110;
    /*if we need feed back the value to user space, we need these 2 lines code, this is a sample*/

    switch (sub_cmd) {
        case AML_IWP_AMSDU_STATE:
            aml_ap_get_amsdu_state();
            break;

        case AML_IWP_AMPDU_STATE:
            aml_ap_get_ampdu_state();
            break;

        case AML_IWP_UDP_INFO:
            for (i = 0; i < udp_cnt; i++) {
                printk("%s streamid=%d tx is %d, rx is %d\n", __func__, aml_udp_info[i].streamid, aml_udp_info[i].tx, aml_udp_info[i].rx);
            }
            aml_udp_timer.udp_timer_stop = 1;
            aml_udp_timer.run_flag = 0;
            udp_cnt = 0;
            os_timer_ex_del(&aml_udp_timer.udp_send_timeout, CANCEL_SLEEP);
            break;

        case AML_IWP_COUNTRY:
            printk("country code: %s\n", wifimac->wm_country.iso);
            break;

        case AML_IWP_CHAN_LIST:
            WIFI_CHANNEL_LOCK(wifimac);
            for (i = 0; i < wifimac->wm_nchans; i++) {
                c = &wifimac->wm_channels[i];
                printk("channel:%d\tfrequency:%d \tbandwidth:%dMHz \n", c->chan_pri_num, c->chan_cfreq1, ((1 << c->chan_bw) * 20));
            }
            WIFI_CHANNEL_UNLOCK(wifimac);
            break;

        case AML_IWP_CHIP_ID:
            snprintf(buf, 21, "get_chip_id 0x%x 0x%x",0x8, 0x9);
            printk("%s: buf %s\n", __func__, buf);
            aml_get_chip_id(NULL, buf, 0);
            break;

        case AML_IWP_AP_IP:
            aml_get_ap_ip(wnet_vif, NULL, 0);
            break;

        case AML_IWP_ROAM_CHAN:
            aml_get_roaming_candidate_chans(wnet_vif, NULL, 0);
            break;

        case AML_IWP_SESSION:
            aml_sta_get_wfd_session(wnet_vif, NULL, 0);
            break;
#ifdef CONFIG_P2P
        case AML_IWP_P2P_DEV_ID:
            aml_get_p2p_device_addr(wnet_vif, buf, 0);
            break;
#endif
        case AML_IWP_WIFI_MAC:
            aml_get_wifi_mac_addr(wnet_vif, buf, 30);
            break;

    }

    return 0;
}


static int aml_ap_set_udp_info(struct net_device *dev,
    struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    char **arg;
    int cmd_arg;
    char sep = ',';

    char buf[40] = {0};
    if (copy_from_user(buf, wrqu->data.pointer, 40)) {
        return -EFAULT;
    }
    printk("%s: %s\n", __func__, buf);

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    wifi_mac_set_udp_info(arg);
    kfree(arg);

    return 0;

}

static int aml_ap_get_udp_info(struct net_device *dev,
    struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{

    int i = 0;
    for (i = 0; i < udp_cnt; i++) {
        printk("%s streamid=%d tx is %d, rx is %d\n", __func__, aml_udp_info[i].streamid, aml_udp_info[i].tx, aml_udp_info[i].rx);
    }
    aml_udp_timer.udp_timer_stop = 1;
    aml_udp_timer.run_flag = 0;
    udp_cnt = 0;
    os_timer_ex_del(&aml_udp_timer.udp_send_timeout, CANCEL_SLEEP);

    return 0;
}

static int aml_set_country_code(struct net_device *dev,
    struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    printk("%s, %s\n", __func__,extra);
    wifi_mac_set_country_code(extra);

    return 0;

}

static int aml_get_country_code(struct net_device *dev,
    struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    struct wifi_mac *wifimac = wifi_mac_get_mac_handle();
    printk("country code: %s\n", wifimac->wm_country.iso);

    return 0;
}

static int aml_get_channel_list(struct net_device *dev,
    struct iw_request_info *info, union iwreq_data *wrqu, char *extra) {
    struct wifi_mac *wifimac = wifi_mac_get_mac_handle();
    struct wifi_channel *c;
    int i = 0 ;

    WIFI_CHANNEL_LOCK(wifimac);
    for (i = 0; i < wifimac->wm_nchans; i++) {
        c = &wifimac->wm_channels[i];
        printk("channel:%d\tfrequency:%d \tbandwidth:%dMHz \n", c->chan_pri_num, c->chan_cfreq1, ((1 << c->chan_bw) * 20));
    }
    WIFI_CHANNEL_UNLOCK(wifimac);

     return 0;
}

static int aml_ap_set_arp_rx(struct net_device *dev,
    struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    char **arg;
    int cmd_arg;
    char sep = ',';

    char buf[40] = {0};
    if (copy_from_user(buf, wrqu->data.pointer, 40)) {
        return -EFAULT;
    }
    printk("%s: %s\n", __func__, buf);

    arg = aml_cmd_char_prase(sep, buf, &cmd_arg);
    wifi_mac_ap_set_arp_rx(arg);
    kfree(arg);

    return 0;
}
#if defined(CONFIG_WEXT_PRIV) || LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 32)
static iw_handler aml_iwpriv_private_handler[] = {
    aml_ap_send_addba_req,
    aml_iwpriv_send,
    //NULL,
    /*if we need feed back the value to user space, we need jump command for large buffer*/
    aml_iwpriv_get,
    NULL,
    aml_ap_set_udp_info,
    NULL,
    aml_ap_set_arp_rx,
    NULL,
    aml_ap_set_amsdu_state,
    aml_ap_set_ampdu_state,
    aml_set_country_code,
};

static const struct iw_priv_args aml_iwpriv_private_args[] = {
/*iwpriv set command, there is more parameters*/
{
    SIOCIWFIRSTPRIV,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 7, 0, "ap_addba_req"},

/*iwpriv set command, there is one parameters*/
{
    SIOCIWFIRSTPRIV + 1,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, ""},
{
    AML_IWP_11H,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "ap_set_11h"},
{
    AML_IWP_ROAM_THRESH_2G,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "set_roam_thr_2g"},
{
    AML_IWP_ROAM_THRESH_5G,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "set_roam_thr_5g"},
{
    AML_IWP_ROAM_MODE,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "set_roam_mode"},
{
    AML_IWP_MARK_DFS_CHAN,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "mark_dfs_chan"},
{
    AML_IWP_UNMARK_DFS_CHAN,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "unmark_dfs_chan"},
{
    AML_IWP_WEAK_THRESH,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "set_weak_thresh"},
{
    AML_IWP_EAT_COUNT,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "set_eat_count"},
{
    AML_IWP_AGGR_THRESH,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "set_aggr_thresh"},
{
    AML_IWP_HEART_INTERVAL,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "set_hrt_int"},
{
    AML_IWP_BSS_COEX,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "send_bss_coex"},
{
    AML_IWP_TXAGGR_STA,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "get_txaggr_sta"},
{
    AML_IWP_WMM_AC_DELTS,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "wmm_ac_delts"},
{
    AML_IWP_SHORT_GI,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "set_short_gi"},
{
    AML_IWP_DYNAMIC_BW,
    IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "set_dynamic_bw"},




/*iwpriv get command*/
{
    /*if we need feed back the value to user space, we need jump command for large buffer*/
    SIOCIWFIRSTPRIV + 2,
    0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, ""},
{
    AML_IWP_AMSDU_STATE,
    0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "ap_get_amsdu"},
{
    AML_IWP_AMPDU_STATE,
    0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "ap_get_ampdu"},
{
    AML_IWP_UDP_INFO,
    0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "ap_get_udp_info"},
{
    AML_IWP_COUNTRY,
    0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_country"},
{
    AML_IWP_CHAN_LIST,
    0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_chan_list"},
{
    AML_IWP_CHIP_ID,
    0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_chip_id"},
{
    AML_IWP_AP_IP,
    0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_ap_ip"},
{
    AML_IWP_ROAM_CHAN,
    0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_roam_chan"},
{
    AML_IWP_SESSION,
    0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "sta_get_session"},
#ifdef CONFIG_P2P
{
    AML_IWP_P2P_DEV_ID,
    0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_p2p_dev_id"},
#endif
{
    AML_IWP_WIFI_MAC,
    0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_wifi_mac"},


{
    SIOCIWFIRSTPRIV + 4,
    IW_PRIV_TYPE_CHAR | 40, 0, "ap_set_udp_info"},
{
    SIOCIWFIRSTPRIV + 6,
    IW_PRIV_TYPE_CHAR | 40, 0, "ap_set_arp_rx"},
{
    SIOCIWFIRSTPRIV + 8,
    IW_PRIV_TYPE_CHAR | 3, 0, "ap_set_amsdu"},
{
    SIOCIWFIRSTPRIV + 9,
    IW_PRIV_TYPE_CHAR | 3, 0, "ap_set_ampdu"},
{
    SIOCIWFIRSTPRIV + 10,
    IW_PRIV_TYPE_CHAR | 3, 0, "set_country"},

};
#endif

#ifdef CONFIG_WIRELESS_EXT
struct iw_handler_def w1_iw_handle = {
#if defined(CONFIG_WEXT_PRIV) || LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 32)
    .num_private = ARRAY_SIZE(aml_iwpriv_private_handler),
    .num_private_args = ARRAY_SIZE(aml_iwpriv_private_args),
    .private = aml_iwpriv_private_handler,
    .private_args = aml_iwpriv_private_args,
#endif
};
#endif
