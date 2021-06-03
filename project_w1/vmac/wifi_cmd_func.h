/*
 ****************************************************************************************
 *
 * Copyright (C) Amlogic 2020
 *
 * Project: cmd_func
 *
 * Description:
 *     cmd_func struct and define
 *
 *
 ****************************************************************************************
 */
#ifndef _AML_CMD_FUNC_H_
#define _AML_CMD_FUNC_H_

typedef int (*cmd_func_ptr)(struct wlan_net_vif *wnet_vif, char* buf, int len);

typedef struct cmd_to_func_table
{
     char name[16];
     cmd_func_ptr cmd_proc_func;
} cmd_to_func_table_t;

struct udp_info
{
    struct os_timer_ext udp_send_timeout;
    unsigned short dst_port;
    unsigned short src_port;
    unsigned int dst_ip;
    unsigned int src_ip;
    unsigned int seq;
    unsigned short pkt_len;
    unsigned char streamid;
    unsigned char out;
    unsigned char udp_timer_stop;
    unsigned int tx;
    unsigned int rx;
};

struct udp_timer
{
    struct os_timer_ext udp_send_timeout;
    unsigned char udp_timer_stop;
    unsigned char run_flag;
};

extern struct udp_info aml_udp_info[];
extern struct udp_timer aml_udp_timer;
extern int udp_cnt;
extern cmd_to_func_table_t cmd_to_func[];

extern int aml_set_chip_id(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_get_chip_id(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_set_mac_addr(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_get_mac_addr(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_sta_send_addba_req(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_sta_send_coexist_mgmt(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_get_drv_txaggr_status(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_get_wifi_mac_addr(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_sta_get_wfd_session(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_set_bt_device_id(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_get_bt_device_id(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_get_p2p_device_addr(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_wmm_ac_addts(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_wmm_ac_delts(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern void aml_set_mac_amsdu_switch_state(char* arg);
extern void aml_set_drv_ampdu_switch_state(char* arg);
extern int aml_set_mac_amsdu(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_set_drv_ampdu(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_update_wmm_arg(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_set_dynamic_bw(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_set_short_gi(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_set_eat_count_max(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_set_aggr_thresh(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_set_hrtimer_interval(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_get_ap_ip(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_set_roaming_threshold_2g(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_set_roaming_threshold_5g(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_get_roaming_candidate_chans(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_set_roaming_candidate_chans(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_set_roaming_mode(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int wifi_mac_set_udp_info(char** buf);
extern int aml_set_udp_info(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_get_udp_info(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_mark_dfs_channel(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_unmark_dfs_channel(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern void wifi_mac_set_country_code(char* arg);
extern void wifi_mac_ap_set_11h(unsigned char channel);
extern void wifi_mac_ap_set_arp_rx(char** buf);
extern int aml_set_device_sn(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_get_device_sn(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_set_signal_power_weak_thresh_for_narrow_bandwidth(struct wlan_net_vif *wnet_vif, char* buf, int len);
extern int aml_set_signal_power_weak_thresh_for_wide_bandwidth(struct wlan_net_vif *wnet_vif, char* buf, int len);
#endif
