#include "w1_sdio.h"
#include <linux/mutex.h>

#include "chip_pmu_reg.h"
#include "chip_intf_reg.h"
#include "wifi_intf_addr.h"
#include "wifi_drv_reg_ops.h"
#include "rf_d_top_reg.h"
#include "wifi_coex_addr.h"

struct amlw1_hwif_sdio g_w1_hwif_sdio;
struct amlw1_hif_ops g_w1_hif_ops;

unsigned char w1_sdio_wifi_bt_alive;
unsigned char w1_sdio_driver_insmoded;
unsigned char w1_sdio_after_porbe;
unsigned char wifi_in_insmod;
unsigned char  wifi_sdio_access = 1;
unsigned int  shutdown_i = 0;
#define  I2C_CLK_QTR   0x4

static DEFINE_MUTEX(wifi_bt_sdio_mutex);

#define AML_W1_BT_WIFI_MUTEX_ON() do {\
    mutex_lock(&wifi_bt_sdio_mutex);\
} while (0)

#define AML_W1_BT_WIFI_MUTEX_OFF() do {\
    mutex_unlock(&wifi_bt_sdio_mutex);\
} while (0)

/* protect cmd53 and host sleep request */
static DEFINE_MUTEX(wifi_sdio_power_mutex);

void aml_wifi_sdio_power_lock(void)
{
    mutex_lock(&wifi_sdio_power_mutex);
}

void aml_wifi_sdio_power_unlock(void)
{
    mutex_unlock(&wifi_sdio_power_mutex);
}

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

    if (!(reg_tmp & BIT(23))) {
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
    /*
     * make sure function 5 section address-mapping feature is disabled,
     * when this feature is disabled, 
     * all 128k space in one sdio-function use only 
     * one address-mapping: 32-bit AHB Address = BaseAddr + cmdRegAddr 
     */
    reg_tmp = g_w1_hif_ops.hi_read_word( RG_SDIO_IF_MISC_CTRL);
    
    if (!(reg_tmp & BIT(23))) {
        reg_tmp |= BIT(23);
        g_w1_hif_ops.hi_write_word( RG_SDIO_IF_MISC_CTRL, reg_tmp);
    }
    /*config msb 15 bit address in BaseAddr Register*/
    g_w1_hif_ops.hi_write_reg32(RG_SCFG_FUNC5_BADDR_A,addr & 0xfffe0000);

    g_w1_hif_ops.bt_hi_write_sram((unsigned char *)&data,
        /*sdio cmd 52/53 can only take 17 bit address*/
        (unsigned char*)(SYS_TYPE)(addr & 0x1ffff), sizeof(unsigned int));
}

void aml_w1_bt_sdio_read_sram (unsigned char *buf, unsigned char *addr, SYS_TYPE len)
{
    g_w1_hif_ops.hi_bottom_read(SDIO_FUNC5, ((SYS_TYPE)addr & SDIO_ADDR_MASK),
        buf, len, (len > 8 ? SDIO_OPMODE_INCREMENT : SDIO_OPMODE_FIXED));
}

/*For BT use only start */
void aml_w1_bt_sdio_write_sram (unsigned char *buf, unsigned char *addr, SYS_TYPE len)
{
    g_w1_hif_ops.hi_bottom_write(SDIO_FUNC5, ((SYS_TYPE)addr & SDIO_ADDR_MASK),
        buf, len, (len > 8 ? SDIO_OPMODE_INCREMENT : SDIO_OPMODE_FIXED));
}

int aml_w1_sdio_write_reg32(unsigned long sram_addr, unsigned long sramdata)
{
    return g_w1_hif_ops.hi_bottom_write(SDIO_FUNC1, sram_addr&SDIO_ADDR_MASK,
        (unsigned char *)&sramdata,  sizeof(unsigned long), SDIO_OPMODE_INCREMENT);
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

static int _aml_w1_sdio_request_byte(unsigned char func_num,
    unsigned char write, unsigned int reg_addr, unsigned char *byte)
{
    int err_ret;
    struct sdio_func * func = aml_priv_to_func(func_num);
    unsigned char *kmalloc_buf = NULL;
    unsigned char len = sizeof(unsigned char);

#if defined(DBG_PRINT_COST_TIME)
    struct timespec now, before;
    getnstimeofday(&before);
#endif /* End of DBG_PRINT_COST_TIME */

    ASSERT(func != NULL);
    ASSERT(byte != NULL);
    ASSERT(func->num == func_num);

    AML_W1_BT_WIFI_MUTEX_ON();
    kmalloc_buf =  (unsigned char *)kzalloc(len, GFP_DMA);//virt_to_phys(fwICCM);
    if (kmalloc_buf == NULL)
    {
        ERROR_DEBUG_OUT("kmalloc buf fail\n");
        AML_W1_BT_WIFI_MUTEX_OFF();
        return SDIOH_API_RC_FAIL;
    }
    memcpy(kmalloc_buf, byte, len);

    /* Claim host controller */
    sdio_claim_host(func);

    if (write) {
        /* CMD52 Write */
        sdio_writeb(func, *kmalloc_buf, reg_addr, &err_ret);
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

    kfree(kmalloc_buf);
    AML_W1_BT_WIFI_MUTEX_OFF();
    return (err_ret == 0) ? SDIOH_API_RC_SUCCESS : SDIOH_API_RC_FAIL;
}

static int _aml_w1_sdio_request_buffer(unsigned char func_num,
    unsigned int fix_incr, unsigned char write, unsigned int addr, void *buf, unsigned int nbytes)
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

unsigned char aml_w1_sdio_bottom_read8(unsigned char  func_num, int addr);

//cmd53
int aml_w1_sdio_bottom_read(unsigned char func_num, int addr, void *buf, size_t len, int incr_addr)
{
    void *kmalloc_buf = NULL;
    int result;
    int align_len = 0;

    ASSERT(func_num != SDIO_FUNC0);

    if (!wifi_sdio_access) {
        if (func_num == SDIO_FUNC5) {
            /*SDIO_FUNC5 ignor*/
            ERROR_DEBUG_OUT("SDIO_FUNC5, func num %d, addr 0x%08x\n", func_num, addr);

        } else if (func_num == SDIO_FUNC1) {
            /*SDIO_FUNC1 ignor*/
            ERROR_DEBUG_OUT("SDIO_FUNC1, func num %d, addr 0x%08x\n", func_num, addr);

        }  else if ((func_num == SDIO_FUNC2) && (addr == 0x00005080)) {
            /*SDIO_FUNC2 && 0x00005080 ignor*/
            ERROR_DEBUG_OUT("SDIO_FUNC1, func num %d, addr 0x%08x\n", func_num, addr);

        } else {
            ERROR_DEBUG_OUT("fw recovery downloading, func num %d, addr 0x%08x\n", func_num, addr);
            return -1;
        }
    }


    aml_wifi_sdio_power_lock();

    if (host_wake_w1_req != NULL)
    {
        if (host_wake_w1_req() == 0)
        {
            aml_wifi_sdio_power_unlock();
            ERROR_DEBUG_OUT("aml_w1_sdio_bottom_read, host wake w1 fail\n");
            return -1;
        }
    }
    {
        unsigned char fw_st = aml_w1_sdio_bottom_read8(SDIO_FUNC1, 0x23c) & 0xF;
        if (fw_st != 6)
            printk("%s:%d, BUG! fw_st %x, func_num %x, addr %x \n", __func__, __LINE__, fw_st, func_num, addr);
    }
    AML_W1_BT_WIFI_MUTEX_ON();
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

    if (kmalloc_buf == NULL)
    {
        ERROR_DEBUG_OUT("kmalloc buf fail\n");
        AML_W1_BT_WIFI_MUTEX_OFF();
        aml_wifi_sdio_power_unlock();
        return SDIOH_API_RC_FAIL;
    }

    result = _aml_w1_sdio_request_buffer(func_num, incr_addr, SDIO_READ, addr, kmalloc_buf, len);

    if (func_num != SDIO_FUNC6)
    {
        memcpy(buf, kmalloc_buf, len);
        kfree(kmalloc_buf);
    }

    AML_W1_BT_WIFI_MUTEX_OFF();
    aml_wifi_sdio_power_unlock();
    return result;
}


//cmd53
int aml_w1_sdio_bottom_write(unsigned char func_num, int addr, void *buf, size_t len, int incr_addr)
{
    void *kmalloc_buf;
    int result;

    if (!wifi_sdio_access) {
        if (func_num == SDIO_FUNC5) {
            /*SDIO_FUNC5 ignor*/
            ERROR_DEBUG_OUT("SDIO_FUNC5, func num %d, addr 0x%08x\n", func_num, addr);

        } else if (func_num == SDIO_FUNC1) {
            /*SDIO_FUNC1 ignor*/
            ERROR_DEBUG_OUT("SDIO_FUNC1, func num %d, addr 0x%08x\n", func_num, addr);

        }  else if ((func_num == SDIO_FUNC2) && (addr == 0x00005080)) {
            /*SDIO_FUNC2 && 0x00005080 ignor*/
            ERROR_DEBUG_OUT("SDIO_FUNC2, func num %d, addr 0x%08x\n", func_num, addr);

        } else {
            ERROR_DEBUG_OUT("fw recovery downloading, func num %d, addr 0x%08x\n", func_num, addr);
            return -1;
        }
    }

    aml_wifi_sdio_power_lock();
    ASSERT(func_num != SDIO_FUNC0);

    if (host_wake_w1_req != NULL)
    {
        if (host_wake_w1_req() == 0)
        {
            aml_wifi_sdio_power_unlock();
            ERROR_DEBUG_OUT("aml_w1_sdio_bottom_write, host wake w1 fail\n");
            return -1;
        }
    }

    {
        unsigned char fw_st = aml_w1_sdio_bottom_read8(SDIO_FUNC1, 0x23c) & 0xF;
        if (fw_st != 6)
            printk("%s:%d, BUG! fw_st %x, func_num %x, addr %x \n", __func__, __LINE__, fw_st, func_num, addr);
    }

    AML_W1_BT_WIFI_MUTEX_ON();
    kmalloc_buf =  (unsigned char *)kzalloc(len, GFP_DMA);//virt_to_phys(fwICCM);
    if(kmalloc_buf == NULL)
    {
        ERROR_DEBUG_OUT("kmalloc buf fail\n");
        AML_W1_BT_WIFI_MUTEX_OFF();
        aml_wifi_sdio_power_unlock();
        return SDIOH_API_RC_FAIL;
    }
    memcpy(kmalloc_buf, buf, len);

    result = _aml_w1_sdio_request_buffer(func_num, incr_addr, SDIO_WRITE, addr, kmalloc_buf, len);

    kfree(kmalloc_buf);
    AML_W1_BT_WIFI_MUTEX_OFF();
    aml_wifi_sdio_power_unlock();
    return result;
}

void aml_w1_sdio_read_sram (unsigned char *buf, unsigned char *addr, SYS_TYPE len)
{
    g_w1_hif_ops.hi_bottom_read(SDIO_FUNC2, (SYS_TYPE)addr&SDIO_ADDR_MASK,
        buf, len, (len > 8 ? SDIO_OPMODE_INCREMENT : SDIO_OPMODE_FIXED));
}

void aml_w1_sdio_write_sram (unsigned char *buf, unsigned char *addr, SYS_TYPE len)
{
    g_w1_hif_ops.hi_bottom_write(SDIO_FUNC2, (SYS_TYPE)addr&SDIO_ADDR_MASK,
        buf, len, (len > 8 ? SDIO_OPMODE_INCREMENT : SDIO_OPMODE_FIXED));
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
            (unsigned char*)(SYS_TYPE)(addr), sizeof(unsigned int));
    }
    return regdata;
}

void aml_w1_sdio_write_word(unsigned int addr, unsigned int data)
{
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
        aml_w1_sdio_write_sram((unsigned char *)&data,
            (unsigned char*)(SYS_TYPE)(addr), sizeof(unsigned int));
    }
}

//cmd52
int aml_w1_sdio_bottom_write8(unsigned char  func_num, int addr, unsigned char data)
{
    int ret = 0;

    ASSERT(func_num != SDIO_FUNC0);
    ret =  _aml_w1_sdio_request_byte(func_num, SDIO_WRITE, addr, &data);

    return ret;
}

//cmd52
unsigned char aml_w1_sdio_bottom_read8(unsigned char  func_num, int addr)
{
    unsigned char sramdata;
    
    _aml_w1_sdio_request_byte(func_num, SDIO_READ, addr, &sramdata);
    return sramdata;
}

/* cmd52
   buff[7:0],     // funcIoNum
   buff[25:8],    // regAddr
   buff[39:32]);  // regValue
*/
void aml_w1_sdio_bottom_write8_func0(unsigned long sram_addr, unsigned char sramdata)
{
    _aml_w1_sdio_request_byte(SDIO_FUNC0, SDIO_WRITE, sram_addr, &sramdata);
}

//cmd52
unsigned char aml_w1_sdio_bottom_read8_func0(unsigned long sram_addr)
{
    unsigned char sramdata;

    _aml_w1_sdio_request_byte(SDIO_FUNC0, SDIO_READ, sram_addr, &sramdata);
    return sramdata;
}

void aml_w1_sdio_write_cmd32(unsigned long sram_addr, unsigned long sramdata)
{
#if defined (HAL_SIM_VER)
    aml_sdio_read_write(sram_addr&SDIO_ADDR_MASK,	(unsigned char *)&sramdata, 4,
                        SDIO_FUNC3,SDIO_RW_FLAG_WRITE,SDIO_F_SYNCHRONOUS);
#elif defined (HAL_FPGA_VER)
    g_w1_hif_ops.hi_bottom_write(SDIO_FUNC3, sram_addr&SDIO_ADDR_MASK,
        (unsigned char *)&sramdata, sizeof(unsigned long), SDIO_OPMODE_INCREMENT);
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

    AML_W1_BT_WIFI_MUTEX_ON();
    /* we shall suspend all card for sdio. */
    for (i = SDIO_FUNC1; i <= FUNCNUM_SDIO_LAST; i++)
    {
        func = aml_priv_to_func(i);
        if (func == NULL)
            continue;
        flags = sdio_get_host_pm_caps(func);

        if ((flags & MMC_PM_KEEP_POWER) != 0)
            ret = sdio_set_host_pm_flags(func, MMC_PM_KEEP_POWER);

        if (ret != 0) {
            AML_W1_BT_WIFI_MUTEX_OFF();
            return -1;
        }

        /*
         * if we don't use sdio irq, we can't get functions' capability with
         * MMC_PM_WAKE_SDIO_IRQ, so we don't need set irq for wake up
         * sdio for upcoming suspend.
         */
        if ((flags & MMC_PM_WAKE_SDIO_IRQ) != 0)
            ret = sdio_set_host_pm_flags(func, MMC_PM_WAKE_SDIO_IRQ);

        if (ret != 0) {
            AML_W1_BT_WIFI_MUTEX_OFF();
            return -1;
        }
    }

    AML_W1_BT_WIFI_MUTEX_OFF();
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
    g_w1_hif_ops.hi_bottom_write(SDIO_FUNC1, sram_addr&SDIO_ADDR_MASK,
        (unsigned char *)&sramdata, sizeof(unsigned long), SDIO_OPMODE_INCREMENT);
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
        ERROR_DEBUG_OUT("[sdio sg alloc_scat_req]: no mem\n");
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

    AML_W1_BT_WIFI_MUTEX_ON();
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

    AML_W1_BT_WIFI_MUTEX_OFF();
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
        ERROR_DEBUG_OUT("error: no complete function\n");
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


void write_byte_8ba(unsigned char Bus, unsigned char SlaveAddr,
    unsigned char RegAddr, unsigned char Data)
{
    unsigned int tmp,cnt = 0;

    // Set the I2C bus to 100khz
    tmp = aml_w1_sdio_read_word(I2C_CONTROL_REG);
    tmp = (tmp & (~(0x3FF << I2C_CLOCK_OFFSET))) | (I2C_CLK_QTR << I2C_CLOCK_OFFSET);
    aml_w1_sdio_write_word(I2C_CONTROL_REG, tmp);

    // Set the I2C Address
    aml_w1_sdio_write_word(I2C_SLAVE_ADDR, SlaveAddr);
    // Fill the token registers
    tmp = aml_w1_sdio_read_word(I2C_TOKEN_LIST_REG0);
    tmp = (I2C_END  << 16)             |
            (I2C_DATA << 12)             |    // Write Data
            (I2C_DATA << 8)              |    // Write RegAddr
            (I2C_SLAVE_ADDR_WRITE << 4)  |
            (I2C_START << 0);
    aml_w1_sdio_write_word(I2C_TOKEN_LIST_REG0, tmp);

    // Fill the write data registers
    aml_w1_sdio_write_word(I2C_TOKEN_WDATA_REG0,(Data << 8) | (RegAddr << 0));
    // Start and Wait
    tmp = aml_w1_sdio_read_word(I2C_CONTROL_REG);
    tmp &= (~(1 << 0));
    aml_w1_sdio_write_word(I2C_CONTROL_REG, tmp);
    tmp |= ( (1 << 0));
    aml_w1_sdio_write_word(I2C_CONTROL_REG, tmp);

    do {
        tmp  = aml_w1_sdio_read_word(I2C_CONTROL_REG);

        cnt++;
        if (cnt == 1000) {
            printk("-------[ERR]-----> i2c[W] err\n");
            break;
        }
    } while (tmp & (1 << 2));

}

unsigned char read_byte_8ba(unsigned char Bus, unsigned char SlaveAddr, unsigned char RegAddr)
{
    //struct hw_interface* hif = hif_get_hw_interface();
    unsigned int tmp,cnt = 0;

    // Set the I2C bus to 100khz
    tmp = aml_w1_sdio_read_word(I2C_CONTROL_REG);
    tmp = (tmp & (~(0x3FF << I2C_CLOCK_OFFSET))) | (I2C_CLK_QTR << I2C_CLOCK_OFFSET);
    aml_w1_sdio_write_word(I2C_CONTROL_REG, tmp);

    // Set the I2C Address
    aml_w1_sdio_write_word(I2C_SLAVE_ADDR, SlaveAddr);
    // Fill the token registers
    tmp = aml_w1_sdio_read_word(I2C_TOKEN_LIST_REG0);
    tmp =   (I2C_END  << 24)             |
            (I2C_DATA_LAST << 20)        |  // this is a data read
            (I2C_SLAVE_ADDR_READ << 16)  |
            (I2C_START << 12)            |
            (I2C_DATA << 8)              |  // This is a data write
            (I2C_SLAVE_ADDR_WRITE << 4)  |
            (I2C_START << 0);
    aml_w1_sdio_write_word(I2C_TOKEN_LIST_REG0, tmp);

    // Fill the write data registers
    aml_w1_sdio_write_word(I2C_TOKEN_WDATA_REG0,(RegAddr << 0));
    // Start and Wait
    tmp = aml_w1_sdio_read_word(I2C_CONTROL_REG);
    tmp &= (~(1 << 0));
    aml_w1_sdio_write_word(I2C_CONTROL_REG, tmp);
    tmp |= ( (1 << 0));
    aml_w1_sdio_write_word(I2C_CONTROL_REG, tmp);

    do {
        tmp  = aml_w1_sdio_read_word(I2C_CONTROL_REG);

        cnt++;
        if (cnt == 1000) {
            printk("-------[ERR]-----> i2c[W] err\n");
            break;
        }
    } while (tmp & (1 << 2));

    tmp  = aml_w1_sdio_read_word(I2C_TOKEN_RDATA_REG0) & 0xff;
    return (unsigned char)tmp;
}


//void write_rdaddr_32ba(U8 Bus, U8 SlaveAddr, U32 TkData0, U32 TkData1)
void write_word_32ba(unsigned char Bus, unsigned char SlaveAddr,
    unsigned int StartToken, unsigned int Data)
{
    //struct hw_interface* hif = hif_get_hw_interface();
    unsigned int tmp,cnt = 0;

    //printk("%s(%d) token 0x%x data 0x%x\n", __func__, __LINE__, StartToken,Data);

    // Set the I2C bus to 100khz
    tmp = aml_w1_sdio_read_word(I2C_CONTROL_REG);
    tmp = (tmp & (~(0x3FF << I2C_CLOCK_OFFSET))) | (I2C_CLK_QTR << I2C_CLOCK_OFFSET);
    aml_w1_sdio_write_word(I2C_CONTROL_REG, tmp);

    // Set the I2C Address
    aml_w1_sdio_write_word(I2C_SLAVE_ADDR, SlaveAddr);
    // Fill the token registers
    tmp = aml_w1_sdio_read_word(I2C_TOKEN_LIST_REG0);
    tmp =   (I2C_END << 28)              |    //
            (I2C_DATA << 24)             |    //
            (I2C_DATA << 20)             |    //
            (I2C_DATA << 16)             |    //
            (I2C_DATA << 12)             |    //
            (I2C_DATA << 8)              |    //  This shall be related to start token (token = 0x04 for receiving paddr,  token = 0x00 for receiving pwdata)
            (I2C_SLAVE_ADDR_WRITE << 4)  |
            (I2C_START << 0);
    aml_w1_sdio_write_word(I2C_TOKEN_LIST_REG0, tmp);

    //Fill the write data registers
    aml_w1_sdio_write_word(I2C_TOKEN_WDATA_REG0,StartToken | (Data<<8));
    aml_w1_sdio_write_word(I2C_TOKEN_WDATA_REG1,(Data >> 24));
    //Start and Wait
    tmp = aml_w1_sdio_read_word(I2C_CONTROL_REG);
    tmp &= (~(1 << 0));
    aml_w1_sdio_write_word(I2C_CONTROL_REG, tmp);
    tmp |= ( (1 << 0));
    aml_w1_sdio_write_word(I2C_CONTROL_REG, tmp);

    tmp = 0;
    do {
        tmp  = aml_w1_sdio_read_word(I2C_CONTROL_REG);

        cnt++;
        if (cnt == 100000) {
            ERROR_DEBUG_OUT("-------[ERR]-----> i2c[W] err\n");
            break;
        }
    } while (tmp & (1 << 2));
}

unsigned int read_word_32ba(unsigned int SlaveAddr, unsigned int RegAddr)
{
    //struct hw_interface* hif = hif_get_hw_interface();
    unsigned int tmp,cnt = 0;

    // Set the I2C bus to 100khz
    tmp = aml_w1_sdio_read_word(I2C_CONTROL_REG);
    tmp = (tmp & (~(0x3FF << I2C_CLOCK_OFFSET))) | (I2C_CLK_QTR << I2C_CLOCK_OFFSET);
    aml_w1_sdio_write_word(I2C_CONTROL_REG, tmp);

    // Set the I2C Address
    aml_w1_sdio_write_word(I2C_SLAVE_ADDR, SlaveAddr);
    // Fill the token registers
    tmp = aml_w1_sdio_read_word(I2C_TOKEN_LIST_REG0);
    tmp =    (I2C_DATA  << 28)            |
             (I2C_DATA  << 24)            |
             (I2C_DATA  << 20)            |
             (I2C_SLAVE_ADDR_READ  << 16) |  // the second burst is for READ
             (I2C_START << 12)            |  // after addr pointer has been reset to 0x00, we start a new burst for reading data out
             (I2C_DATA  << 8)             |  // This shall be related a 0x00 written into rf i2c in order to reset addr pointer to 0x00
             (I2C_SLAVE_ADDR_WRITE << 4)  |  // the first burst is for WRITE token 0x00 to reset addr pointer
             (I2C_START << 0);               // start the first burst
    aml_w1_sdio_write_word(I2C_TOKEN_LIST_REG0, tmp);

    tmp = aml_w1_sdio_read_word(I2C_TOKEN_LIST_REG1);
    tmp = (I2C_END       << 4) | (I2C_DATA_LAST << 0);
    aml_w1_sdio_write_word(I2C_TOKEN_LIST_REG1, tmp);

    // Fill the write data registers
    aml_w1_sdio_write_word(I2C_TOKEN_WDATA_REG0,RegAddr << 0);
    aml_w1_sdio_write_word(I2C_TOKEN_WDATA_REG1,0);
    // Start and Wait
    tmp = aml_w1_sdio_read_word(I2C_CONTROL_REG);
    tmp &= (~(1 << 0));
    aml_w1_sdio_write_word(I2C_CONTROL_REG, tmp);
    tmp |= ( (1 << 0));
    aml_w1_sdio_write_word(I2C_CONTROL_REG, tmp);

    tmp = 0;
    do {
        tmp  = aml_w1_sdio_read_word(I2C_CONTROL_REG);

        cnt++;
        if (cnt == 100000) {
            ERROR_DEBUG_OUT("-------[ERR]-----> i2c[R] err\n");
            break;
        }
    } while( tmp & (1 << 2));

    tmp = aml_w1_sdio_read_word(I2C_TOKEN_RDATA_REG0);
    return tmp;
}

unsigned int rf_i2c_read(unsigned int reg_addr)
{
    unsigned char bus = 0;
    unsigned int slave_addr = 0x7a;
    unsigned int read_data = 0;
    unsigned int start_token;

    start_token = 0x04;

    /* 0x4-0x7 save address of read register in slave device*/
    write_word_32ba(bus, slave_addr, start_token, reg_addr);

    /* 0x0-0x3 save address of read data in slave device*/
    read_data = read_word_32ba(slave_addr, 0x0);
    return read_data;
}

void rf_i2c_write(unsigned int reg_addr, unsigned int data)
{
    unsigned char bus = 0;
    unsigned int slave_addr = 0x7a;
    unsigned int start_token;

    /* 0x0-0x3 save address of write data in slave device*/
    start_token = 0x00;  //for data to be written in
    write_word_32ba(bus, slave_addr, 0x00, data);

    //system_wait(1);

    /* 0x4-0x7 save address of write register in slave device*/
    start_token = 0x04;  //for address to be written to
    write_word_32ba(bus, slave_addr, start_token, reg_addr);

    write_byte_8ba(bus, slave_addr, 0x8, bus);
}

void config_pmu_reg_off(void)
{
    int value_pmu_A12 = 0;
    int value_pmu_A15 = 0;
    int value_pmu_A17 = 0;
    int value_pmu_A18 = 0;
    int value_pmu_A20 = 0;
    int value_pmu_A22 = 0;
    int value_pmu_A24 = 0;
    int value_aon30   = 0;

    RG_AON_A30_FIELD_T reg_aon30_data;
    RG_AON_A29_FIELD_T reg_aon29_data;

    unsigned char host_req_status= 0;
    unsigned int reg_val = 0;

    //reg_val = rf_read_register(RG_TOP_A2);
    reg_val = rf_i2c_read(RG_TOP_A2);
    reg_val = reg_val &(~0x1f);
    /*RF enter sx mode*/
    // reg_val = reg_val |(0x15 );

    /*RF enter sleep mode*/
    //reg_val = reg_val |(0x10 );

    /*2.4G sx mode*/
    reg_val |= 0x15;
    rf_i2c_write(RG_TOP_A2, reg_val);

    /*infor BT ,wifi not work*/
    reg_val  = aml_w1_sdio_read_word(RG_PMU_A16);
    reg_val &= (~BIT(31));
    aml_w1_sdio_write_word(RG_PMU_A16, reg_val);

    /*close all WIFI coexist thread*/
    reg_val  = aml_w1_sdio_read_word(RG_COEX_WF_OWNER_CTRL);
    reg_val &= (~BIT(28) );
    aml_w1_sdio_write_word(RG_COEX_WF_OWNER_CTRL, reg_val);

    {
        value_pmu_A12 = aml_w1_sdio_read_word(RG_PMU_A12);
        value_pmu_A15 = aml_w1_sdio_read_word(RG_PMU_A15);
        value_pmu_A17 = aml_w1_sdio_read_word(RG_PMU_A17);
        value_pmu_A18 = aml_w1_sdio_read_word(RG_PMU_A18);
        value_pmu_A20 = aml_w1_sdio_read_word(RG_PMU_A20);
        value_pmu_A22 = aml_w1_sdio_read_word(RG_PMU_A22);
        value_pmu_A24 = aml_w1_sdio_read_word(RG_PMU_A24);
        value_aon30   = aml_w1_sdio_read_word(RG_AON_A30);
        printk("%s power off: before write A12=0x%x, A15=0x%x, A17=0x%x, A18=0x%x, A20=0x%x, A22=0x%x, A24=0x%x, AON30=0x%x\n",
            __func__, value_pmu_A12,value_pmu_A15,value_pmu_A17,value_pmu_A18,value_pmu_A20,value_pmu_A22,value_pmu_A24, value_aon30);

        aml_w1_sdio_write_word(RG_INTF_CPU_CLK, 0x4f070001);

        reg_aon29_data.data = aml_w1_sdio_read_word(RG_AON_A29);
        reg_aon29_data.b.rg_ana_bpll_cfg |= BIT(1) | BIT(0);
        aml_w1_sdio_write_word(RG_AON_A29, reg_aon29_data.data);

        aml_w1_sdio_write_word(RG_PMU_A12, 0x8ea2e);
        aml_w1_sdio_write_word(RG_PMU_A14, 0x1);
        aml_w1_sdio_write_word(RG_PMU_A16, 0x0);
        aml_w1_sdio_write_word(RG_PMU_A22, 0x707);
        aml_w1_sdio_write_word(RG_PMU_A18, 0x8700);
        aml_w1_sdio_write_word(RG_PMU_A20, 0x3ff01ff);
        aml_w1_sdio_write_word(RG_PMU_A17, 0x703);
        //aml_w1_sdio_write_word(RG_PMU_A24, 0x3f20000);

        reg_aon30_data.data = aml_w1_sdio_read_word(RG_AON_A30);
        /* change rf to idle mode */
        reg_aon30_data.b.rg_always_on_cfg4 |= BIT(12);
        aml_w1_sdio_write_word(RG_AON_A30, reg_aon30_data.data);

        value_pmu_A12 = aml_w1_sdio_read_word(RG_PMU_A12);
        value_pmu_A15 = aml_w1_sdio_read_word(RG_PMU_A15);
        value_pmu_A17 = aml_w1_sdio_read_word(RG_PMU_A17);
        value_pmu_A18 = aml_w1_sdio_read_word(RG_PMU_A18);
        value_pmu_A20 = aml_w1_sdio_read_word(RG_PMU_A20);
        value_pmu_A22 = aml_w1_sdio_read_word(RG_PMU_A22);
        value_pmu_A24 = aml_w1_sdio_read_word(RG_PMU_A24);
        value_aon30   = aml_w1_sdio_read_word(RG_AON_A30);
        printk("%s power off: after write A12=0x%x, A15=0x%x, A17=0x%x, A18=0x%x, A20=0x%x, A22=0x%x, A24=0x%x, AON30=0x%x\n",
            __func__, value_pmu_A12,value_pmu_A15,value_pmu_A17,value_pmu_A18,value_pmu_A20,value_pmu_A22,value_pmu_A24, value_aon30);

        //force wifi pmu fsm to sleep mode
        host_req_status = (0x8 << 1)| BIT(0);
        aml_w1_sdio_bottom_write8(SDIO_FUNC1, 0x221, host_req_status);
    }
}

static void aml_sdio_shutdown(struct device *device)
{
    printk("===>>> enter %s <<<===\n", __func__);
    shutdown_i += 1;
    if (shutdown_i == 1) {
        config_pmu_reg_off();
    } else if (shutdown_i == 7) {
        shutdown_i = 0;
        printk("===>>> end <<<===\n");
    } else {
        ;
    }
    printk("=== shutdown_i:%d ===\n", shutdown_i);
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
    .drv.shutdown = aml_sdio_shutdown,
};

#ifdef NOT_AMLOGIC_PLATFORM

#define	AML_STATIC_VERSION_STR      "101.10.361.10 (wlan=r892223-20210623-1)"

#define DHD_SKB_1PAGE_BUFSIZE       (PAGE_SIZE * 1)
#define DHD_SKB_2PAGE_BUFSIZE       (PAGE_SIZE * 2)
#define DHD_SKB_4PAGE_BUFSIZE       (PAGE_SIZE * 4)

#define DHD_SKB_1PAGE_BUF_NUM   8
#define DHD_SKB_2PAGE_BUF_NUM   8
#define DHD_SKB_4PAGE_BUF_NUM   1

#define FW_VERBOSE_RING_SIZE            (256 * 1024)
#define DHD_PREALLOC_MEMDUMP_RAM_SIZE       (1290 * 1024)
#define NAN_EVENT_RING_SIZE     (64 * 1024)
#define WLAN_SKB_1_2PAGE_BUF_NUM ((DHD_SKB_1PAGE_BUF_NUM) + \
        (DHD_SKB_2PAGE_BUF_NUM))
#define WLAN_SKB_BUF_NUM ((WLAN_SKB_1_2PAGE_BUF_NUM) + (DHD_SKB_4PAGE_BUF_NUM))

void *wlan_static_dhd_memdump_ram_buf;
void *wlan_static_nan_event_ring_buf;
void *wlan_static_fw_verbose_ring_buf;

enum dhd_prealloc_index {
    DHD_PREALLOC_SKB_BUF = 4,
    DHD_PREALLOC_MEMDUMP_RAM = 11,
    DHD_PREALLOC_FW_VERBOSE_RING = 20,
    DHD_PREALLOC_NAN_EVENT_RING = 23,
    DHD_PREALLOC_MAX
};

void *aml_mem_prealloc(int section, unsigned long size)
{
    PRINT("sectoin %d, size %ld\n", section, size);

    if (section == DHD_PREALLOC_MEMDUMP_RAM) {
        if (size > DHD_PREALLOC_MEMDUMP_RAM_SIZE) {
            PRINT("request MEMDUMP_RAM(%lu) > %d\n",
                size, DHD_PREALLOC_MEMDUMP_RAM_SIZE);
            return NULL;
        }
        return wlan_static_dhd_memdump_ram_buf;
    }

    if (section == DHD_PREALLOC_FW_VERBOSE_RING) {
        if (size > FW_VERBOSE_RING_SIZE) {
            PRINT("request FW_VERBOSE_RING(%lu) > %d\n",
                size, FW_VERBOSE_RING_SIZE);
            return NULL;
        }
        return wlan_static_fw_verbose_ring_buf;
    }

    if (section < 0 || section > DHD_PREALLOC_MAX)
        PRINT("request section id(%d) is out of max %d\n",
            section, DHD_PREALLOC_MAX);

    PRINT("failed to alloc section %d, size=%ld\n",
        section, size);

    return NULL;
}
EXPORT_SYMBOL(aml_mem_prealloc);

int aml_init_wlan_mem(void)
{
    unsigned long size = 0;
    PRINT("%s\n", AML_STATIC_VERSION_STR);

    wlan_static_dhd_memdump_ram_buf = kmalloc(DHD_PREALLOC_MEMDUMP_RAM_SIZE, GFP_KERNEL);
    if (!wlan_static_dhd_memdump_ram_buf)
         goto err_mem_alloc;
    size += DHD_PREALLOC_MEMDUMP_RAM_SIZE;
    PRINT("sectoin %d, size=%d\n",
        DHD_PREALLOC_MEMDUMP_RAM, DHD_PREALLOC_MEMDUMP_RAM_SIZE);

    wlan_static_fw_verbose_ring_buf = kmalloc(FW_VERBOSE_RING_SIZE, GFP_KERNEL);
    if (!wlan_static_fw_verbose_ring_buf)
        goto err_mem_alloc;
    size += FW_VERBOSE_RING_SIZE;
    PRINT("sectoin %d, size=%d\n",
        DHD_PREALLOC_FW_VERBOSE_RING, FW_VERBOSE_RING_SIZE);

    PRINT("prealloc ok: %ld(%ldK)\n", size, size / 1024);
    return 0;

err_mem_alloc:
    kfree(wlan_static_dhd_memdump_ram_buf);
    kfree(wlan_static_fw_verbose_ring_buf);
    PRINT("Failed to mem_alloc for WLAN\n");

    return -ENOMEM;
}
#endif

int  aml_w1_sdio_init(void)
{
    int err = 0;

    //amlwifi_set_sdio_host_clk(200000000);//200MHZ

    err = sdio_register_driver(&aml_w1_sdio_driver);
    w1_sdio_driver_insmoded = 1;
    wifi_in_insmod = 0;
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
//EXPORT_SYMBOL(aml_w1_sdio_exit);

EXPORT_SYMBOL(w1_sdio_driver_insmoded);
EXPORT_SYMBOL(wifi_in_insmod);
EXPORT_SYMBOL(w1_sdio_after_porbe);
EXPORT_SYMBOL(host_wake_w1_req);
EXPORT_SYMBOL(host_suspend_req);
EXPORT_SYMBOL(host_resume_req);
EXPORT_SYMBOL(wifi_sdio_access);

EXPORT_SYMBOL(aml_wifi_sdio_power_lock);
EXPORT_SYMBOL(aml_wifi_sdio_power_unlock);
/*set_wifi_bt_sdio_driver_bit() is used to determine whether to unregister sdio power driver.
  *Only when w1_sdio_wifi_bt_alive is 0, then call aml_w1_sdio_exit().
*/
void set_wifi_bt_sdio_driver_bit(bool is_register, int shift)
{
    AML_W1_BT_WIFI_MUTEX_ON();
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
    AML_W1_BT_WIFI_MUTEX_OFF();
}
EXPORT_SYMBOL(set_wifi_bt_sdio_driver_bit);
EXPORT_SYMBOL(g_w1_hwif_sdio);
EXPORT_SYMBOL(g_w1_hif_ops);

static int aml_w1_sdio_insmod(void)
{
#ifdef NOT_AMLOGIC_PLATFORM
    int ret;
    ret = aml_init_wlan_mem();
    if (ret) {
        PRINT("aml_init_wlan_mem err: %d \n", ret);
        return -ENOMEM;
    }
#endif
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
