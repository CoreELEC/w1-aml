#ifndef __VM_WIFI_ACL_H__
#define __VM_WIFI_ACL_H__

#define ACL_HASHSIZE    32
#define ACL_HASH(addr)  \
        (((const unsigned char *)(addr))[WIFINET_ADDR_LEN - 1] % ACL_HASHSIZE)
        
struct wifi_mac_aclator *wifi_mac_acl_attach(void);
void wifi_mac_acl_deattach(void);
#endif
