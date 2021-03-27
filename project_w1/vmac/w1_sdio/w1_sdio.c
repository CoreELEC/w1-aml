#include "w1_sdio.h"
#include <linux/mutex.h>

struct amlw1_hwif_sdio g_w1_hwif_sdio;
struct amlw1_hif_ops g_w1_hif_ops;

unsigned char w1_sdio_wifi_bt_alive;
unsigned char w1_sdio_driver_insmoded;
unsigned char w1_sdio_after_porbe;
static DEFINE_MUTEX(wifi_bt_sdio_mutex);

unsigned char (*host_wake_w1_req)(void);
int (*host_suspend_req)(struct device *device);
int (*host_resume_req)(struct device *device);

struct sdio_func *aml_priv_to_func(int func_n)
{
    ASSERT(func_n >= 0 &&  func_n < SDIO_FUNCNUM_MAX);
    return g_w1_hwif_sdio.sdio_func_if[func_n];
}

unsigned int aml_w1_bt_hi_read_word(unsigned int addr)
{
    unsigned int regdata = 0;
    unsigned int reg_tmp;
    /*
     * make sure function 5 section address-mapping feature is disabled,
     * when this feature is disabled,
     * all 128k space in one sdio-function use only
     * one address-mapping: 32-bit AHB Address = BaseAddr + cmdRegAddr
     */

    reg_tmp = g_w1_hif_ops.hi_read_word( RG_SDIO_IF_MISC_CTRL);

    if((reg_tmp & BIT(23)) != 1)
    {
        reg_tmp |= BIT(23);
        g_w1_hif_ops.hi_write_word( RG_SDIO_IF_MISC_CTRL, reg_tmp);
    }

    /*config msb 15 bit address in BaseAddr Register*/
    g_w1_hif_ops.hi_write_reg32(RG_SCFG_FUNC5_BADDR_A,addr & 0xfffe0000);
    g_w1_hif_ops.bt_hi_read_sram((unsigned char*)(SYS_TYPE)&regdata,
        /*sdio cmd 52/53 can only take 17 bit address*/
        (unsigned char*)(SYS_TYPE)(addr & 0x1ffff), sizeof(unsigned int));
    return regdata;
}

void aml_w1_bt_hi_write_word(unsigned int addr,unsigned int data)
{
    unsigned int reg_tmp;

    unsigned char* sdio_kmm = (unsigned char*)kzalloc(sizeof(data), GFP_DMA|GFP_ATOMIC);
    ASSERT(sdio_kmm);

    memcpy(sdio_kmm, &data, sizeof(data));
    /*
     * make sure function 5 section address-mapping feature is disabled,
     * when this feature is disabled, 
     * all 128k space in one sdio-function use only 
     * one address-mapping: 32-bit AHB Address = BaseAddr + cmdRegAddr 
     */
    reg_tmp = g_w1_hif_ops.hi_read_word(RG_SDIO_IF_MISC_CTRL);
    
    if((reg_tmp & BIT(23)) != 1)
    {
        reg_tmp |= BIT(23);
        g_w1_hif_ops.hi_write_word(RG_SDIO_IF_MISC_CTRL , reg_tmp);
    }
    /*config msb 15 bit address in BaseAddr Register*/
    g_w1_hif_ops.hi_write_reg32(RG_SCFG_FUNC5_BADDR_A,addr & 0xfffe0000);

    g_w1_hif_ops.bt_hi_write_sram(sdio_kmm,
        /*sdio cmd 52/53 can only take 17 bit address*/
        (unsigned char*)(SYS_TYPE)(addr & 0x1ffff), sizeof(unsigned int));

    kfree(sdio_kmm);
}

void aml_w1_bt_sdio_read_sram (unsigned char *buf, unsigned char *addr, SYS_TYPE len)
{
    g_w1_hif_ops.hi_bottom_read(SDIO_FUNC5, ((SYS_TYPE)addr & SDIO_ADDR_MASK),
        buf, len, (len > 8 ? SDIO_OPMODE_INCREMENT : SDIO_OPMODE_FIXED));
}

/*For BT use only start */
void aml_w1_bt_sdio_write_sram (unsigned char *buf, unsigned char *addr, SYS_TYPE len)
{
    //struct hal_private *hal_priv = hal_get_priv();
    unsigned char *sdio_kmm = (unsigned char*)kzalloc(len, GFP_DMA|GFP_ATOMIC);
    ASSERT(sdio_kmm);

    memcpy(sdio_kmm, buf, len);
    g_w1_hif_ops.hi_bottom_write(SDIO_FUNC5, ((SYS_TYPE)addr & SDIO_ADDR_MASK),
        sdio_kmm, len, (len > 8 ? SDIO_OPMODE_INCREMENT : SDIO_OPMODE_FIXED));

    kfree(sdio_kmm);
}

int aml_w1_sdio_write_reg32(unsigned long sram_addr, unsigned long sramdata)
{
    int ret = 0;
    unsigned char* sdio_kmm = (unsigned char*)kzalloc(sizeof(unsigned long), GFP_DMA |GFP_ATOMIC);
    ASSERT(sdio_kmm);
    memcpy(sdio_kmm, &sramdata, sizeof(unsigned long));
    
    ret=  g_w1_hif_ops.hi_bottom_write(SDIO_FUNC1, sram_addr&SDIO_ADDR_MASK,
        (unsigned char *)sdio_kmm,  sizeof(unsigned long), SDIO_OPMODE_INCREMENT);

    kfree(sdio_kmm);

    return ret;
}

unsigned int aml_w1_aon_read_reg(unsigned int addr)
{
    unsigned int regdata = 0;

    regdata = g_w1_hif_ops.bt_hi_read_word((addr));
    return regdata;
}

/* aon module address from 0x00f00000, we need read/write by sdio func5 */
void aml_w1_aon_write_reg(unsigned int addr,unsigned int data)
{
    g_w1_hif_ops.bt_hi_write_word((addr), data);
}

static int _aml_w1_sdio_request_byte(unsigned char   func_num,
                                    unsigned char   write,
                                    unsigned int      reg_addr,
                                    unsigned char  *byte)
{
    int err_ret;
    struct sdio_func * func = aml_priv_to_func(func_num);

#if defined(DBG_PRINT_COST_TIME)
    struct timespec now, before;
    getnstimeofday(&before);
#endif /* End of DBG_PRINT_COST_TIME */

    ASSERT(func != NULL);
    ASSERT(byte != NULL);
    ASSERT(func->num == func_num);

    /* Claim host controller */
    sdio_claim_host(func);

    if (write) {
        /* CMD52 Write */
        sdio_writeb(func, *byte, reg_addr, &err_ret);
    }
    else {
        /* CMD52 Read */
        *byte = sdio_readb(func, reg_addr, &err_ret);
    }

    /* Release host controller */
    sdio_release_host(func);

#if defined(DBG_PRINT_COST_TIME)
    getnstimeofday(&now);

    printk("[sdio byte]: len=1 cost=%lds %luus\n",
        now.tv_sec-before.tv_sec, now.tv_nsec/1000 - before.tv_nsec/1000);
#endif /* End of DBG_PRINT_COST_TIME */

    return (err_ret == 0) ? SDIOH_API_RC_SUCCESS : SDIOH_API_RC_FAIL;
}

static int _aml_w1_sdio_request_buffer(unsigned char func_num,
                                    unsigned int    fix_incr,
                                    unsigned char write,
                                    unsigned int    addr,
                                    void	            *buf,
                                    unsigned int    nbytes)
{
    int err_ret;
    int align_nbytes = nbytes;
    struct sdio_func * func = aml_priv_to_func(func_num);
    bool fifo = (fix_incr == SDIO_OPMODE_FIXED);

    ASSERT(buf != NULL);
    ASSERT(func != NULL);
    ASSERT(fix_incr == SDIO_OPMODE_FIXED|| fix_incr == SDIO_OPMODE_INCREMENT);
    ASSERT(func->num == func_num);

    /* Claim host controller */
    sdio_claim_host(func);

    if (write && !fifo)
    {
        /* write, increment */
        align_nbytes = sdio_align_size(func, nbytes);
        err_ret = sdio_memcpy_toio(func, addr, buf, align_nbytes);
    }
    else if (write)
    {
        /* write, fifo */
        err_ret = sdio_writesb(func, addr, buf, align_nbytes);
    }
    else if (fifo)
    {
        /* read */
        err_ret = sdio_readsb(func, buf, addr, align_nbytes);
    }
    else
    {
        /* read */
        align_nbytes = sdio_align_size(func, nbytes);
        err_ret = sdio_memcpy_fromio(func, buf, addr, align_nbytes);
    }

    /* Release host controller */
    sdio_release_host(func);

    return (err_ret == 0) ? SDIOH_API_RC_SUCCESS : SDIOH_API_RC_FAIL;
}

//cmd53
int aml_w1_sdio_bottom_read(unsigned char  func_num, int addr, void *buf, size_t len,int incr_addr)
{
    void *kmalloc_buf = NULL;
    int result;
    int align_len = 0;

    ASSERT(func_num != SDIO_FUNC0);

    if (host_wake_w1_req != NULL)
    {
        if (host_wake_w1_req() == 0)
        {
            printk("aml_w1_sdio_bottom_read, host wake w1 fail\n");
            return -1;
        }
    }

    /* read block mode */
    if (func_num != SDIO_FUNC6)
    {
        if (incr_addr == SDIO_OPMODE_INCREMENT)
        {
            struct sdio_func * func = aml_priv_to_func(func_num);
            align_len = sdio_align_size(func, len);
        }
        else
            align_len = len;

        kmalloc_buf = (unsigned char *)kzalloc(align_len, GFP_DMA|GFP_ATOMIC);
    }
    else
    {
        kmalloc_buf = buf;
    }
    if(kmalloc_buf == NULL)
    {
        printk("kmalloc buf fail\n");
        return SDIOH_API_RC_FAIL;
    }

    result = _aml_w1_sdio_request_buffer(func_num, incr_addr, SDIO_READ, addr, kmalloc_buf, len);

    if (func_num != SDIO_FUNC6)
    {
        memcpy(buf, kmalloc_buf, len);
        kfree(kmalloc_buf);
    }
    return result;
}


//cmd53
int aml_w1_sdio_bottom_write(unsigned char  func_num,int addr, void *buf, size_t len,int incr_addr)
{
    void *kmalloc_buf;
    int result;
  
    ASSERT(func_num != SDIO_FUNC0);

    if (host_wake_w1_req != NULL)
    {
        if (host_wake_w1_req() == 0)
        {
            printk("aml_w1_sdio_bottom_write, host wake w1 fail\n");
            return -1;
        }
    }

    kmalloc_buf =  (unsigned char *)kzalloc(len, GFP_DMA);//virt_to_phys(fwICCM);
    if(kmalloc_buf == NULL)
    {
        printk("kmalloc buf fail\n");
        return SDIOH_API_RC_FAIL;
    }
    memcpy(kmalloc_buf, buf, len);
    
    result = _aml_w1_sdio_request_buffer(func_num, incr_addr, SDIO_WRITE, addr, kmalloc_buf, len);

    kfree(kmalloc_buf);
    return result;
}

void aml_w1_sdio_read_sram (unsigned char *buf, unsigned char *addr, SYS_TYPE len)
{
    g_w1_hif_ops.hi_bottom_read(SDIO_FUNC2, (SYS_TYPE)addr&SDIO_ADDR_MASK,
        buf, len, (len > 8 ? SDIO_OPMODE_INCREMENT : SDIO_OPMODE_FIXED));
}


void aml_w1_sdio_write_sram (unsigned char *buf, unsigned char *addr, SYS_TYPE len)
{
    unsigned char* sdio_kmm = (unsigned char*)kzalloc(len, GFP_DMA |GFP_ATOMIC);
    ASSERT(sdio_kmm);
    memcpy(sdio_kmm, buf, len);

    g_w1_hif_ops.hi_bottom_write(SDIO_FUNC2, (SYS_TYPE)addr&SDIO_ADDR_MASK,
        sdio_kmm, len, (len > 8 ? SDIO_OPMODE_INCREMENT : SDIO_OPMODE_FIXED));

    kfree(sdio_kmm);
}

unsigned int aml_w1_sdio_read_word(unsigned int addr)
{
    unsigned int regdata = 0;

// for bt access always on reg
    if((addr & 0x00f00000) == 0x00f00000)
    {
        regdata = aml_w1_aon_read_reg(addr);
    }
    else if(((addr & 0x00f00000) == 0x00b00000)||
        ((addr & 0x00f00000) == 0x00d00000)||
        ((addr & 0x00f00000) == 0x00900000))
    {
        regdata = aml_w1_aon_read_reg(addr);
    }
    else if(((addr & 0x00f00000) == 0x00200000)||
        ((addr & 0x00f00000) == 0x00300000)||
        ((addr & 0x00f00000) == 0x00400000))
    {
        regdata = aml_w1_aon_read_reg(addr);
    }
    else
    {
        aml_w1_sdio_read_sram((unsigned char*)(SYS_TYPE)&regdata,
                                        (unsigned char*)(SYS_TYPE)(addr),
                                        sizeof(unsigned int));
    }
    return regdata;
}

void aml_w1_sdio_write_word(unsigned int addr, unsigned int data)
{
    unsigned char* sdio_kmm  = (unsigned char*)kzalloc(sizeof(unsigned int), GFP_DMA|GFP_ATOMIC);
    ASSERT(sdio_kmm);
    memcpy(sdio_kmm, &data, sizeof(data));
    // for bt access always on reg
    if((addr & 0x00f00000) == 0x00f00000)
    {
        aml_w1_aon_write_reg(addr, data);
    }
    else if(((addr & 0x00f00000) == 0x00b00000)||
        ((addr & 0x00f00000) == 0x00d00000)||
        ((addr & 0x00f00000) == 0x00900000))
    {
        aml_w1_aon_write_reg(addr, data);
    }
    else if(((addr & 0x00f00000) == 0x00200000)||
        ((addr & 0x00f00000) == 0x00300000)||
        ((addr & 0x00f00000) == 0x00400000))
    {
        aml_w1_aon_write_reg((addr), data);
    }
    else
    {
        aml_w1_sdio_write_sram(sdio_kmm,
                                    (unsigned char*)(SYS_TYPE)(addr),
                                    sizeof(unsigned int));
    }

    kfree(sdio_kmm);
}

//cmd52
int aml_w1_sdio_bottom_write8(unsigned char  func_num,int addr, unsigned char data)
{
    int ret = 0;
    unsigned char *sdio_kmm = (unsigned char*)kzalloc(sizeof(data), GFP_DMA |GFP_ATOMIC);

    ASSERT(sdio_kmm != NULL);
    ASSERT(func_num != SDIO_FUNC0);

    memcpy(sdio_kmm, &data, sizeof(char));
    ret =  _aml_w1_sdio_request_byte(func_num, SDIO_WRITE, addr, sdio_kmm);
    kfree(sdio_kmm);

    return ret;
}

//cmd52
unsigned char aml_w1_sdio_bottom_read8(unsigned char  func_num, int addr)
{
    unsigned char sramdata;
    unsigned char* sdio_kmm = (unsigned char*)kmalloc(sizeof(unsigned char), GFP_DMA |GFP_ATOMIC);
    ASSERT(sdio_kmm);
    
    _aml_w1_sdio_request_byte(func_num, SDIO_READ, addr, sdio_kmm);
    
    memcpy(&sramdata, sdio_kmm, sizeof(unsigned char));
    kfree(sdio_kmm);
    
    return sramdata;
}

/* cmd52
   buff[7:0],     // funcIoNum
   buff[25:8],    // regAddr
   buff[39:32]);  // regValue
*/
void aml_w1_sdio_bottom_write8_func0(unsigned long sram_addr, unsigned char sramdata)
{
    unsigned char * sdio_kmm = (unsigned char *)kzalloc(sizeof(unsigned char), GFP_DMA |GFP_ATOMIC);
    ASSERT(sdio_kmm);

    //__virt_to_phys((unsigned long)&sramdata);
    //__phys_addr_symbol((unsigned long)&sramdata);
   
    memcpy(sdio_kmm, &sramdata, sizeof(unsigned char));
    _aml_w1_sdio_request_byte(SDIO_FUNC0, SDIO_WRITE, sram_addr, sdio_kmm);

    kfree(sdio_kmm);
}

//cmd52
unsigned char aml_w1_sdio_bottom_read8_func0(unsigned long sram_addr)
{
    unsigned char sramdata;
    unsigned char* sdio_kmm = (unsigned char*)kzalloc(sizeof(unsigned char), GFP_DMA |GFP_ATOMIC);
    ASSERT(sdio_kmm);
    
    _aml_w1_sdio_request_byte(SDIO_FUNC0, SDIO_READ, sram_addr, sdio_kmm);

    memcpy( &sramdata, sdio_kmm,sizeof(unsigned char));

    kfree(sdio_kmm);
    return sramdata;
}

void aml_w1_sdio_write_cmd32(unsigned long sram_addr, unsigned long sramdata)
{
#if defined (HAL_SIM_VER)
    aml_sdio_read_write(sram_addr&SDIO_ADDR_MASK,	(unsigned char *)&sramdata, 4,
                        SDIO_FUNC3,SDIO_RW_FLAG_WRITE,SDIO_F_SYNCHRONOUS);
#elif defined (HAL_FPGA_VER)
   unsigned char* sdio_kmm = (unsigned char*)kzalloc(sizeof(unsigned long), GFP_DMA|GFP_ATOMIC);
   ASSERT(sdio_kmm != NULL);
   memcpy(sdio_kmm, &sramdata, sizeof(unsigned long));
   
   g_w1_hif_ops.hi_bottom_write(SDIO_FUNC3, sram_addr&SDIO_ADDR_MASK,
                                                 (unsigned char *)sdio_kmm, sizeof(unsigned long), SDIO_OPMODE_INCREMENT);

   kfree(sdio_kmm);                                              
#endif /* End of HAL_SIM_VER */
}

int aml_w1_sdio_suspend(unsigned int suspend_enable)
{
    mmc_pm_flag_t flags;
    struct sdio_func *func = NULL;
    int ret = 0, i;

    if (suspend_enable == 0)
    {
        /* when enable == 0 that's resume operation
        and we do nothing for resume now. */
        return ret;
    }

    /* just clear sdio clock value for emmc init when resume */
    //amlwifi_set_sdio_host_clk(0);

    /* we shall suspend all card for sdio. */
    for (i = SDIO_FUNC1; i <= FUNCNUM_SDIO_LAST; i++)
    {
        func = aml_priv_to_func(i);
        if (func == NULL)
            continue;
        flags = sdio_get_host_pm_caps(func);

        if ((flags & MMC_PM_KEEP_POWER) != 0)
            ret = sdio_set_host_pm_flags(func, MMC_PM_KEEP_POWER);

        if (ret != 0)
            return -1;

        /*
         * if we don't use sdio irq, we can't get functions' capability with
         * MMC_PM_WAKE_SDIO_IRQ, so we don't need set irq for wake up
         * sdio for upcoming suspend.
         */
        if ((flags & MMC_PM_WAKE_SDIO_IRQ) != 0)
            ret = sdio_set_host_pm_flags(func, MMC_PM_WAKE_SDIO_IRQ);

        if (ret != 0)
            return -1;
    }
    return ret;
}

unsigned long  aml_w1_sdio_read_reg8(unsigned long sram_addr )
{
    unsigned char regdata[8] = {0};

    g_w1_hif_ops.hi_bottom_read(SDIO_FUNC1, sram_addr&SDIO_ADDR_MASK, regdata, 1, SDIO_OPMODE_INCREMENT);
    return regdata[0];
}

void   aml_w1_sdio_write_reg8(unsigned long sram_addr, unsigned long sramdata)
{
    unsigned char* sdio_kmm = (unsigned char*)kzalloc(sizeof(unsigned long), GFP_DMA |GFP_ATOMIC);

    ASSERT(sdio_kmm);
    memcpy(sdio_kmm, &sramdata, sizeof(unsigned long));
    g_w1_hif_ops.hi_bottom_write(SDIO_FUNC1, sram_addr&SDIO_ADDR_MASK,
                                                   (unsigned char *)sdio_kmm, sizeof(unsigned long), SDIO_OPMODE_INCREMENT);
    kfree(sdio_kmm);                                                   
}

unsigned long  aml_w1_sdio_read_reg32(unsigned long sram_addr)
{
    unsigned long sramdata;

    g_w1_hif_ops.hi_bottom_read(SDIO_FUNC1, sram_addr&SDIO_ADDR_MASK, &sramdata, 4, SDIO_OPMODE_INCREMENT);
    return sramdata;
}

struct amlw_hif_scatter_req *aml_w1_sdio_scatter_req_get(void)
{
    struct amlw1_hwif_sdio *hif_sdio = &g_w1_hwif_sdio;

    struct amlw_hif_scatter_req *scat_req = NULL;

    ASSERT(hif_sdio != NULL);

    scat_req = hif_sdio->scat_req;

    if (scat_req->free)
    {
        scat_req->free = false;
    }
    else if (scat_req->scat_count != 0) // get scat_req, but not build scatter list
    {
        scat_req = NULL;
    }

    return scat_req;
}

static int amlw_w1_sdio_alloc_prep_scat_req(struct amlw1_hwif_sdio *hif_sdio)
{
    struct amlw_hif_scatter_req * scat_req = NULL;

    ASSERT(hif_sdio != NULL);

    /* allocate the scatter request */
    scat_req = kzalloc(sizeof(struct amlw_hif_scatter_req), GFP_KERNEL);
    if (scat_req == NULL)
    {
        printk("[sdio sg alloc_scat_req]: no mem\n");
        return 1;
    }

    scat_req->free = true;
    hif_sdio->scat_req = scat_req;

    return 0;
}

int aml_w1_sdio_enable_scatter(void)
{
    struct amlw1_hwif_sdio *hif_sdio = &g_w1_hwif_sdio;
    int ret;

    ASSERT(hif_sdio != NULL);

    if (hif_sdio->scatter_enabled)
        return 0;

    // TODO : getting hw_config to configure scatter number;

    hif_sdio->scatter_enabled = true;

    ret = amlw_w1_sdio_alloc_prep_scat_req(&g_w1_hwif_sdio);

    return ret;
}

int aml_w1_sdio_scat_rw(struct scatterlist *sg_list, unsigned int sg_num, unsigned int blkcnt,
        unsigned char func_num, unsigned int addr, unsigned char write)
{
    struct mmc_request mmc_req;
    struct mmc_command mmc_cmd;
    struct mmc_data    mmc_dat;
    struct sdio_func *func = aml_priv_to_func(func_num);
    int ret = 0;

    memset(&mmc_req, 0, sizeof(struct mmc_request));
    memset(&mmc_cmd, 0, sizeof(struct mmc_command));
    memset(&mmc_dat, 0, sizeof(struct mmc_data));

    mmc_dat.sg     = sg_list;
    mmc_dat.sg_len = sg_num;
    mmc_dat.blksz  = FUNC4_BLKSIZE;
    mmc_dat.blocks = blkcnt;
    mmc_dat.flags  = write ? MMC_DATA_WRITE : MMC_DATA_READ;

    mmc_cmd.opcode = SD_IO_RW_EXTENDED;
    mmc_cmd.arg    = write ? 1 << 31 : 0;
    mmc_cmd.arg   |= (func_num & 0x7) << 28;
    mmc_cmd.arg   |= 1 << 27;	/* block basic */
    mmc_cmd.arg   |= 0 << 26;	/* 1 << 26;*/   	/*0 fix address */
    mmc_cmd.arg   |= (addr & 0x1ffff)<< 9;
    mmc_cmd.arg   |= blkcnt & 0x1ff;
    mmc_cmd.flags  = MMC_RSP_SPI_R5 | MMC_RSP_R5 | MMC_CMD_ADTC;

    mmc_req.cmd = &mmc_cmd;
    mmc_req.data = &mmc_dat;

    sdio_claim_host(func);
    mmc_set_data_timeout(&mmc_dat, func->card);
    mmc_wait_for_req(func->card->host, &mmc_req);
    sdio_release_host(func);

    if (mmc_cmd.error || mmc_dat.error) {
	    printk("ERROR CMD53 %s cmd_error = %d data_error=%d\n",
		write ? "write" : "read", mmc_cmd.error, mmc_dat.error);
	    ret  = mmc_cmd.error;
    }

    return ret;
}

void aml_w1_sdio_scat_complete (struct amlw_hif_scatter_req * scat_req)
{
    int  i;
    struct amlw1_hwif_sdio *hif_sdio = &g_w1_hwif_sdio;

    ASSERT(scat_req != NULL);
    ASSERT(hif_sdio != NULL);

    if (scat_req->complete)
    {
        for (i = 0; i < scat_req->scat_count; i++)
        {
            (scat_req->complete)(scat_req->scat_list[i].skbbuf);
            scat_req->scat_list[i].skbbuf = NULL;
        }
    }
    else
    {
        printk("error: no complete function\n");
    }

    scat_req->free = true;
    scat_req->scat_count = 0;
    scat_req->len = 0;
    scat_req->addr = 0;
    memset(scat_req->sgentries, 0, SDIO_MAX_SG_ENTRIES * sizeof(struct scatterlist));
}

void aml_w1_sdio_cleanup_scatter(void)
{
    struct amlw1_hwif_sdio *hif_sdio = &g_w1_hwif_sdio;
    printk("[sdio sg cleanup]: enter\n");

    ASSERT(hif_sdio != NULL);

    if (!hif_sdio->scatter_enabled)
        return;

    hif_sdio->scatter_enabled = false;

    /* empty the free list */
    kfree(hif_sdio->scat_req);

    printk("[sdio sg cleanup]: exit\n");

    return;
}

void aml_w1_sdio_recv_frame (unsigned char *buf, unsigned char *addr, SYS_TYPE len)
{
    g_w1_hif_ops.hi_bottom_read(SDIO_FUNC6, ((SYS_TYPE)addr & SDIO_ADDR_MASK),
        buf, len, SDIO_OPMODE_INCREMENT);
}

void aml_w1_sdio_init_ops(void)
{
    struct amlw1_hif_ops* ops = &g_w1_hif_ops;

    ops->hi_bottom_write8 = aml_w1_sdio_bottom_write8;
    ops->hi_bottom_read8 = aml_w1_sdio_bottom_read8;
    ops->hi_bottom_read = aml_w1_sdio_bottom_read;
    ops->hi_bottom_write = aml_w1_sdio_bottom_write;
    ops->hi_write8_func0 = aml_w1_sdio_bottom_write8_func0;
    ops->hi_read8_func0 = aml_w1_sdio_bottom_read8_func0;

    ops->hi_enable_scat = aml_w1_sdio_enable_scatter;
    ops->hi_cleanup_scat = aml_w1_sdio_cleanup_scatter;
    ops->hi_get_scatreq = aml_w1_sdio_scatter_req_get;
    ops->hi_rcv_frame = aml_w1_sdio_recv_frame;

    ops->hi_read_reg8 = aml_w1_sdio_read_reg8;
    ops->hi_write_reg8 = aml_w1_sdio_write_reg8;
    ops->hi_read_reg32 = aml_w1_sdio_read_reg32;
    ops->hi_write_reg32 = aml_w1_sdio_write_reg32;
    ops->hi_write_cmd = aml_w1_sdio_write_cmd32;
    ops->hi_write_sram = aml_w1_sdio_write_sram;
    ops->hi_read_sram = aml_w1_sdio_read_sram;

    ops->hi_write_word = aml_w1_sdio_write_word;
    ops->hi_read_word = aml_w1_sdio_read_word;

    ops->bt_hi_write_sram = aml_w1_bt_sdio_write_sram;
    ops->bt_hi_read_sram = aml_w1_bt_sdio_read_sram;
    ops->bt_hi_write_word = aml_w1_bt_hi_write_word;
    ops->bt_hi_read_word = aml_w1_bt_hi_read_word;
    ops->hif_suspend = aml_w1_sdio_suspend;

    //ops->hif_get_sts = hif_get_sts;
    //ops->hif_pt_rx_start = hif_pt_rx_start;
    //ops->hif_pt_rx_stop = hif_pt_rx_stop;
    w1_sdio_after_porbe = 1;

    // check and wake w1 firstly.
    host_wake_w1_req = NULL;
    host_suspend_req = NULL;
}

int aml_w1_sdio_probe(struct sdio_func *func, const struct sdio_device_id *id)
{
    int ret = 0;
    static struct sdio_func sdio_func_0;

    sdio_claim_host(func);
    ret = sdio_enable_func(func);
    if (ret)
        goto sdio_enable_error;

    sdio_set_block_size(func, 512);

    printk("%s(%d): func->num %d sdio block size=%d, \n", __func__, __LINE__,
        func->num,  func->cur_blksize);

    if (func->num == 1)
    {
        sdio_func_0.num = 0;
        sdio_func_0.card = func->card;
        g_w1_hwif_sdio.sdio_func_if[0] = &sdio_func_0;
    }
    g_w1_hwif_sdio.sdio_func_if[func->num] = func;
    printk("%s(%d): func->num %d sdio_func=%p, \n", __func__, __LINE__,
        func->num,  func);

    sdio_release_host(func);
    sdio_set_drvdata(func, (void *)(&g_w1_hwif_sdio));
    if (func->num != FUNCNUM_SDIO_LAST)
    {
        printk("%s(%d):func_num=%d, last func num=%d\n", __func__, __LINE__,
            func->num, FUNCNUM_SDIO_LAST);
        return 0;
    }

    aml_w1_sdio_init_ops();

    return ret;

sdio_enable_error:
    printk("sdio_enable_error:  line %d\n",__LINE__);
    sdio_release_host(func);

    return ret;
}

static void  aml_w1_sdio_remove(struct sdio_func *func)
{
    if (func== NULL)
    {
        return ;
    }

    printk("\n==========================================\n");
    printk("aml_sdio_remove++ func->num =%d \n",func->num);
    printk("==========================================\n");

    sdio_claim_host(func);
    sdio_disable_func(func);
    sdio_release_host(func);

    host_wake_w1_req = NULL;
    host_suspend_req = NULL;
    host_resume_req = NULL;
}

static int aml_sdio_pm_suspend(struct device *device)
{
    if (host_suspend_req != NULL)
        return host_suspend_req(device);
    else
        return aml_w1_sdio_suspend(1);
}

static int aml_sdio_pm_resume(struct device *device)
{
    if (host_resume_req != NULL)
        return host_resume_req(device);
    else
        return 0;
}

static SIMPLE_DEV_PM_OPS(aml_sdio_pm_ops, aml_sdio_pm_suspend,
                     aml_sdio_pm_resume);


static const struct sdio_device_id aml_w1_sdio[] =
{
    {SDIO_DEVICE(W1_VENDOR_AMLOGIC,W1_PRODUCT_AMLOGIC) },
    {SDIO_DEVICE(W1_VENDOR_AMLOGIC_EFUSE,W1_PRODUCT_AMLOGIC_EFUSE)},
    {}
};

static struct sdio_driver aml_w1_sdio_driver =
{
    .name = "aml_w1_sdio",
    .id_table = aml_w1_sdio,
    .probe = aml_w1_sdio_probe,
    .remove = aml_w1_sdio_remove,
    .drv.pm = &aml_sdio_pm_ops,
};

int  aml_w1_sdio_init(void)
{
	int err = 0;

	//amlwifi_set_sdio_host_clk(200000000);//200MHZ

	err = sdio_register_driver(&aml_w1_sdio_driver);
	w1_sdio_driver_insmoded = 1;
	PRINT("*****************aml sdio common driver is insmoded********************\n");
	if (err)
        	PRINT("failed to register sdio driver: %d \n", err);

    	return err;
}
EXPORT_SYMBOL(aml_w1_sdio_init);

void  aml_w1_sdio_exit(void)
{
    PRINT("aml_w1_sdio_exit++ \n");
    sdio_unregister_driver(&aml_w1_sdio_driver);
    w1_sdio_driver_insmoded = 0;
    w1_sdio_after_porbe = 0;
    PRINT("*****************aml sdio common driver is rmmoded********************\n");
}
EXPORT_SYMBOL(w1_sdio_driver_insmoded);
EXPORT_SYMBOL(w1_sdio_after_porbe);
EXPORT_SYMBOL(host_wake_w1_req);
EXPORT_SYMBOL(host_suspend_req);
EXPORT_SYMBOL(host_resume_req);

/*set_wifi_bt_sdio_driver_bit() is used to determine whether to unregister sdio power driver.
  *Only when w1_sdio_wifi_bt_alive is 0, then call aml_w1_sdio_exit().
*/
void set_wifi_bt_sdio_driver_bit(bool is_register, int shift)
{
	mutex_lock(&wifi_bt_sdio_mutex);
	if (is_register) {
		w1_sdio_wifi_bt_alive |= (1 << shift);
		PRINT("Insmod %s sdio driver!\n", (shift ? "WiFi":"BT"));
	} else {
		PRINT("Rmmod %s sdio driver!\n", (shift ? "WiFi":"BT"));
		w1_sdio_wifi_bt_alive &= ~(1 << shift);
		if (!w1_sdio_wifi_bt_alive) {
			aml_w1_sdio_exit();
		}
	}
	mutex_unlock(&wifi_bt_sdio_mutex);
}
EXPORT_SYMBOL(set_wifi_bt_sdio_driver_bit);
EXPORT_SYMBOL(g_w1_hwif_sdio);
EXPORT_SYMBOL(g_w1_hif_ops);

static int aml_w1_sdio_insmod(void)
{
	aml_w1_sdio_init();
	printk("%s(%d) start...\n",__func__, __LINE__);
	return 0;
}

static void aml_w1_sdio_rmmod(void)
{
	aml_w1_sdio_exit();
}

module_init(aml_w1_sdio_insmod);
module_exit(aml_w1_sdio_rmmod);
MODULE_LICENSE("GPL");
