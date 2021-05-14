#ifdef AUTO_RATE_SIM
#include "type.h"
#include "opt_sim2.h"
#include "mac80211_temp.h"
#include "cfg80211.h"
struct sk_buff g_skbuffer[MAX_SKB_NUM];
#else
#include <linux/netdevice.h>
#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/debugfs.h>
#include <linux/random.h>
#include <linux/ieee80211.h>
#include <linux/slab.h>
#include <net/mac80211.h>
#include "wifi_mac_com.h"
#endif 
#include "osdep.h"
#include "rc80211_minstrel.h"
#include "rc80211_minstrel_ht.h"
#include "rc80211_minstrel_init.h"
#include "wifi_debug.h"
#include "wifi_mac.h"
#include "fi_sdio.h"
#include "wifi_rate_ctrl.h"
#include "wifi_pkt_desc.h"

static struct ieee80211_channel aml_2ghz_channels[] = {
	CHAN2G(1, 2412, 0),
	CHAN2G(2, 2417, 0),
	CHAN2G(3, 2422, 0),
	CHAN2G(4, 2427, 0),
	CHAN2G(5, 2432, 0),
	CHAN2G(6, 2437, 0),
	CHAN2G(7, 2442, 0),
	CHAN2G(8, 2447, 0),
	CHAN2G(9, 2452, 0),
	CHAN2G(10, 2457, 0),
	CHAN2G(11, 2462, 0),
	CHAN2G(12, 2467, 0),
	CHAN2G(13, 2472, 0),
	CHAN2G(14, 2484, 0),
};

static struct ieee80211_rate aml_legacy_rates[] = {
	RATETAB_ENT(10, 0x1, 0),
	RATETAB_ENT(20, 0x2, 0),
	RATETAB_ENT(55, 0x4, 0),
	RATETAB_ENT(110, 0x8, 0),
	RATETAB_ENT(60, 0x10, 0),
	RATETAB_ENT(90, 0x20, 0),
	RATETAB_ENT(120, 0x40, 0),
	RATETAB_ENT(180, 0x80, 0),
	RATETAB_ENT(240, 0x100, 0),
	RATETAB_ENT(360, 0x200, 0),
	RATETAB_ENT(480, 0x400, 0),
	RATETAB_ENT(540, 0x800, 0),
};

static struct ieee80211_channel aml_5ghz_channels[] = {
	CHAN5G(34, 0), CHAN5G(36, 0),
	CHAN5G(38, 0), CHAN5G(40, 0),
	CHAN5G(42, 0), CHAN5G(44, 0),
	CHAN5G(46, 0), CHAN5G(48, 0),
	CHAN5G(52, 0), CHAN5G(56, 0),
	CHAN5G(60, 0), CHAN5G(64, 0),
	CHAN5G(100, 0), CHAN5G(104, 0),
	CHAN5G(108, 0), CHAN5G(112, 0),
	CHAN5G(116, 0), CHAN5G(120, 0),
	CHAN5G(124, 0), CHAN5G(128, 0),
	CHAN5G(132, 0), CHAN5G(136, 0),
	CHAN5G(140, 0), CHAN5G(149, 0),
	CHAN5G(153, 0), CHAN5G(157, 0),
	CHAN5G(161, 0), CHAN5G(165, 0),
	CHAN5G(184, 0), CHAN5G(188, 0),
	CHAN5G(192, 0), CHAN5G(196, 0),
	CHAN5G(200, 0), CHAN5G(204, 0),
	CHAN5G(208, 0), CHAN5G(212, 0),
	CHAN5G(216, 0),
};

static struct ieee80211_supported_band aml_band_24ghz = {
	.n_channels = ARRAY_SIZE(aml_2ghz_channels),
	.channels = aml_2ghz_channels,
	.band = IEEE80211_BAND_2GHZ,
	.n_bitrates = aml_legacy_rates_size,
	.bitrates = aml_legacy_rates,
	.ht_cap.cap = 0,/*Need to be initialized later*/
	.ht_cap.ht_supported = true,
};

static struct ieee80211_supported_band aml_band_5ghz = {
	.n_channels = ARRAY_SIZE(aml_5ghz_channels),
	.channels = aml_5ghz_channels,
	.band = IEEE80211_BAND_5GHZ,
	.n_bitrates = aml_legacy_rates_size - 4 ,/*Eliminate 11b rate*/
	.bitrates = aml_legacy_rates - 4,/*Eliminate 11b rate*/
	.ht_cap.cap = 0,  /*Need to be initialized later*/
	.ht_cap.ht_supported = true,
};

static struct ieee80211_sta_ht_cap aml_get_ht_cap(struct aml_rate_adaptation_dev *aml_minstrel_dev, struct ieee80211_sta_ht_cap *p_ht_cap)
{
	int i;
	struct ieee80211_sta_ht_cap ht_cap = {0};
	p_ht_cap->ht_supported = 0;

	if (!(aml_minstrel_dev->ht_cap_info & WMI_HT_CAP_ENABLED))
		return ht_cap;

	ht_cap.ht_supported = 1;
	ht_cap.ampdu_factor = IEEE80211_HT_MAX_AMPDU_64K;
	ht_cap.ampdu_density = IEEE80211_HT_MPDU_DENSITY_8;
	ht_cap.cap |= IEEE80211_HT_CAP_SUP_WIDTH_20_40;
	ht_cap.cap |= IEEE80211_HT_CAP_DSSSCCK40;
	ht_cap.cap |= WLAN_HT_CAP_SM_PS_STATIC << IEEE80211_HT_CAP_SM_PS_SHIFT;

	if (aml_minstrel_dev->ht_cap_info & WMI_HT_CAP_HT20_SGI)
		ht_cap.cap |= IEEE80211_HT_CAP_SGI_20;

	if (aml_minstrel_dev->ht_cap_info & WMI_HT_CAP_HT40_SGI)
		ht_cap.cap |= IEEE80211_HT_CAP_SGI_40;

	if (aml_minstrel_dev->ht_cap_info & WMI_HT_CAP_DYNAMIC_SMPS) {
		u32 smps;

		smps   = WLAN_HT_CAP_SM_PS_DYNAMIC;
		smps <<= IEEE80211_HT_CAP_SM_PS_SHIFT;

		ht_cap.cap |= smps;
	}

	if (aml_minstrel_dev->ht_cap_info & WMI_HT_CAP_TX_STBC)
		ht_cap.cap |= IEEE80211_HT_CAP_TX_STBC;

	if (aml_minstrel_dev->ht_cap_info & WMI_HT_CAP_RX_STBC) {
		u32 stbc;

		stbc   = aml_minstrel_dev->ht_cap_info;
		stbc  &= WMI_HT_CAP_RX_STBC;
		stbc >>= WMI_HT_CAP_RX_STBC_MASK_SHIFT;
		stbc <<= IEEE80211_HT_CAP_RX_STBC_SHIFT;
		stbc  &= IEEE80211_HT_CAP_RX_STBC;

		ht_cap.cap |= stbc;
	}

	if (aml_minstrel_dev->ht_cap_info & WMI_HT_CAP_LDPC)
		ht_cap.cap |= IEEE80211_HT_CAP_LDPC_CODING;

	if (aml_minstrel_dev->ht_cap_info & WMI_HT_CAP_L_SIG_TXOP_PROT)
		ht_cap.cap |= IEEE80211_HT_CAP_LSIG_TXOP_PROT;

	/* max AMSDU is implicitly taken from vht_cap_info */
	if (aml_minstrel_dev->vht_cap_info & WMI_VHT_CAP_MAX_MPDU_LEN_MASK)
		ht_cap.cap |= IEEE80211_HT_CAP_MAX_AMSDU;

	for (i = 0; i < aml_minstrel_dev->num_rf_chains; i++)
		ht_cap.mcs.rx_mask[i] = 0xFF;

	ht_cap.mcs.tx_params |= IEEE80211_HT_MCS_TX_DEFINED;

	memcpy(p_ht_cap, &ht_cap, sizeof(struct ieee80211_sta_ht_cap));
	return ht_cap;
}


static struct ieee80211_sta_vht_cap aml_create_vht_cap(struct aml_rate_adaptation_dev *aml_minstrel_dev, int rate_mode)
{
    struct ieee80211_sta_vht_cap vht_cap = {0};
    u16 mcs_map;
    int i;

    /*0: legacy rate, 1:ht rate, 2:vht rate*/
    if (rate_mode == 2) {
        vht_cap.vht_supported = 1;

    } else {
         vht_cap.vht_supported = 0;
    }
    vht_cap.cap = aml_minstrel_dev->vht_cap_info;

    mcs_map = 0;
    for (i = 0; i < 8; i++) {
        if (i < aml_minstrel_dev->num_rf_chains)
            mcs_map |= IEEE80211_VHT_MCS_SUPPORT_0_9 << (i * 2);
        else
            mcs_map |= IEEE80211_VHT_MCS_NOT_SUPPORTED << (i * 2);
    }

    vht_cap.vht_mcs.rx_mcs_map = /*cpu_to_le16*/(mcs_map);
    vht_cap.vht_mcs.tx_mcs_map = /*cpu_to_le16*/(mcs_map);

    return vht_cap;
}

static struct aml_rate_adaptation_dev  g_aml_rate_adaptation_dev;
static struct ieee80211_hw g_hw;
static struct wiphy g_wiphy;

static struct ieee80211_sta g_sta;
#ifdef AUTO_RATE_SIM
    static struct ieee80211_sta_rates g_rates[4];
#endif
 /* allocate memory and init in alloc_sta*/
struct minstrel_ht_sta_priv *g_minstrel_ht_sta_priv = NULL;
struct minstrel_sta_info *g_minstrel_sta_info = NULL;
unsigned int g_channel_band = IEEE80211_BAND_5GHZ;
struct minstrel_priv *g_minstel_pri = NULL;
bool g_mcs_rate_support = true;
//struct ieee80211_tx_rate_control txrc;
int g_rate_mode = 0;   /*0: legacy rate, 1:ht rate, 2:vht rate*/

struct file *fp = NULL;
void aml_minstrel_attach(void)
{
    struct minstrel_rate_control_ops* p_rate_control_ops_ht = NULL;

    memset(&g_hw,0,sizeof(g_hw));
    memset(&g_wiphy,0,sizeof(g_wiphy));

    g_hw.max_rates = 4;
    g_hw.max_rate_tries = 3;
    g_hw.wiphy = &g_wiphy;
    g_hw.wiphy->bands[NL80211_BAND_2GHZ] = &aml_band_24ghz;
    g_hw.wiphy->bands[NL80211_BAND_5GHZ] = &aml_band_5ghz;
    p_rate_control_ops_ht = get_rate_control_ops_ht();
    g_minstel_pri = p_rate_control_ops_ht->alloc(&g_hw);
}

void aml_minstrel_detach(void)
{
    struct minstrel_rate_control_ops *p_rate_control_ops = NULL;

    printk("%s\n", __func__);
    g_aml_rate_adaptation_dev.ht_cap_info = 0;
    p_rate_control_ops = get_rate_control_ops();
    p_rate_control_ops->free(g_minstel_pri);
    g_minstel_pri = NULL;
}

unsigned int support_legacy_rate_init( struct wifi_station *sta ,  struct ieee80211_sta *p_ieee_sta,unsigned int channel_band)
{
    int i = 0;
    unsigned int bit_val = 0;

    for( i = 0; i<sta->sta_rates.dot11_rate_num; i++){
        switch(sta->sta_rates.dot11_rate[i]&0x7f){
            case 0x02: /*1M*/
                bit_val |= 0x1;
                break;

            case 0x04:  /*2M*/
                  bit_val |= 0x2;
                break;

            case 0x0b:   /*5.5M*/
                bit_val |= 0x4;
                break;

            case 0x16: /*11M*/
                 bit_val |= 0x8;
                break;

            case 0x0c:  /*6M*/
                 bit_val |= 0x10;
                break;

            case 0x12:  /*9M*/
                  bit_val |= 0x20;
                break;

            case 0x18: /*12M*/
                bit_val |= 0x40;
                break;

            case 0x24: /*18M*/
                bit_val |= 0x80;
                break;

            case 0x30:   /*24M*/
                 bit_val |= 0x100;
                break;

            case 0x48:   /*36M*/
                bit_val |= 0x200;
                break;

            case 0x60:   /*48M*/
                bit_val |= 0x400;
                break;

            case 0x6c:   /*54M*/
                bit_val |= 0x800;
                break;
            default :
                printk("%s, %d input rate error\n", __func__, __LINE__);
               break;
        }
    }

    if (channel_band == IEEE80211_BAND_2GHZ) {
        p_ieee_sta->supp_rates[IEEE80211_BAND_2GHZ]  = bit_val;

    } else if (channel_band == IEEE80211_BAND_5GHZ) {
        p_ieee_sta->supp_rates[IEEE80211_BAND_5GHZ] = (bit_val >> 4);

    } else {
        printk("%s, %d input channel_band error\n", __func__, __LINE__);
    }

    printk("%s, %d channel_band=%d, bit_val=0x%04x\n", __func__, __LINE__, channel_band, bit_val);
    return 0;
}

void aml_rate_adaptation_dev_init(struct wifi_station *sta, int rate_mode, unsigned int channel_band, struct ieee80211_sta *p_ieee_sta)
{
    g_aml_rate_adaptation_dev.num_rf_chains = 1;

    g_aml_rate_adaptation_dev.ht_cap_info = 0;
    g_aml_rate_adaptation_dev.vht_cap_info = sta->sta_vhtcap;

    if (rate_mode) {
        g_aml_rate_adaptation_dev.ht_cap_info |= WMI_HT_CAP_ENABLED;

    if (sta->sta_htcap & WMI_HT_CAP_HT20_SGI)
        g_aml_rate_adaptation_dev.ht_cap_info |= WMI_HT_CAP_HT20_SGI;

    if (sta->sta_htcap & WMI_HT_CAP_HT40_SGI)
        g_aml_rate_adaptation_dev.ht_cap_info |= WMI_HT_CAP_HT40_SGI;

    if (sta->sta_htcap & WMI_HT_CAP_DYNAMIC_SMPS)
        g_aml_rate_adaptation_dev.ht_cap_info |= WMI_HT_CAP_DYNAMIC_SMPS;

    if (sta->sta_htcap & WMI_HT_CAP_LDPC)
        g_aml_rate_adaptation_dev.ht_cap_info |= WMI_HT_CAP_LDPC;
    }

    if (channel_band == IEEE80211_BAND_2GHZ) {
        int i = 0;
        g_aml_rate_adaptation_dev.sband = &aml_band_24ghz;

        printk("support rate start\n");
        for (i = 0; i < sta->sta_rates.dot11_rate_num; i++)
        {
            printk("%02x  ",sta->sta_rates.dot11_rate[i]);
        }
        printk("\n");
    } else {
        g_aml_rate_adaptation_dev.sband = &aml_band_5ghz;
        g_aml_rate_adaptation_dev.sband->vht_cap = aml_create_vht_cap(&g_aml_rate_adaptation_dev, rate_mode);
    }

}
void aml_minstrel_init( 
#ifdef AUTO_RATE_SIM    
    void
#else    
    void *p_sta
#endif     
)
{
    struct minstrel_rate_control_ops *p_rate_control_ops = NULL;
    struct minstrel_rate_control_ops *p_rate_control_ops_ht = NULL;
    struct ieee80211_sta *p_ieee_sta = NULL;
    struct wifi_station *sta  = (struct wifi_station *)p_sta;
    struct minstrel_ht_sta_priv *p_minstrel_ht_sta_priv = NULL;
    struct minstrel_sta_info *p_minstrel_sta_info = NULL;
    unsigned int channel_band = IEEE80211_BAND_5GHZ;
    bool mcs_rate_support = true;
    int rate_mode = 0;   /*0: legacy rate, 1:ht rate, 2:vht rate*/

#ifdef AUTO_RATE_SIM
    memset(&g_rates, 0, sizeof(g_rates));
    rate_mode = g_rate_mode;
    g_sta.smps_mode = IEEE80211_SMPS_OFF;
    g_sta.rates = g_rates;
    p_ieee_sta = &g_sta;
#else
    DPRINTF(AML_DEBUG_WARNING, "%s: sta=%p\n", __func__, sta);
    p_ieee_sta = &(sta->ieee_sta);
    DPRINTF(AML_DEBUG_RATE, "%s(%d): p_ieee_sta=%p\n", __func__, __LINE__, p_ieee_sta);
    p_ieee_sta->smps_mode = IEEE80211_SMPS_OFF;
    p_ieee_sta->rates = &(sta->sta_ieee_rates);
    p_ieee_sta->bandwidth = sta->sta_chbw;
    DPRINTF(AML_DEBUG_RATE,"aml_minstrel_init,bandwidth=%d\n",  p_ieee_sta->bandwidth);

    if (sta->sta_wnet_vif->vm_curchan == NULL) {
        printk("%s vm_curchan is NULL, just return\n", __func__);
        return;
    }

    if (sta->sta_flags & WIFINET_NODE_VHT) {
        rate_mode = 2;
    } else if ( sta->sta_flags & WIFINET_NODE_HT) {
        rate_mode = 1;
    }

    if (WIFINET_IS_CHAN_2GHZ(sta->sta_wnet_vif->vm_curchan)) {
        channel_band = IEEE80211_BAND_2GHZ;
    } else {
        channel_band = IEEE80211_BAND_5GHZ;
    }
#endif

    /*0: legacy rate, 1:ht rate, 2:vht rate*/
    if (rate_mode) {
        g_mcs_rate_support = mcs_rate_support = true;

    } else {
        g_mcs_rate_support = mcs_rate_support = false;
    }

    DPRINTF(AML_DEBUG_WARNING, "%s(%d): channel_band=%d, rate_mode=%d\n", __func__, __LINE__, channel_band, rate_mode);

    support_legacy_rate_init(sta, p_ieee_sta, channel_band);

    aml_rate_adaptation_dev_init(sta, rate_mode, channel_band, p_ieee_sta);

    if (channel_band == IEEE80211_BAND_5GHZ) {
        p_ieee_sta->vht_cap = g_aml_rate_adaptation_dev.sband->vht_cap;
        DPRINTF(AML_DEBUG_RATE, "%s(%d):vht_supported = %d\n", __func__, __LINE__, p_ieee_sta->vht_cap.vht_supported);
    }

    aml_get_ht_cap(&g_aml_rate_adaptation_dev, &(g_aml_rate_adaptation_dev.sband->ht_cap));
    p_ieee_sta->ht_cap = g_aml_rate_adaptation_dev.sband->ht_cap;

    DPRINTF(AML_DEBUG_WARNING, "%s, ht_supported:%d, minstel_pri=%p, p_ieee_sta=%p\n", __func__, p_ieee_sta->ht_cap.ht_supported, g_minstel_pri, p_ieee_sta);

    p_rate_control_ops = get_rate_control_ops();
    p_rate_control_ops_ht = get_rate_control_ops_ht();
    if (mcs_rate_support) {
        p_minstrel_ht_sta_priv = p_rate_control_ops_ht->alloc_sta(g_minstel_pri, p_ieee_sta, GFP_ATOMIC);
        p_rate_control_ops_ht->rate_init(g_minstel_pri, g_aml_rate_adaptation_dev.sband, p_ieee_sta, p_minstrel_ht_sta_priv);
    } else {
        p_minstrel_sta_info = p_rate_control_ops->alloc_sta(g_minstel_pri, p_ieee_sta, GFP_ATOMIC);
        p_rate_control_ops->rate_init(g_minstel_pri, g_aml_rate_adaptation_dev.sband, p_ieee_sta, p_minstrel_sta_info);
    }

#ifdef AUTO_RATE_SIM
    g_minstrel_ht_sta_priv = p_minstrel_ht_sta_priv;
    g_minstrel_sta_info = p_minstrel_sta_info;
#else
    sta->sta_minstrel_ht_priv = p_minstrel_ht_sta_priv;
    sta->sta_minstrel_info = p_minstrel_sta_info;
    sta->minstrel_init_flag = 1;

    if (mcs_rate_support) {
        DPRINTF(AML_DEBUG_WARNING, "%s: sta:%p, sta_minstrel_ht_priv:%p\n", __func__, sta, sta->sta_minstrel_ht_priv);
    } else {
        DPRINTF(AML_DEBUG_WARNING, "%s: sta:%p, sta_minstrel_info:%p\n", __func__, sta, sta->sta_minstrel_info);
    }
#endif
}

void aml_minstrel_deinit(void *p_sta)
{
    struct wifi_station *sta = (struct wifi_station *)p_sta;
    struct minstrel_rate_control_ops* p_rate_control_ops = NULL;
    struct minstrel_rate_control_ops *p_rate_control_ops_ht = NULL;
    printk("%s:%04x ", __func__, sta->sta_flags);

    if ((sta->sta_flags & WIFINET_NODE_VHT) || (sta->sta_flags & WIFINET_NODE_HT)) {
        p_rate_control_ops_ht = get_rate_control_ops_ht();
        printk("ht free:%p\n", sta->sta_minstrel_ht_priv);
        p_rate_control_ops_ht->free_sta(sta->sta_minstrel_ht_priv);
        sta->sta_minstrel_ht_priv = NULL;

    } else {
        p_rate_control_ops = get_rate_control_ops();
        printk("free:%p\n", sta->sta_minstrel_info);
        p_rate_control_ops->free_sta(sta->sta_minstrel_info);
        sta->sta_minstrel_info = NULL;
    }
    sta->minstrel_init_flag = 0;
}

void fix_rate_set( unsigned int mcs)
{
	if (mcs != 0) {
		g_minstel_pri->fixed_rate_idx = mcs;
		//g_minstel_pri->fixed_rate_idx = (unsigned int)( unsigned int)25*10  +( (unsigned int)mcs&0xf  );
		DPRINTF(AML_DEBUG_RATE, "%s(%d):set static rate=%d\n", __func__, __LINE__, mcs);
	} else {
		g_minstel_pri->fixed_rate_idx = (u32) -1;
		DPRINTF(AML_DEBUG_RATE, "%s(%d): set auto rate\n", __func__, __LINE__);
	}
}

static void rate_control_fill_sta_table(struct ieee80211_sta *sta,
    struct ieee80211_tx_info *info, struct ieee80211_tx_rate *rates, int max_rates)
{
    struct ieee80211_sta_rates *ratetbl = NULL;
    int i;

    ratetbl = (sta->rates);

    /* Fill remaining rate slots with data from the sta rate table. */
    max_rates = MIN(max_rates, IEEE80211_TX_RATE_TABLE_SIZE);
    for (i = 0; i < max_rates; i++) {
        if ((i < ARRAY_SIZE(info->control.rates)) && (info->control.rates[i].idx >= 0) && info->control.rates[i].count) {
            if (rates != info->control.rates)
                rates[i] = info->control.rates[i];

        } else if (ratetbl) {
            rates[i].idx = ratetbl->rate[i].idx;
            rates[i].flags = ratetbl->rate[i].flags;
            rates[i].count = 2;//ratetbl->rate[i].count;

        } else {
            rates[i].idx = -1;
            rates[i].count = 0;
        }

        if (rates[i].idx < 0 || !rates[i].count)
            break;
    }

    rates[3].idx = -1;
    rates[3].count = 0;
    rates[3].flags  = 0;

    for (i = 0; i < max_rates; i++) {
        if (rates[i].idx < 0) {
            continue;
        }
        //DPRINTF(AML_DEBUG_RATE, "%s(%d): rates[%d].idx=%d,count=%d, flags=%d\n", __func__, __LINE__, i, rates[i].idx, rates[i].count, rates[i].flags);
    }
}

int minsstrel_rate_index_to_vendor_rate_code(int minstrel_rate_idx, struct ieee80211_sta *p_ieee80211_sta)
{
    enum ieee80211_band band = g_aml_rate_adaptation_dev.sband->band;

    if (p_ieee80211_sta->vht_cap.vht_supported && ((minstrel_rate_idx >= 0) && (minstrel_rate_idx <= 9))) {
        return WIFINET_RATE_VHT_MCS + minstrel_rate_idx;

    } else if (p_ieee80211_sta->ht_cap.ht_supported && ((minstrel_rate_idx >= 0) && (minstrel_rate_idx <= 7))) {
        return WIFINET_RATE_MCS + minstrel_rate_idx;

    } else {
        if (band == IEEE80211_BAND_5GHZ) {
            /*because 5G band haven't 11b rate ,so need plus 4*/
            return minstrel_rate_idx += 4;

        } else {
            return  minstrel_rate_idx;
        }
    }

    DPRINTF(AML_DEBUG_RATE,"rate convert error function %s, line =%d\n",__func__, __LINE__ );
    return 0;
}

unsigned int protocol_rate_to_vendor_rate(unsigned int protocol_rate)
{

    //For 11b: (0x82 -0x80) * 500K =  1M
    //For 11g:  0xc * 500k = 6M
    unsigned int ret = 0;
    //DPRINTF(AML_DEBUG_RATE,"rate 0x%x %s(%d)\n", protocol_rate, __func__, __LINE__);
    switch(protocol_rate)
    {
        case  0x02:// 1M
            ret = WIFI_11B_1M;
            break;
        case   0x04:// 2M
             ret = WIFI_11B_2M;
            break;
        case   0x0b://5.5M
            ret = WIFI_11B_5M;
            break;
        case   0x16://11M
             ret = WIFI_11B_11M;
            break;
        case   0x0c://6M
             ret = WIFI_11G_6M;
            break;
        case   0x12://9M
             ret = WIFI_11G_9M;
            break;
        case   0x18://12M
            ret = WIFI_11G_12M;
            break;
        case   0x24: //18M
            ret = WIFI_11G_18M;
            break;
        case    0x30: //24M
            ret = WIFI_11G_24M;
            break;
        case   0x48:// 36M
            ret = 9;
            break;
        case    0x60:// 48M
            ret = WIFI_11G_48M;
            break;
        case    0x6c:// 54M
             ret = WIFI_11G_54M ;
            break;
        default:
            printk("protocol rate to vendor rate convert errore protocol_rate = 0x%x %s(%d)\n", protocol_rate, __func__, __LINE__);
            ret =  0;
            break;

    }
    
     return ret;
}


unsigned char minstrel_find_rate(
    struct aml_ratecontrol ratectrl[]
#ifndef AUTO_RATE_SIM
,
   void *p_sta
#endif 
)
{
    struct ieee80211_tx_info tx_info;
    struct ieee80211_tx_info *info = &tx_info;
    struct wifi_station *sta  = (struct wifi_station *)p_sta ;
    int i;
    struct minstrel_rate_control_ops* p_rate_control_ops = NULL;
    void *priv_sta = NULL;
    int mcs_rate = 0;
    struct ieee80211_sta *p_ieee_sta;
    struct minstrel_ht_sta_priv *p_minstrel_ht_sta_priv = NULL;
    struct minstrel_sta_info *p_minstrel_sta_info = NULL;
    int rate_mode = 0;   /*0: legacy rate, 1:ht rate, 2:vht rate*/

#ifdef AUTO_RATE_SIM
    rate_mode = g_rate_mode;
    p_ieee_sta = &g_sta;
    p_minstrel_ht_sta_priv = g_minstrel_ht_sta_priv;
    p_minstrel_sta_info = g_minstrel_sta_info;
#else
    p_ieee_sta = &(sta->ieee_sta);
    p_minstrel_ht_sta_priv = sta->sta_minstrel_ht_priv;
    p_minstrel_sta_info = sta->sta_minstrel_info;

    if (!sta->minstrel_init_flag) {
        printk("%s minstrel not init, sta:%p\n", __func__, sta);
        return 0;
    }

    if (sta->sta_flags & WIFINET_NODE_VHT) {
        rate_mode = 2;
    } else  if (sta->sta_flags & WIFINET_NODE_HT) {
        rate_mode = 1;
    } else {
        rate_mode = 0;
    }

    if (sta->sta_wnet_vif->vm_fixed_rate.mode == WIFINET_FIXED_RATE_MCS) {
        if ((sta->sta_wnet_vif->vm_fixed_rate.rateinfo)&0x80) {
            g_minstel_pri->fixed_rate_idx = sta->sta_wnet_vif->vm_fixed_rate.rateinfo;
        } else {
            g_minstel_pri->fixed_rate_idx = protocol_rate_to_vendor_rate(sta->sta_wnet_vif->vm_fixed_rate.rateinfo);
        }
    } else {
        g_minstel_pri->fixed_rate_idx = ((u32) -1);
    }
#endif

    if (rate_mode >= 1) {
        mcs_rate = 1;
    }

    memset(&tx_info, 0,sizeof(struct ieee80211_tx_info));
    for (i = 0; i < IEEE80211_TX_MAX_RATES; i++) {
        info->control.rates[i].idx = -1;
        info->control.rates[i].flags = 0;
        info->control.rates[i].count = 0;
    }

    if (g_minstel_pri->fixed_rate_idx != ((u32) -1)) {
        for (i = 0; i < IEEE80211_TX_MAX_RATES-1; i++) {
            ratectrl[i].vendor_rate_code = g_minstel_pri->fixed_rate_idx;
            ratectrl[i].rate_index = g_minstel_pri->fixed_rate_idx&0xf;
            info->control.rates[i].idx = ratectrl[i].rate_index;
            //ratectrl[i].flags = minstrel_mcs_groups[25].flags;
            ratectrl[i].trynum = 2;
            ratectrl[i].maxampdulen = max_4ms_framelen[0][HT_RC_2_MCS(ratectrl[i].vendor_rate_code )];
            // DPRINTF(AML_DEBUG_RATE, "%s(%d): vendor_rate_code=0x%x, rate_index=%d, maxampdulen=%d\n", 
            //                 __func__, __LINE__,  ratectrl[i].vendor_rate_code, ratectrl[i].rate_index,  ratectrl[i].maxampdulen); 
        }

        return 1;
    }

    if (mcs_rate == 1) {
        p_rate_control_ops =  get_rate_control_ops_ht();
        priv_sta = p_minstrel_ht_sta_priv;

    } else {
        p_rate_control_ops =  get_rate_control_ops();
        priv_sta = p_minstrel_sta_info;
    }

    if (priv_sta == NULL) {
        DPRINTF(AML_DEBUG_WARNING, "%s: mcs_rate=%d, sta:%p\n", __func__,  mcs_rate, sta);
    }

    p_rate_control_ops->get_rate(g_minstel_pri, p_ieee_sta , priv_sta, info);
    rate_control_fill_sta_table(p_ieee_sta, info, info->control.rates, ARRAY_SIZE(info->control.rates) - 1) ;

    for (i = 0; i < ARRAY_SIZE(info->control.rates); i++) {
        ratectrl[i].rate_index = info->control.rates[i].idx;
        ratectrl[i].shortgi_en = info->control.rates[i].flags & IEEE80211_TX_RC_SHORT_GI ? 1: 0;
        ratectrl[i].vendor_rate_code = minsstrel_rate_index_to_vendor_rate_code(ratectrl[i].rate_index, p_ieee_sta);
        ratectrl[i].trynum = info->control.rates[i].count;
        ratectrl[i].flags = info->control.rates[i].flags;

        if (IS_MCS_RATE(ratectrl[i].vendor_rate_code)) {
            ratectrl[i].maxampdulen = max_4ms_framelen[0][HT_RC_2_MCS(ratectrl[i].vendor_rate_code)];
        }

        if (info->control.rates[i].idx < 0) {
            continue;
        }
        DPRINTF(AML_DEBUG_RATE, "%s(%d): ratectrl[%d].rate_index =%d, vendor_rate_code =0x%x, trynum =%d, maxampdulen=%d, flags=%d\n", __func__, __LINE__, i,
            ratectrl[i].rate_index,ratectrl[i].vendor_rate_code,ratectrl[i].trynum,ratectrl[i].maxampdulen, info->control.rates[i].flags);
    }

    ratectrl[0].trynum = 30;
    ratectrl[1].trynum = 30;
    ratectrl[2].trynum = 40;

    if (info->flags & IEEE80211_TX_CTL_RATE_CTRL_PROBE) {
        ratectrl[0].flags  |= HAL_RATECTRL_USE_SAMPLE_RATE;
    }

    return 1;
}



void minstrel_tx_complete( 
    struct aml_ratecontrol *rc
#ifndef AUTO_RATE_SIM
,
   void *p_sta
#endif 
)
{
    void *priv_sta = NULL;
    struct wifi_station *sta = ( struct wifi_station * )p_sta;
    struct minstrel_rate_control_ops* p_rate_control_ops = NULL;
    struct minstrel_ht_sta_priv *p_minstrel_ht_sta_priv = NULL;
    struct minstrel_sta_info *p_minstrel_sta_info = NULL;
    struct ieee80211_tx_info info;
    struct ieee80211_tx_rate *ar = info.status.rates;
    int i = 0;
    int rate_mode = 0;
    int mcs_rate = 0;

#ifdef AUTO_RATE_SIM
    p_minstrel_ht_sta_priv = g_minstrel_ht_sta_priv;
    p_minstrel_sta_info  =   g_minstrel_sta_info;
    rate_mode =  g_rate_mode;
#else
    p_minstrel_ht_sta_priv = sta->sta_minstrel_ht_priv;
    p_minstrel_sta_info = sta->sta_minstrel_info; 

    if (!sta->minstrel_init_flag) {
        printk("%s minstrel not init, sta:%p\n", __func__, sta);
        return;
    }

    if (sta->sta_flags & WIFINET_NODE_VHT) {
        rate_mode = 2;
    } else  if (sta->sta_flags & WIFINET_NODE_HT) {
        rate_mode = 1;
    } else {
        rate_mode = 0;
    }
#endif 

    if (rate_mode >= 1) {
        mcs_rate = 1;
    }

    memset(&info, 0, sizeof(struct ieee80211_tx_info));
    if (rc[0].flags & HAL_RATECTRL_TX_SEND_SUCCES) {
        info.flags |= IEEE80211_TX_STAT_ACK;
        rc[0].flags &= ~HAL_RATECTRL_TX_SEND_SUCCES;
    }

    for (i = 0; i < IEEE80211_TX_MAX_RATES; i++) {
        ar[i].idx = -1;
        ar[i].count = 0;
        ar[i].flags = 0; 

        if (rc[i].trynum != 0) {//not use if trynum is 0
            ar[i].count = rc[i].trynum;
            ar[i].idx   = rc[i].rate_index;
            ar[i].flags = rc[i].flags;
            /*only used in vht debug*/
            //ar[i].flags |= IEEE80211_TX_RC_VHT_MCS;
        }
        //DPRINTF(AML_DEBUG_RATE, "%s(%d): ar[%d].idx=%d, count=%d, flags=%d\n", __func__, __LINE__,  i, ar[i].idx, ar[i].count, ar[i].flags);
    }

    if (mcs_rate == 1) {
        p_rate_control_ops =  get_rate_control_ops_ht();
        if (p_minstrel_ht_sta_priv == NULL) {
            return;
        }
        priv_sta = p_minstrel_ht_sta_priv;

    } else {
        p_rate_control_ops =  get_rate_control_ops();
        if (p_minstrel_sta_info == NULL) {
            return;
        }
        priv_sta = p_minstrel_sta_info;
    }

    p_rate_control_ops->tx_status(g_minstel_pri, g_aml_rate_adaptation_dev.sband,  priv_sta, &info);
}

void  minstrel_set_sta_bandwidth( int bw )
{
	g_sta.bandwidth = bw;
}

