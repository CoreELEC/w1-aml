#ifndef _WIFI_SKBBUF_H_
#define _WIFI_SKBBUF_H_

#include <linux/skbuff.h>

#define OS_SKB_CB_MAXLEN (48)//kernel skb -> cb len = 48
#define os_skb_data(_skb) ((_skb)->data)
#define os_skb_cb(_skb) ((_skb)->cb)
#define os_skb_get_pktlen(_skb) ((_skb)->len)
#define os_skb_get_tailroom(_skb) skb_tailroom(_skb)
#define os_skb_get_priority(_skb) ((_skb)->priority)
#define os_skb_set_priority(_skb, _p) ((_skb)->priority = (_p))
#define os_skb_hdrspace(_skb) skb_headroom(_skb)
#define os_skb_next(_skb) ((_skb)->next)
#define os_skb_alloc(_len) dev_alloc_skb(ALIGN(_len, 4))

/*shijie.chen add assert and if() to debug free skb. */
#ifdef NET_SKBUFF_DATA_USES_OFFSET
#define os_skb_free(skbbuf)                 do{\
                                                            if ((os_skb_data(skbbuf) == NULL)\
                                                                || (skbbuf->head == NULL)\
                                                                || (skbbuf->tail == 0)\
                                                                || (skbbuf->end == 0)\
                                                                || ((skbbuf->data - skbbuf->head) > (skbbuf->tail))\
                                                                || ((skbbuf->tail > skbbuf->end)))\
                                                            {\
                                                                ASSERT((os_skb_data(skbbuf) != NULL));\
                                                                ASSERT((skbbuf->head != NULL));\
                                                                ASSERT((skbbuf->tail != 0));\
                                                                ASSERT((skbbuf->end != 0));\
                                                                ASSERT((skbbuf->head < skbbuf->data));\
                                                                ASSERT((skbbuf->data - skbbuf->head) < (skbbuf->tail));\
                                                                ASSERT((skbbuf->tail < skbbuf->end));\
                                                                printk("%s:%d, free skb fail\n", __func__, __LINE__);\
                                                            }\
                                                            else\
                                                                consume_skb(skbbuf);\
                                                                skbbuf = NULL;\
                                                        }\
                                                        while(0)
#else
#define os_skb_free(skbbuf) do{ consume_skb(skbbuf);  skbbuf = NULL; } while(0)
#endif

#define os_skb_push(_skb, _len) skb_push(_skb, _len)
#define os_skb_trim(_skb, _len) skb_trim(_skb,_len)
#define os_skb_pull(_skb, _len) skb_pull(_skb, _len)
#define os_skb_put(_skb, _len) skb_put(_skb, _len)

#define OS_SKBBUF struct sk_buff
#define OS_SKBBUF_FREE(_skb) os_skb_free(_skb)
#define OS_SKBBUF_ALLOC(_s) dev_alloc_skb(ALIGN(_s, 4))
#define OS_SKBBUF_DATA(_skb) ((_skb)->data)
#define OS_SKBBUF_LEN(_skb) ((_skb)->len)
#define OS_SKBBUF_PULL skb_pull
#define OS_SKBBUF_PUT skb_put

#endif//_WIFI_SKBBUF_H_
