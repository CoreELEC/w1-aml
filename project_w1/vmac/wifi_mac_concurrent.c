#include "wifi_mac_com.h"

#ifdef  CONFIG_CONCURRENT_MODE
void concurrent_vsdb_init(struct wifi_mac *wifimac)
{
    wifimac->wm_vsdb_slot = CONCURRENT_SLOT_NONE;
    wifimac->wm_vsdb_flags = 0;
    wifimac->vsdb_mode_set_noa_enable = 0;
}

static void concurrent_change_channel_timeout_ex(SYS_TYPE param1,
                                            SYS_TYPE param2,SYS_TYPE param3, SYS_TYPE param4,SYS_TYPE param5)
{
    struct wifi_mac *wifimac = (struct wifi_mac *)param1;
    struct drv_private *drv_priv = wifimac->drv_priv;
    struct wlan_net_vif *main_vmac = drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC];
    struct wlan_net_vif *p2p_vmac = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];

    AML_PRINT(AML_DBG_MODULES_P2P, "++\n");

    if (vm_p2p_is_state(p2p_vmac->vm_p2p, NET80211_P2P_STATE_LISTEN)) {
        vm_p2p_cancel_remain_channel(p2p_vmac->vm_p2p);
        AML_PRINT(AML_DBG_MODULES_P2P, "p2p in listen state\n");
        return;

    } else if (vm_p2p_is_state(p2p_vmac->vm_p2p, NET80211_P2P_STATE_SCAN)) {
        AML_PRINT(AML_DBG_MODULES_P2P, "p2p in scan state\n");
        return;
    }

    if (main_vmac->vm_curchan != WIFINET_CHAN_ERR)
    {
        AML_PRINT(AML_DBG_MODULES_P2P, "pri_chan%d\n", main_vmac->vm_curchan->chan_pri_num);
        wifi_mac_restore_wnet_vif_channel(main_vmac);
    }
}

int concurrent_change_channel_timeout(void * data)
{
    struct wifi_mac *wifimac = (struct wifi_mac *)data;

    DPRINTF(AML_DEBUG_ANY, "%s\n", __func__);

    wifimac->wm_p2p_connection_protect = 0;
    wifi_mac_add_work_task(wifimac, concurrent_change_channel_timeout_ex,NULL,(SYS_TYPE)data,0,0,0,0 );
    return OS_TIMER_NOT_REARMED;
}

void concurrent_channel_restore(int target_channel, struct wlan_net_vif *wnet_vif, int times)
{
    struct wifi_mac *wifimac = wnet_vif->vm_wmac;
    struct wifi_channel *main_vmac_chan = NULL;

    DPRINTF(AML_DEBUG_CFG80211, "%s\n", __func__);

    main_vmac_chan = wifi_mac_get_main_vmac_channel(wifimac);
    if ((main_vmac_chan != WIFINET_CHAN_ERR) && (times != 0))
    {
        os_timer_ex_start_period(&wifimac->wm_concurrenttimer, times);
    }
}

void concurrent_channel_protection(struct wlan_net_vif *wnet_vif, int times)
{
    struct wifi_mac *wifimac = wnet_vif->vm_wmac;

    DPRINTF(AML_DEBUG_CFG80211, "%s wm_p2p_connection_protect_period:%d\n", __func__, times);

    wifimac->wm_p2p_connection_protect = 1;
    wifimac->wm_p2p_connection_protect_period = jiffies + times * HZ / 1000;
}

static void concurrent_vsdb_do_channel_change_ex(SYS_TYPE param1,
    SYS_TYPE param2,SYS_TYPE param3, SYS_TYPE param4,SYS_TYPE param5)
{
    struct wifi_mac *wifimac = (struct wifi_mac *)param1;
    struct drv_private *drv_priv = wifimac->drv_priv;
    struct wlan_net_vif *main_vmac = drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC];
    struct wlan_net_vif *p2p_vmac = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];
    struct wlan_net_vif *selected_vmac = NULL;

    if (wifimac->wm_nrunning > 1) {
        if (wifimac->wm_vsdb_flags & CONCURRENT_CHANNEL_SWITCH) {
            wifimac->wm_vsdb_flags &= ~CONCURRENT_CHANNEL_SWITCH;
            if (wifimac->wm_vsdb_slot == CONCURRENT_SLOT_STA) {
                if (p2p_vmac->vm_curchan != WIFINET_CHAN_ERR) {
                    wifimac->wm_vsdb_slot = CONCURRENT_SLOT_P2P;
                    wifi_mac_restore_wnet_vif_channel(p2p_vmac);
                }

            } else if (wifimac->wm_vsdb_slot == CONCURRENT_SLOT_P2P) {
                if (main_vmac->vm_curchan != WIFINET_CHAN_ERR) {
                    wifimac->wm_vsdb_slot = CONCURRENT_SLOT_STA;
                    wifi_mac_restore_wnet_vif_channel(main_vmac);
                }
            }

            DPRINTF(AML_DEBUG_CONNECT, "%s, slot:%d\n", __func__, wifimac->wm_vsdb_slot);

            if (wifimac->wm_vsdb_slot != CONCURRENT_SLOT_NONE) {
                selected_vmac = drv_priv->drv_wnet_vif_table[wifimac->wm_vsdb_slot];
                wifi_mac_scan_notify_leave_or_back(selected_vmac, 0);
            }
        }
    }

    wifimac->wm_vsdb_flags = 0;
}

void concurrent_vsdb_do_channel_change(void * data)
{
    struct wifi_mac *wifimac = (struct wifi_mac *)data;
    wifimac->wm_vsdb_flags &= ~CONCURRENT_NOTIFY_AP_SUCCESS;
    wifi_mac_add_work_task(wifimac, concurrent_vsdb_do_channel_change_ex,NULL,(SYS_TYPE)data,0,0,0,0);
}

static void concurrent_vsdb_change_channel_ex(SYS_TYPE param1,
                                            SYS_TYPE param2,SYS_TYPE param3, SYS_TYPE param4,SYS_TYPE param5)
{
    struct wifi_mac *wifimac = (struct wifi_mac *)param1;
    struct drv_private *drv_priv = wifimac->drv_priv;
    struct wlan_net_vif *main_vmac = drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC];
    struct wlan_net_vif *p2p_vmac = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];

    DPRINTF(AML_DEBUG_CONNECT, "%s, slot:%d\n", __func__, wifimac->wm_vsdb_slot);

    if ((wifimac->wm_nrunning > 1) && (wifimac->wm_vsdb_flags & CONCURRENT_CHANNEL_SWITCH)) {
        if (wifimac->wm_vsdb_slot == CONCURRENT_SLOT_STA) {
            wifi_mac_scan_notify_leave_or_back(main_vmac, 1);
            wifimac->drv_priv->drv_ops.drv_set_is_mother_channel(wifimac->drv_priv, main_vmac->wnet_vif_id, 1);
            wifimac->wm_vsdb_flags |= CONCURRENT_NOTIFY_AP;

        } else if (wifimac->wm_vsdb_slot == CONCURRENT_SLOT_P2P) {
            if (p2p_vmac->vm_p2p->p2p_role == NET80211_P2P_ROLE_CLIENT) {
                wifi_mac_scan_notify_leave_or_back(p2p_vmac, 1);
                wifimac->drv_priv->drv_ops.drv_set_is_mother_channel(wifimac->drv_priv, p2p_vmac->wnet_vif_id, 1);
                wifimac->wm_vsdb_flags |= CONCURRENT_NOTIFY_AP;
            }
        }

    } else if ((wifimac->wm_nrunning < 2) && (wifimac->wm_vsdb_flags & CONCURRENT_CHANNEL_SWITCH)) {
        wifimac->wm_vsdb_flags = 0;
    }
}

int concurrent_vsdb_change_channel(void * data)
{
    return 0;
}

void concurrent_vsdb_prepare_change_channel(struct wifi_mac *wifimac)
{
    struct wifi_mac_scan_state *ss = wifimac->wm_scan;
    struct drv_private *drv_priv = wifimac->drv_priv;
    struct wlan_net_vif *p2p_vmac = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];
    if ((wifimac->wm_nrunning > 1 && wifimac->wm_vsdb_slot == CONCURRENT_SLOT_P2P && p2p_vmac->vm_p2p->p2p_role == NET80211_P2P_ROLE_GO) ||
        (wifimac->wm_vsdb_slot == CONCURRENT_SLOT_STA && wifimac->vsdb_mode_set_noa_enable == 1)) {
        if (p2p_vmac->vm_p2p->p2p_negotiation_state != NET80211_P2P_STATE_GO_COMPLETE) {
            return;
        }
        if (wifimac->vsdb_mode_set_noa_enable == 0) {
            unsigned long long tsf = wifimac->drv_priv->drv_ops.drv_hal_get_tsf(wifimac->drv_priv, p2p_vmac->wnet_vif_id);
            unsigned int ltsf;
            unsigned int next_tbtt;
            struct p2p_noa noa = {0};

            ltsf = (unsigned int)tsf;
            next_tbtt = roundup(ltsf, p2p_vmac->vm_bcn_intval*TU_DURATION);

            noa.start = next_tbtt + p2p_vmac->vm_bcn_intval * TU_DURATION;
            noa.count = 255;
            noa.duration = p2p_vmac->vm_bcn_intval * TU_DURATION;
            noa.interval = 2 * p2p_vmac->vm_bcn_intval * TU_DURATION;
            p2p_vmac->vm_p2p->noa_index++;
            wifimac->vsdb_mode_set_noa_enable = 1;
            vm_p2p_noa_start(p2p_vmac->vm_p2p, &noa);
        }
        return;
    }

    //printk("%s\n", __func__);
    if ((wifimac->wm_nrunning > 1)
        && (time_after(jiffies, wifimac->wm_vsdb_switch_time + (WIFINET_SCAN_DEFAULT_INTERVAL * HZ / 1000))
        || (wifimac->wm_vsdb_switch_time == 0))) {
        if (!(wifimac->wm_vsdb_flags & CONCURRENT_CHANNEL_SWITCH)) {
            wifimac->wm_vsdb_flags |= CONCURRENT_CHANNEL_SWITCH;
            wifi_mac_add_work_task(wifimac, concurrent_vsdb_change_channel_ex,NULL,(SYS_TYPE)wifimac,0,0,0,0);
            wifimac->wm_vsdb_switch_time = jiffies;

        } else {
            //printk("not right time\n");
        }

        if ((wifimac->wm_vsdb_slot == CONCURRENT_SLOT_P2P)
            && (wifimac->wm_flags & WIFINET_F_SCAN) && (!(ss->scan_StateFlags & SCANSTATE_F_RESTORE))) {
            os_timer_ex_start_period(&ss->ss_scan_timer, (WIFINET_SCAN_DEFAULT_INTERVAL - ss->scan_chan_wait - 1));
        }
    }
}
#endif//CONFIG_CONCURRENT_MODE

unsigned char concurrent_check_is_vmac_same_pri_channel(struct wifi_mac *wifimac)
{
    struct drv_private *drv_priv = wifimac->drv_priv;
    struct wlan_net_vif *main_vmac = drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC];
    struct wlan_net_vif *p2p_vmac = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];

    //DPRINTF(AML_DEBUG_P2P, "%s\n", __func__);

    if ((main_vmac->vm_curchan == WIFINET_CHAN_ERR) || (p2p_vmac->vm_curchan == WIFINET_CHAN_ERR)) {
        return 1;

    } else {
        return (main_vmac->vm_curchan->chan_pri_num == p2p_vmac->vm_curchan->chan_pri_num);
    }
}

unsigned char concurrent_check_vmac_is_AP(struct wifi_mac *wifimac)
{
    struct drv_private *drv_priv = wifimac->drv_priv;
    struct wlan_net_vif *p2p_vmac = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];
    //DPRINTF(AML_DEBUG_P2P, "%s\n", __func__);
    if (p2p_vmac->vm_opmode == WIFINET_M_HOSTAP)
        return 1;
    else
        return 0;
}

struct wlan_net_vif *wifi_mac_running_main_wnet_vif(struct wifi_mac *wifimac)
{
    struct drv_private *drv_priv = wifimac->drv_priv;
    struct wlan_net_vif *main_vmac = drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC];

    if (main_vmac->vm_state == WIFINET_S_CONNECTED)
    {
        return main_vmac;
    }

    return NULL;
}

struct wlan_net_vif *wifi_mac_running_wnet_vif(struct wifi_mac *wifimac)
{
    struct drv_private *drv_priv = wifimac->drv_priv;
    struct wlan_net_vif *main_vmac = drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC];
    struct wlan_net_vif *p2p_vmac = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];

    if (wifimac->wm_nrunning != 1) {
        return NULL;
    }

    if (main_vmac->vm_state == WIFINET_S_CONNECTED)
    {
        return main_vmac;
    }
    else if (p2p_vmac->vm_state == WIFINET_S_CONNECTED)
    {
        return p2p_vmac;
    }

    return NULL;
}

