#include "wifi_mac_com.h"
#include "wifi_iwpriv_cmd.h"
#include "wifi_cmd_func.h"

extern char **aml_cmd_char_prase(char sep, const char *str, int *size);

static int aml_ap_send_addba_req(struct net_device *dev,
                struct iw_request_info *info,
                union iwreq_data *wrqu, char *extra) {
    char **arg;
    int cmd_arg;
    char sep = ',';

    arg = aml_cmd_char_prase(sep, extra, &cmd_arg);
    printk("%s, %s, %s\n", __func__,*arg, *(arg+1));

    vm_send_addba_req(*arg, simple_strtoul(arg[1],NULL,10));
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

static iw_handler aml_iwpriv_private_handler[] = {
    aml_ap_send_addba_req,
    NULL,
    aml_ap_set_amsdu_state,
    NULL,
    aml_ap_set_ampdu_state,
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



};


struct iw_handler_def w1_iw_handle = {
    .num_private = ARRAY_SIZE(aml_iwpriv_private_handler),
    .num_private_args = ARRAY_SIZE(aml_iwpriv_private_args),
    .private = aml_iwpriv_private_handler,
    .private_args = aml_iwpriv_private_args,
};
