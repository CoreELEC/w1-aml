#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include "wifi_mac_com.h"
#include "wifi_cmd_func.h"
#include "wifi_drv_reg_ops.h"
#include "wifi_cfg80211.h"
#include "wifi_mac_if.h"

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

