#include "wifi_drv_capture.h"

#ifdef WIFI_CAPTURE

unsigned char readbuf[TESTBUSBUF_LEN];
int g_test_bus = 0;
int g_bcap_name = 0;
int g_test_gain = 0;

unsigned int dut_get_reg_frag(int address, int high_bit, int low_bit)
{
    unsigned int tmp = 0;
    unsigned char error;
    struct hw_interface* hif = hif_get_hw_interface();

    int bitwidth = high_bit - low_bit + 1;
    int mask = (bitwidth == 32) ? -1 : ((1 << (bitwidth)) - 1);
    if ((bitwidth < 1) || (low_bit > 31) || (high_bit > 31)) {
        error = 1;
        printk("get reg frag error!addr 0x%x high_bit %d low_bit %d\n", address, high_bit, low_bit);
    } else {
        tmp = hif->hif_ops.hi_read_word(address);
        tmp >>= low_bit;
        tmp &= mask;
       error = 0;
    }
    return (tmp);
}

#define BASE_AHB_TESTBUS_CAPTURE 0x00b00000
#if 0
int dut_v32_tx(unsigned int *outbuffer, unsigned int *inbufer, const unsigned long long bitnum)
{
    int inoffset = 0;
    unsigned long long indata = inbufer[0];
    unsigned long long indatabitnum = 32;
    int remainbitnum = 0;
    int len = INBUFFER_LEN_VALID_BIT / bitnum ;
    int outoffset = len - 1;
    unsigned long long tmp = 0;


    //printk("bitnum111=%016llx\n", bitnum);

    for (inoffset = 0; inoffset < (INBUFFER_LEN / 4); inoffset++){
        for (;;) {
            //printk("bit = %08x, indata=%016llx, ((1<<bitnum)-1)=%016llx\n",  (long unsigned int )indata&((1<<bitnum)-1), indata, (long unsigned int )((1<<bitnum)-1) );
            outbuffer[outoffset] = indata & (((unsigned long long)1 <<  bitnum) - (unsigned long long)1);
            outoffset--;
            if (outoffset < 0) {
                return len * 4;
            }
            indata = (indata >> bitnum);
            indatabitnum -= bitnum;
            if (indatabitnum < bitnum) {
                break;
            }
        }
        remainbitnum = indatabitnum;
        tmp = inbufer[inoffset + 1];
        indata = indata | (tmp << remainbitnum);
        indatabitnum = 32 + remainbitnum;
    }
    return len * 4;
}
#endif

int dut_dump_data(unsigned int addr, unsigned char *data, int len)
{
    //#define MAX_SDIO_ACCESS_LEN (512)
#define MAX_SDIO_ACCESS_LEN (4096)
    int remain = len;
    unsigned char* d_ptr = data;
    unsigned int a_cnt = 0;
    //unsigned int i = 0;
    struct hw_interface* hif = hif_get_hw_interface();
    unsigned int sdio_base = 0;

    ASSERT(data);
    ASSERT(len > 0);

    for (;;) {
        //printk("%s(%d) read addr=0x%4x, data_len=%d\n", __func__, __LINE__,BASE_AHB_TESTBUS_CAPTURE+addr+ a_cnt, MAX_SDIO_ACCESS_LEN);
        if (sdio_base !=  ((BASE_AHB_TESTBUS_CAPTURE +  addr + a_cnt) & 0xffff0000)) {
            sdio_base = (BASE_AHB_TESTBUS_CAPTURE + addr + a_cnt) & 0xffff0000;
            hif->hif_ops.hi_write_reg32(RG_SCFG_FUNC5_BADDR_A,
                (unsigned long)(BASE_AHB_TESTBUS_CAPTURE + addr + a_cnt) & 0xffff0000);
            printk("sdio base addr change ,sdio base addr=0x%08x\n", sdio_base);
        }

        if (remain <= MAX_SDIO_ACCESS_LEN) {
            hif->hif_ops.bt_hi_read_sram(d_ptr,
                (unsigned char*)(SYS_TYPE)((SYS_TYPE)(BASE_AHB_TESTBUS_CAPTURE + addr + a_cnt) &  (SYS_TYPE)0xffff),
                (SYS_TYPE)remain);
            break;
        } else {
            hif->hif_ops.bt_hi_read_sram(d_ptr,
                (unsigned char*)(SYS_TYPE)((SYS_TYPE)(BASE_AHB_TESTBUS_CAPTURE + addr + a_cnt) & (SYS_TYPE)0xffff),
                MAX_SDIO_ACCESS_LEN);

            remain -= MAX_SDIO_ACCESS_LEN;
            d_ptr += (unsigned long)MAX_SDIO_ACCESS_LEN;
            a_cnt += MAX_SDIO_ACCESS_LEN;
        }
    }
#if 0
    printk("print data start\n");
    for ( i = 0; i < len ; i++) {
        printk("0x%02x  ", data[i]);
        if ( i %16 == 0 ) {
            printk("\n");
        }
    }
    printk("\n");
#endif
    return 0;
}

void dut_set_tbus(int trigger)
{
    unsigned int read_tmp = 0;
    struct hw_interface* hif = hif_get_hw_interface();
#if 0
    struct timeval t_start;
    struct timeval t_end;
    unsigned int stopaddr = 0;
    int i = 0;
    unsigned int tmp = 0;
#endif

    read_tmp = hif->hif_ops.hi_read_word(TBC_OFFSET_114);
    read_tmp = (read_tmp &~ (1 << 8) ); // 0->1 , lock date to hw
    hif->hif_ops.hi_write_word(TBC_OFFSET_114, read_tmp);

    read_tmp = hif->hif_ops.hi_read_word(TBC_OFFSET_114);
    read_tmp = (read_tmp &~ (1 << 10) );

    hif->hif_ops.hi_write_word(TBC_OFFSET_114, read_tmp);
    dut_set_reg_frag(TBC_OFFSET_114 , 16, 12, trigger);
    read_tmp = hif->hif_ops.hi_read_word(TBC_OFFSET_114);

    read_tmp = (read_tmp | (1 << 8));
    hif->hif_ops.hi_write_word(TBC_OFFSET_114, read_tmp);
#if 0
    do_gettimeofday(&t_start);

    for (i = 0; i < 50000000; i++) {
        stopaddr =  hw_if->hif_ops.hi_read_word(TBC_OFFSET_120);
        if ( tmp == stopaddr ) {
            do_gettimeofday(&t_end);
            printk("used time %dus stopaddr=0x%08x, =%d\n",
                _get_tv_us(&t_end, &t_start),  stopaddr, i );
            break;
        }
        tmp = stopaddr;
        //printk("stopaddr=0x%0x\n", stopaddr);
        //msleep(5);
    }
    printk("stopaddr=0x%0x\n", stopaddr);
#endif
}



/*
comand:
bit31:bit28  is stop_mode for rsv
bit27:bit16  is delay time
bit15:bit8    is testbus capture data bandwith
bit7:bit4      is trigger ,tbc_stop_sel
bit3:bit0      is test mode,   1, test mode,  0,capture adc data

<--WIFI CAP CMD -->e.g:
start cmd: iw dev wlan0 vendor send 0xc3 0xc4 0x16 0x00 0x00 0xtest bus 0x00
stop cmd: iw dev wlan0 vendor send 0xc3 0xc4 0x17
*/

void dut_set_bcap_log_name(unsigned int value)
{
    g_bcap_name = value;
}

void dut_set_gain(unsigned int value)
{
    unsigned int rd_value = 0;
    unsigned int chk_value = 0;
    new_get_reg(0x00a0810c, &rd_value);
    rd_value &= (~0x3ff0);
    rd_value |= ((value & 0x3ff) << 4);// gain value
    new_set_reg(0x00a0810c, rd_value);
    g_test_gain = value & 0x3ff;

    new_get_reg(0x00a0810c, &chk_value);
    printk("check gain 0x%x\n", chk_value);
}

int dut_start_capture(unsigned int value)
{

    unsigned int wf_bt_mode = 0;
    struct hw_interface* hif = NULL;

    int test_mode = value & 0x1;
    int trigger = (value >> 4) & 0x1;
    int test_bus = (value >> 8) & 0xff;
    unsigned int trigger_delay_time = (value >> 16) & 0xff;/*delay how many us */
    //unsigned int stop_mode = (value>>28)&0x7;
    wf_bt_mode = (value >> 31) & 0x1;

    hif = hif_get_hw_interface();
    g_test_bus = test_bus; // for log file name

    trigger_delay_time = trigger_delay_time * 80;  /*cycle number*/
    hif->hif_ops.hi_write_word(0x00a0b000, 0xffffffff); //phy clock mask
    test_bus = (0x01 << 24) | (test_bus << 16) | 0x0008;
    hif->hif_ops.hi_write_word(TBC_OFFSET_100, test_bus);//test bus enable
    hif->hif_ops.hi_write_word(TBC_OFFSET_108, 0x00000000);//valid
    hif->hif_ops.hi_write_word(TBC_OFFSET_10C, TBC_ADDR_BEGIN_OFFSET);//config testbus capture address begin
    hif->hif_ops.hi_write_word(TBC_OFFSET_110, TBC_ADDR_END_OFFSET);//config testbus capture address end
    hif->hif_ops.hi_write_word(TBC_OFFSET_118, trigger_delay_time);//config testbus trigger delay time
    hif->hif_ops.hi_write_word(TBC_RAM_SHARE, TBC_RAM_SHARE_MASK); //ram share enable

    dut_set_reg_frag(TBC_OFFSET_114, 4, 0, 0x1c);// config tbc_test_bus_width, by default : 0x1c
    dut_set_reg_frag(TBC_OFFSET_114, 9, 9, test_mode & 0x1);// Set test_mode
    dut_set_reg_frag(TBC_OFFSET_114, 10, 10, 0 & 0x01);//
    dut_set_reg_frag(TBC_OFFSET_114 , 8, 8, 0);// 0->1 , lock date to hw
    dut_set_reg_frag(TBC_OFFSET_114 , 10, 10, 0);// sw stop

    dut_set_reg_frag(TBC_OFFSET_114 , 16, 12, trigger);
    dut_set_reg_frag(TBC_OFFSET_114 , 8, 8, 1);// enable

    printk("start_capture: \ntmode 0x%08x, trg 0x%08x, trigger_delay_time=%d, test_bus=0x%08x\nphy100=0x%08x, phy114=0x%08x\n",
        test_mode, trigger, trigger_delay_time, test_bus,
        hif->hif_ops.hi_read_word(TBC_OFFSET_100),
        hif->hif_ops.hi_read_word(TBC_OFFSET_114));

    return 0;
}

int dut_stop_capture(void)
{
    //unsigned int ram_share = 0;
    //struct hw_interface* hif = hif_get_hw_interface();
    struct file *fp = NULL;
    char fp_path[100] = {'\0'};

    if (g_bcap_name != 0) {
        sprintf(fp_path,"/data/aml_capture_chn_0x%x.log", g_bcap_name);
    } else {
        if (g_test_gain == 0) {
            sprintf(fp_path,"/data/aml_capture_0x%x.log", g_test_bus);
        } else {
            sprintf(fp_path,"/data/aml_capture_0x%x_gain_0x%x.log", g_test_bus, g_test_gain);
        }
    }

    printk("cap_log file %s\n", fp_path);
    fp = filp_open(fp_path, O_RDWR | O_CREAT | O_APPEND | O_TRUNC, 0777);

    if (IS_ERR(fp)) {
        printk("create file error/n");
        return -1;
    }

    //ram_share =  hif->hif_ops.hi_read_word(TBC_RAM_SHARE);

    dut_stop_tbus_to_get_sram(fp, 0, 1);

    if (fp) {
        filp_close(fp, NULL);
        printk("close file and exit\n");
    }

    // return the capture buf to mac
    //ram_share &= 0x7fffffff;
    //hif->hif_ops.hi_write_word(TBC_RAM_SHARE, ram_share);

    g_test_bus = 0;
    g_test_gain = 0;
    g_bcap_name = 0;

    return 0;
}

//0x12 0x34 0x56 0x78 -> 0x12345678 0x11223344(ASCII) big end
static void str_2_acsi_32bits(char* str_in, char* str_out)
{
    int i = 0;
    short high = 0;
    short low = 0;

    ASSERT(str_in);
    ASSERT(str_out);

    // to be the ASCI from a str
    for (i = 0; i < 4 ; i++) {
        high = ((*(str_in+i) >>  4) & 0xf);
        low = ((*(str_in+i)) & 0xf);

        if ((high <= 0x9) && (high >= 0x0)) {
            str_out[7 - (i * 2 + 1)] = high + 0x30; // 0 ~ 9 -> ASCI 0~9
        } else if (high <= 0xf && high >= 0xa) {
            str_out[7 - (i * 2 + 1)] = high + 0x37; //A~F -> ASCII A~F
        }

        if (low <= 0x9 && low >= 0x0) {
            str_out[7 - i * 2] = low + 0x30;
        } else if (low <= 0xf && low >= 0xa) {
            str_out[7 - i * 2] = low + 0x37; //A~F -> ASCII A~F
        }
    }

    return;
}


//0:use sw stop   1:use hw stop
int  dut_stop_tbus_to_get_sram(struct file *filep, int stop_ctrl, int save_file)
{
    #define HARDWARE_CONTROL 1
    unsigned int read_tmp = 0;
    unsigned int stopaddr = 0;
    unsigned int *pdata = 0;
    unsigned int i = 0;
    int cap_len = 0;
    int temp_addr = 0;
    unsigned int stop_flag = 0;
    int testbitwidth = 0;
    loff_t  file_pos = 0;
    struct hw_interface* hif = hif_get_hw_interface();

    mm_segment_t fs;
    fs = get_fs();
    set_fs(KERNEL_DS);

    cap_len = hif->hif_ops.hi_read_word(TBC_OFFSET_110);
    read_tmp = hif->hif_ops.hi_read_word(TBC_OFFSET_114);

    testbitwidth = (read_tmp & 0x1f) + 1;

    if (HARDWARE_CONTROL == stop_ctrl) {
        stop_flag = hif->hif_ops.hi_read_word(TBC_OFFSET_128);
        stop_flag = stop_flag & (1 << 24);
        if (stop_flag == 0) {
            dut_set_reg_frag(TBC_OFFSET_114, 8, 8, 0);//testbus capture disable
            ERROR_DEBUG_OUT("NO DATA !!! Testbus Capture Disable!!!\n");
            return 0;
        }
    }

    read_tmp = hif->hif_ops.hi_read_word(TBC_OFFSET_114);
    read_tmp = (read_tmp | (1 << 10) );  // tbc_stop
    hif->hif_ops.hi_write_word(TBC_OFFSET_114, read_tmp);

    stopaddr = hif->hif_ops.hi_read_word(TBC_OFFSET_120);
    while (temp_addr != stopaddr) {
        temp_addr = stopaddr;
        msleep(20);
        stopaddr = hif->hif_ops.hi_read_word(TBC_OFFSET_120);
        AML_OUTPUT(" read stop addr....\n");
    }

    stopaddr = (stopaddr & 0x1c) ? (stopaddr + 4) : stopaddr ;
    if (stopaddr > cap_len) {
        ERROR_DEBUG_OUT("stopaddr>cap_len !!, stopaddr=0x%x, cap_len=0x%x\n", stopaddr, cap_len);
        return 0;
    }

    hif->hif_ops.hi_write_word(TBC_RAM_SHARE, TBC_RAM_SHARE_MASK); //ram share enable

    dut_dump_data(stopaddr, &readbuf[0], cap_len-stopaddr);
    AML_OUTPUT("cap_len=0x%08x, stopaddr=0x%08x\n", cap_len, stopaddr);
    dut_dump_data(TBC_ADDR_BEGIN_OFFSET, &readbuf[cap_len-stopaddr], stopaddr - TBC_ADDR_BEGIN_OFFSET);

    pdata = (unsigned int *)&readbuf[0];

    AML_OUTPUT("testbitwidth=0x%08x\n", testbitwidth);

    if (save_file) {
        for(i = 0; i < 1024 * 4 * 10; i++) {
            vfs_write(filep, &readbuf[i], sizeof(unsigned char), &file_pos);
        }
        vfs_write(filep, (char*)&testbitwidth, sizeof(unsigned int), &file_pos);
    }

    vfs_fsync(filep, 0);
    set_fs(fs);
    AML_OUTPUT("handle capture data complete\n");
    return 1;
}

//0:use sw stop   1:use hw stop
int iwp_stop_tbus_to_get_sram(short *buf)
{
    #define HARDWARE_CONTROL 1
    unsigned int read_tmp = 0;
    unsigned int stopaddr = 0;
    unsigned int *pdata = 0;
    unsigned int i = 0;
    int cap_len = 0;
    int temp_addr = 0;
    int testbitwidth = 0;
    struct hw_interface* hif = hif_get_hw_interface();

    cap_len = hif->hif_ops.hi_read_word(TBC_OFFSET_110);
    read_tmp = hif->hif_ops.hi_read_word(TBC_OFFSET_114);

    testbitwidth = (read_tmp & 0x1f) + 1;


    read_tmp = hif->hif_ops.hi_read_word(TBC_OFFSET_114);
    read_tmp = (read_tmp | (1 << 10) );  // tbc_stop
    hif->hif_ops.hi_write_word(TBC_OFFSET_114, read_tmp);

    stopaddr = hif->hif_ops.hi_read_word(TBC_OFFSET_120);
    while (temp_addr != stopaddr) {
        temp_addr = stopaddr;
        msleep(20);
        stopaddr = hif->hif_ops.hi_read_word(TBC_OFFSET_120);
        AML_OUTPUT(" read stop addr....\n");
    }

    stopaddr = (stopaddr & 0x1c) ? (stopaddr + 4) : stopaddr ;
    if (stopaddr > cap_len) {
        ERROR_DEBUG_OUT("stopaddr>cap_len !!, stopaddr=0x%x, cap_len=0x%x\n", stopaddr, cap_len);
        return 0;
    }

    hif->hif_ops.hi_write_word(TBC_RAM_SHARE, TBC_RAM_SHARE_MASK); //ram share enable

    AML_OUTPUT("cap_len=0x%08x, stopaddr=0x%08x\n", cap_len, stopaddr);
    dut_dump_data(stopaddr, &readbuf[0], 1024);

    pdata = (unsigned int *)&readbuf[0];

    for(i = 0; i < 1024; i++) {
        *buf = readbuf[i];
        buf++;
    }

    return 1;
}
unsigned char dut_set_reg_frag(int address, int bit_end, int bit_start, int value)
{
    unsigned int tmp = 0;
    unsigned char error;
    int bitwidth = bit_end-bit_start + 1;
    int max_value = (bitwidth == 32) ? -1 : ((1 << (bitwidth)) - 1);

    struct hw_interface* hif = hif_get_hw_interface();
    tmp = hif->hif_ops.hi_read_word(address);
    //printk("tmp is 0x%x\n", tmp);

    if ((bitwidth < 1) || (bit_start > 31) || (bit_end > 31)) {
        while (1) {
           printk("----------------->Set_RegFragment is halt.\n");
        }
        error = 1;
    } else {
        error = 0;
        tmp &= ~(max_value << bit_start); // clear [bit_end: bit_start]
        tmp |= ((value&max_value) << bit_start);
        //printk("addr 0x%x tmp2 is 0x%x\n", address, tmp);
        hif->hif_ops.hi_write_word(address, tmp);
    }

    return error;
}

int dut_bt_start_capture(unsigned int value)
{

    unsigned int wf_bt_mode = 0;
    struct hw_interface* hif = NULL;

    int test_mode = value & 0x1;
    int trigger = (value >> 4) & 0x1;
    int test_bus = (value >> 8) & 0xff;

    wf_bt_mode = (value >> 31) & 0x1;
    hif = hif_get_hw_interface();
    g_test_bus = test_bus; // for log file name

    hif->hif_ops.hi_write_word(0x00a0b000, 0xffffffff); //phy clock mask
    test_bus = (0x01 << 24) | (test_bus << 16) | 0x0008;
    hif->hif_ops.hi_write_word(TBC_OFFSET_100, test_bus);//test bus enable
    hif->hif_ops.hi_write_word(TBC_OFFSET_108, 0x00000000);//valid
    hif->hif_ops.hi_write_word(TBC_OFFSET_10C, 0x00020000);//config testbus capture address begin
    hif->hif_ops.hi_write_word(TBC_OFFSET_110, TBC_ADDR_END_OFFSET);//config testbus capture address end
    hif->hif_ops.hi_write_word(TBC_RAM_SHARE, 0xf000007f); //ram share enable

    hif->hif_ops.hi_write_word(TBC_OFFSET_114, 0x0000051c); //ram share enable

    printk("start_capture: \ntmode 0x%08x, trg 0x%08x,  test_bus=0x%08x\nphy100=0x%08x, phy114=0x%08x\n",
        test_mode, trigger, test_bus,
        hif->hif_ops.hi_read_word(TBC_OFFSET_100),
        hif->hif_ops.hi_read_word(TBC_OFFSET_114));

   return 0;
}

int dut_bt_stop_capture(void)
{
    struct file *fp = NULL;
    char fp_path[100] = {'\0'};

    if (g_bcap_name != 0) {
        sprintf(fp_path,"/data/aml_capture_chn_0x%x.log", g_bcap_name);
    } else {
        if (g_test_gain == 0) {
            sprintf(fp_path,"/data/aml_capture_0x%x.log", g_test_bus);
        } else {
            sprintf(fp_path,"/data/aml_capture_0x%x_gain_0x%x.log", g_test_bus, g_test_gain);
        }
    }

    printk("cap_log file %s\n", fp_path);
    fp = filp_open(fp_path,O_RDWR | O_CREAT | O_APPEND | O_TRUNC, 0777);

    if (IS_ERR(fp)) {
        ERROR_DEBUG_OUT("create file error/n");
        return -1;
    }


    dut_stop_tbus_to_get_sram(fp, 0, 1);

    if (fp) {
        filp_close(fp, NULL);
        printk("close file and exit\n");
    }

    g_test_bus = 0;
    g_test_gain = 0;
    g_bcap_name = 0;

    return 0;
}

#endif
