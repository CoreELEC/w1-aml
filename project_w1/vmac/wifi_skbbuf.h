#ifndef _WIFI_SKBBUF_H_
#define _WIFI_SKBBUF_H_

#include <linux/skbuff.h>

//#define SKBBUF_DEBUG

#define OS_SKB_CB_MAXLEN (48)//kernel skb -> cb len = 48
#define os_skb_data(_skb) ((_skb)->data)
#define os_skb_cb(_skb) ((_skb)->cb)
#define os_skb_get_pktlen(_skb) ((_skb)->len)
#define os_skb_get_tailroom(_skb) skb_tailroom(_skb)
#define os_skb_get_priority(_skb) ((_skb)->priority)
#define os_skb_set_priority(_skb, _p) ((_skb)->priority = (_p))
#define os_skb_hdrspace(_skb) skb_headroom(_skb)
#define os_skb_next(_skb) ((_skb)->next)

#define os_skb_push(_skb, _len) skb_push(_skb, _len)
#define os_skb_trim(_skb, _len) skb_trim(_skb,_len)
#define os_skb_pull(_skb, _len) skb_pull(_skb, _len)
#define os_skb_put(_skb, _len) skb_put(_skb, _len)

#ifdef SKBBUF_DEBUG
static int skb_alloc_time = 0;
static int skb_free_time = 0;
#define os_skb_alloc(_len) do { dev_alloc_skb(ALIGN(_len, 4)); } while(0)
#define os_skb_free(skbbuf) do { consume_skb(skbbuf);  skbbuf = NULL; } while(0)
#define os_skb_copy(skb, priority) do { skb_copy(skb, priority); } while(0)
#define os_skb_copy_expand(skb, header_len, tail_len, priority) do { skb_copy_expand(skb, header_len, tail_len, priority); } while(0)
#else
#define os_skb_alloc(_len) dev_alloc_skb(ALIGN(_len, 4))
#define os_skb_free(skbbuf) do { consume_skb(skbbuf);  skbbuf = NULL; } while(0)
#define os_skb_copy(skb, priority) skb_copy(skb, priority)
#define os_skb_copy_expand(skb, header_len, tail_len, priority) skb_copy_expand(skb, header_len, tail_len, priority)
#endif

#endif//_WIFI_SKBBUF_H_
