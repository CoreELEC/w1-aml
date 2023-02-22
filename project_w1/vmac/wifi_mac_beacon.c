/*
 ****************************************************************************************
 *
 * Copyright (C) Amlogic 2010-2014
 *
 * Project: 11N 80211 mac  layer Software
 *
 * Description:
 *     wifi_mac beacon alloc/initial/update module
 *
 *
 ****************************************************************************************
 */

#include "wifi_mac_com.h"

static unsigned char *
wifi_mac_beacon_init(struct wifi_station *sta, struct wifi_mac_beacon_offsets *bo, unsigned char *frm)
{
    struct wlan_net_vif *wnet_vif = sta->sta_wnet_vif;
    struct wifi_mac *wifimac = sta->sta_wmac;
    unsigned short capinfo;
    struct wifi_mac_rateset *rs = &sta->sta_rates;

    KASSERT(wnet_vif->vm_curchan != WIFINET_CHAN_ERR, ("no bss chan"));
    printk("%s(%d)\n", __func__, __LINE__);

    memset(frm, 0, 8);
    frm += 8;
    *(unsigned short *)frm = htole16(wnet_vif->vm_bcn_intval);
    frm += 2;

    if (wnet_vif->vm_opmode == WIFINET_M_IBSS)
        capinfo = WIFINET_CAPINFO_IBSS;
    else
        capinfo = WIFINET_CAPINFO_ESS;

    if (wnet_vif->vm_flags & WIFINET_F_PRIVACY)
        capinfo |= WIFINET_CAPINFO_PRIVACY;

    if ((wnet_vif->vm_flags & WIFINET_F_SHPREAMBLE) && WIFINET_IS_CHAN_2GHZ(wnet_vif->vm_curchan))
        capinfo |= WIFINET_CAPINFO_SHORT_PREAMBLE;

    if (wifimac->wm_flags & WIFINET_F_SHSLOT)
        capinfo |= WIFINET_CAPINFO_SHORT_SLOTTIME;

    if (wifimac->wm_flags & WIFINET_F_DOTH)
        capinfo |= WIFINET_CAPINFO_SPECTRUM_MGMT;

    bo->bo_caps = (unsigned short *)frm;
    *(unsigned short *)frm = htole16(capinfo);

    frm += 2;
    *frm++ = WIFINET_ELEMID_SSID;
    if ((wnet_vif->vm_flags & WIFINET_F_HIDESSID) == 0) {
        *frm++ = sta->sta_esslen;
        memcpy(frm, sta->sta_essid, sta->sta_esslen);
        frm += sta->sta_esslen;

    } else {
        *frm++ = 0;
    }

    bo->bo_rates = frm;
    frm = wifi_mac_add_rates(frm, rs);

    /*VHT mode ,non-support DSSS Parameter Set*/
    *frm++ = WIFINET_ELEMID_DSPARMS;
    *frm++ = 1;
    bo->bo_channel = frm;
    *frm++ = wifi_mac_chan2ieee(wifimac, wnet_vif->vm_curchan);

    bo->bo_tim = frm;
    if (wnet_vif->vm_opmode == WIFINET_M_IBSS) {
        *frm++ = WIFINET_ELEMID_IBSSPARMS;
        *frm++ = 2;
        *frm++ = 0;
        *frm++ = 0;
        bo->bo_tim_len = 0;

    } else {
        struct wifi_mac_tim_ie *tie = (struct wifi_mac_tim_ie *) frm;

        tie->tim_ie = WIFINET_ELEMID_TIM;
        tie->tim_len = 4;
        tie->tim_count = 0;
        tie->tim_period = wnet_vif->vm_dtim_period;
        tie->tim_bitctl = 0;
        tie->tim_bitmap[0] = 0;
        frm += sizeof(struct wifi_mac_tim_ie);
        bo->bo_tim_len = 1;
    }
    bo->bo_tim_trailer = frm;

    if ((wnet_vif->vm_opmode == WIFINET_M_HOSTAP)
        && ((wifimac->wm_flags & WIFINET_F_DOTH) || (wifimac->wm_flags_ext & WIFINET_FEXT_COUNTRYIE))) {
        frm = wifi_mac_add_country(frm, wifimac);
    }

    if ((wnet_vif->vm_opmode == WIFINET_M_HOSTAP) && (wifimac->wm_flags & WIFINET_F_DOTH)) {
        *frm++ = WIFINET_ELEMID_PWRCNSTR;
        *frm++ = 1;
        *frm++ = WIFINET_PWRCONSTRAINT_VAL(wnet_vif);
    }
    bo->bo_chanswitch = frm;

    if (WIFINET_INCLUDE_11G(wnet_vif->vm_mac_mode)) {
        bo->bo_erp = frm;
        frm = wifi_mac_add_erp(frm, wifimac);
    }

    if (wnet_vif->vm_flags & WIFINET_F_WPA)
        frm = wifi_mac_add_wpa(frm, wnet_vif);

    frm = wifi_mac_add_xrates(frm, rs);
    if (wnet_vif->vm_flags & WIFINET_F_WME) {
        bo->bo_wme = frm;
        frm = wifi_mac_add_wme_param(frm, &wifimac->wm_wme[wnet_vif->wnet_vif_id], WIFINET_VMAC_UAPSD_ENABLED(wnet_vif));
        wnet_vif->vm_flags &= ~WIFINET_F_WMEUPDATE;
    }

    if (WIFINET_INCLUDE_11N(wnet_vif->vm_mac_mode) && wifi_mac_is_ht_enable(wnet_vif->vm_mainsta)) {
        bo->bo_htcap = frm;
        frm = wifi_mac_add_htcap(frm, sta);

        bo->bo_htinfo = frm;
        frm = wifi_mac_add_htinfo(frm, sta);

        if (!(wifimac->wm_flags_ext & WIFINET_FEXT_COEXT_DISABLE)) {
            bo->bo_obss_scan = frm;
            frm = wifi_mac_add_obss_scan(frm, sta);
            bo->bo_extcap = frm;
            frm = wifi_mac_add_extcap(frm, sta);
        }
    }

    if (wifi_mac_is_vht_enable(wnet_vif)) {
        bo->bo_vhtcap = frm;
        frm = wifi_mac_add_vht_cap(frm, sta);

        bo->bo_vhtop = frm;
        frm = wifi_mac_add_vht_opt(frm, sta, WIFINET_FC0_SUBTYPE_BEACON);
        //frm = wifi_mac_add_vht_txpw(frm, sta);
        //frm = wifi_mac_add_vht_ch_sw_wrp(frm, sta);
        //frm = wifi_mac_add_vht_ext_bss_ld(frm, sta);
        //frm = wifi_mac_add_vht_quiet_ch(frm, sta);
        //frm = wifi_mac_add_vht_op_md_ntf(frm, sta);
    }

    bo->bo_appie_buf = frm;
    bo->bo_appie_buf_len = 0;
    wnet_vif->vm_flags_ext |= WIFINET_FEXT_APPIE_UPDATE;
    bo->bo_tim_trailerlen = frm - bo->bo_tim_trailer;
    bo->bo_chanswitch_trailerlen = frm - bo->bo_chanswitch;
    bo->bo_initial = 1;
    bo->bo_bcn_seq = 0;
    return frm;
}

struct sk_buff *_wifi_mac_beacon_alloc(struct wifi_station *sta, struct wifi_mac_beacon_offsets *bo)
{
    struct wlan_net_vif *wnet_vif = sta->sta_wnet_vif;
    struct wifi_mac *wifimac = sta->sta_wmac;
    struct wifi_frame *wh;
    struct sk_buff *skb;
    int pktlen;
    unsigned char *frm;
    int htcaplen = 0, htinfolen = 0;

    htcaplen += sizeof(struct wifi_mac_ie_htcap);
    htinfolen += sizeof(struct wifi_mac_ie_htinfo);
    pktlen = 8 + sizeof(unsigned short) + sizeof(unsigned short)
        + 2 + sta->sta_esslen + 2 + WIFINET_RATE_SIZE
        /*VHT mode ,non-support DSSS Parameter Set*/
        + 2 + 1 //DSSS Parameter Set Len = 3Byte
        + 2 + 4 + wnet_vif->vm_tim_len
        + wifimac->wm_countryinfo.country_len + 2 + 3 + 5 + 2 + 1
        + 2 + (WIFINET_RATE_MAXSIZE - WIFINET_RATE_SIZE)
        + (wnet_vif->vm_caps & WIFINET_C_WME ? sizeof(struct wifi_mac_wme_param) : 0)
        + (wnet_vif->vm_caps & WIFINET_C_WPA ? (2 * sizeof(struct wifi_mac_ie_wpa)) : 0)
        + htcaplen + htinfolen + sizeof(struct wifi_mac_ie_obss_scan) + sizeof(struct wifi_mac_ie_ext_cap)
        + WIFINET_APPIE_MAX + sizeof(struct wifi_mac_ie_vht_cap) + sizeof(struct wifi_mac_ie_vht_opt)
        + sizeof(struct wifi_mac_ie_vht_txpwr_env) + sizeof(struct wifi_mac_ie_vht_ch_sw_wrp);

    skb = wifi_mac_get_mgmt_frm(wifimac, pktlen);
    if (skb == NULL) {
        WIFINET_DPRINTF_STA(AML_DEBUG_ANY, sta, "cannot get buf; size %u", pktlen);
        wnet_vif->vif_sts.sts_tx_no_buf++;
        return NULL;
    }
    frm = os_skb_put(skb, pktlen);


    frm = wifi_mac_beacon_init(sta, bo, frm);
    os_skb_trim(skb, frm - os_skb_data(skb));

    wh = (struct wifi_frame *)os_skb_push(skb, sizeof(struct wifi_frame));
    wh->i_fc[0] = WIFINET_FC0_VERSION_0 | WIFINET_FC0_TYPE_MGT | WIFINET_FC0_SUBTYPE_BEACON;
    wh->i_fc[1] = WIFINET_FC1_DIR_NODS;
    *(unsigned short *)wh->i_dur = 0;
    WIFINET_ADDR_COPY(wh->i_addr1, BROADCAST_ADDRESS);
    WIFINET_ADDR_COPY(wh->i_addr2, wnet_vif->vm_myaddr);
    WIFINET_ADDR_COPY(wh->i_addr3, sta->sta_bssid);
    *(unsigned short *)wh->i_seq = 0;

    return skb;
}

int _wifi_mac_beacon_update(struct wifi_station *sta,
                           struct wifi_mac_beacon_offsets *bo, struct sk_buff *skb, int mcast)
{
    struct wlan_net_vif *wnet_vif = sta->sta_wnet_vif;
    struct wifi_mac *wifimac = sta->sta_wmac;
    int len_changed = 0;
    unsigned short capinfo;
    struct wifi_channel *main_vmac_chan = NULL;
    unsigned char concurrent_set_channel = 1;

    if (bo->bo_initial != 1)
    {
        printk(KERN_CRIT "%s %d, bug to fix\n",__func__,__LINE__);
        return len_changed;
    }

    if ((wifimac->wm_flags & WIFINET_F_DOTH) && (wnet_vif->vm_flags & WIFINET_F_CHANSWITCH) &&
        (wnet_vif->vm_chanchange_count == wifimac->wm_doth_tbtt))
    {
        unsigned char *frm;
        struct wifi_mac_ie_vht_opt *vhtop = (struct wifi_mac_ie_vht_opt *) bo->bo_vhtop;
        int center_chan = 0;

        if (wnet_vif->vm_bandwidth == WIFINET_BWC_WIDTH20)
        {
            center_chan = wifimac->wm_doth_channel;
        }
        else if (wnet_vif->vm_bandwidth == WIFINET_BWC_WIDTH40)
        {
            if (wnet_vif->scnd_chn_offset == WIFINET_HTINFO_EXTOFFSET_ABOVE)
                center_chan = wifimac->wm_doth_channel + 2;
            else if (wnet_vif->scnd_chn_offset == WIFINET_HTINFO_EXTOFFSET_ABOVE)
                center_chan = wifimac->wm_doth_channel - 2;
            else
                center_chan = wifimac->wm_doth_channel;
        }
        else if (wnet_vif->vm_bandwidth == WIFINET_BWC_WIDTH80)
        {
            center_chan = vhtop->vht_op_ch_freq_seg1;
        }
        else
        {
            ERROR_DEBUG_OUT("not support bandwidth %d yet\n", wnet_vif->vm_bandwidth);
        }

    if (IS_APSTA_CONCURRENT(aml_wifi_get_con_mode())) {

        if (wifi_mac_is_others_wnet_vif_running(wnet_vif) == true) {
            main_vmac_chan = wifi_mac_get_main_vmac_channel(wifimac);

            if ((main_vmac_chan)
                && (main_vmac_chan->chan_pri_num == wnet_vif->vm_curchan->chan_pri_num)
                && (main_vmac_chan->chan_bw == wnet_vif->vm_bandwidth)
                && (center_chan == wifi_mac_Mhz2ieee(main_vmac_chan->chan_cfreq1, 0))) {

                concurrent_set_channel = 0;//no need set channel again due to bw
            }
        } else {
            /*no other vmac running, no need set concurrent channel*/
            concurrent_set_channel = 0;
        }


        if (concurrent_set_channel) { /*need think if system want to change channel*/
            struct wlan_net_vif *main_wnet_vif = wifi_mac_running_main_wnet_vif(wifimac);

            /*sta connect to 5G ap, softap need update channel/band/mac_mode as  sta*/
            if (main_wnet_vif && main_vmac_chan) {
                wnet_vif->vm_curchan = main_vmac_chan;
                wnet_vif->vm_bandwidth = main_vmac_chan->chan_bw;
                wnet_vif->vm_mac_mode = main_wnet_vif->vm_mac_mode;
                sta->sta_chbw = main_vmac_chan->chan_bw;

                if (wnet_vif->vm_curchan->chan_bw >= WIFINET_BWC_WIDTH40) {
                    wnet_vif->vm_htcap |= WIFINET_HTCAP_SUPPORTCBW40;
                }

                if (main_vmac_chan->chan_pri_num < wifi_mac_Mhz2ieee(main_vmac_chan->chan_cfreq1, 0)) {
                    wnet_vif->scnd_chn_offset = WIFINET_HTINFO_EXTOFFSET_ABOVE;
                } else if (main_vmac_chan->chan_pri_num > wifi_mac_Mhz2ieee(main_vmac_chan->chan_cfreq1, 0)) {
                    wnet_vif->scnd_chn_offset = WIFINET_HTINFO_EXTOFFSET_BELOW;
                } else{
                    wnet_vif->scnd_chn_offset = WIFINET_HTINFO_EXTOFFSET_NA;
                }

                wifi_mac_set_wnet_vif_channel(wnet_vif, main_vmac_chan->chan_pri_num, main_vmac_chan->chan_bw, wifi_mac_Mhz2ieee(main_vmac_chan->chan_cfreq1, 0));
                printk("set ap %s(%d) chan %d, mac mode %d, band %d\n",__func__,__LINE__,
                      wnet_vif->vm_curchan->chan_pri_num, wnet_vif->vm_mac_mode, wnet_vif->vm_bandwidth);
            }
        }
    } else {
        if (wifi_mac_set_wnet_vif_channel(wnet_vif, wifimac->wm_doth_channel, wnet_vif->vm_bandwidth, center_chan) == false)
        {
            return 0;
        }
    }
        WIFINET_BEACON_LOCK(wifimac);
        wnet_vif->vm_chanchange_count = 0;
        wnet_vif->vm_flags &= ~WIFINET_F_CHANSWITCH;
        wifimac->wm_flags &= ~WIFINET_F_CHANSWITCH;
        os_skb_pull(skb, sizeof(struct wifi_frame));
        os_skb_trim(skb, 0);
        frm = os_skb_data(skb);
        if (frm == NULL)
        {
            WIFINET_BEACON_UNLOCK(wifimac);
            printk("<running> %s %d ERROR\n",__func__,__LINE__);
            return 0;
        }
        os_skb_put(skb, wifi_mac_beacon_init(sta, bo, frm) - frm);
        os_skb_push(skb, sizeof(struct wifi_frame));
        WIFINET_BEACON_UNLOCK(wifimac);
        len_changed = 1;
        WIFINET_DPRINTF( AML_DEBUG_DEBUG, "::INFO::switch channel WIFINET_F_DOTH %d \n",*bo->bo_channel);
    }
    else if(wnet_vif->vm_flags & WIFINET_F_CHANSWITCH)
    {
        *bo->bo_channel= wifi_mac_chan2ieee(wifimac, wnet_vif->vm_curchan);
        WIFINET_BEACON_LOCK(wifimac);
        wnet_vif->vm_flags &= ~WIFINET_F_CHANSWITCH;
        WIFINET_BEACON_UNLOCK(wifimac);
        WIFINET_DPRINTF( AML_DEBUG_DEBUG, "::INFO::switch channel in beacon, channel %d \n",*bo->bo_channel);
    }
    WIFINET_BEACON_LOCK(wifimac);
    if (wnet_vif->vm_opmode == WIFINET_M_IBSS)
        capinfo = WIFINET_CAPINFO_IBSS;
    else
        capinfo = WIFINET_CAPINFO_ESS;

    if (wnet_vif->vm_flags & WIFINET_F_PRIVACY)
        capinfo |= WIFINET_CAPINFO_PRIVACY;
    if ((wnet_vif->vm_flags & WIFINET_F_SHPREAMBLE) &&
        WIFINET_IS_CHAN_2GHZ(wnet_vif->vm_curchan))
        capinfo |= WIFINET_CAPINFO_SHORT_PREAMBLE;
    if (wifimac->wm_flags & WIFINET_F_SHSLOT)
        capinfo |= WIFINET_CAPINFO_SHORT_SLOTTIME;
    if (wifimac->wm_flags & WIFINET_F_DOTH)
        capinfo |= WIFINET_CAPINFO_SPECTRUM_MGMT;
    *bo->bo_caps = htole16(capinfo);

    if (wnet_vif->vm_flags & WIFINET_F_WME)
    {
        struct wifi_mac_wme_state *wme = &wifimac->wm_wme[wnet_vif->wnet_vif_id];
        if (wme == NULL)
        {
            printk("<running> %s %d ERROR!!\n",__func__,__LINE__);
        }

        if (wnet_vif->vm_flags_ext & WIFINET_FEXT_WMETUN)
        {
            if (wme->wme_flags & WME_F_AGGRESSIVE)
            {
                if (wme->wme_hipri_traffic >
                    wme->wme_hipri_switch_thresh)
                {
                    WIFINET_DPRINTF_STA( AML_DEBUG_WME, sta,
                                         "%s: traffic %u, disable aggressive mode",
                                         __func__, wme->wme_hipri_traffic);
                    wme->wme_flags &= ~WME_F_AGGRESSIVE;
                    wifi_mac_wme_updateparams_locked(wnet_vif);
                    wme->wme_hipri_traffic =
                        wme->wme_hipri_switch_hysteresis;
                }
                else
                    wme->wme_hipri_traffic = 0;
            }
            else
            {
                if (wme->wme_hipri_traffic <=
                    wme->wme_hipri_switch_thresh)
                {
                    WIFINET_DPRINTF_STA( AML_DEBUG_WME, sta,
                                         "%s: traffic %u, enable aggressive mode",
                                         __func__, wme->wme_hipri_traffic);
                    wme->wme_flags |= WME_F_AGGRESSIVE;
                    wifi_mac_wme_updateparams_locked(wnet_vif);
                    wme->wme_hipri_traffic = 0;
                }
                else
                    wme->wme_hipri_traffic =
                        wme->wme_hipri_switch_hysteresis;
            }
        }

        if (wnet_vif->vm_flags & WIFINET_F_WMEUPDATE)
        {
            wifi_mac_add_wme_param(bo->bo_wme, wme, WIFINET_VMAC_UAPSD_ENABLED(wnet_vif));
            wnet_vif->vm_flags &= ~WIFINET_F_WMEUPDATE;
        }
    }

    {
        struct wifi_frame  *wh;
        wh = (struct wifi_frame  *)os_skb_data(skb);
        if (wh == NULL)
        {
            ERROR_DEBUG_OUT("<running> ERROR!!\n");

        } else {
            bo->bo_bcn_seq = (bo->bo_bcn_seq < 4095) ? (bo->bo_bcn_seq + 1) : 0;
                *(unsigned short *)(wh->i_seq) = htole16(bo->bo_bcn_seq << WIFINET_SEQ_SEQ_SHIFT);
        }
    }

    if (WIFINET_INCLUDE_11N(wnet_vif->vm_mac_mode) && wifi_mac_is_ht_enable(wnet_vif->vm_mainsta))
    {
        struct wifi_mac_ie_htinfo_cmn *htinfo;

        int txchwidth = ((wnet_vif->vm_bandwidth == WIFINET_BWC_WIDTH40)
                    || (wnet_vif->vm_bandwidth == WIFINET_BWC_WIDTH80))
                    ? WIFINET_HTINFO_TXWIDTH_2040 : WIFINET_HTINFO_TXWIDTH_20;

        if (wifi_mac_is_ht_enable(wnet_vif->vm_mainsta) )
        {
            if(bo->bo_htinfo == NULL)
            {
                WIFINET_BEACON_UNLOCK(wifimac);
                return 2;
            }

            htinfo = &((struct wifi_mac_ie_htinfo *)bo->bo_htinfo)->hi_ie;
            htinfo->hi_txchwidth = txchwidth;
        }
    }
    WIFINET_BEACON_UNLOCK(wifimac);
    if (wnet_vif->vm_opmode == WIFINET_M_HOSTAP)
    {
        struct wifi_mac_tim_ie *tie = (struct wifi_mac_tim_ie *) bo->bo_tim;

        WIFINET_BEACON_LOCK(wifimac);
        if (wnet_vif->vm_flags & WIFINET_F_TIMUPDATE)
        {
            unsigned int timlen, timoff, i;
            if (wnet_vif->vm_ps_pending != 0)
            {
                timoff = 128;
                for (i = 0; i < wnet_vif->vm_tim_len; i++)
                    if (wnet_vif->vm_tim_bitmap[i])
                    {
                        timoff = i &~ 1;
                        break;
                    }
                KASSERT(wnet_vif->vm_tim_len != 0, ("tim bitmap len error!!!"));
                for (i = wnet_vif->vm_tim_len-1; i >= timoff; i--)
                    if (wnet_vif->vm_tim_bitmap[i])
                        break;
                timlen = 1 + (i - timoff);
            }
            else
            {
                timoff = 0;
                timlen = 1;
            }
            if (timlen != bo->bo_tim_len)
            {
                int trailer_adjust = (int)(long)(tie->tim_bitmap+timlen) - (int)(long)(bo->bo_tim_trailer);
                memmove(tie->tim_bitmap+timlen, bo->bo_tim_trailer,
                        bo->bo_tim_trailerlen);
                bo->bo_tim_trailer = tie->tim_bitmap+timlen;
                bo->bo_chanswitch += trailer_adjust;
                bo->bo_wme += trailer_adjust;
                bo->bo_erp += trailer_adjust;
                bo->bo_appie_buf += trailer_adjust;
                if (timlen > bo->bo_tim_len)
                    os_skb_put(skb, timlen - bo->bo_tim_len);
                else
                    os_skb_trim(skb, os_skb_get_pktlen(skb) - (bo->bo_tim_len - timlen));
                bo->bo_tim_len = timlen;

                tie->tim_len = 3 + timlen;
                tie->tim_bitctl = timoff;
                len_changed = 1;
            }
            memcpy(tie->tim_bitmap, wnet_vif->vm_tim_bitmap + timoff,
                   bo->bo_tim_len);

            wnet_vif->vm_flags &= ~WIFINET_F_TIMUPDATE;

            WIFINET_DPRINTF_STA( AML_DEBUG_PWR_SAVE, sta,
                                 "%s: TIM updated, pending %u, off %u, len %u",
                                 __func__, wnet_vif->vm_ps_pending, timoff, timlen);
        }
        WIFINET_BEACON_UNLOCK(wifimac);
        {
            struct drv_private *drv_priv = wifimac->drv_priv;
            struct drv_txlist *txlist = &drv_priv->drv_txlist_table[HAL_WME_MCAST];

#ifdef  AML_MCAST_QUEUE
            if (wnet_vif->vm_mqueue_flag_send & MCAST_SEND_FLAG_BEACON_DTIM)
            {
                wnet_vif->vm_mqueue_flag_send = 0;
                wifimac->drv_priv->drv_ops.drv_txq_backup_send(drv_priv, txlist);

#ifdef CONFIG_P2P
                if (txlist->txlist_backup_qcnt && P2P_NoA_START_FLAG(wnet_vif->vm_p2p->HiP2pNoaCountNow))
                {
                    wnet_vif->vm_mqueue_flag_send |= MCAST_SEND_FLAG_NOA_END_RETRY;
                }
#endif
            }

            WIFINET_BEACON_LOCK(wifimac);
            if (tie->tim_count == 0)
                tie->tim_count = tie->tim_period - 1;
            else
                tie->tim_count--;

            tie->tim_bitctl &= ~1;
            if (txlist->txlist_backup_qcnt)
            {
                if ((tie->tim_count == 0) || (tie->tim_period == 1))
                {
                    tie->tim_bitctl |= 1;
                    if (wnet_vif->vm_ps_sta)
                    {
                        wnet_vif->vm_mqueue_flag_send |= MCAST_SEND_FLAG_BEACON_DTIM;
                    }
                }
            }
            WIFINET_BEACON_UNLOCK(wifimac);

#endif
        }
        WIFINET_BEACON_LOCK(wifimac);
        if ((wifimac->wm_flags & WIFINET_F_DOTH) && (wifimac->wm_flags & WIFINET_F_CHANSWITCH))
        {

            if (!wnet_vif->vm_chanchange_count)
            {
                wnet_vif->vm_flags |= WIFINET_F_CHANSWITCH;

                memmove(bo->bo_chanswitch + WIFINET_CHANSWITCHANN_BYTES,
                        bo->bo_chanswitch, bo->bo_chanswitch_trailerlen);

                bo->bo_chanswitch[0] = WIFINET_ELEMID_CHANSWITCHANN;
                bo->bo_chanswitch[1] = 3;
                bo->bo_chanswitch[2] = 1;
                bo->bo_chanswitch[3] = wifimac->wm_doth_channel;
                bo->bo_chanswitch[4] = wifimac->wm_doth_tbtt;
                bo->bo_chanswitch_trailerlen += WIFINET_CHANSWITCHANN_BYTES;
                bo->bo_tim_trailerlen += WIFINET_CHANSWITCHANN_BYTES;
                bo->bo_wme += WIFINET_CHANSWITCHANN_BYTES;
                bo->bo_erp += WIFINET_CHANSWITCHANN_BYTES;
                bo->bo_appie_buf += WIFINET_CHANSWITCHANN_BYTES;

                os_skb_put(skb, WIFINET_CHANSWITCHANN_BYTES);
                len_changed = 1;
            }
            else
            {
                bo->bo_chanswitch[4]--;
            }

        }
        if (((jiffies - wifimac->wm_time_nonerp_present) >= WIFINET_INACT_NONERP * HZ)
            && wifimac->wm_nonerpsta == 0 && (wifimac->wm_flags & WIFINET_F_USEPROT))
        {
            wifimac->wm_flags &= ~WIFINET_F_USEPROT;
            wifimac->wm_flags_ext |= WIFINET_FEXT_ERPUPDATE;
            wifi_mac_update_protmode(wifimac);
            wifi_mac_set_shortslottime(wifimac, 1);
        }
        if (wifimac->wm_flags_ext & WIFINET_FEXT_ERPUPDATE)
        {
            wifi_mac_add_erp(bo->bo_erp, wifimac);
            wifimac->wm_flags_ext &= ~WIFINET_FEXT_ERPUPDATE;
        }

        if ((wifimac->wm_flags & WIFINET_F_DOTH) && (wifimac->wm_flags & WIFINET_F_CHANSWITCH))
        {
            wnet_vif->vm_chanchange_count++;
            if (wnet_vif->vm_chanchange_count == wifimac->wm_doth_tbtt) {
                wnet_vif->vm_flags |= WIFINET_F_CHANSWITCH;
            }
        }

        if (((jiffies - wifimac->wm_time_noht_present) >= WIFINET_INACT_HT * HZ)
            && (wifimac->wm_flags_ext & WIFINET_F_NONHT_AP)) {
            wifimac->wm_flags_ext &= ~(WIFINET_F_NONHT_AP);
            wifi_mac_ht_prot(wifimac, sta, WIFINET_BEACON_UPDATE);
        }

        if (WIFINET_INCLUDE_11N(wnet_vif->vm_mac_mode) && wifi_mac_is_ht_enable(wnet_vif->vm_mainsta))
        {
            if (wifimac->wm_flags_ext & WIFINET_FEXT_HTUPDATE)
            {
                wifimac->wm_flags_ext &= ~WIFINET_FEXT_HTUPDATE;
                wifi_mac_add_htinfo(bo->bo_htinfo, sta);
                wifi_mac_add_htcap(bo->bo_htcap, sta);
                if (!(wifimac->wm_flags_ext & WIFINET_FEXT_COEXT_DISABLE))
                    wifi_mac_add_obss_scan(bo->bo_obss_scan, sta);
            }
        }
        WIFINET_BEACON_UNLOCK(wifimac);
    }
    WIFINET_BEACON_LOCK(wifimac);
    if (wnet_vif->vm_flags_ext & WIFINET_FEXT_BR_UPDATE)
    {
        int n;
        struct wifi_mac_rateset *rs = &wnet_vif->vm_mainsta->sta_rates;
        n = rs->dot11_rate_num;
        if (n > WIFINET_RATE_SIZE)
        {
            n = WIFINET_RATE_SIZE;
        }
        memcpy(bo->bo_rates + 2, rs->dot11_rate, n);
        bo->bo_rates[1] = n;
        wnet_vif->vm_flags_ext &= ~WIFINET_FEXT_BR_UPDATE;
    }
    WIFINET_BEACON_UNLOCK(wifimac);
#ifdef  CONFIG_P2P
    if (wnet_vif->app_ie[WIFINET_APPIE_FRAME_BEACON].length
        && (wnet_vif->vm_p2p->p2p_enable == 1)
        && (wnet_vif->vm_p2p->p2p_flag & P2P_NOA_START_FLAG_HI))
    {
        printk("%s(%d) noa_len %d\n",__func__,__LINE__,
            wnet_vif->app_ie[WIFINET_APPIE_FRAME_BEACON].length);
        vm_p2p_update_noa_count_start(wnet_vif->vm_p2p);
    }
#endif
    WIFINET_BEACON_LOCK(wifimac);
    if (wnet_vif->vm_flags_ext & WIFINET_FEXT_APPIE_UPDATE)
    {
        if (wnet_vif->app_ie[WIFINET_APPIE_FRAME_BEACON].length != bo->bo_appie_buf_len)
        {
            int diff_len = wnet_vif->app_ie[WIFINET_APPIE_FRAME_BEACON].length - bo->bo_appie_buf_len;;

            DPRINTF(AML_DEBUG_BEACON,"%s(%d), diff_len %d, len %d, tail %d, end %d \n",
                   __func__, __LINE__, diff_len, skb->len, skb->tail, skb->end);

            if (wnet_vif->app_ie[WIFINET_APPIE_FRAME_BEACON].length > bo->bo_appie_buf_len)
                os_skb_put(skb, diff_len);
            else
                os_skb_trim(skb, os_skb_get_pktlen(skb) + diff_len);

            bo->bo_appie_buf_len = wnet_vif->app_ie[WIFINET_APPIE_FRAME_BEACON].length;
            bo->bo_chanswitch_trailerlen += diff_len;
            bo->bo_tim_trailerlen += diff_len;

            len_changed = 1;
        }
        if (wnet_vif->app_ie[WIFINET_APPIE_FRAME_BEACON].ie != NULL)
        {
            memcpy(bo->bo_appie_buf,wnet_vif->app_ie[WIFINET_APPIE_FRAME_BEACON].ie,
                wnet_vif->app_ie[WIFINET_APPIE_FRAME_BEACON].length);
        }
        wnet_vif->vm_flags_ext &= ~WIFINET_FEXT_APPIE_UPDATE;
    }
    WIFINET_BEACON_UNLOCK(wifimac);

    return len_changed;
}

int wifi_mac_beacon_alloc(void * ieee, int wnet_vif_id)
{
    struct wifi_mac *wifimac = NET80211_HANDLE(ieee);
    struct wlan_net_vif *wnet_vif = NULL;
    struct sk_buff * skbbuf;
    struct wifi_station *sta;
    unsigned char bcn_rate = WIFI_11N_MCS0;
    int len;

    wnet_vif = wifi_mac_get_wnet_vif_by_vid(wifimac, wnet_vif_id);
    WIFINET_BEACONBUF_LOCK(wifimac);

    skbbuf = wnet_vif->vm_beaconbuf;
    if (skbbuf != NULL) {
        wifi_mac_beacon_free(ieee,  wnet_vif_id);
        skbbuf = NULL;
    }
    sta = wnet_vif->vm_mainsta;

    skbbuf = _wifi_mac_beacon_alloc(sta, &wnet_vif->vm_beaconbuf_offset);
    if (skbbuf == NULL) {
        DPRINTF(AML_DEBUG_ANY,"%s %d wifi_mac_beacon_alloc ERROR\n",__func__,__LINE__);
        WIFINET_BEACONBUF_UNLOCK(wifimac);
        return -ENOMEM;
    }
    wnet_vif->vm_beaconbuf = skbbuf;
    len = os_skb_get_pktlen(skbbuf);

    DPRINTF(AML_DEBUG_WARNING,"%s %d Put beacon to HW;  wnet_vif_id %d len %d Bcn init rate 0x%x flag %x\n",
            __func__,__LINE__,wnet_vif_id,len,bcn_rate,CHAN_BW_20M);

    wifimac->drv_priv->drv_ops.Phy_PutBeaconBuf(wifimac->drv_priv, wnet_vif_id, os_skb_data(skbbuf), len, bcn_rate, wnet_vif->vm_bandwidth);
    wifimac->drv_priv->drv_ops.Phy_SetBeaconStart(wifimac->drv_priv,wnet_vif_id,wnet_vif->vm_bcn_intval,0,wnet_vif->vm_opmode);

    WIFINET_BEACONBUF_UNLOCK(wifimac);
    DPRINTF(AML_DEBUG_BEACON,"<running> %s %d %d \n",__func__,__LINE__,wnet_vif->vm_bcn_intval);
    return 0;
}


int wifi_mac_beacon_alloc_ex(SYS_TYPE param1,
                            SYS_TYPE param2,SYS_TYPE param3,
                            SYS_TYPE param4,SYS_TYPE param5)
{
    void * ieee = (void *)param1;
    struct wlan_net_vif *wnet_vif = (struct wlan_net_vif *)param4;
    int wnet_vif_id = (int)param2;

    if (wnet_vif->wnet_vif_replaycounter != (int)param5) {
        return -1;
    }

    if (wnet_vif->vm_state != WIFINET_S_CONNECTED) {
        return -1;
    }

    printk("%s %d \n",__func__,__LINE__);
    return wifi_mac_beacon_alloc(ieee, wnet_vif_id);
}

int wifi_mac_sta_beacon_init(struct wlan_net_vif *wnet_vif)
{
    struct wifi_mac *wifimac = wnet_vif->vm_wmac;

    wifimac->drv_priv->drv_ops.Phy_beaconinit(wifimac->drv_priv,wnet_vif->wnet_vif_id, wnet_vif->vm_bcn_intval);
    return 0;
}

void wifi_mac_sta_beacon_init_ex (SYS_TYPE param1,
                                SYS_TYPE param2,SYS_TYPE param3,
                                SYS_TYPE param4,SYS_TYPE param5)
{
    struct wlan_net_vif *wnet_vif = (struct wlan_net_vif *)param4;
    if(wnet_vif->wnet_vif_replaycounter != (int)param5)
        return ;
    wifi_mac_sta_beacon_init(wnet_vif);
}

void wifi_mac_beacon_free(void * ieee, int wnet_vif_id)
{
    struct wifi_mac *wifimac = NET80211_HANDLE(ieee);
    struct wlan_net_vif *wnet_vif = NULL;

    wnet_vif = wifi_mac_get_wnet_vif_by_vid(wifimac, wnet_vif_id);
    if (wnet_vif->vm_beaconbuf != NULL)
    {
        struct sk_buff * skbbuf = ( struct sk_buff *)wnet_vif->vm_beaconbuf;
        wifi_mac_complete_wbuf(skbbuf, 0);
    }
    wnet_vif->vm_beaconbuf = NULL;
    wnet_vif->vm_beaconbuf_offset.bo_initial =0;
}

void wifi_mac_beacon_config(void * ieee, int wnet_vif_id)
{
    struct wifi_mac *wifimac = NET80211_HANDLE(ieee);
    struct wlan_net_vif *wnet_vif = NULL;
    struct wifi_station *sta = NULL;

    wnet_vif = wifi_mac_get_wnet_vif_by_vid(wifimac,wnet_vif_id);
    if (wnet_vif == NULL)
        return;
    sta = wnet_vif->vm_mainsta;
   wifimac->drv_priv->drv_ops.Phy_beaconinit(wifimac->drv_priv,wnet_vif_id, wnet_vif->vm_bcn_intval);
}

void wifi_mac_beacon_sync(void * ieee, int wnet_vif_id)
{
    wifi_mac_beacon_config(ieee, wnet_vif_id);
}


int wifi_mac_update_beacon(void * ieee, int wnet_vif_id,
                      struct sk_buff * skbbuf, int mcast)
{
    struct wifi_mac *wifimac = NET80211_HANDLE(ieee);
    struct wlan_net_vif *wnet_vif = NULL;
    int error = 0;

    wnet_vif = wifi_mac_get_wnet_vif_by_vid(wifimac, wnet_vif_id);
    if (wnet_vif == NULL)
        return -EINVAL;

    error = _wifi_mac_beacon_update(wnet_vif->vm_mainsta, &wnet_vif->vm_beaconbuf_offset, skbbuf, mcast);
    if (error==2)
    {
        /* FIXME: Remove, Beacon lock in drv_main.c bcn_tx_ok irq */
        WIFINET_BEACONBUF_UNLOCK(wifimac);
        printk("<running> %s %d  beacon_realloc\n",__func__,__LINE__);
        wifi_mac_beacon_alloc(ieee,  wnet_vif_id);
        WIFINET_BEACONBUF_LOCK(wifimac);
    }
    return error;
}

void wifi_mac_process_beacon_miss_ex(SYS_TYPE arg)
{
    struct wlan_net_vif *wnet_vif = (struct wlan_net_vif *)arg;
    struct wifi_mac *wifimac = wnet_vif->vm_wmac;

    printk("%s vid:%d, vm_bmiss_count:%d\n", __func__, wnet_vif->wnet_vif_id, wnet_vif->vm_bmiss_count);

    if (wnet_vif->vm_opmode != WIFINET_M_STA ||
        wnet_vif->vm_state != WIFINET_S_CONNECTED)
        return;

    /*
    * Actually, we will lost at least 25 beacons here,
    * because 'vm_swbmiss' timeout is 2500ms.
    */
    if (wnet_vif->vm_bmiss_count++ < WIFINET_BMISS_COUNT_MAX) {
        /* we should wakeup when beacon miss happened */
        if (wifi_mac_pwrsave_is_wnet_vif_sleeping(wnet_vif) == 0) {
            wifi_mac_pwrsave_wakeup(wnet_vif, WKUP_FROM_BCN_MISS);
        }
        /*if not in roaming mode, triger roaming */
        if ((wifimac->wm_roaming == WIFINET_ROAMING_BASIC) && (wnet_vif->vm_chan_roaming_scan_flag != 1)
             && (wnet_vif->vm_bmiss_count >= 2)) {
            printk("Miss beacon trigger roaming\n");
            wifi_mac_roaming_trigger(wnet_vif);
         }
        return;
    }

    /*
    * when 2 beacons lost: 1) sta is in roaming, then do asso
    * otherwise 2) sta lost ap totally then do scan again
    */
    wnet_vif->vm_bmiss_count = 0;
    if (wifimac->wm_roaming == WIFINET_ROAMING_FAST) {
        DPRINTF(AML_DEBUG_WARNING,"roaming bcn lost...\n");
        wifi_mac_top_sm(wnet_vif, WIFINET_S_ASSOC, 1);
    }
    else {
        DPRINTF(AML_DEBUG_WARNING,"bcn lost...re-scan\n");

        wnet_vif->vm_chan_roaming_scan_flag = 0;

        wifi_mac_top_sm(wnet_vif, WIFINET_S_SCAN, 0);
    }
}

static int wnet_vif_beaconmiss_taskid = -1;

int wifi_mac_process_beacon_miss(struct wlan_net_vif *wnet_vif)
{
    struct wifi_mac *wifimac = wnet_vif->vm_wmac;

    if (wnet_vif_beaconmiss_taskid == -1)
    {
        wnet_vif_beaconmiss_taskid = wifi_mac_register_behindTask(wifimac,wifi_mac_process_beacon_miss_ex);
        wifi_mac_call_task(wifimac,wnet_vif_beaconmiss_taskid,(SYS_TYPE)wnet_vif);
    }
    else
    {
        wifi_mac_call_task(wifimac,wnet_vif_beaconmiss_taskid,(SYS_TYPE)wnet_vif);
    }
    return 0;
}

int wifi_mac_set_beacon_miss_ex(struct wlan_net_vif *wnet_vif,
    unsigned char enable, int period)
{
    struct wifi_mac *wifimac = wnet_vif->vm_wmac;

    wifimac->drv_priv->drv_ops.drv_set_beacon_miss(wifimac->drv_priv, wnet_vif->wnet_vif_id, enable, period);
    return 0;
}

void wifi_mac_set_beacon_miss(SYS_TYPE param1,
                                SYS_TYPE param2,SYS_TYPE param3,
                                SYS_TYPE param4,SYS_TYPE param5)
{
    struct wlan_net_vif *wnet_vif = (struct wlan_net_vif *)param4;
    unsigned char enable = (unsigned char)param3;
    int period = (int)param5;

    if (enable == 1 && period < 100/*ms*/)
    {
        WIFINET_DPRINTF(AML_DEBUG_WARNING, "period: %d, error\n", period);
        return;
    }
    wifi_mac_set_beacon_miss_ex(wnet_vif, enable, period);
    return;
}

int wifi_mac_set_vsdb_ex(struct wlan_net_vif *wnet_vif, unsigned char enable)
{
    struct wifi_mac *wifimac = wnet_vif->vm_wmac;

    printk("%s, vid:%d, enable:%d\n", __func__, wnet_vif->wnet_vif_id, enable);
    wifimac->drv_priv->drv_ops.drv_set_vsdb(wifimac->drv_priv, wnet_vif->wnet_vif_id, enable);
    return 0;
}

int wifi_mac_set_vsdb(SYS_TYPE param1, SYS_TYPE param2,SYS_TYPE param3,
    SYS_TYPE param4,SYS_TYPE param5)
{
    struct wlan_net_vif *wnet_vif = (struct wlan_net_vif *)param4;
    unsigned char enable = (unsigned char)param3;

    wifi_mac_set_vsdb_ex(wnet_vif, enable);
    return 0;
}

