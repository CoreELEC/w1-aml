#include "wifi_mac_com.h"
#include "wifi_iwpriv_cmd.h"
#include "wifi_cmd_func.h"

extern char **aml_cmd_char_prase(char sep, const char *str, int *size);
extern struct udp_info aml_udp_info;

static int aml_ap_send_addba_req(struct net_device *dev,
                struct iw_request_info *info,
                union iwreq_data *wrqu, char *extra) {
    char **arg;
    int cmd_arg;
    char sep = ',';

    arg = aml_cmd_char_prase(sep, extra, &cmd_arg);
    printk("%s, %s, %s\n", __func__,*arg, *(arg+1));

    wifi_mac_send_addba_req(*arg, simple_strtoul(arg[1],NULL,10));
    kfree(arg);
    return 0;
}

static int aml_ap_set_amsdu_state(struct net_device *dev,
                struct iw_request_info *info,
                union iwreq_data *wrqu, char *extra) {
    printk("%s, %s\n", __func__,extra);

    aml_set_mac_amsdu_switch_state(extra);

    return 0;
}

static int aml_ap_set_ampdu_state(struct net_device *dev,
                struct iw_request_info *info,
                union iwreq_data *wrqu, char *extra) {
    printk("%s, %s\n", __func__,extra);

    aml_set_drv_ampdu_switch_state(extra);

    return 0;
}

static int aml_ap_set_udp_info(struct net_device *dev,
                struct iw_request_info *info,
                union iwreq_data *wrqu, char *extra) {
    char **arg;
    int cmd_arg;
    char sep = ',';

    arg = aml_cmd_char_prase(sep, extra, &cmd_arg);
    wifi_mac_set_udp_info(arg);

    return 0;

}

static int aml_ap_get_udp_info(struct net_device *dev,
               struct iw_request_info *info,
               union iwreq_data *wrqu, char *extra) {

    printk("%s tx is %d, rx is %d\n", __func__, aml_udp_info.tx, aml_udp_info.rx);

    aml_udp_info.udp_timer_stop = 1;
    os_timer_ex_del(&aml_udp_info.udp_send_timeout, CANCEL_SLEEP);

    return 0;
}

static int aml_ap_set_country_code(struct net_device *dev,
                   struct iw_request_info *info,
                   union iwreq_data *wrqu, char *extra) {
    printk("%s, %s\n", __func__,extra);

    wifi_mac_ap_set_country_code(extra);

    return 0;

}

static int aml_ap_set_11h(struct net_device *dev,
                   struct iw_request_info *info,
                   union iwreq_data *wrqu, char *extra) {
    char **arg;
    int cmd_arg;
    char sep = ',';

    arg = aml_cmd_char_prase(sep, extra, &cmd_arg);
    wifi_mac_ap_set_11h(arg);

    return 0;
}

static int aml_ap_set_arp_rx(struct net_device *dev,
                   struct iw_request_info *info,
                   union iwreq_data *wrqu, char *extra) {
    char **arg;
    int cmd_arg;
    char sep = ',';

    arg = aml_cmd_char_prase(sep, extra, &cmd_arg);
    wifi_mac_ap_set_arp_rx(arg);

    return 0;
}

static iw_handler aml_iwpriv_private_handler[] = {
    aml_ap_send_addba_req,
    NULL,
    aml_ap_set_amsdu_state,
    NULL,
    aml_ap_set_ampdu_state,
    NULL,
    aml_ap_set_udp_info,
    NULL,
    aml_ap_get_udp_info,
    NULL,
    aml_ap_set_country_code,
    NULL,
    aml_ap_set_11h,
    NULL,
    aml_ap_set_arp_rx,
    NULL,
};

static const struct iw_priv_args aml_iwpriv_private_args[] = {
{
    SIOCIWFIRSTPRIV,
    IW_PRIV_TYPE_CHAR | 40, 0, "ap_send_add_req"},
{
    SIOCIWFIRSTPRIV + 2,
    IW_PRIV_TYPE_CHAR | 40, 0, "ap_set_amsdu"},
{
    SIOCIWFIRSTPRIV + 4,
    IW_PRIV_TYPE_CHAR | 40, 0, "ap_set_ampdu"},

{
    SIOCIWFIRSTPRIV + 6,
    IW_PRIV_TYPE_CHAR | 40, 0, "ap_set_udp_info"},
{
    SIOCIWFIRSTPRIV + 8,
    IW_PRIV_TYPE_CHAR | 40, 0, "ap_get_udp_info"},
{
    SIOCIWFIRSTPRIV + 10,
    IW_PRIV_TYPE_CHAR | 40, 0, "ap_set_country"},
{
    SIOCIWFIRSTPRIV + 12,
    IW_PRIV_TYPE_CHAR | 40, 0, "ap_set_11h"},
{
    SIOCIWFIRSTPRIV + 14,
    IW_PRIV_TYPE_CHAR | 40, 0, "ap_set_arp_rx"},

};


struct iw_handler_def w1_iw_handle = {
    .num_private = ARRAY_SIZE(aml_iwpriv_private_handler),
    .num_private_args = ARRAY_SIZE(aml_iwpriv_private_args),
    .private = aml_iwpriv_private_handler,
    .private_args = aml_iwpriv_private_args,
};
