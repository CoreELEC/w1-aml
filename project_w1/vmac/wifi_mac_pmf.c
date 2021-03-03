#include "wifi_mac_com.h"
#include "crypto/aes_wrap.h"
#include "crypto/wlancrypto_wrap.h"

#ifdef AML_WPA3

extern unsigned int fi_ahb_read(unsigned int addr);
extern void fi_ahb_write(unsigned int addr,unsigned int data);

void wifi_mac_disable_hw_mgmt_decrypt(void) {
    unsigned int reg_val;

    printk("%s\n", __func__);
    reg_val = fi_ahb_read(0x0a000308);
    fi_ahb_write(0x0a000308, reg_val | BIT(28));

    reg_val = fi_ahb_read(0x0a000314);
    fi_ahb_write(0x0a000314, reg_val | BIT(28));

    reg_val = fi_ahb_read(0x0a000320);
    fi_ahb_write(0x0a000320, reg_val | BIT(28));

    reg_val = fi_ahb_read(0x0a00032c);
    fi_ahb_write(0x0a00032c, reg_val | BIT(28));

    reg_val = fi_ahb_read(0x0a000338);
    fi_ahb_write(0x0a000338, reg_val | BIT(28));

    reg_val = fi_ahb_read(0x0a000340);
    fi_ahb_write(0x0a000340, reg_val | BIT(28));

    reg_val = fi_ahb_read(0x0a000354);
    fi_ahb_write(0x0a000354, reg_val | BIT(28));
}

unsigned char _bip_ccmp_protect(const unsigned char *key, size_t key_len,
    const unsigned char *data, size_t data_len, unsigned char *mic)
{
    unsigned char res = 1;

    if (key_len == 16) {
        if (omac1_aes_128(key, data, data_len, mic)) {
            res = 0;
            printk("%s : omac1_aes_128 fail!", __func__);
        }

    } else if (key_len == 32) {
        if (omac1_aes_256(key, data, data_len, mic)) {
            res = 0;
            printk("%s : omac1_aes_256 fail!", __func__);
        }

    } else {
        printk("%s : key_len not match!", __func__);
        res = 0;
    }

    return  res;
}

unsigned char aml_calculate_bip_mic(unsigned char gmcs, unsigned char *whdr_pos, int len,
    const unsigned char *key, const unsigned char *data, size_t data_len, unsigned char *mic)
{
    unsigned char res = 1;

    if (gmcs == WIFINET_CIPHER_AES_CMAC) {
        if (_bip_ccmp_protect(key, 16, data, data_len, mic) == 0) {
            res = 0;
            printk("%s : _bip_ccmp_protect(128) fail!", __func__);
        }

    } else {
        res = 0;
        printk("%s : unsupport dot11wCipher !\n", __func__);
    }

    return res;
}

int wifi_mac_parse_mmie(struct wifi_station *sta, struct sk_buff *skb, unsigned char subtype)
{
    unsigned char *bip_aad;
    unsigned char *mme;
    unsigned char *frm;
    unsigned int res = 0;
    unsigned int ori_len;
    unsigned short pkt_keyid = 0;
    unsigned long pkt_ipn = 0;
    unsigned long *local_ipn = 0;
    struct wifi_frame *wh;
    unsigned char mic[16];
    unsigned char mic_len;
    unsigned char mme_offset;
    unsigned short pkt_len = skb->len;
    unsigned char hdr_len = sizeof(struct wifi_frame);
    unsigned char i;

    if (sta->pmf_key.wk_key_type == WIFINET_CIPHER_AES_CMAC) {
        mic_len = 8;

    } else {
        mic_len = 16;
    }

    frm = skb->data;
    wh = (struct wifi_frame *)OS_SKBBUF_DATA(skb);

    printk("%s pkt before pkt_len:%d\n", __func__, pkt_len);
    for (i = 0; i < pkt_len; ++i) {
        printk("%02x", frm[i]);
    }
    printk("\n");

    if ((pkt_len < hdr_len) || (pkt_len - hdr_len < mic_len)) {
        printk("pkt_len too small\n");
        return -1;
    }

    mme_offset = (mic_len == 8) ? 18 : 26;
    mme = frm + pkt_len - mme_offset;

    if (*mme != MME_IE_ID) {
        printk("not mmie:%d\n", *mme);
        return -1;
    }

    /* copy key index */
    memcpy(&pkt_keyid, mme + 2, 2);
    pkt_keyid = le16_to_cpu(pkt_keyid);
    if (pkt_keyid != sta->pmf_key.wk_keyix) {
        printk("BIP key index error!\n");
        return -1;
    }

    /* save packet number */
    memcpy(&pkt_ipn, mme + 4, 6);
    pkt_ipn = le64_to_cpu(pkt_ipn);
    local_ipn = (unsigned long *)(sta->pmf_key.wk_keyrsc);
    /* BIP packet number should bigger than previous BIP packet */
    if (pkt_ipn <= *local_ipn) { /* wrap around? */
        printk("replay BIP packet, pkt_ipn%08x, local_ipn:%08x\n", pkt_ipn, *local_ipn);
        return -1;
    }

    ori_len = pkt_len - hdr_len + BIP_AAD_SIZE;
    bip_aad = ZMALLOC(ori_len, "bip_add", GFP_ATOMIC);
    if (bip_aad == NULL) {
        printk("BIP AAD allocate fail\n");
        return -1;
    }

    /* save the frame body + MME (w/o mic) */
    memcpy(bip_aad + BIP_AAD_SIZE, frm + hdr_len, pkt_len - hdr_len - mic_len);

    /* conscruct AAD, copy frame control field */
    memcpy(bip_aad, &wh->i_fc[0], 2);
    clear_retry(bip_aad);
    clear_pwr_mgt(bip_aad);
    clear_more_data(bip_aad);
    /* conscruct AAD, copy address 1 to address 3 */
    memcpy(bip_aad + 2, wh->i_addr1, 18);

    printk("%s bip_add before\n", __func__);
    for (i = 0; i < ori_len; ++i) {
        printk("%02x", bip_aad[i]);
    }
    printk("\n");

    if (aml_calculate_bip_mic(sta->pmf_key.wk_key_type, frm,
        pkt_len, sta->pmf_key.wk_key, bip_aad, ori_len, mic) == 0)
        goto bip_exit;

    /* MIC field should be last 8 bytes of packet (packet without FCS) */
    if (memcmp(mic, wh + pkt_len - mic_len, mic_len)) {
        *local_ipn = pkt_ipn;
        res = 1;
    } else {
        printk("BIP MIC error!\n");
    }

    printk("%s bip_add after\n", __func__);
    for (i = 0; i < ori_len; ++i) {
        printk("%02x", bip_aad[i]);
    }
    printk("\n");

bip_exit:
    FREE(bip_aad, "bip_aad");
    return res;
}

int aml_aes_decrypt(struct wifi_station *sta, struct sk_buff *skb, unsigned char *mgmt_data, unsigned char key_index)
{
    unsigned char *frm;
    struct ieee80211_hdr *hdr;
    unsigned char hdr_len;
    unsigned char key_len;
    struct wlan_net_vif *wnet_vif = sta->sta_wnet_vif;
    unsigned char *plain = NULL;
    size_t plain_len = 0;

    frm = skb->data;
    hdr = (struct ieee80211_hdr *)OS_SKBBUF_DATA(skb);
    hdr_len = sizeof(struct wifi_frame);
    key_len = wnet_vif->vm_nw_keys[key_index].wk_keylen;

    if (key_len == 16) { /* 128 bits */
        plain = ccmp_decrypt(wnet_vif->vm_nw_keys[key_index].wk_key, hdr, frm + hdr_len, /* PN + enc_data + MIC */
            skb->len - hdr_len, /* PN + enc_data + MIC */ &plain_len);

    } else if (key_len == 32) { /* 256 bits */
        plain = ccmp_256_decrypt(wnet_vif->vm_nw_keys[key_index].wk_key, hdr, frm + hdr_len, /* PN + enc_data + MIC */
            skb->len - hdr_len, /* PN + enc_data + MIC */&plain_len);
    }

    if (plain == NULL) {
        printk("%s fail\n", __func__);
        return 0;
    }

    /* Copy @plain back to @frame and free @plain */
    memcpy(frm + hdr_len + 8, plain, plain_len);
    FREE(plain, "plain");

    return 1;
}

int aml_pkt_decrypt(struct wifi_station *sta, struct sk_buff *skb, unsigned char *mgmt_data)
{
    int res = 0;
    unsigned char hdr_len = sizeof(struct wifi_frame);
    unsigned char *iv = skb->data + hdr_len;
    unsigned char key_index = (((iv[3]) >> 6) & 0x3);

    printk("%s key_index(%d)\n", __func__, key_index);
    switch (sta->sta_rsn.rsn_ucastcipherset) {
        case WIFINET_CIPHER_AES_OCB:
        case WIFINET_CIPHER_AES_CCM:
        case WIFINET_CIPHER_AES_CMAC:
            res = aml_aes_decrypt(sta, skb, mgmt_data, key_index);
            break;
        default:
            break;
    }

    return res;
}

int wifi_mac_mgmt_validate(struct wifi_station *sta, struct sk_buff *skb, unsigned char subtype) {
    unsigned char *frm;
    unsigned char category;
    struct wifi_frame *wh;
    unsigned char is_bmc;
    unsigned short data_len;
    unsigned char iv_len;
    unsigned char icv_len;
    unsigned char *mgmt_data;
    unsigned char hdr_len;
    unsigned short decrypt_res;

    frm = skb->data;
    wh = (struct wifi_frame *)OS_SKBBUF_DATA(skb);
    hdr_len = sizeof(struct wifi_frame);
    is_bmc = (WIFINET_IS_MULTICAST(wh->i_addr1));

    if (!(sta->sta_flags_ext & WIFINET_NODE_MFP)) {
        return 0;
    }

    printk("%s fc:%02x\n", __func__, wh->i_fc[1]);
    if (!(wh->i_fc[1] & WIFINET_FC1_WEP)) {
        if (subtype == WIFINET_FC0_SUBTYPE_ACTION)
            category = *(frm + sizeof(struct wifi_frame));

        if (is_bmc) {
            /* broadcast cases */
            if ((subtype == WIFINET_FC0_SUBTYPE_ACTION) && CATEGORY_IS_ROBUST(category)) {
                /* broadcast robust action frame need BIP check */
                goto bip_verify;
            }

            if (subtype == WIFINET_FC0_SUBTYPE_DISASSOC || subtype == WIFINET_FC0_SUBTYPE_DEAUTH) {
                /* broadcast deauth or disassoc frame need BIP check */
                goto bip_verify;
            }
            goto exit;

        } else {
            /* unicast cases */
            goto exit;
        }
    } else {
        /* cases to decrypt mgmt frame */
        /* set iv and icv length */
        SET_ICE_IV_LEN(&iv_len, &icv_len, sta->sta_rsn.rsn_ucastcipherset);

        /* actual management data frame body */
        data_len = skb->len - hdr_len - iv_len - icv_len;
        mgmt_data = ZMALLOC(data_len, "mgmt_data", GFP_ATOMIC);
        if (mgmt_data == NULL) {
            printk("mgmt_data alloc fail\n");
            goto fail;
        }

        #if 1
            /* dump the packet content before decrypt */
            {
                int pp;
                printk("pktlen = %d =>", skb->len);
                for (pp = 0; pp < skb->len; pp++)
                    printk(" %02x ", frm[pp]);
                printk("\n");
            }
        #endif

        decrypt_res = aml_pkt_decrypt(sta, skb, mgmt_data);
        if (decrypt_res) {
            printk("decrypt mgmt pkt fail\n");
            goto fail;
        }
        /* save actual management data frame body */
        memcpy(mgmt_data, frm + hdr_len + iv_len, data_len);
        /* overwrite the iv field */
        memcpy(frm + hdr_len, mgmt_data, data_len);
        /* remove the iv and icv length */
        skb->len = skb->len - iv_len - icv_len;
        FREE(mgmt_data, "mgmt_data");
        goto exit;

        #if 1
            /* print packet content after decryption */
            {
                int pp;
                printk("after decryption pktlen = %d @@=>", skb->len);
                for (pp = 0; pp < skb->len; pp++)
                    printk(" %02x ", frm[pp]);
                printk("\n");
            }
        #endif
    }

bip_verify:
    wifi_mac_parse_mmie(sta, skb, subtype);

exit:
    return 1;

fail:
    return 0;
}


int wifi_mac_add_mmie(struct wifi_station *sta, struct sk_buff *skb, unsigned char subtype) {
    unsigned char *frm;
    unsigned char category;
    struct wifi_frame *wh;
    unsigned short ori_len;
    unsigned char mme_cont[MME_IE_LENGTH] = {0};
    unsigned char mme_clen;
    unsigned char *mgmt_body;
    unsigned char *frm_end;
    unsigned char *bip_aad;
    unsigned short pkt_len_before_add_ie = skb->data_len;
    unsigned short frame_body_len;

    frm = skb->data;
    wh = (struct wifi_frame *)OS_SKBBUF_DATA(skb);

    if ((subtype != WIFINET_FC0_SUBTYPE_DISASSOC) && (subtype != WIFINET_FC0_SUBTYPE_DEAUTH)
        && (subtype != WIFINET_FC0_SUBTYPE_ACTION)) {
        return 1;
    }

    if (subtype == WIFINET_FC0_SUBTYPE_ACTION) {
        category = *(frm + sizeof(struct wifi_frame));
        if (CATEGORY_IS_NON_ROBUST(category))
            return 1;
    }

    wh->i_fc[1] |= WIFINET_FC1_WEP;
    frame_body_len = skb->len - sizeof(struct wifi_frame);
    ori_len = BIP_AAD_SIZE + frame_body_len;
    bip_aad = ZMALLOC(ori_len, "bip_add", GFP_ATOMIC);
    printk("%s len:%d, frame_body_len:%d\n", __func__, skb->len, frame_body_len);

    {
        /* broadcast robust mgmt frame, using BIP */
        unsigned char mic[16] = {0};

        /* IGTK key is not install ex: mesh MFP without IGTK */
        if (sta->pmf_key.wk_valid != 1)
            return 1;

        if (sta->pmf_key.wk_key_type == WIFINET_CIPHER_AES_CMAC) {
            mme_clen = 16;

        } else {
            mme_clen = 24;
        }

        mgmt_body = frm + sizeof(struct wifi_frame);
        frm_end = frm + skb->len - mme_clen - 2;

        if (0) {
            int pp;
            printk("Total packet: %d\n", skb->len);

            for (pp = 0; pp< skb->len; pp++)
                printk(" %02x ", frm[pp]);
            printk("\n");
        }

        mme_cont[0] = MME_IE_ID;
        mme_cont[1] = mme_clen;
        /* octent 0 and 1 is key index ,BIP keyid is 4 or 5, LSB only need octent 0 */
        mme_cont[2] = sta->pmf_key.wk_keyix;

        /* increase PN and apply to packet */
        sta->pmf_key.wk_keyrsc[5]++;
        memcpy(&mme_cont[4], &sta->pmf_key.wk_keyrsc[0], 6);

        /* add MME IE with MIC all zero, MME string doesn't include element id and length */
        memcpy(frm_end, mme_cont, mme_clen + 2);

        /* conscruct AAD, copy frame control field */
        memcpy(bip_aad, &wh->i_fc[0], 2);
        clear_retry(bip_aad);
        clear_pwr_mgt(bip_aad);
        clear_more_data(bip_aad);
        /* conscruct AAD, copy address 1 to address 3 */
        memcpy(bip_aad + 2, wh->i_addr1, 18);
        /* copy management fram body */
        memcpy(bip_aad + BIP_AAD_SIZE, mgmt_body, frame_body_len);

        /* dump total packet include MME with zero MIC */
        if (0) {
            int i;
            printk("bip_aad: ");
            for (i = 0; i < ori_len; i++)
                printk(" %02x ", bip_aad[i]);
            printk("\n");
        }

        /* calculate mic */
        if (aml_calculate_bip_mic(sta->pmf_key.wk_key_type, (unsigned char *)wh, pkt_len_before_add_ie,
            sta->pmf_key.wk_key, bip_aad, ori_len, mic) == 0)
                goto add_fail;

        /* dump calculated mic result */
        if (0) {
            int i;
            printk("Calculated mic result: ");
            for (i = 0; i < 16; i++)
                printk(" %02x ", mic[i]);
            printk("\n");
        }

        frm_end = frm + skb->len;
        /* copy right BIP mic value, total is 128bits, we use the 0~63 bits */
        if (sta->pmf_key.wk_key_type == WIFINET_CIPHER_AES_CMAC)
            memcpy(frm_end - 8, mic, 8);
        else
            memcpy(frm_end - 16, mic, 16);

        /*dump all packet after mic ok */
        if (0) {
            int pp;
            printk("Total packet 2: %d\n", skb->len);

            for (pp = 0; pp< skb->len; pp++)
                printk(" %02x ", frm[pp]);
            printk("\n");
        }
    }

    FREE(bip_aad, "bip_aad");
    return 0;

add_fail:
    FREE(bip_aad, "bip_aad");
    return 1;
}

int wifi_mac_sw_encrypt(struct wifi_station *sta, struct sk_buff *skb) {
    return 1;
}

int wifi_mac_send_sa_query(struct wifi_station *sta, unsigned char *frm, void *arg, int *result) {
    struct wifi_mac_action_sa_query *sa_query = NULL;
    struct wifi_mac_action_mgt_args *actionargs = NULL;
    unsigned char action;
    unsigned short transaction_identifier;

    actionargs = (struct wifi_mac_action_mgt_args *)arg;
    action = actionargs->action;
    transaction_identifier = actionargs->arg1;

    DPRINTF(AML_DEBUG_WARNING, "%s action:%d\n", __func__, action);

    sa_query = (struct wifi_mac_action_sa_query *)frm;
    sa_query->sa_header.ia_category = AML_CATEGORY_SA_QUERY;
    sa_query->sa_header.ia_action = action;
    sa_query->sa_transaction_identifier = transaction_identifier;
    return 0;
}

#endif//AML_WPA3
