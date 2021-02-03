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
int aml_set_hrtimer_interval(struct wlan_net_vif *wnet_vif, char* buf, int len);
#endif
