/*
 ****************************************************************************************
 *
 * Copyright (C) Amlogic 2010-2014
 *
 * Project: 11N 80211 DRIVER  layer Software
 *
 * Description:
 *   DRIVER and HAL layer interface function
 *
 *
 ****************************************************************************************
 */

#include "wifi_mac_com.h"
#include "wifi_rate_ctrl.h"

/*
    1,attach driver callback functions eg. RX_OK, TX_OK, driver_probe()... for hal,.
    2,attach hal layer interface, eg,hal_fill_agg_start(),  phy_en_bcn()....
    3,init work task for driver layer.
*/
void drv_hal_attach( void *   drv_priv,void *cbptr)
{
    struct hal_private* hal_priv = hal_get_priv();
    hal_priv->hal_call_back = cbptr;

    hal_priv->dhcp_offload = dhcp_offload;
    hal_priv->hal_ops.hal_init(drv_priv);
    drv_hal_workitem_inital();
    return ;
}

int drv_hal_detach(void)
{
    struct hal_private* hal_priv = hal_get_priv();

    hal_priv->hal_ops.hal_exit();
    drv_hal_workitem_free();
    printk("<running> %s %d \n",__func__,__LINE__);
    return 0;
}

/*
 * Setup a h/w rate table's reverse lookup table and
 * fill in ack durations.  This routine is called for
 * each rate table returned through the ah_getRateTable
 * method.  The reverse lookup tables are assumed to be
 * initialized to zero (or at least the first entry).
 * We use this as a key that indicates whether or not
 * we've previously setup the reverse lookup table.
 *
 * note: not reentrant, but shouldn't matter
 */
void drv_hal_setupratetable(struct drv_rate_table *rt)
{
    int i;
    unsigned char code,  cix;

    if (rt->dot11rate_to_idx[0] != 0)
    {
        /* already setup */
        return;
    }

    memset( rt->dot11rate_to_idx, 0xff, sizeof(rt->dot11rate_to_idx));
    for (i = 0; i < rt->rateCount; i++)
    {
        if ((rt->info[i].phy == CCK) || (rt->info[i].phy == WOFDM))
        {
            code = WIFINET_GET_RATE_VAL (rt->info[i].dot11Rate);
        }
        else
        {
            code = rt->info[i].vendor_rate_code;
        }
        cix = rt->info[i].controlRate;

        rt->dot11rate_to_idx[code] = i;
        DPRINTF(AML_DEBUG_RATE,"%s(%d) dot11rate_to_idx[0x%x]=%d\n", __func__, __LINE__, code, i);
        /*
         * note: for 11g the control rate to use for 5.5 and 11 Mb/s
         *     depends on whether they are marked as basic rates;
         *     the static tables are setup with an 11b-compatible
         *     2Mb/s rate which will work but is suboptimal
         */
        rt->info[i].lpAckDuration = drv_hal_calc_txtime(rt,WLAN_CTRL_FRAME_SIZE, cix, 0);
        rt->info[i].spAckDuration = drv_hal_calc_txtime(rt,WLAN_CTRL_FRAME_SIZE, cix, 1);
    }
}


// abg dot11_rate= bit7 + bit6~0: bit7 is a basic rate flag, rate value = 500kbps * (bit6~0)
struct drv_rate_table amluno_11bgnac_table =
{
    30,  /* number of rates */
    { 0 },
    {
        /*                                                    short            ctrl  */
        /*                valid                       phy     rateKbps   rateCode    Preamble  dot11Rate Rate */
         /*   1 Mb */ {  HAL_RATECTRL|HAL_SUPPORT,  CCK,    1000,    WIFI_11B_1M,   0x00, (WIFINET_RATE_BASIC| 2),   0 },
        /*   2 Mb */ {  HAL_RATECTRL|HAL_SUPPORT, CCK,     2000,    WIFI_11B_2M,    0x04, (WIFINET_RATE_BASIC| 4),   1 },
        /* 5.5 Mb */ {  HAL_RATECTRL|HAL_SUPPORT, CCK,     5500,    WIFI_11B_5M,    0x04, (WIFINET_RATE_BASIC|11),   2 },
        /*  11 Mb */ {  HAL_RATECTRL|HAL_SUPPORT, CCK,    11000,   WIFI_11B_11M,    0x04, (WIFINET_RATE_BASIC|22),   3 },
        /*   6 Mb */ {  HAL_RATECTRL|HAL_SUPPORT, WOFDM,    6000,    WIFI_11G_6M,    0x00,        12,   4 },
        /*   9 Mb */ {  HAL_RATECTRL|HAL_SUPPORT, WOFDM,    9000,    WIFI_11G_9M,    0x00,        18,   4 },
        /*  12 Mb */ {  HAL_RATECTRL|HAL_SUPPORT, WOFDM,   12000,   WIFI_11G_12M,    0x00,        24,   6 },
        /*  18 Mb */ {  HAL_RATECTRL|HAL_SUPPORT, WOFDM,   18000,   WIFI_11G_18M,    0x00,        36,   6 },
        /*  24 Mb */ {  HAL_RATECTRL|HAL_SUPPORT, WOFDM,   24000,   WIFI_11G_24M,    0x00,        48,   8 },
        /*  36 Mb */ {  HAL_RATECTRL|HAL_SUPPORT, WOFDM,   36000,   WIFI_11G_36M,    0x00,        72,   8 },
        /*  48 Mb */ {  HAL_RATECTRL|HAL_SUPPORT, WOFDM,   48000,   WIFI_11G_48M,    0x00,        96,   8 },
        /*  54 Mb */ {  HAL_RATECTRL|HAL_SUPPORT, WOFDM,   54000,   WIFI_11G_54M,    0x00,       108,   8 },



           /* 6.5 Mb */{HAL_RATECTRL|HAL_SUPPORT, HT,      6500,        WIFI_11N_MCS0,    0x00,         0,   4 },
        /*  13 Mb */{   HAL_RATECTRL|HAL_SUPPORT, HT,      13000,       WIFI_11N_MCS1,    0x00,         1,   6 },
        /*19.5 Mb */ { HAL_RATECTRL|HAL_SUPPORT, HT,      19500,        WIFI_11N_MCS2,    0x00,         2,   6 },
        /*  26 Mb */ { HAL_RATECTRL|HAL_SUPPORT, HT,       26000,       WIFI_11N_MCS3,    0x00,         3,   8 },
        /*  39 Mb */ { HAL_RATECTRL|HAL_SUPPORT, HT,       39000,       WIFI_11N_MCS4,    0x00,         4,   8 },
        /*  52 Mb */ { HAL_RATECTRL|HAL_SUPPORT, HT,       52000,       WIFI_11N_MCS5,    0x00,         5,   8 },
        /*58.5 Mb */ { HAL_RATECTRL|HAL_SUPPORT, HT,      58500,        WIFI_11N_MCS6,    0x00,         6,   8 },
        /*  65 Mb */ {  HAL_RATECTRL|HAL_SUPPORT, HT,      65000,       WIFI_11N_MCS7,    0x00,         7,   8 },

        /*11ac */
        /*                valid                                                phy             rateKbps   rateCode          Preamble    dot11Rate       controlRate */
        /* 6.5 Mb */ {  HAL_RATECTRL|HAL_SUPPORT,    VHT_PHY,      6500,        WIFI_11AC_MCS0,       0x00,          0,                   4 },
        /*  13 Mb */ {  HAL_RATECTRL|HAL_SUPPORT,    VHT_PHY,      13000,       WIFI_11AC_MCS1,        0x00,         1,                   6 },
        /*19.5 Mb */ {  HAL_RATECTRL|HAL_SUPPORT,   VHT_PHY,      19500,        WIFI_11AC_MCS2,        0x00,         2,                   6 },
        /*  26 Mb */ {  HAL_RATECTRL|HAL_SUPPORT,    VHT_PHY,      26000,       WIFI_11AC_MCS3,        0x00,         3,                   8 },
        /*  39 Mb */ {  HAL_RATECTRL|HAL_SUPPORT,    VHT_PHY,      39000,       WIFI_11AC_MCS4,        0x00,         4,                   8 },
        /*  52 Mb */ {  HAL_RATECTRL|HAL_SUPPORT,    VHT_PHY,      52000,       WIFI_11AC_MCS5,        0x00,         5,                   8 },
        /*58.5 Mb */ {  HAL_RATECTRL|HAL_SUPPORT,   VHT_PHY,      58500,        WIFI_11AC_MCS6,        0x00,         6,                   8 },
        /*  65 Mb */ {  HAL_RATECTRL|HAL_SUPPORT,    VHT_PHY,      65000,       WIFI_11AC_MCS7,        0x00,         7,                   8 },
        /*  78 Mb */ {  HAL_RATECTRL|HAL_SUPPORT,    VHT_PHY,      78000,       WIFI_11AC_MCS8,        0x00,         8,                   8 },
        /* 180 Mb */{  HAL_RATECTRL|HAL_SUPPORT,   VHT_PHY,      180000,        WIFI_11AC_MCS9,        0x00,         9,                   8 },

    },
};

#undef  WOFDM
#undef  CCK
#undef   HT
#undef   VHT_PHY
struct drv_rate_table *drv_hal_get_rate_tbl(int mode)
{
    struct drv_rate_table *rt = NULL;

    switch (mode)
    {
        case WIFINET_MODE_11B:
        case WIFINET_MODE_11BG:
        case WIFINET_MODE_11G:
        case WIFINET_MODE_11GN:
        case WIFINET_MODE_11N:
        case WIFINET_MODE_11BGN:
        case WIFINET_MODE_11AC :
        case WIFINET_MODE_11NAC :
        case WIFINET_MODE_11GNAC :
            rt = &amluno_11bgnac_table;
            break;
        default:
            DPRINTF( AML_DEBUG_HAL|AML_DEBUG_ERROR, "%s: invalid mode 0x%x\n", __func__, mode);
            return NULL;
    }
    DPRINTF(AML_DEBUG_RATE, "%s(%d): mode=0x%x\n", __func__, __LINE__, mode);

    return rt;
}


/*
 * Compute the time to transmit a frame of length frameLen bytes
 * using the specified rate, phy, and short preamble setting.
 */
unsigned short
drv_hal_calc_txtime(const struct drv_rate_table *rates,
                      unsigned int frameLen,
                      unsigned short rateix,
                      int shortPreamble)
{
    unsigned int bitsPerSymbol, numBits, numSymbols, phyTime, txTime;
    unsigned int kbps;

    kbps = rates->info[rateix].rateKbps;
    /*
     * index can be invalid duting dynamic Turbo transitions.
     */
    if (kbps == 0) return 0;
    switch (rates->info[rateix].phy)
    {
        case WIFINET_T_CCK:
            phyTime = CCK_PREAMBLE_BITS + CCK_PLCP_BITS;
            if (shortPreamble && rates->info[rateix].shortPreamble)
                phyTime >>= 1;

            numBits = frameLen << 3;
            txTime = CCK_SIFS_TIME + phyTime + ((numBits * 1000)/kbps);
            break;


        case WIFINET_T_OFDM:
        {
            /* full rate channel */
            bitsPerSymbol = (kbps * OFDM_SYMBOL_TIME) / 1000;

            numBits = OFDM_PLCP_BITS + (frameLen << 3);
            numSymbols = howmany(numBits, bitsPerSymbol);
            txTime = OFDM_SIFS_TIME + OFDM_PREAMBLE_TIME + (numSymbols * OFDM_SYMBOL_TIME);
        }
        break;

        default:
            txTime = 0;
            break;
    }
    return txTime;
}

unsigned char drv_hal_wnet_vif_staid(unsigned char vm_opmode,unsigned short sta_associd)
{
    return (vm_opmode == WIFINET_M_STA )? 1:sta_associd&0xff;
}
unsigned short drv_hal_staid(enum hal_op_mode hal_opmode,unsigned short sta_associd)
{
    return (hal_opmode == WIFI_M_STA )? 1:sta_associd&0x3fff;
}
unsigned char drv_hal_nsta_staid(struct wifi_station *sta)
{
    return drv_hal_wnet_vif_staid(sta->sta_wnet_vif->vm_opmode,sta->sta_associd);
}

/* * Find the country code. */
int find_country_code(unsigned char *countryString)
{
    int i;
    if (strlen(countryString) != 2)
        return 0xff;

    for (i=0; i<WIFI_country_MAX; i++)
    {
        if ((all_countries_name[i][0] == countryString[0]) &&
            (all_countries_name[i][1] == countryString[1]))
            return i;
    }
    return 0xff;        /* Not found */
}

static void
drv_hal_workitem_task(SYS_TYPE iparam)
{
    struct hal_private *hal_priv = hal_get_priv();
    unsigned short STATUS;
    unsigned char *EltPtr =NULL;
    struct _CO_SHARED_FIFO* pWorkFifo = &hal_priv->WorkFifo;
    struct hal_work_task *pWorkTask = NULL;

    while (CO_SharedFifoEmpty(pWorkFifo, CO_WORK_FREE))
    {
        STATUS = CO_SharedFifoGet(pWorkFifo, CO_WORK_FREE, 1, &EltPtr);
        ASSERT(STATUS == CO_STATUS_OK);
        pWorkTask = (struct hal_work_task *)EltPtr;

        if (pWorkTask->task != NULL)
        {
            pWorkTask->task(pWorkTask->param1, pWorkTask->param2, pWorkTask->param3,
                pWorkTask->param4, pWorkTask->param5);
        }

        if (pWorkTask->taskcallback != NULL)
        {
            pWorkTask->taskcallback(pWorkTask->param1, pWorkTask->param2, pWorkTask->param3,
                pWorkTask->param4, pWorkTask->param5);
        }

        STATUS = CO_SharedFifoPut(pWorkFifo, CO_WORK_FREE, 1);
        ASSERT(STATUS == CO_STATUS_OK);
    }
}

void drv_hal_workitem_free(void)
{

}

int drv_hal_add_workitem(WorkHandler task, WorkHandler taskcallback, SYS_TYPE param1,
    SYS_TYPE param2, SYS_TYPE param3, SYS_TYPE param4, SYS_TYPE param5)
{
    struct hal_private* hal_priv = hal_get_priv();
    unsigned short STATUS;
    unsigned char *EltPtr = NULL;
    struct _CO_SHARED_FIFO *pWorkFifo = &hal_priv->WorkFifo;
    struct hal_work_task *pWorkTask = NULL;
    ASSERT(task != NULL);

    STATUS = CO_SharedFifoGet(pWorkFifo, CO_WORK_GET, 1, &EltPtr);
    if (STATUS != CO_STATUS_OK) {
        printk("%s:%d work fifo overflow\n", __func__, __LINE__);
        CO_SharedFifo_Dump(pWorkFifo, CO_WORK_GET);
        CO_SharedFifo_Dump(pWorkFifo, CO_WORK_FREE);
    }

    pWorkTask = ( struct hal_work_task *)EltPtr;
    pWorkTask->param1 = param1;
    pWorkTask->param2 = param2;
    pWorkTask->param3 = param3;
    pWorkTask->param4 = param4;
    pWorkTask->param5 = param5;
    pWorkTask->taskcallback= taskcallback;
    pWorkTask->task = task;
    STATUS = CO_SharedFifoPut(pWorkFifo, CO_WORK_GET, 1);
    if (STATUS != CO_STATUS_OK) {
        printk("%s:%d, work fifo overflow\n", __func__, __LINE__);
        CO_SharedFifo_Dump(pWorkFifo, CO_WORK_GET);
        CO_SharedFifo_Dump(pWorkFifo, CO_WORK_FREE);
    }
    hal_priv->hal_ops.hal_call_task(hal_priv->WorkFifo_task_id,(SYS_TYPE)NULL);
    return 0;
}

int drv_hal_workitem_inital(void)
{
    struct hal_private* hal_priv = hal_get_priv();
    int res;

    ASSERT(&hal_priv->WorkFifo);
    ASSERT(hal_priv->WorkFifoBuf);

    CO_SharedFifoInit(&hal_priv->WorkFifo, (SYS_TYPE)hal_priv->WorkFifoBuf, (void *)hal_priv->WorkFifoBuf,
        WORK_ITEM_NUM, sizeof(struct  hal_work_task), CO_SF_WORK_NBR);

    printk("%s hal_priv->WorkFifo:%p\n", __func__, &hal_priv->WorkFifo);
    res = (SYS_TYPE)hal_priv->hal_ops.hal_reg_task(drv_hal_workitem_task);

    if (res < 0) {
        ASSERT(0);
        printk("WorkFifo_task_id error !\n");
    } else {
        hal_priv->WorkFifo_task_id = res;
    }
    return 0;
}

