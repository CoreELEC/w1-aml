
//#define RUN_PT

#ifdef RUN_PT
#include "stdafx.h"
#include "..\..\dut_dll\DUT_dll.h"
#include "t902x_calibration.h"
#include "DUTReadCfgFile.h"
#include "math.h"
#include "example.h"
#include "IQmeasure.h"
#include "IQsetting.h"
#include "dut_tx_test.h"
#else
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/timer.h>
#include "wifi_hal_com.h"
#include "wifi_mac.h"
#include "t9023_calb.h"
#include "wifi_drv_reg_ops.h"
#include <linux/delay.h>
#endif

#ifdef RF_T9023

struct calb_rst_out calb_val;
struct calb_iqxel_out calb_iqxel;

static unsigned int i2c_read_reg(unsigned int addr)
{
    unsigned int data;
    new_get_reg(addr, &data); 

    return data;
}

static void i2c_write_reg(unsigned int addr,unsigned int data)
{
    new_set_reg(addr,data); 
}

#ifdef RUN_PT
void  i2c_set_reg_fragment(unsigned int addr,unsigned int bit_end,
	unsigned int bit_start,unsigned int value)
{
    unsigned int tmp;
    unsigned int bitwidth = bit_end - bit_start + 1;
    int max_value = (bitwidth == 32)? -1 : (1 << (bitwidth)) - 1;

    ASSERT((bitwidth > 0)||(bit_start <= 31)||(bit_end <= 31));

    tmp = i2c_read_reg(addr);
    tmp &= ~(max_value << bit_start); // clear [bit_end: bit_start]
    tmp |= ((value & max_value) << bit_start);

    i2c_write_reg(addr,tmp);
}

unsigned int  i2c_get_reg_fragment(int address, int bit_end, int bit_start)
{
	unsigned int tmp;
	int bitwidth= bit_end-bit_start + 1;
	int max_value = (bitwidth==32)?-1: (1<<(bitwidth)) - 1;

	tmp = i2c_read_reg(address);
	tmp >>= bit_start;
	tmp &= max_value;
	return (tmp);
}
#endif 

void bat_set_reg(unsigned int reg_cfg[][2], int reg_cfg_len)
{
	int i  = 0;
	for(i = 0; i < reg_cfg_len; i++)
	{
		new_set_reg(reg_cfg[i][0], reg_cfg[i][1]);
	}
}

void init_t9023(void)
{
	unsigned int agc_9023_tbl[][2] = 
	{
		{0x00008114, 0x00000005},
		{0x00008300, 0x00000011},
		{0x00008304, 0x00000012},
		{0x00008308, 0x00000013},
		{0x0000830c, 0x00000014},
		{0x00008310, 0x00000015},
		{0x00008314, 0x00000016},
		{0x00008318, 0x00000024},
		{0x0000831c, 0x00000025},
		{0x00008320, 0x00000026},
		{0x00008324, 0x00000034},
		{0x00008328, 0x00000035},
		{0x0000832c, 0x00000036},
		{0x00008330, 0x00000044},
		{0x00008334, 0x00000045},
		{0x00008338, 0x00000046},
		{0x0000833c, 0x00000054},
		{0x00008340, 0x00000055},
		{0x00008344, 0x00000056},
		{0x00008348, 0x00000064},
		{0x0000834c, 0x00000065},
		{0x00008350, 0x00000066}, 
		{0x00008354, 0x00000074},
		{0x00008358, 0x00000075},
		{0x0000835c, 0x00000076},
		{0x00008360, 0x00000084},
		{0x00008364, 0x00000085},
		{0x00008368, 0x00000086},
		{0x0000836c, 0x00000094},
		{0x00008370, 0x00000095},
		{0x00008374, 0x00000096},
		{0x00008378, 0x00000097},
		{0x0000837c, 0x00000098},
		{0x00008380, 0x00000099},
		{0x00008384, 0x0000009a},
		{0x00008388, 0x0000009b},
		{0x0000838c, 0x0000009c},
		{0x00008390, 0x0000009c},
		{0x00008114, 0x00000105},
		{0x00008050, 0x003ec104},
		{0x00008054, 0x003ec104}
	};
   
	unsigned int t9023_init_cfg[][2] = 
	{
		{0xff000000,0x00000000},
		{0xff000004,0xffffffff},
		// 3212 ->3210
		{0xff000008,0x00003210},  
		{0xff00000c,0x000f0180},
		{0xff000010,0x000081c0},
		{0xff000014,0x00000000},
		{0xff000018,0x00000000},
		{0xff00001c,0x00000073},
		{0xff000020,0x000000bd},
		// 3f10->3f11
		{0xff000100,0x00003f10},  
		{0xff000104,0x00100400},
		{0xff00010c,0x00311112},
		{0xff000110,0x00000381},
		{0xff000118,0x000a0751},//  0a->a1
		{0xff00011c,0x000014f0},
		{0xff000120,0x00000000},
		{0xff000124,0x00000000},
		{0xff00012c,0x00000011},
		
		//{0xff00013c,0x00000003},
		{0xff00013c,0x00000043},
		
		{0xff000140,0x00070070},
		{0xff000144,0x789abcde},
		{0xff000148,0x00000046},
		{0xff00014c,0x00000000},
		{0xff00015c,0x00000000},
		{0xff000160,0x000003ff},
		{0xff000164,0x000013ff},
		{0xff000168,0x000007ff},
		{0xff00016c,0x0000ffff},
		{0xff000170,0x0000ffff},
		{0xff000174,0x0000ffff},
		{0xff000178,0x0000ffff},
		{0xff000180,0x00000707},
		{0xff000184,0x00202000},
		{0xff000188,0x00080100},
		{0xff00018c,0x17828900},
		{0xff000190,0x00001a00},
		{0xff000194,0x02607770},
		{0xff000198,0x00000880},
		{0xff00019c,0x0012777f},
		{0xff0001a0,0x00001010},
		{0xff0001a4,0x00000081},
		{0xff0001a8,0x0000ffff},
		{0xff0001ac,0x0000ffff},
		{0xff000200,0x03639000},
        
	    //{0xff000204,0x0ac811c0},//adb on for tx pa linearity
		{0xff000204,0x0c8811c0},//adb close
		
		{0xff000208,0x00110360},
		{0xff00020c,0x00001077},
		{0xff000210,0x00000000},
		{0xff000214,0x00000000},
		{0xff000218,0x0000001f},
		{0xff00021c,0x00000000},
		{0xff000220,0x00000000},
		{0xff000300,0x0000ffff},
		{0xff000304,0x0000ffff},
		{0xff000308,0x00019c01},
		{0xff00030c,0x00000000},
		{0xff000310,0x00000000},
		{0xff000314,0x0003a020},
		{0xff000318,0x03fc61d7},
		{0xff00031c,0x45789acd},
		{0xff000320,0x00550723},
		//{0xff000324,0x00000000},
		{0xff000324,0x32020000},//LNA tank offset after tank calb
		{0xff000328,0x00000000},
		{0xff00032c,0x3a321a12},
		{0xff000330,0x7a725a52},
		{0xff000334,0x0007ec7e},
		{0xff000338,0x03343737},
		{0xff00033c,0x0c030201},
		{0xff000340,0x003c1c14},
		{0xff000344,0x41331005},
		{0xff000348,0x00113300},
		{0xff00034c,0x00003af8},
		{0xff000350,0x00000000},
		{0xff000354,0x1e14191e},
		{0xff000358,0x00001419},
		{0xff00035c,0x00122357},
		{0xff000360,0x00003311},
		{0xff000364,0x00003c11},
		{0xff000368,0x000ce739},
		{0xff00036c,0x00000225},
		{0xff000370,0x00000225},
		{0xff000374,0x00033311},
		{0xff000378,0x00022522},
		{0xff00037c,0x019ce739},
		{0xff000380,0x40404040},
		{0xff000384,0x40404040},
		{0xff000388,0x40404040},
		{0xff00038c,0x40404040},
		{0xff000390,0x40404040},
		{0xff000394,0x40404040},
		{0xff000398,0x40404040},
		{0xff00039c,0x40404040},
		{0xff0003a0,0x40404040},
		{0xff0003a4,0x40404040},
		{0xff0003a8,0x40404040},
		{0xff0003ac,0x40404040},
		{0xff0003b0,0x00400040},
		{0xff0003b4,0x00100000},
		{0xff0003b8,0x82800000},
		{0xff0003bc,0x82800000},
		{0xff0003c0,0x82808280},
		{0xff0003c4,0x00008280},
		{0xff0003c8,0x82808280},
		{0xff0003d4,0x00000000},

		//{0xff0003d8,0x02ed8066},
		{0xff0003d8,0x00ed8066},//[27:25]:0 RSSI PGA Gain

		{0xff0003dc,0x005a0037},

		//{0xff0003e0,0x01abb76f},
		{0xff0003e0,0x01abb7fd},//dpd atten + Gm

		{0xff0003e4,0x0006c002},
		{0xff0003e8,0x0000000e},
		{0xff0003ec,0x0000b6dc},
		// 2508->3508
		{0xff0003f0,0x00003508},
		{0xff0003f4,0x00000000},
		{0xff0003f8,0x00000000},
		{0xff0003fc,0x00000000},
		
		//digtial
		{0x0000b234,0x00000000},//open spectrum filter
		{0x0000e008,0x00111100},//close esti, open tx, rx
		{0x0000e410,0x00000000},//select ofdm tone
		{0x0000e408,0x00200000},//close tone1
		{0x0000e40c,0x90200000},//open tone2
		//{0x0000e488,0x000a0010},//tx_negate_msb:1,rg_tx_iqmm_bypass:0,rg_tx_iq_swap:0
		{0x0000e488,0x000a0110},//tx_negate_msb:1,rg_tx_iqmm_bypass:0,rg_tx_iq_swap:0
		{0x0000e808,0x01100000},//rx_rxiq_swap:1 
		{0x0000e5b8,0x00000008},//set digital gain
		{0x0000e4f0,0x15008080},//set dpd bypass
		//{0x0000e4b4,0x80800000},
		
		//set dac clk phase   
		{0x0000b078,0x00640701},//0x00600701:tx testbus;0x00600721:rx testbus
		{0x0000b100,0x01900008},//0x00600701:tx testbus;0x00600721:rx testbus
	};

	bat_set_reg(agc_9023_tbl, sizeof(agc_9023_tbl)/sizeof(agc_9023_tbl[0]));

	bat_set_reg(t9023_init_cfg, sizeof(t9023_init_cfg)/sizeof(t9023_init_cfg[0]));

	i2c_set_reg_fragment(0xe82c, 4, 4, 1);//RX IRR bypass

    //revise agc reg
	new_set_reg(0x00008018,0x0480400a); // det wait timeout, 8us
    new_set_reg(0x00008020,0x00204e20); // ramp down thr
    new_set_reg(0x0000812c,0x2f231100); // rf gain step
    new_set_reg(0x000080b0,0x0e0e0e12); // wait time after sat
    new_set_reg(0x00008060,0xc39aa9b0); // stf thr
    new_set_reg(0x00008128,0x00000211); // rssi det enable [12]
    new_set_reg(0x00008084,0x01107800); // rf gain sel [28]
    new_set_reg(0x00008158,0x1011a016); // samp sat disable
    new_set_reg(0x00008038,0x321c1c53); // 1st ad sat avglen
    new_set_reg(0x0000810c,0x000019c0); // auto gain
    new_set_reg(0x00008008,0x01c401c4); // target gain

    i2c_set_reg_fragment(0x8010, 28, 28, 1);//disable agc cca detect
	
   #ifdef RUN_PT 
        TRACE("RF INIT DONE\n");
   #else
        printk("RF INIT DONE\n");
   #endif
}


struct rx_dc_result rx_dc_rst[1024];
struct rx_dc_dbg  rx_dc_dbg_info[1024];
unsigned int rst_cnt = 0;
unsigned int dbg_info_cnt = 0;
void do_rxdc_calb(struct calb_rst_out* calb_val, unsigned int bw_idx, unsigned int tia_idx, unsigned int lpf_idx)
 {
	

#ifdef RUN_PT
	int i = 0;
	char path[1024] = CALB_OUTPUT_PATH ;
	FILE *fp = NULL;
	
	sprintf(path,"%s%s",g_CurrentPath,CALB_OUTPUT_PATH);
	
	fp = fopen((char *)path, "a+");
	if (!fp){
		TRACE(" output file open fail\r\n");
		return ;
	}
	fprintf(fp, "rxdc log........\n");
#else
      printk( "rxdc log........\n");
#endif

	TestRxDC_417(calb_val,rx_dc_rst, &rst_cnt,rx_dc_dbg_info, &dbg_info_cnt,bw_idx, tia_idx, lpf_idx);
   
#ifdef RUN_PT
	for(i = 0; i < rst_cnt; i++){
		 fprintf(fp, "rxdc calibration result: num= %d dci=%d,  dcq=%d, codei=%d,  codeq=%d\n",
			 rx_dc_rst[i].num, 
			 rx_dc_rst[i].dci,
			 rx_dc_rst[i].dcq, 
			 rx_dc_rst[i].code_i, 
			 rx_dc_rst[i].code_q);
	}

	for(i = 0; i < dbg_info_cnt; i++){
        fprintf(fp,"code_i=0x%x, code_q=0x%x, reg03b0=0x%x\n",
			rx_dc_dbg_info[i].code_i,
			rx_dc_dbg_info[i].code_q,
			rx_dc_dbg_info[i].rd_reg
			);
	}

	fclose(fp);
#endif

 }

void reset_rxdc_rf_reg(void )
{
       new_set_reg(0xff000380,0x40404040);
	new_set_reg(0xff000384,0x40404040);
	new_set_reg(0xff000388,0x40404040);
	new_set_reg(0xff00038c,0x40404040);
	new_set_reg(0xff000390,0x40404040);
	new_set_reg(0xff000394,0x40404040);
	new_set_reg(0xff000398,0x40404040);
	new_set_reg(0xff00039c,0x40404040);
	new_set_reg(0xff0003a0,0x40404040);
	new_set_reg(0xff0003a4,0x40404040);
	new_set_reg(0xff0003a8,0x40404040);
	new_set_reg(0xff0003ac,0x40404040);
}

void set_rxdc_calb(struct calb_rst_out* calb_val, unsigned int bw_idx, unsigned int tia_idx, unsigned int lpf_idx)
{
      unsigned int base_addr_I = 0;
	unsigned int base_addr_Q = 0 ;
	unsigned int offset_addr_I = 0;
	unsigned int offset_addr_Q = 0;
    
	base_addr_I = 0xff000380 + tia_idx*12 + (lpf_idx/4)*4;
	base_addr_Q = 0xff000398 + tia_idx*12 + (lpf_idx/4)*4;

	offset_addr_I = (lpf_idx%4)*8;
	offset_addr_Q = (lpf_idx%4)*8;

	i2c_set_reg_fragment(base_addr_I, offset_addr_I+6, offset_addr_I, calb_val->rx_dc[bw_idx][tia_idx][lpf_idx].rxdc_codei);// I 
	i2c_set_reg_fragment(base_addr_Q, offset_addr_Q+6, offset_addr_Q, calb_val->rx_dc[bw_idx][tia_idx][lpf_idx].rxdc_codeq);// Q
}


void do_tx_dc_irr_calb(struct calb_rst_out* calb_val, unsigned int chn_idx, unsigned int gain_idx)
 {
	unsigned int data0 = 0;
	unsigned int data1 = 0;
	
	unsigned int dc_rdy = 0;
	unsigned int irr_rdy = 0;
	unsigned int unlock_cnt = 0;
	unsigned int TIME_OUT_CNT = 300;

	unsigned int dci = 0;
	unsigned int dcq = 0;	
	unsigned int alpha = 0;
	unsigned int theta = 0;

	//TX IRR&DC
	unsigned int origin_rg008;
	unsigned int origin_rg01c;
	unsigned int origin_rg208;
	unsigned int origin_rg308;
	unsigned int origin_rg314;
	//digital
	unsigned int origin_e008;
	unsigned int origin_b234;
	unsigned int origin_e018;
	unsigned int origin_e4b4;
	unsigned int origin_e40c;
    unsigned int origin_e410;
	unsigned int origin_ec50;
	unsigned int origin_e488;
    unsigned int origin_e4f0;
	unsigned int origin_e82c;
	unsigned int origin_e5b8;
	unsigned int origin_e4b8;
	unsigned int origin_ec4c;
	unsigned int origin_ec54;
	unsigned int origin_ec58;
	unsigned int origin_ec5c;
	unsigned int origin_ed00;
	unsigned int origin_ed04;
	unsigned int origin_ec38;

#ifdef RUN_PT
	char path[1024] = CALB_OUTPUT_PATH ;
	FILE *fp = NULL;
	
	sprintf(path,"%s%s",g_CurrentPath,CALB_OUTPUT_PATH );
	
	fp = fopen((char *)path, "a+");
	if (!fp){
		TRACE(" output file open fail\r\n");
		return ;
	}

	fprintf(fp, "txdcirr log........\n");
#else
      printk("txdcirr log........\n");
#endif
	
	new_get_reg(0xff000008, &origin_rg008);
	new_get_reg(0xff00001c, &origin_rg01c);
	new_get_reg(0xff000208, &origin_rg208);
	new_get_reg(0xff000308, &origin_rg308);
	new_get_reg(0xff000314, &origin_rg314);

	i2c_set_reg_fragment(0xff00001c, 7, 0, 0xff);
	i2c_set_reg_fragment(0xff000208, 0, 0, 1);

	//new_set_reg(0xff00020c, 0x00001077);//TX V2I gain(0), MIX gain(7), PA gain(7)
	//new_set_reg(0xff0003f0, 0x00003508);//TX LPF gain(3)
	i2c_set_reg_fragment(0xff000308, 7,  0, 0x01);//BW 20M
	i2c_set_reg_fragment(0xff000308,  15,  8, 0x1a);//0x19
           
	i2c_set_reg_fragment(0xff000314, 27, 12, 0x7ffe);

	//new_set_reg(0xff000200, 0x03639000);//DCOCI:0, DCOCQ:0
	//new_set_reg(0xff000200, 0x03639050);//DCOCI:1, DCOCQ:1(default) 
	//new_set_reg(0xff000200, 0x036390a0);//DCOCI:2, DCOCQ:2 comment:0(0),1(5),2(a),3(f)
	//new_set_reg(0xff000200, 0x036390f0);//DCOCI:3, DCOCQ:3
    
	i2c_set_reg_fragment(0xff000008, 1, 0, 2);//set tx mode
    i2c_set_reg_fragment(0xff000100, 0, 0, 0);//do sx calib
	i2c_set_reg_fragment(0xff000100, 0, 0, 1);
	//i2c_set_reg_fragment(0xff000008, 1, 0, 2);

	
	new_get_reg(0xe008, &origin_e008);
	new_get_reg(0xb234, &origin_b234);
	new_get_reg(0xe018, &origin_e018);
	new_get_reg(0xe4b4, &origin_e4b4);
	new_get_reg(0xe40c, &origin_e40c);
	new_get_reg(0xe410, &origin_e410);
	new_get_reg(0xec50, &origin_ec50);
	new_get_reg(0xe488, &origin_e488);
	new_get_reg(0xe4f0, &origin_e4f0);
	new_get_reg(0xe82c, &origin_e82c);
	new_get_reg(0xe5b8, &origin_e5b8);
	new_get_reg(0xe4b8, &origin_e4b8);
	new_get_reg(0xec4c, &origin_ec4c);
	new_get_reg(0xec54, &origin_ec54);
	new_get_reg(0xec58, &origin_ec58);
	new_get_reg(0xec5c, &origin_ec5c);
	new_get_reg(0xed00, &origin_ed00);
	new_get_reg(0xed04, &origin_ed04);
    new_get_reg(0xec38, &origin_ec38);


	new_set_reg(0xe008, 0x00000000);//disable esti,tx,rx
	new_set_reg(0xb234, 0x00000001);//close open spectrum,must be closed in single tone mode
	new_set_reg(0xe018, 0x00010100);//calib mode:enable:1, mode:TX DC/IRR
	i2c_set_reg_fragment(0xe4b4, 31, 31, 0);//set TX DC/IRR comp manual mode to 0(use esti value)
	//new_set_reg(0xe408, 0x00200000);//close tone1
	new_set_reg(0xe40c, 0x90200000);//open  tone2
	new_set_reg(0xe410, 0x00000001);//1: select single tone 
	new_set_reg(0xec50, 0x10000000);//read response set 0; response bypass set 1
	
	new_set_reg(0xe4f0, 0x15008080);//TX DPD bypass
	i2c_set_reg_fragment(0xe82c, 4, 4, 1);//RX IRR bypass

	new_set_reg(0xe5b8, 0x00000008);//bb gain
	i2c_set_reg_fragment(0xe4b8, 31, 24, 0x40);//set txpwctrl gain
	i2c_set_reg_fragment(0xe4b8, 16, 16, 1);//set txpwctrl gain manual to 1

	i2c_set_reg_fragment(0xec4c, 29, 29, 1);//set TX DC/IRR code continue to 1
	i2c_set_reg_fragment(0xec4c, 31, 31, 1);//set TX DC/IRR mode continue to 1

	new_set_reg(0xec54, 0x7f7f7f7f);//set max
	new_set_reg(0xec58, 0x80808080);//set min
	new_set_reg(0xec5c, 0x00041111);//set step, set search four time(max:2^4-1)
	new_set_reg(0xed00, 0x000001ff);//wait(max:2^24-1)
	new_set_reg(0xed04, 0x00001000);//calc:4096(max:2^15-1)
	new_set_reg(0xec38, 0x00000006);//alpha factor 0x00000005(max:10)
    

	new_set_reg(0xe008, 0x11111100);//enable esti,tx,rx

	new_get_reg(0xec4c, &data0);
	new_get_reg(0xec50, &data1);
	dc_rdy = (data0 >> 24 ) & 0x1;
	irr_rdy = (data1 >> 24) & 0x1;

	while(unlock_cnt < TIME_OUT_CNT){
		if(dc_rdy == 0|| irr_rdy == 0){
			new_get_reg(0xec4c, &data0);
			new_get_reg(0xec50, &data1);

			dc_rdy = (data0 >> 24 ) & 0x1;
			irr_rdy = (data1 >> 24) & 0x1;

			unlock_cnt = unlock_cnt + 1;
                   #ifdef RUN_PT
			    TRACE("unlock_cnt:%d, TX DC IRR NOT DOING...\n",unlock_cnt);
                   #else
                        printk("unlock_cnt:%d, TX DC IRR NOT DOING...\n",unlock_cnt);
                   #endif
		}else{

                      #ifdef RUN_PT
			    TRACE("TX DC IRR READY\n");
                      #else
                        printk("TX DC IRR READY\n");
                       #endif
                    break;
		}
	}

	dci = data0 & 0xff;
	dcq =  (data0 >> 12) & 0xff;
	alpha = data1 & 0xff;
	theta = (data1 >> 12) & 0xff;
    
	i2c_set_reg_fragment(0xe4b4, 31, 31, 1);   //set TX DC/IRR comp manual mode to 1
	i2c_set_reg_fragment(0xe4b4,  7, 0, dci);  //set esti DC_I
	i2c_set_reg_fragment(0xe4b4, 15, 8, dcq);  //set esti DC_Q
	i2c_set_reg_fragment(0xe4b8,  7, 0, theta);//set esti theta
	i2c_set_reg_fragment(0xe4b8, 15, 8, alpha);//set esti alpha

       #ifdef RUN_PT
	// TO OUTPUT 
	    TRACE("----------------------> TX DC&IRR out: dci: 0x%x dcq: 0x%x alpha:0x%x  theta:0x%x\n", dci, dcq, alpha, theta);
	    fprintf(fp,"dci: 0x%x dcq: 0x%x alpha:0x%x  theta:0x%x\n", dci, dcq, alpha, theta);
       #else
           printk("----------------------> TX DC&IRR out: dci: 0x%x dcq: 0x%x alpha:0x%x  theta:0x%x\n", dci, dcq, alpha, theta);
       #endif
       
	calb_val->tx_dc_irr[chn_idx][gain_idx].tx_dc_i = dci;
	calb_val->tx_dc_irr[chn_idx][gain_idx].tx_dc_q = dcq;
	calb_val->tx_dc_irr[chn_idx][gain_idx].tx_alpha = alpha;
	calb_val->tx_dc_irr[chn_idx][gain_idx].tx_theta = theta;

	
	new_set_reg(0xe008, 0x00111100);   //disable esti; open tx,rx

       #ifdef RUN_PT
	    TRACE("TX DC&IRR calib finish \n");
        #else
           printk("TX DC&IRR calib finish \n"); 
        #endif
	//set RF reg to initial vaule
	if(1){
		new_set_reg(0xff000008,origin_rg008);
		new_set_reg(0xff00001c,origin_rg01c);
	    new_set_reg(0xff000208,origin_rg208);
	    new_set_reg(0xff000308,origin_rg308);
	    new_set_reg(0xff000314,origin_rg314);
	}
	//set digital reg to initial vaule
	if(1){
		new_set_reg(0xe008, origin_e008);
		new_set_reg(0xb234, origin_b234);
		new_set_reg(0xe018, origin_e018);
		//new_set_reg(0xe4b4, origin_e4b4);
		//new_set_reg(0xe4b8, origin_e4b8);
		new_set_reg(0xe40c, origin_e40c);
		new_set_reg(0xe410, origin_e410);
		new_set_reg(0xe488, origin_e488);
		new_set_reg(0xe4f0, origin_e4f0);
		new_set_reg(0xe5b8, origin_e5b8);
		new_set_reg(0xe82c, origin_e82c);
		new_set_reg(0xec50, origin_ec50);
		new_set_reg(0xec4c, origin_ec4c);
		new_set_reg(0xec54, origin_ec54);
		new_set_reg(0xec58, origin_ec58);
		new_set_reg(0xec5c, origin_ec5c);
		new_set_reg(0xed00, origin_ed00);
		new_set_reg(0xed04, origin_ed04);
		new_set_reg(0xec38, origin_ec38);
	}

       #ifdef RUN_PT
    	    fclose(fp);
       #endif
 }


#define RF_BW_20M (0)
#define RF_BW_40M (1)
#define RF_BW_80M (2)

void do_tx_dpd_irr_calb(struct calb_rst_out* calb_val, 
						bool tx_dciq_manual, 
						unsigned int chn_idx,
						unsigned int bw_idx,
						unsigned int gain_idx)
{

	
	unsigned rf_bw = 0;
	unsigned int origin_rg008 = 0;
	unsigned int origin_rg01c = 0;
	unsigned int origin_rg164 = 0;
	unsigned int origin_rg200 = 0;
	unsigned int origin_rg204 = 0;
	unsigned int origin_rg208 = 0;
	unsigned int origin_rg308 = 0;
	unsigned int origin_rg314 = 0;
	unsigned int origin_rg3e0 = 0;
	unsigned int origin_rg3e8 = 0;
	//digital
    unsigned int origin_e008;
	unsigned int origin_b234;
	unsigned int origin_e018;
	unsigned int origin_e4f0;
	unsigned int origin_e5b8;
	unsigned int origin_e4b8;
	unsigned int origin_ec2c;
	unsigned int origin_e40c;
	unsigned int origin_e410;
	unsigned int origin_e014;

	unsigned int unlock_cnt = 0;
	unsigned int TIME_OUT_CNT = 300;


      unsigned int dci = 0;
	unsigned int dcq = 0;	
	unsigned int alpha = 0;
	unsigned int theta = 0;


	unsigned int data;
	unsigned int rx_irr_rdy;
	unsigned int coeff_eup0;
	unsigned int coeff_eup1;
	unsigned int coeff_eup2;
	unsigned int coeff_eup3;
	unsigned int coeff_pup0;
	unsigned int coeff_pup1;
	unsigned int coeff_pup2;
	unsigned int coeff_pup3;
	unsigned int coeff_elow0;
	unsigned int coeff_elow1;
	unsigned int coeff_elow2;
	unsigned int coeff_elow3;
	unsigned int coeff_plow0;
	unsigned int coeff_plow1;
	unsigned int coeff_plow2;
	unsigned int coeff_plow3;
	unsigned int tmp;

	unsigned int rx_irr_bw[]= {0,1,2};
	unsigned int rx_irr_manual = 0;

	rf_bw = rx_irr_bw[bw_idx];

       #ifdef RUN_PT
           char path[1024] = CALB_OUTPUT_PATH ;
    	    FILE *fp = NULL;
    	
    	    sprintf(path,"%s%s",g_CurrentPath,CALB_OUTPUT_PATH );
    	
    	    fp = fopen((char *)path, "a+");
    	    if (!fp){
    		TRACE(" output file open fail\r\n");
    		return ;
    	    }
       #endif
      
	new_get_reg(0xff000008, &origin_rg008);
	new_get_reg(0xff00001c, &origin_rg01c);
	new_get_reg(0xff000164, &origin_rg164);
	new_get_reg(0xff000200, &origin_rg200);
	new_get_reg(0xff000204, &origin_rg204);
	new_get_reg(0xff000208, &origin_rg208);
	//new_get_reg(0xff00020c, &origin_rg20c);
	new_get_reg(0xff000308, &origin_rg308);
	new_get_reg(0xff000314, &origin_rg314);
	//new_get_reg(0xff0003f0, &origin_rg3f0);
	new_get_reg(0xff0003e0, &origin_rg3e0);
	new_get_reg(0xff0003e8, &origin_rg3e8);
	
	//Rx DPD LOOP IRR
	new_set_reg(0xff00001c,0x000000ff);
    new_set_reg(0xff000164,0x00001bff);
    
	new_set_reg(0xff000200,0x03639000);//tx dcoc i/q,txm_ct(default)
	new_set_reg(0xff000204,0x0c8811c0);//tx adb close
	new_set_reg(0xff000208,0x00110360);//tx dcoc i/q,txm_ct(default)
	//new_set_reg(0xff00020c,0x00001077);//tx v2i.mixr pa gain(default)
    
	//new_set_reg(0xff000308,0x00019c1d);// FE+LPF gain, BW:160M, ADC 320M
	new_set_reg(0xff000308,0x00017c1d);// FE+LPF gain, BW:160M, ADC 320M

	new_set_reg(0xff000314,0x03ffe619);

	//new_set_reg(0xff0003f0,0x00003508);// tx lpf gain

    new_set_reg(0xff0003e0,0x01abb7fc);// GM/dpd atten
	new_set_reg(0xff0003e8,0x00000006);// close ADC notch filter	

	i2c_set_reg_fragment(0xff0003f0, 13, 12, 0);//set tx lpf gain
    
	i2c_set_reg_fragment(0xff000008,1,0,2);// set tx mode to enable dpd loop;//if tx&rx path loop all open, set mode is tx(2), if only tx path open, set mode is tx(2), if only rx path open, set mode is rx(3)
	i2c_set_reg_fragment(0xff000100,0,0,0);// triger sx freq lock
	i2c_set_reg_fragment(0xff000100,0,0,1);
	//i2c_set_reg_fragment(0xff000008,1,0,2);
   
	

	new_get_reg(0xe008, &origin_e008);
	new_get_reg(0xb234, &origin_b234);
	new_get_reg(0xe018, &origin_e018);
	//origin_e82c=i2c_get_reg_fragment(0xe82c, 4, 4);
	new_get_reg(0xe4f0, &origin_e4f0);
	new_get_reg(0xe5b8, &origin_e5b8);
	new_get_reg(0xe4b8, &origin_e4b8);
	new_get_reg(0xec2c, &origin_ec2c);
	new_get_reg(0xe40c, &origin_e40c);
	new_get_reg(0xe410, &origin_e410);
	new_get_reg(0xe014, &origin_e014);
	
    
	new_set_reg(0xe008, 0x00000000);//disable esti,tx,rx
	new_set_reg(0xb234, 0x00000001);//close open spectrum,must be closed in single tone mode
	new_set_reg(0xe018, 0x00010500);//calib mode:enable:1, mode:RX IRR 5

	if(rx_irr_manual == 0){
		new_set_reg(0xe82c, 0x00000a10);//set RXIRR comp bypass 1, manual mode to 0
	}else{
		new_set_reg(0xe82c, 0x00000a11);//set RXIRR comp bypass 1, manual mode to 1
	}
	
	new_set_reg(0xe4f0, 0x15008080);//set TX DPD bypass 1
	new_set_reg(0xe5b8, 0x00000008);//set digital gain
	i2c_set_reg_fragment(0xe4b8, 31, 24, 0x1a);//set txpwctrl gain
	i2c_set_reg_fragment(0xe4b8, 16, 16, 1);//set txpwctrl gain manual to 1

	new_set_reg(0xec2c, 0x10001100);//read response set 0; response bypass set 1
	//new_set_reg(0xe408, 0x00200000);//close tone1
	new_set_reg(0xe40c, 0x10200000);//open tone2
	new_set_reg(0xe410, 0x00000001);//1: select single tone 
	
	
	//i2c_set_reg_fragment(0xe488, 0, 0, 0);//set TX DC/IQ bypass to 0
	//i2c_set_reg_fragment(0xe488, 4, 4, 1);//set TX negate msb to 1
	//i2c_set_reg_fragment(0xe488, 8, 8, 0);//set TX IQ swap to 1
	
	if(tx_dciq_manual){
        i2c_set_reg_fragment(0xe4b4, 31, 31, 1);//set TX DC/IQ comp manual to 1
        

		dci   = calb_val->tx_dc_irr[chn_idx][3*gain_idx].tx_dc_i;
		dcq   = calb_val->tx_dc_irr[chn_idx][3*gain_idx].tx_dc_q;
		alpha = calb_val->tx_dc_irr[chn_idx][3*gain_idx].tx_alpha;
		theta = calb_val->tx_dc_irr[chn_idx][3*gain_idx].tx_theta;
        
		i2c_set_reg_fragment(0xe4b4,  7, 0, dci   & 0xff); //set TX DC_I
		i2c_set_reg_fragment(0xe4b4, 15, 8, dcq   & 0xff); //set TX DC_Q
		i2c_set_reg_fragment(0xe4b8,  7, 0, theta & 0xff); //set TX theta
		i2c_set_reg_fragment(0xe4b8, 15, 8, alpha & 0xff); //set TX alpha
	}

	if(rf_bw == RF_BW_20M){
		new_set_reg(0xe014, 0x00100000);//set RX IRR BW to 20M:b17,b16 for bw
		new_set_reg(0xec78, 0x00199999);// tone
		new_set_reg(0xec7c, 0x00399999);// tone
		new_set_reg(0xec80, 0x01c66666);// tone
		new_set_reg(0xec84, 0x01e66666);// tone
		new_set_reg(0xec88, 0x00000000);// tone
		new_set_reg(0xec8c, 0x00000000);// tone
		new_set_reg(0xec90, 0x00000000);// tone
		new_set_reg(0xec94, 0x00000000);// tone
		new_set_reg(0xec98, 0x2868b42f);
		new_set_reg(0xec9c, 0x00000000);
		new_set_reg(0xeca0, 0xd7984fd1);
		new_set_reg(0xeca4, 0x00000000);
		new_set_reg(0xeca8, 0x00000000);
		new_set_reg(0xecac, 0x00000000);
		new_set_reg(0xecb0, 0x00000000);
		new_set_reg(0xecb4, 0x00000000);
		new_set_reg(0xecb8, 0xd7984fd1);
		new_set_reg(0xecbc, 0x00000000);
		new_set_reg(0xecc0, 0x2868b42f);
		new_set_reg(0xecc4, 0x00000000);
		new_set_reg(0xecc8, 0x00000000);
		new_set_reg(0xeccc, 0x00000000);
		new_set_reg(0xecd0, 0x00000000);
		new_set_reg(0xecd4, 0x00000000);
	}else if(rf_bw == RF_BW_40M){
		new_set_reg(0xe014, 0x00110000);//set RX IRR BW to 40M
		new_set_reg(0xec78, 0x00266666);// tone
		new_set_reg(0xec7c, 0x00733333);// tone
		new_set_reg(0xec80, 0x018ccccc);// tone
		new_set_reg(0xec84, 0x01d99999);// tone
		new_set_reg(0xec88, 0x00000000);// tone
		new_set_reg(0xec8c, 0x00000000);// tone
		new_set_reg(0xec90, 0x00000000);// tone
		new_set_reg(0xec94, 0x00000000);// tone
		new_set_reg(0xec98, 0x0970f1a6);
		new_set_reg(0xec9c, 0x00000000);
		new_set_reg(0xeca0, 0xf690125a);
		new_set_reg(0xeca4, 0x00000000);
		new_set_reg(0xeca8, 0x00000000);
		new_set_reg(0xecac, 0x00000000);
		new_set_reg(0xecb0, 0x00000000);
		new_set_reg(0xecb4, 0x00000000);
		new_set_reg(0xecb8, 0xf690125a);
		new_set_reg(0xecbc, 0x00000000);
		new_set_reg(0xecc0, 0x0970f1a6);
		new_set_reg(0xecc4, 0x00000000);
		new_set_reg(0xecc8, 0x00000000);
		new_set_reg(0xeccc, 0x00000000);
		new_set_reg(0xecd0, 0x00000000);
		new_set_reg(0xecd4, 0x00000000);
	}else if(rf_bw == RF_BW_80M){
		new_set_reg(0xe014, 0x00120000);//set RX IRR BW to 80M
		new_set_reg(0xec78, 0x004ccccc);// tone
		new_set_reg(0xec7c, 0x00a66666);// tone
		new_set_reg(0xec80, 0x00d33333);// tone
		new_set_reg(0xec84, 0x00f99999);// tone
		new_set_reg(0xec88, 0x01066666);// tone
		new_set_reg(0xec8c, 0x012ccccc);// tone
		new_set_reg(0xec90, 0x01599999);// tone
		new_set_reg(0xec94, 0x01b33333);// tone
		new_set_reg(0xec98, 0x097ef751);
		new_set_reg(0xec9c, 0x0292fbb7);
		new_set_reg(0xeca0, 0xd196369c);
		new_set_reg(0xeca4, 0xf4d11ae1);
		new_set_reg(0xeca8, 0x4663ad86);
		new_set_reg(0xecac, 0x0dd8d7c6);
		new_set_reg(0xecb0, 0xde88288d);
		new_set_reg(0xecb4, 0xfac511a2);
		new_set_reg(0xecb8, 0xde88288d);
		new_set_reg(0xecbc, 0xfac511a2);
		new_set_reg(0xecc0, 0x4663ad86);
		new_set_reg(0xecc4, 0x0dd8d7c6);
		new_set_reg(0xecc8, 0xd196369c);
		new_set_reg(0xeccc, 0xf4d11ae1);
		new_set_reg(0xecd0, 0x097ef751);
		new_set_reg(0xecd4, 0x0292fbb7);
	}

	new_set_reg(0xe008, 0x11111100);//enable esti,tx,rx

       #ifdef RUN_PT
	    DUT_PRINT_B("Rx IRR done\r\n");
        #else
           printk("Rx IRR done\r\n");
        #endif
        
	new_get_reg(0xec2c, &data);
	rx_irr_rdy = (data >> 31) & 0x1;
    
	
	while(unlock_cnt < TIME_OUT_CNT){
		if(rx_irr_rdy == 0){
			new_get_reg(0xec2c, &data);

			rx_irr_rdy = (data >> 31) & 0x1;
                    #ifdef RUN_PT
			    TRACE("RX IRR NOT DOING...\n");
                    #else
                        printk("RX IRR NOT DOING...\n");
                    #endif
		}else{

                     #ifdef RUN_PT
            			TRACE("RX IRR READY\n");
                     #else
                          printk("RX IRR READY\n");
                     #endif
                     
			break;
		}
	}

	new_get_reg(0xec0c, &tmp);
	coeff_eup0 = tmp & 0x7fff;
	coeff_eup1 = (tmp>>16) & 0x7fff;

	new_get_reg(0xec10, &tmp);
	coeff_eup2 = tmp & 0x7fff;
	coeff_eup3 = (tmp>>16) & 0x7fff;

	new_get_reg(0xec14, &tmp);
	coeff_pup0 = tmp & 0x7fff;
	coeff_pup1 = (tmp>>16) & 0x7fff;

	new_get_reg(0xec18, &tmp);
	coeff_pup2 = tmp & 0x7fff;
	coeff_pup3 = (tmp>>16) & 0x7fff;

	new_get_reg(0xec1c, &tmp);
	coeff_elow0 = tmp & 0x7fff;
	coeff_elow1 = (tmp>>16) & 0x7fff;

	new_get_reg(0xec20, &tmp);
	coeff_elow2 = tmp & 0x7fff;
	coeff_elow3 = (tmp>>16) & 0x7fff;

	new_get_reg(0xec24, &tmp);
	coeff_plow0 = tmp & 0x7fff;
	coeff_plow1 = (tmp>>16) & 0x7fff;

	new_get_reg(0xec28, &tmp);
	coeff_plow2 = tmp & 0x7fff;
	coeff_plow3 = (tmp>>16) & 0x7fff;

	if(rx_irr_manual == 1){
		i2c_set_reg_fragment(0xe80c, 14,  0, coeff_eup0);
		i2c_set_reg_fragment(0xe80c, 30, 16, coeff_eup1);
		i2c_set_reg_fragment(0xe810, 14,  0, coeff_eup2);
		i2c_set_reg_fragment(0xe810, 30, 16, coeff_eup3);
		i2c_set_reg_fragment(0xe814, 14,  0, coeff_pup0);
		i2c_set_reg_fragment(0xe814, 30, 16, coeff_pup1);
		i2c_set_reg_fragment(0xe818, 14,  0, coeff_pup2);
		i2c_set_reg_fragment(0xe818, 30, 16, coeff_pup3);
		i2c_set_reg_fragment(0xe81c, 14,  0, coeff_elow0);
		i2c_set_reg_fragment(0xe81c, 30, 16, coeff_elow1);
		i2c_set_reg_fragment(0xe820, 14,  0, coeff_elow2);
		i2c_set_reg_fragment(0xe820, 30, 16, coeff_elow3);
		i2c_set_reg_fragment(0xe824, 14,  0, coeff_plow0);
		i2c_set_reg_fragment(0xe824, 30, 16, coeff_plow1);
		i2c_set_reg_fragment(0xe828, 14,  0, coeff_plow2);
		i2c_set_reg_fragment(0xe828, 30, 16, coeff_plow3);
	}

    #ifdef RUN_PT
	//TO OUTPUT
	TRACE("coeff_eup0:%x\ncoeff_eup1:%x\ncoeff_eup2:%x\ncoeff_eup3:%x\n", coeff_eup0, coeff_eup1, coeff_eup2, coeff_eup3);
	fprintf(fp, "coeff_eup0:%x\ncoeff_eup1:%x\ncoeff_eup2:%x\ncoeff_eup3:%x\n", coeff_eup0, coeff_eup1, coeff_eup2, coeff_eup3);

	TRACE("coeff_pup0:%x\ncoeff_pup1:%x\ncoeff_pup2:%x\ncoeff_pup3:%x\n", coeff_pup0, coeff_pup1, coeff_pup2, coeff_pup3);
	fprintf(fp, "coeff_pup0:%x\ncoeff_pup1:%x\ncoeff_pup2:%x\ncoeff_pup3:%x\n", coeff_pup0, coeff_pup1, coeff_pup2, coeff_pup3);

	TRACE("coeff_elow0:%x\ncoeff_elow1:%x\ncoeff_elow2:%x\ncoeff_elow3:%x\n", coeff_elow0, coeff_elow1, coeff_elow2, coeff_elow3);
	fprintf(fp,"coeff_elow0:%x\ncoeff_elow1:%x\ncoeff_elow2:%x\ncoeff_elow3:%x\n", coeff_elow0, coeff_elow1, coeff_elow2, coeff_elow3 );

	TRACE("coeff_plow0:%x\ncoeff_plow1:%x\ncoeff_plow2:%x\ncoeff_plow3:%x\n", coeff_plow0, coeff_plow1, coeff_plow2, coeff_plow3);
	fprintf(fp, "coeff_plow0:%x\ncoeff_plow1:%x\ncoeff_plow2:%x\ncoeff_plow3:%x\n", coeff_plow0, coeff_plow1, coeff_plow2, coeff_plow3);
     #endif
      
       new_set_reg(0xe008, 0x00111100);//disable esti; open tx,rx
    
	
	i2c_set_reg_fragment(0xe4b8, 31, 24, 0x80);//set txpwctrl gain to original
	i2c_set_reg_fragment(0xe4b8, 16, 16, 1);//set txpwctrl gain manual to 1
	
	if(rx_irr_manual == 0){
		new_set_reg(0xe82c, 0x00000a00);//set RXIRR comp bypass 0, manual mode to 0
	}else{
		new_set_reg(0xe82c, 0x00000a01);//set RXIRR comp bypass 0, manual mode to 1
	}
	new_set_reg(0xe40c, 0x00200000);//close tone2

     #ifdef RUN_PT
	TRACE("RX IRR Calib finish\n");
     #else
       printk("RX IRR Calib finish\n");
     #endif
     
	//set RF reg to initial value
	if(1){
		new_set_reg(0xff000008,origin_rg008);
		new_set_reg(0xff00001c,origin_rg01c);
		new_set_reg(0xff000164,origin_rg164);
		new_set_reg(0xff000200,origin_rg200);
		new_set_reg(0xff000204,origin_rg204);
		new_set_reg(0xff000208,origin_rg208);
		//new_set_reg(0xff00020c,origin_rg20c);
		new_set_reg(0xff000308,origin_rg308);
		new_set_reg(0xff000314,origin_rg314);
		//new_set_reg(0xff0003f0,origin_rg3f0);
		new_set_reg(0xff0003e0,origin_rg3e0);
		new_set_reg(0xff0003e8,origin_rg3e8);
	}
	//set digital reg to initial vaule
	if(1){
		new_set_reg(0xe008, origin_e008);
		new_set_reg(0xb234, origin_b234);
		new_set_reg(0xe018, origin_e018);
		new_set_reg(0xe4f0, origin_e4f0);
		new_set_reg(0xe5b8, origin_e5b8);
		new_set_reg(0xe4b8, origin_e4b8);
		new_set_reg(0xe40c, origin_e40c);
		new_set_reg(0xe410, origin_e410);
		new_set_reg(0xe014, origin_e014);
        //i2c_set_reg_fragment(0xe82c, 4, 4, origin_e82c&0x1);//notice:must be not to orginal value as for DPD delay may change
		new_set_reg(0xec2c, origin_ec2c);
	}
	
    #ifdef RUN_PT
	fclose(fp);
    #endif
    
}



#define RF_BW_20M (0)
#define RF_BW_40M (1)
#define RF_BW_80M (2)

void t9023_stop_ofdm(void)
{
      #ifdef RUN_PT
	char* cmd_buf;
	cmd_buf="iw dev wlan0 vendor send 0xc3 0xc4 0x08 0x08\r\n";//stop
	new_uart_send_cmd(cmd_buf);
	Sleep(50);
	new_uart_rcv();
      #else
          struct hal_private *hal_priv = hal_get_priv();
      
             if(hal_priv->hi_dpd_tx_thread )
            {
                init_completion(&hal_priv->hi_dpd_tx_thread_completion);
                hal_priv->hi_dpd_tx_thread_quit = 1;
                kthread_stop(hal_priv->hi_dpd_tx_thread);
                wait_for_completion(&hal_priv->hi_dpd_tx_thread_completion);
                hal_priv->hi_dpd_tx_thread =0;

                 printk("stop dpd thread...\n");
            }

            printk("TX DPD DONE\n");
      #endif
      
}

void t9023_tx_ofdm(unsigned int mode, unsigned int bw_idx, unsigned int rate, 
				   unsigned int tx_num, unsigned int tx_len)
{
      #ifdef RUN_PT
       char* cmd_buf[9];
	unsigned int bw[] = {RF_BW_20M, RF_BW_40M, RF_BW_80M};
	memset(cmd_buf, 0, sizeof(cmd_buf));

	unsigned int rf_bw = 0;
	rf_bw = bw[bw_idx];
    
	cmd_buf[0]="iw dev wlan0 vendor send 0xc3 0xc4 0x08 0x08\r\n";//stop
	cmd_buf[1]="iw dev wlan0 set bitrates vht-mcs-2.4 1:0\r\n"; // rate

	if(rf_bw == RF_BW_20M){
		cmd_buf[2]="iw dev wlan0 vendor send 0xc3 0xc4 0x07 0x0\r\n"; //bw
		cmd_buf[3]="sh rw.sh 0x0000b22c 0x80000000\r\n"; // bbp bw 	
	}else if(rf_bw == RF_BW_40M){
		cmd_buf[2]="iw dev wlan0 vendor send 0xc3 0xc4 0x07 0x1\r\n"; //bw
		cmd_buf[3]="sh rw.sh 0x0000b22c 0x90000000\r\n"; // bbp bw 	
	}else if(rf_bw == RF_BW_80M){
		cmd_buf[2]="iw dev wlan0 vendor send 0xc3 0xc4 0x07 0x2\r\n"; //bw
		cmd_buf[3]="sh rw.sh 0x0000b22c 0xa0000000\r\n"; // bbp bw 	
	}

	cmd_buf[4]="iw dev wlan0 vendor send 0xc3 0xc4 0x06 0x7f 0xff 0xff 0xff\r\n"; // packet num
	cmd_buf[5]="iw dev wlan0 vendor send 0xc3 0xc4 0x13 0x01\r\n";//si
	cmd_buf[6]="iw dev wlan0 vendor send 0xc3 0xc4 0x12\r\n";//init b2b
	cmd_buf[7]="iw dev wlan0 vendor send 0xc3 0xc4 0x15 0x10 0x00\r\n";//packet length=10
	
	cmd_buf[8]="iw dev wlan0 vendor send 0xc3 0xc4 0x08 0x02\r\n"; // start
	

	for(int i = 0; i < sizeof(cmd_buf)/sizeof(cmd_buf[0]); i++)
	{
		new_uart_send_cmd(cmd_buf[i]);
		Sleep(50);
		new_uart_rcv();
	}
    #else
      struct hal_private *hal_priv = hal_get_priv();
     printk("start dpd thread...\n");
     hal_priv->hi_dpd_tx_thread = kthread_run(t9023_dpd_tx_func, hal_priv, "t9023_dpd_tx_thread");
          
    #endif
    
}


void do_tx_dpd_calb(struct calb_rst_out* calb_val, 
					bool tx_dciq_manual,
					unsigned int chn_idx,
					unsigned int bw_idx,
					unsigned int gain_idx)
{
    //TX DPD
	//RF register
	//original

	//digital
       unsigned int sync_gap = 0;
       unsigned int data = 0;
	unsigned int snr = 0;
	unsigned int tx_dpd_ready = 0;
      unsigned int unlock_cnt = 0;
	unsigned int TIME_OUT_CNT = 300;

	unsigned int coeff_I[15];
	unsigned int coeff_Q[15];
	unsigned int ck = 0;

#ifdef RUN_PT
       float temp_I = 0;//0.0000;
	float temp_Q = 0;//0.0000;
#endif
	
	unsigned int origin_008 = 0;
	unsigned int origin_01c = 0;
	unsigned int origin_164 = 0;
	unsigned int origin_200 = 0;
	unsigned int origin_204 = 0;
	unsigned int origin_208 = 0;
	unsigned int origin_308 = 0;
	unsigned int origin_314 = 0;
	unsigned int origin_3e0 = 0;
	unsigned int origin_3e8 = 0;

	//digital
    unsigned int origin_b22c;
    unsigned int origin_e008;
	unsigned int origin_e018;
	unsigned int origin_e404;
	unsigned int origin_e410;
	unsigned int origin_e4b8;
	unsigned int origin_e4f0;
	unsigned int origin_ed18;
	unsigned int origin_ed58;
	unsigned int origin_ed00;
	unsigned int origin_ed04;
	unsigned int origin_ed08;
	unsigned int origin_ed60;

	unsigned int dci = 0;
	unsigned int dcq = 0;	
	unsigned int alpha = 0;
	unsigned int theta = 0;
       int start_point;
	int end_point;

	unsigned int bw_val[] = {RF_BW_20M, RF_BW_40M, RF_BW_80M};

	unsigned int rf_bw = bw_val[bw_idx];

	bool tx_dpd_manual = 0;

       #ifdef RUN_PT
        	char path[1024] = CALB_OUTPUT_PATH ;
        	FILE *fp = NULL;

        	sprintf(path,"%s%s",g_CurrentPath,CALB_OUTPUT_PATH );
        	
        	fp = fopen((char *)path, "a+");
        	if (!fp){
        		TRACE(" output file open fail\r\n");
        		return ;
        	}

        	fprintf(fp, "dpd log........\n");
       #else
        	printk( "dpd log........\n");   
       #endif
    
	if(rf_bw == RF_BW_20M){
		if(DPD_LOOP_IRR_EN == 1)
            sync_gap = 81;
		else
			sync_gap = 57+1;
	}else if(rf_bw == RF_BW_40M){
		if(DPD_LOOP_IRR_EN == 1)
            sync_gap = 81;
		else
			sync_gap = 57+1;
	}else if(rf_bw == RF_BW_80M){
		if(DPD_LOOP_IRR_EN == 1)
            sync_gap = 79+1;
		else
			sync_gap = 55+1;
	}

	new_get_reg(0xff000008,&origin_008);
	new_get_reg(0xff00001c,&origin_01c);
	new_get_reg(0xff000164,&origin_164);
	new_get_reg(0xff000200,&origin_200);
	new_get_reg(0xff000204,&origin_204);
	new_get_reg(0xff000208,&origin_208);
	//new_get_reg(0xff00020c,&origin_20c);
	new_get_reg(0xff000308,&origin_308);
	new_get_reg(0xff000314,&origin_314);
	//new_get_reg(0xff0003f0,&origin_3f0);
	new_get_reg(0xff0003e0,&origin_3e0);
	new_get_reg(0xff0003e8,&origin_3e8);
    
	new_set_reg(0xff00001c,0x000000ff);//enable RX LDO in mode2
    new_set_reg(0xff000164,0x00001bff);//enable DPD LO in mode2
    
	new_set_reg(0xff000200,0x03639000);//tx dcoc i/q,txm_ct(default)
	new_set_reg(0xff000204,0x19fc01c0);//tx adb on:19fc01c0
	new_set_reg(0xff000208,0x00110360);//tx dcoc i/q,txm_ct(default)
	//new_set_reg(0xff00020c,0x00001077);//tx v2i.mixr pa gain(default)
    
	new_set_reg(0xff000308,0x00017b1d);// 0x00017c1d, FE+LPF gain, BW:160M, ADC 320M
	//new_set_reg(0xff000308,0x00019c1d);// FE+LPF gain, BW:160M, ADC 320M
	
	
	new_set_reg(0xff000314,0x03ffe619);// RX mode2 control
	//new_set_reg(0xff0003f0,0x00003508);// tx lpf gain
    new_set_reg(0xff0003e0,0x01abb7fd);// GM/dpd atten
	new_set_reg(0xff0003e8,0x00000006);// close ADC notch filter

	i2c_set_reg_fragment(0xff000008,1,0,2);// set tx mode to enable dpd loop
	i2c_set_reg_fragment(0xff000100,0,0,0);// triger sx freq lock
	i2c_set_reg_fragment(0xff000100,0,0,1);
	//i2c_set_reg_fragment(0xff000008,1,0,2);

    
	
	
	
	new_get_reg(0xb22c, &origin_b22c);
	new_get_reg(0xe008, &origin_e008);
	new_get_reg(0xe018, &origin_e018);
	new_get_reg(0xe404, &origin_e404);
	new_get_reg(0xe410, &origin_e410);
	new_get_reg(0xe4b8, &origin_e4b8);
	new_get_reg(0xe4f0, &origin_e4f0);
	new_get_reg(0xed18, &origin_ed18);
	new_get_reg(0xed58, &origin_ed58);
	new_get_reg(0xed00, &origin_ed00);
	new_get_reg(0xed04, &origin_ed04);
	new_get_reg(0xed08, &origin_ed08);
	new_get_reg(0xed60, &origin_ed60);
	
    
    //send ofdm packet
	t9023_tx_ofdm(2,bw_idx,0x80,1000000,1000);

       #ifdef RUN_PT
         Sleep(1000);
	#else
         msleep(1000);
    	#endif
	
       start_point = 50;
	end_point = 70;

	//for(sync_gap = start_point; sync_gap < end_point; sync_gap ++)
	{
		new_set_reg(0xe008, 0x00000000); //disable esti,tx,rx
		new_set_reg(0xe018, 0x00010300); //calib mode:enable:1, mode:TX DPD
		new_set_reg(0xe404, 0x00000003); //enalbe calc gain from packet head

		i2c_set_reg_fragment(0x8010, 28, 28, 1);//disable agc cca detect

		//i2c_set_reg_fragment(0xb234, 0, 0, 0);//open spectrum filter
		
		//new_set_reg(0xe488,0x000a0010); //tx IQ swap(bit8)
		//new_set_reg(0xe808,0x01100002); //rx IQ swap(bit1)

		if(tx_dciq_manual){
			i2c_set_reg_fragment(0xe4b4, 31, 31, 1);//set TX DC/IQ comp manual to 1
		

			dci   = calb_val->tx_dc_irr[chn_idx][3*gain_idx].tx_dc_i;
			dcq   = calb_val->tx_dc_irr[chn_idx][3*gain_idx].tx_dc_q;
			alpha = calb_val->tx_dc_irr[chn_idx][3*gain_idx].tx_alpha;
			theta = calb_val->tx_dc_irr[chn_idx][3*gain_idx].tx_theta;

			i2c_set_reg_fragment(0xe4b4,  7, 0, dci   & 0xff); //set TX DC_I
			i2c_set_reg_fragment(0xe4b4, 15, 8, dcq   & 0xff); //set TX DC_Q
			i2c_set_reg_fragment(0xe4b8,  7, 0, theta & 0xff); //set TX theta
			i2c_set_reg_fragment(0xe4b8, 15, 8, alpha & 0xff); //set TX alpha
		}

		new_set_reg(0xe410, 0x00000000); //0:select OFDM signal;1:signal tone 

		if(rf_bw == RF_BW_20M){
			i2c_set_reg_fragment(0xe4b8, 31, 24, 0xb0); //set pwtrl gain
		}else if(rf_bw == RF_BW_40M){
			i2c_set_reg_fragment(0xe4b8, 31, 24, 0xb0); //set pwtrl gain
		}else if(rf_bw == RF_BW_80M){
			i2c_set_reg_fragment(0xe4b8, 31, 24, 0x90); //set pwtrl gain
		}

		i2c_set_reg_fragment(0xe4f0, 7, 0, 0x80);//dpd in gain 
		i2c_set_reg_fragment(0xe4f0, 24, 24, 1);//set dpd comp type to 1:coeff
		i2c_set_reg_fragment(0xe4f0, 26, 26, 1);//set dpd comp bypass to 1
		i2c_set_reg_fragment(0xe4f0, 28, 28, 1);//set dpd comp bypass man to 1
		if (tx_dpd_manual == 0)
			i2c_set_reg_fragment(0xe4f0, 31, 31, 0);//set dpd comp manual mode to 0
		else
			i2c_set_reg_fragment(0xe4f0, 31, 31, 1);//set dpd comp manual mode to 1
	    
		i2c_set_reg_fragment(0xed18,  0,  0, 0);//set dpd esti response to 0
		i2c_set_reg_fragment(0xed18,  2,  2, 0);//set dpd esti response bypass to 0
		i2c_set_reg_fragment(0xed18, 11,  4, 0x01);//set dpd esti alpa
		i2c_set_reg_fragment(0xed18, 31, 24, 0x06);//set dpd pow threshold
		i2c_set_reg_fragment(0xed18, 18, 12, sync_gap&0x7f);//set dpd coeff esti sync gap

		i2c_set_reg_fragment(0xed58, 18, 16, 4);//set dpd snr esti alpha
		
		new_set_reg(0xed00, 0x000000d2); //set dpd wait count
		new_set_reg(0xed04, 0x00001000); //set dpd calc count
		new_set_reg(0xed08, 0x0fffffff); //set dpd esti count

		new_set_reg(0xe008, 0x11111100); //enable esti,tx,rx

		
		new_get_reg(0xed18, &data);//read data after DPD done
		
		tx_dpd_ready = (data >> 22) & 0x1;
	   		
		while(unlock_cnt < TIME_OUT_CNT){
			if(tx_dpd_ready == 0){
				new_get_reg(0xed18, &data);
		
				tx_dpd_ready = (data >> 22) & 0x1;
	            
				unlock_cnt = unlock_cnt + 1;

                           #ifdef RUN_PT
				    TRACE("unlock_cnt:%d, TX DPD NOT DOING...\n", unlock_cnt);
                           #else
                               //printk("unlock_cnt:%d, TX DPD NOT DOING...\n", unlock_cnt);
                           #endif
			}else{
			       #ifdef RUN_PT 
				    TRACE("TX DPD FINISH\n");
                            #else
                               printk("TX DPD FINISH\n");
                            #endif
                            
				break;
			}
		}
	    
		new_get_reg(0xed60, &data);
		snr = data & 0x1ff; 
		
		if(snr > 256){
			snr = snr - 512;
		}

             #ifdef RUN_PT
		    TRACE("snr: %d,  sync_gap:%d\n", snr, sync_gap);
             #else
                 printk("snr: %d,  sync_gap:%d\n", snr, sync_gap);
             #endif
		//new_set_reg(0xe488,0x000a0010); //tx IQ swap(bit8)

		if(1){//print coeff
			coeff_I[0]  = i2c_get_reg_fragment(0xed1c, 11, 0);
			coeff_Q[0]  = i2c_get_reg_fragment(0xed1c, 23, 12);
			coeff_I[1]  = i2c_get_reg_fragment(0xed20, 11, 0);
			coeff_Q[1]  = i2c_get_reg_fragment(0xed20, 23, 12);
			coeff_I[2]  = i2c_get_reg_fragment(0xed24, 11, 0);
			coeff_Q[2]  = i2c_get_reg_fragment(0xed24, 23, 12);
			coeff_I[3]  = i2c_get_reg_fragment(0xed28, 11, 0);
			coeff_Q[3]  = i2c_get_reg_fragment(0xed28, 23, 12);
			coeff_I[4]  = i2c_get_reg_fragment(0xed2c, 11, 0);
			coeff_Q[4]  = i2c_get_reg_fragment(0xed2c, 23, 12);
			coeff_I[5]  = i2c_get_reg_fragment(0xed30, 11, 0);
			coeff_Q[5]  = i2c_get_reg_fragment(0xed30, 23, 12);
			coeff_I[6]  = i2c_get_reg_fragment(0xed34, 11, 0);
			coeff_Q[6]  = i2c_get_reg_fragment(0xed34, 23, 12);
			coeff_I[7]  = i2c_get_reg_fragment(0xed38, 11, 0);
			coeff_Q[7]  = i2c_get_reg_fragment(0xed38, 23, 12);
			coeff_I[8]  = i2c_get_reg_fragment(0xed3c, 11, 0);
			coeff_Q[8]  = i2c_get_reg_fragment(0xed3c, 23, 12);
			coeff_I[9]  = i2c_get_reg_fragment(0xed40, 11, 0);
			coeff_Q[9]  = i2c_get_reg_fragment(0xed40, 23, 12);
			coeff_I[10] = i2c_get_reg_fragment(0xed44, 11, 0);
			coeff_Q[10] = i2c_get_reg_fragment(0xed44, 23, 12);
			coeff_I[11] = i2c_get_reg_fragment(0xed48, 11, 0);
			coeff_Q[11] = i2c_get_reg_fragment(0xed48, 23, 12);
			coeff_I[12] = i2c_get_reg_fragment(0xed4c, 11, 0);
			coeff_Q[12] = i2c_get_reg_fragment(0xed4c, 23, 12);
			coeff_I[13] = i2c_get_reg_fragment(0xed50, 11, 0);
			coeff_Q[13] = i2c_get_reg_fragment(0xed50, 23, 12);
			coeff_I[14] = i2c_get_reg_fragment(0xed54, 11, 0);
			coeff_Q[14] = i2c_get_reg_fragment(0xed54, 23, 12);

			i2c_set_reg_fragment(0xe4f4, 11,  0, coeff_I[0]);
			i2c_set_reg_fragment(0xe4f4, 27, 16, coeff_Q[0]);
			i2c_set_reg_fragment(0xe4f8, 11,  0, coeff_I[1]);
			i2c_set_reg_fragment(0xe4f8, 27, 16, coeff_Q[1]);
			i2c_set_reg_fragment(0xe4fc, 11,  0, coeff_I[2]);
			i2c_set_reg_fragment(0xe4fc, 27, 16, coeff_Q[2]);
			i2c_set_reg_fragment(0xe500, 11,  0, coeff_I[3]);
			i2c_set_reg_fragment(0xe500, 27, 16, coeff_Q[3]);
			i2c_set_reg_fragment(0xe504, 11,  0, coeff_I[4]);
			i2c_set_reg_fragment(0xe504, 27, 16, coeff_Q[4]);
			i2c_set_reg_fragment(0xe508, 11,  0, coeff_I[5]);
			i2c_set_reg_fragment(0xe508, 27, 16, coeff_Q[5]);
			i2c_set_reg_fragment(0xe50c, 11,  0, coeff_I[6]);
			i2c_set_reg_fragment(0xe50c, 27, 16, coeff_Q[6]);
			i2c_set_reg_fragment(0xe510, 11,  0, coeff_I[7]);
			i2c_set_reg_fragment(0xe510, 27, 16, coeff_Q[7]);
			i2c_set_reg_fragment(0xe514, 11,  0, coeff_I[8]);
			i2c_set_reg_fragment(0xe514, 27, 16, coeff_Q[8]);
			i2c_set_reg_fragment(0xe518, 11,  0, coeff_I[9]);
			i2c_set_reg_fragment(0xe518, 27, 16, coeff_Q[9]);
			i2c_set_reg_fragment(0xe51c, 11,  0, coeff_I[10]);
			i2c_set_reg_fragment(0xe51c, 27, 16, coeff_Q[10]);
			i2c_set_reg_fragment(0xe520, 11,  0, coeff_I[11]);
			i2c_set_reg_fragment(0xe520, 27, 16, coeff_Q[11]);
			i2c_set_reg_fragment(0xe524, 11,  0, coeff_I[12]);
			i2c_set_reg_fragment(0xe524, 27, 16, coeff_Q[12]);
			i2c_set_reg_fragment(0xe528, 11,  0, coeff_I[13]);
			i2c_set_reg_fragment(0xe528, 27, 16, coeff_Q[13]);
			i2c_set_reg_fragment(0xe52c, 11,  0, coeff_I[14]);
			i2c_set_reg_fragment(0xe52c, 27, 16, coeff_Q[14]);


                    for(ck = 0; ck < 15; ck++){
                           calb_val->tx_dpd[chn_idx][bw_idx][gain_idx].tx_dpd_coeff_i[ck] = coeff_I[ck];
				calb_val->tx_dpd[chn_idx][bw_idx][gain_idx].tx_dpd_coeff_q[ck] = coeff_Q[ck];
                    }
                    
                    calb_val->tx_dpd[chn_idx][bw_idx][gain_idx].snr = snr;

 #ifdef RUN_PT
			for(ck = 0; ck < 15; ck++){
				if(coeff_I[ck] > (1<<11)){
					temp_I = (float)((float)coeff_I[ck] -(float)(1<<12));
				}else{
					temp_I =(float) (coeff_I[ck]);
				}

				temp_I = temp_I / ( (float)(1 <<10));

				if(coeff_Q[ck] > (1<<11)){
					temp_Q = (float)coeff_Q[ck] - (float)(1<<12);
				}else{
					temp_Q = (float)coeff_Q[ck];
				}

				temp_Q = temp_Q / (float)(1 <<10);
                          
    				fprintf(fp, "chn[%d] bw[%d] gain[%d] coeff_i %f coeff_q %f\n",
    							 chn_idx,bw_idx,gain_idx,temp_I,temp_Q);
    				
    		              TRACE("%f, %f\n", temp_I, temp_Q);
                        
			}//end for(ck = 0; ck < 15; ck++)
	 
#endif		
			
		}//end if() //print coeff

             #ifdef RUN_PT
		    fprintf(fp, "snr: %d\n", snr);
             #else
                printk("snr: %d\n", snr);
             #endif
             
	    i2c_set_reg_fragment(0xe4f0, 26, 26, 0);//set dpd comp bypass to 0
	    new_set_reg(0xe008,0x00111100); //disable esti, enable tx, rx
	
    }//for(sync_gap = start_point; sync_gap < end_point; sync_gap ++)
	
	//i2c_set_reg_fragment(0xb234,0, 0, 0);//open spectrum filter
	i2c_set_reg_fragment(0x8010, 28, 28, 0);//enable agc cca detect
	
	//set RF reg to initial value
     if(1){
		new_set_reg(0xff000008,origin_008);
	    new_set_reg(0xff00001c,origin_01c);
	    new_set_reg(0xff000164,origin_164);
	    //new_set_reg(0xff000200,origin_200);
		//new_set_reg(0xff000204,origin_204);
		//new_set_reg(0xff000208,origin_208);
		//new_set_reg(0xff00020c,origin_20c);
		
		new_set_reg(0xff000308,origin_308);
		new_set_reg(0xff000314,origin_314);
		//new_set_reg(0xff0003f0,origin_3f0);
		new_set_reg(0xff0003e0,origin_3e0);
		new_set_reg(0xff0003e8,origin_3e8);
	}
	//set digital reg to inital value
	if(1){
		new_set_reg(0xb22c, origin_b22c);
		new_set_reg(0xe008, origin_e008);
		new_set_reg(0xe018, origin_e018);
		new_set_reg(0xe404, origin_e404);
		new_set_reg(0xe410, origin_e410);
		new_set_reg(0xe4b8, origin_e4b8);
		new_set_reg(0xe4f0, origin_e4f0);
		new_set_reg(0xed18, origin_ed18);
		new_set_reg(0xed58, origin_ed58);
		new_set_reg(0xed00, origin_ed00);
		new_set_reg(0xed04, origin_ed04);
		new_set_reg(0xed08, origin_ed08);
		new_set_reg(0xed60, origin_ed60);
	}
    
    //stop ofdm
	t9023_stop_ofdm();
    #ifdef RUN_PT
	TRACE("TX DPD calib finish \n");
	fclose(fp);
    #endif
}



struct rx_lna_output1 rx_lna_out1[1024];
struct rx_lna_output2 rx_lna_out2[1024];
unsigned int out_cnt1 = 0;
unsigned int out_cnt2 = 0;


void do_rx_lna_calb(struct calb_rst_out* calb_val, 
					bool tx_dciq_manual,
					unsigned int chn,
					unsigned int chn_idx)
 {
	
       #ifdef RUN_PT
        int i = 0;
        char path[1024] = CALB_OUTPUT_PATH ;
        FILE *fp = NULL;
        sprintf(path,"%s%s",g_CurrentPath,CALB_OUTPUT_PATH );
        fp = fopen((char *)path, "a+");
        if (!fp) 
        {
            TRACE(" output file open fail\r\n");
        	return ;
        }
	
	    fprintf(fp, "rxlna log........\n");
       #else
            printk("rxlna log........\n");
       #endif

	TestRxLNA_417(calb_val,tx_dciq_manual,chn,chn_idx,
		          rx_lna_out1,&out_cnt1, rx_lna_out2, &out_cnt2);

      #ifdef RUN_PT
        	for(i = 0; i < out_cnt1; i++)
        	{
        	  fprintf(fp, "code:%.2d,  pwr %lld\n",rx_lna_out1[i].code, rx_lna_out1[i].pwr);
        	}

        	for(i = 0; i < out_cnt2; i++)
        	{
        	   fprintf(fp,"chn_idx:%d, RXLNA code out %d\n",rx_lna_out2[i].chn_idx, rx_lna_out2[i].code_out);
        	}

        	fclose(fp);
        	TRACE("Test RX LNA finish \n");	
	#else
            printk("Test RX LNA finish \n");	
       #endif
}


#define RF_BW_20M (0)
#define RF_BW_40M (1)
#define RF_BW_80M (2)

void do_rx_irr_calb(struct calb_rst_out* calb_val,
					bool tx_dciq_manual,
					unsigned chn_idx,
					unsigned int bw_idx)
{
	
	unsigned rf_bw = 0;
	unsigned int origin_rg008 = 0;
	unsigned int origin_rg01c = 0;
	unsigned int origin_rg164 = 0;
	unsigned int origin_rg208 = 0;
	unsigned int origin_rg308 = 0;
	unsigned int origin_rg314 = 0;
	unsigned int origin_rg320 = 0;
    unsigned int origin_rg3f0 = 0;

	//digital
    unsigned int origin_e008;
	unsigned int origin_b22c;
	unsigned int origin_b234;
	unsigned int origin_e018;
	unsigned int origin_e82c;
	unsigned int origin_e4f0;
	unsigned int origin_e5b8;
	unsigned int origin_e4b8;
	unsigned int origin_ec2c;
	unsigned int origin_e40c;
	unsigned int origin_e410;
	unsigned int origin_e014;

	unsigned int unlock_cnt = 0;
	unsigned int TIME_OUT_CNT = 300;

	unsigned int data;
	unsigned int rx_irr_rdy;
	unsigned int coeff_eup0;
	unsigned int coeff_eup1;
	unsigned int coeff_eup2;
	unsigned int coeff_eup3;
	unsigned int coeff_pup0;
	unsigned int coeff_pup1;
	unsigned int coeff_pup2;
	unsigned int coeff_pup3;
	unsigned int coeff_elow0;
	unsigned int coeff_elow1;
	unsigned int coeff_elow2;
	unsigned int coeff_elow3;
	unsigned int coeff_plow0;
	unsigned int coeff_plow1;
	unsigned int coeff_plow2;
	unsigned int coeff_plow3;
	unsigned int tmp;

	unsigned int rx_irr_bw[]= {0,1,2};
	unsigned int rx_irr_manual = 0;

	rf_bw = rx_irr_bw[bw_idx];

       #ifdef RUN_PT
               char path[1024] = CALB_OUTPUT_PATH ;
        	FILE *fp = NULL;
        	
        	sprintf(path,"%s%s",g_CurrentPath,CALB_OUTPUT_PATH );
        	
        	fp = fopen((char *)path, "a+");
        	if (!fp){
        		TRACE(" output file open fail\r\n");
        		return ;
        	}
        	fprintf(fp, "rxirr log........\n");
       #else
             printk( "rxirr log........\n");
       #endif
       
	new_get_reg(0xff000008, &origin_rg008);
	new_get_reg(0xff00001c, &origin_rg01c);
	new_get_reg(0xff000164, &origin_rg164);
	new_get_reg(0xff000208, &origin_rg208);
	new_get_reg(0xff000308, &origin_rg308);
	new_get_reg(0xff000314, &origin_rg314);
	new_get_reg(0xff000320, &origin_rg320);
	new_get_reg(0xff0003f0, &origin_rg3f0);

	//Rx IRR
	i2c_set_reg_fragment(0xff00001c,  7,  0, 0xff);
	i2c_set_reg_fragment(0xff000164, 10, 10, 1);
	i2c_set_reg_fragment(0xff000208, 28, 28, 1);

	i2c_set_reg_fragment(0xff000308,  0,  0, 1);//set man mode to 1
	i2c_set_reg_fragment(0xff000308, 15,  8, 0x25);//0x26
	i2c_set_reg_fragment(0xff000308, 16, 16, 1);//fix rx fe+lpf gain

	if(rf_bw == RF_BW_20M){
		i2c_set_reg_fragment(0xff000308,3,2,RF_BW_20M);
		i2c_set_reg_fragment(0xff000308,4,4,0);// adc 160M
	}else if(rf_bw == RF_BW_40M){
	   i2c_set_reg_fragment(0xff000308,3,2,RF_BW_40M);
	   i2c_set_reg_fragment(0xff000308,4,4,0);//adc 160M
	}else if(rf_bw == RF_BW_80M){
		i2c_set_reg_fragment(0xff000308,3,2,RF_BW_80M);
		i2c_set_reg_fragment(0xff000308,4,4,1);//adc 320
	}

	i2c_set_reg_fragment(0xff000314, 31, 0, 0x03fff9d7);
	
	//new_set_reg(0xff000320, 0x00551c23);//RX LNA man set to 12 for 5050
	
	i2c_set_reg_fragment(0xff000008, 1, 0, 2);//set mode
	i2c_set_reg_fragment(0xff000100, 0, 0, 0);// triger sx freq lock
	i2c_set_reg_fragment(0xff000100, 0, 0, 1);
   
	

	new_get_reg(0xe008, &origin_e008);
	new_get_reg(0xb22c, &origin_b22c);
	new_get_reg(0xb234, &origin_b234);
	new_get_reg(0xe018, &origin_e018);
	origin_e82c=i2c_get_reg_fragment(0xe82c, 4, 4);
	new_get_reg(0xe4f0, &origin_e4f0);
	new_get_reg(0xe5b8, &origin_e5b8);
	new_get_reg(0xe4b8, &origin_e4b8);
	new_get_reg(0xec2c, &origin_ec2c);
	new_get_reg(0xe40c, &origin_e40c);
	new_get_reg(0xe410, &origin_e410);
	new_get_reg(0xe014, &origin_e014);

	new_set_reg(0xe008, 0x00000000);//disable esti,tx,rx
	new_set_reg(0xb234, 0x00000001);//notice:close open spectrum,must be closed in single tone mode
	new_set_reg(0xe018, 0x00010500);//calib mode:enable:1, mode:RX IRR 5
	
	if(rx_irr_manual == 0){
		new_set_reg(0xe82c, 0x00000a10);//set RXIRR comp bypass 1, manual mode to 0
	}else{
		new_set_reg(0xe82c, 0x00000a11);//set RXIRR comp bypass 1, manual mode to 1
	}

	new_set_reg(0xe4f0, 0x15008080);//set TX DPD bypass 1
	
    if(tx_dciq_manual){
        i2c_set_reg_fragment(0xe4b4, 31, 31, 1);//set TX DC/IQ comp manual to 1

        i2c_set_reg_fragment(0xe4b4,  7,  0, calb_val->tx_dc_irr[chn_idx][0].tx_dc_i);//set TX DCI    using tx dc/iq gain:773(idx:0)
		i2c_set_reg_fragment(0xe4b4, 15,  8, calb_val->tx_dc_irr[chn_idx][0].tx_dc_q);//set TX DCQ
		i2c_set_reg_fragment(0xe4b8,  7,  0, calb_val->tx_dc_irr[chn_idx][0].tx_theta);//set TX theta
        i2c_set_reg_fragment(0xe4b8, 15,  8, calb_val->tx_dc_irr[chn_idx][0].tx_alpha);//set TX alpha
	}

	new_set_reg(0xe5b8, 0x00000008);//set digital gain
	i2c_set_reg_fragment(0xe4b8, 31, 24, 0x10);//set txpwctrl gain
	i2c_set_reg_fragment(0xe4b8, 16, 16, 1);//set txpwctrl gain manual to 1

	new_set_reg(0xec2c, 0x10001100);//read response set 0; response bypass set 1
	//new_set_reg(0xe408, 0x00200000);//close tone1
	new_set_reg(0xe40c, 0x10200000);//open tone2
	new_set_reg(0xe410, 0x00000001);//1: select single tone 
	//new_set_reg(0xe808, 0x01100002);//set RX IQ swap to 1
	
	//i2c_set_reg_fragment(0xe488, 0, 0, 0);//set TX DC/IQ comp bypass to 0
	//i2c_set_reg_fragment(0xe488, 4, 4, 1);//set TX negate msb to 1
	//i2c_set_reg_fragment(0xe488, 8, 8, 0);//set TX IQ swap to 1
	//new_set_reg(0xe5b8, 0x00000002);//set TX atten factor to 2

	if(rf_bw == RF_BW_20M){
		new_set_reg(0xe014, 0x00100000);//set RX IRR BW to 20M:b17,b16 for bw
		new_set_reg(0xec78, 0x00199999);// tone
		new_set_reg(0xec7c, 0x00399999);// tone
		new_set_reg(0xec80, 0x01c66666);// tone
		new_set_reg(0xec84, 0x01e66666);// tone
		new_set_reg(0xec88, 0x00000000);// tone
		new_set_reg(0xec8c, 0x00000000);// tone
		new_set_reg(0xec90, 0x00000000);// tone
		new_set_reg(0xec94, 0x00000000);// tone
		new_set_reg(0xec98, 0x2868b42f);
		new_set_reg(0xec9c, 0x00000000);
		new_set_reg(0xeca0, 0xd7984fd1);
		new_set_reg(0xeca4, 0x00000000);
		new_set_reg(0xeca8, 0x00000000);
		new_set_reg(0xecac, 0x00000000);
		new_set_reg(0xecb0, 0x00000000);
		new_set_reg(0xecb4, 0x00000000);
		new_set_reg(0xecb8, 0xd7984fd1);
		new_set_reg(0xecbc, 0x00000000);
		new_set_reg(0xecc0, 0x2868b42f);
		new_set_reg(0xecc4, 0x00000000);
		new_set_reg(0xecc8, 0x00000000);
		new_set_reg(0xeccc, 0x00000000);
		new_set_reg(0xecd0, 0x00000000);
		new_set_reg(0xecd4, 0x00000000);
	}else if(rf_bw == RF_BW_40M){
		new_set_reg(0xe014, 0x00110000);//set RX IRR BW to 40M
		new_set_reg(0xec78, 0x00266666);// tone
		new_set_reg(0xec7c, 0x00733333);// tone
		new_set_reg(0xec80, 0x018ccccc);// tone
		new_set_reg(0xec84, 0x01d99999);// tone
		new_set_reg(0xec88, 0x00000000);// tone
		new_set_reg(0xec8c, 0x00000000);// tone
		new_set_reg(0xec90, 0x00000000);// tone
		new_set_reg(0xec94, 0x00000000);// tone
		new_set_reg(0xec98, 0x0970f1a6);
		new_set_reg(0xec9c, 0x00000000);
		new_set_reg(0xeca0, 0xf690125a);
		new_set_reg(0xeca4, 0x00000000);
		new_set_reg(0xeca8, 0x00000000);
		new_set_reg(0xecac, 0x00000000);
		new_set_reg(0xecb0, 0x00000000);
		new_set_reg(0xecb4, 0x00000000);
		new_set_reg(0xecb8, 0xf690125a);
		new_set_reg(0xecbc, 0x00000000);
		new_set_reg(0xecc0, 0x0970f1a6);
		new_set_reg(0xecc4, 0x00000000);
		new_set_reg(0xecc8, 0x00000000);
		new_set_reg(0xeccc, 0x00000000);
		new_set_reg(0xecd0, 0x00000000);
		new_set_reg(0xecd4, 0x00000000);
	}else if(rf_bw == RF_BW_80M){
		new_set_reg(0xe014, 0x00120000);//set RX IRR BW to 80M
		new_set_reg(0xec78, 0x004ccccc);// tone
		new_set_reg(0xec7c, 0x00a66666);// tone
		new_set_reg(0xec80, 0x00d33333);// tone
		new_set_reg(0xec84, 0x00f99999);// tone
		new_set_reg(0xec88, 0x01066666);// tone
		new_set_reg(0xec8c, 0x012ccccc);// tone
		new_set_reg(0xec90, 0x01599999);// tone
		new_set_reg(0xec94, 0x01b33333);// tone
		new_set_reg(0xec98, 0x097ef751);
		new_set_reg(0xec9c, 0x0292fbb7);
		new_set_reg(0xeca0, 0xd196369c);
		new_set_reg(0xeca4, 0xf4d11ae1);
		new_set_reg(0xeca8, 0x4663ad86);
		new_set_reg(0xecac, 0x0dd8d7c6);
		new_set_reg(0xecb0, 0xde88288d);
		new_set_reg(0xecb4, 0xfac511a2);
		new_set_reg(0xecb8, 0xde88288d);
		new_set_reg(0xecbc, 0xfac511a2);
		new_set_reg(0xecc0, 0x4663ad86);
		new_set_reg(0xecc4, 0x0dd8d7c6);
		new_set_reg(0xecc8, 0xd196369c);
		new_set_reg(0xeccc, 0xf4d11ae1);
		new_set_reg(0xecd0, 0x097ef751);
		new_set_reg(0xecd4, 0x0292fbb7);
	}

	new_set_reg(0xe008, 0x11111100);//enable esti,tx,rx

      #ifdef RUN_PT
	DUT_PRINT_B("Rx IRR done\r\n");
      #else
      printk("Rx IRR done\r\n");
      #endif
      
	new_get_reg(0xec2c, &data);
	rx_irr_rdy = (data >> 31) & 0x1;

	
	while(unlock_cnt < TIME_OUT_CNT){
		if(rx_irr_rdy == 0){
			new_get_reg(0xec2c, &data);

			rx_irr_rdy = (data >> 31) & 0x1;

                    #ifdef RUN_PT
			    TRACE("RX IRR NOT DOING...\n");
                    #else
                        //printk("RX IRR NOT DOING...\n");
                    #endif
		}else{
                    #ifdef RUN_PT
			    TRACE("RX IRR READY\n");
                    #else
                       // printk("RX IRR READY\n");
                    #endif

                    break;
		}
	}

	new_get_reg(0xec0c, &tmp);
	coeff_eup0 = tmp & 0x7fff;
	coeff_eup1 = (tmp>>16) & 0x7fff;

	new_get_reg(0xec10, &tmp);
	coeff_eup2 = tmp & 0x7fff;
	coeff_eup3 = (tmp>>16) & 0x7fff;

	new_get_reg(0xec14, &tmp);
	coeff_pup0 = tmp & 0x7fff;
	coeff_pup1 = (tmp>>16) & 0x7fff;

	new_get_reg(0xec18, &tmp);
	coeff_pup2 = tmp & 0x7fff;
	coeff_pup3 = (tmp>>16) & 0x7fff;

	new_get_reg(0xec1c, &tmp);
	coeff_elow0 = tmp & 0x7fff;
	coeff_elow1 = (tmp>>16) & 0x7fff;

	new_get_reg(0xec20, &tmp);
	coeff_elow2 = tmp & 0x7fff;
	coeff_elow3 = (tmp>>16) & 0x7fff;

	new_get_reg(0xec24, &tmp);
	coeff_plow0 = tmp & 0x7fff;
	coeff_plow1 = (tmp>>16) & 0x7fff;

	new_get_reg(0xec28, &tmp);
	coeff_plow2 = tmp & 0x7fff;
	coeff_plow3 = (tmp>>16) & 0x7fff;

	if(rx_irr_manual == 1){
		i2c_set_reg_fragment(0xe80c, 14,  0, coeff_eup0);
		i2c_set_reg_fragment(0xe80c, 30, 16, coeff_eup1);
		i2c_set_reg_fragment(0xe810, 14,  0, coeff_eup2);
		i2c_set_reg_fragment(0xe810, 30, 16, coeff_eup3);
		i2c_set_reg_fragment(0xe814, 14,  0, coeff_pup0);
		i2c_set_reg_fragment(0xe814, 30, 16, coeff_pup1);
		i2c_set_reg_fragment(0xe818, 14,  0, coeff_pup2);
		i2c_set_reg_fragment(0xe818, 30, 16, coeff_pup3);
		i2c_set_reg_fragment(0xe81c, 14,  0, coeff_elow0);
		i2c_set_reg_fragment(0xe81c, 30, 16, coeff_elow1);
		i2c_set_reg_fragment(0xe820, 14,  0, coeff_elow2);
		i2c_set_reg_fragment(0xe820, 30, 16, coeff_elow3);
		i2c_set_reg_fragment(0xe824, 14,  0, coeff_plow0);
		i2c_set_reg_fragment(0xe824, 30, 16, coeff_plow1);
		i2c_set_reg_fragment(0xe828, 14,  0, coeff_plow2);
		i2c_set_reg_fragment(0xe828, 30, 16, coeff_plow3);
	}

	if(rf_bw == RF_BW_20M){
		new_set_reg(0xb22c, 0x80000000);//80M:a1; 40M;90; 20M:80
	}else if(rf_bw == RF_BW_40M){
		new_set_reg(0xb22c, 0x90000000);//80M:a1; 40M;90; 20M:80
	}else if(rf_bw == RF_BW_80M){
		new_set_reg(0xb22c, 0xa0000000);//80M:a1; 40M;90; 20M:80
	}

	new_set_reg(0xe008, 0x00111100);//disable esti; open tx,rx
	//new_set_reg(0xe018, 0x00000500);//calib mode:enable:1, mode:RX IRR 5
	
	if(rx_irr_manual == 0){
		new_set_reg(0xe82c, 0x00000a00);//set RXIRR comp bypass 0, manual mode to 0
	}else{
		new_set_reg(0xe82c, 0x00000a01);//set RXIRR comp bypass 0, manual mode to 1
	}

	//i2c_set_reg_fragment(0xe4b8, 31, 24, 0x80);//set txpwctrl gain
	i2c_set_reg_fragment(0xe4b8, 16, 16, 1);//set txpwctrl gain manual to 1
	
	new_set_reg(0xe4f0, 0x15008080);//set TX DPD bypass 1
	new_set_reg(0xe410, 0x00000000);//0: select ofdm 
	new_set_reg(0xb100, 0x01970008);//set testbus to read snr 
	i2c_set_reg_fragment(0x8010, 28, 28, 0);//enable agc cca detect

       #ifdef RUN_PT
	//TO OUTPUT
	TRACE("coeff_eup0:%x\ncoeff_eup1:%x\ncoeff_eup2:%x\ncoeff_eup3:%x\n", coeff_eup0, coeff_eup1, coeff_eup2, coeff_eup3);
	fprintf(fp, "coeff_eup0:%x\ncoeff_eup1:%x\ncoeff_eup2:%x\ncoeff_eup3:%x\n", coeff_eup0, coeff_eup1, coeff_eup2, coeff_eup3);

	TRACE("coeff_pup0:%x\ncoeff_pup1:%x\ncoeff_pup2:%x\ncoeff_pup3:%x\n", coeff_pup0, coeff_pup1, coeff_pup2, coeff_pup3);
	fprintf(fp, "coeff_pup0:%x\ncoeff_pup1:%x\ncoeff_pup2:%x\ncoeff_pup3:%x\n", coeff_pup0, coeff_pup1, coeff_pup2, coeff_pup3);

	TRACE("coeff_elow0:%x\ncoeff_elow1:%x\ncoeff_elow2:%x\ncoeff_elow3:%x\n", coeff_elow0, coeff_elow1, coeff_elow2, coeff_elow3);
	fprintf(fp,"coeff_elow0:%x\ncoeff_elow1:%x\ncoeff_elow2:%x\ncoeff_elow3:%x\n", coeff_elow0, coeff_elow1, coeff_elow2, coeff_elow3 );

	TRACE("coeff_plow0:%x\ncoeff_plow1:%x\ncoeff_plow2:%x\ncoeff_plow3:%x\n", coeff_plow0, coeff_plow1, coeff_plow2, coeff_plow3);
	fprintf(fp, "coeff_plow0:%x\ncoeff_plow1:%x\ncoeff_plow2:%x\ncoeff_plow3:%x\n", coeff_plow0, coeff_plow1, coeff_plow2, coeff_plow3);
       #else
       printk("coeff_eup0:%x\ncoeff_eup1:%x\ncoeff_eup2:%x\ncoeff_eup3:%x\n", coeff_eup0, coeff_eup1, coeff_eup2, coeff_eup3);
	
	printk("coeff_pup0:%x\ncoeff_pup1:%x\ncoeff_pup2:%x\ncoeff_pup3:%x\n", coeff_pup0, coeff_pup1, coeff_pup2, coeff_pup3);
	
	printk("coeff_elow0:%x\ncoeff_elow1:%x\ncoeff_elow2:%x\ncoeff_elow3:%x\n", coeff_elow0, coeff_elow1, coeff_elow2, coeff_elow3);	

	printk("coeff_plow0:%x\ncoeff_plow1:%x\ncoeff_plow2:%x\ncoeff_plow3:%x\n", coeff_plow0, coeff_plow1, coeff_plow2, coeff_plow3);
 
       #endif
       

	calb_val->rx_irr[chn_idx][bw_idx].coeff_eup[0] = coeff_eup0;
	calb_val->rx_irr[chn_idx][bw_idx].coeff_eup[1] = coeff_eup1;
	calb_val->rx_irr[chn_idx][bw_idx].coeff_eup[2] = coeff_eup2;
	calb_val->rx_irr[chn_idx][bw_idx].coeff_eup[3] = coeff_eup3;

	calb_val->rx_irr[chn_idx][bw_idx].coeff_pup[0] = coeff_pup0;
	calb_val->rx_irr[chn_idx][bw_idx].coeff_pup[1] = coeff_pup1;
	calb_val->rx_irr[chn_idx][bw_idx].coeff_pup[2] = coeff_pup2;
	calb_val->rx_irr[chn_idx][bw_idx].coeff_pup[3] = coeff_pup3;

	calb_val->rx_irr[chn_idx][bw_idx].coeff_elow[0] = coeff_elow0;
	calb_val->rx_irr[chn_idx][bw_idx].coeff_elow[1] = coeff_elow1;
	calb_val->rx_irr[chn_idx][bw_idx].coeff_elow[2] = coeff_elow2;
	calb_val->rx_irr[chn_idx][bw_idx].coeff_elow[3] = coeff_elow3;

	calb_val->rx_irr[chn_idx][bw_idx].coeff_plow[0] = coeff_plow0;
	calb_val->rx_irr[chn_idx][bw_idx].coeff_plow[1] = coeff_plow1;
	calb_val->rx_irr[chn_idx][bw_idx].coeff_plow[2] = coeff_plow2;
	calb_val->rx_irr[chn_idx][bw_idx].coeff_plow[3] = coeff_plow3;

      #ifdef RUN_PT
	    fclose(fp);
      #endif

	//set RF reg to initial value
	if(1){
		new_set_reg(0xff000008,origin_rg008);
		new_set_reg(0xff00001c,origin_rg01c);
		new_set_reg(0xff000164,origin_rg164);
		new_set_reg(0xff000208,origin_rg208);
		new_set_reg(0xff000308,origin_rg308);
		new_set_reg(0xff000314,origin_rg314);
		new_set_reg(0xff000320,origin_rg320);
	}

	//set digital reg to initial vaule
	if(1){
		new_set_reg(0xe008, origin_e008);
		new_set_reg(0xb22c, origin_b22c);
		new_set_reg(0xb234, origin_b234);
		new_set_reg(0xe018, origin_e018);
		
		new_set_reg(0xe4f0, origin_e4f0);
		new_set_reg(0xe5b8, origin_e5b8);
		new_set_reg(0xe4b8, origin_e4b8);
		
		new_set_reg(0xe40c, origin_e40c);
		new_set_reg(0xe410, origin_e410);

		new_set_reg(0xe014, origin_e014);

		i2c_set_reg_fragment(0xe82c, 4, 4, origin_e82c&0x1);
		new_set_reg(0xec2c, origin_ec2c);
	}
}


void TestRxDC(struct rx_dc_result* rx_dc_rst,unsigned int * rst_cnt, struct rx_dc_dbg * rx_dc_dbg_info, unsigned int * dbg_inf_cnt)
{
       unsigned int data = 0;
	int num = 0;
	unsigned int code_i=0;
	unsigned int code_q = 0;
	unsigned int value_i = 0;
	unsigned int value_q = 0;
       int dci =0;
	int dcq = 0;

	int max_i = 0;
	int max_j = 0;
	


	 //rx dc
	new_set_reg(0xff000008,0x00002313);
	new_set_reg(0xff000318,0x03fc61d5);
	new_set_reg(0xff000308,0x00019c01);
	new_set_reg(0xff0003b0,0x01400140);

	i2c_write_reg(0xec44, 0x00000000);
	i2c_write_reg(0xe008, 0x10101000);

    i2c_write_reg(0xe018, 0x00010100);
    i2c_write_reg(0xe018, 0x00010000);
	
	i2c_write_reg(0xe008,0x11111100);
	i2c_write_reg(0xe010,0x00000000);
	//i2c_write_reg(0xe018,0x00010000);
	i2c_write_reg(0xff0003b0,0x01400140);

	i2c_write_reg(0xff000308,0x00019c01);

	value_i = value_q = 0x40;
	i2c_set_reg_fragment(0xff0003b0,7,0,value_i);
	i2c_set_reg_fragment(0xff0003b0,22,16,value_q);

	i2c_write_reg(0xed04,0x00000400);
	

	for(num = 6;num >= -1;num--){
		if(num == -1){
			i2c_set_reg_fragment(0xff0003b0, 0, 0, 0);
		    i2c_set_reg_fragment(0xff0003b0, 0+16, 0+16, 0);
		}else{
		    i2c_set_reg_fragment(0xff0003b0, num, num, 1);
		    i2c_set_reg_fragment(0xff0003b0, num+16, num+16, 1);
		}

             #ifdef RUN_PT
		    TRACE("RXDC num = %d\n",num);
              #else
                 printk("RXDC num = %d\n",num); 
              #endif
              
		i2c_write_reg(0xec44,0x80000000);

              #ifdef RUN_PT
            		Sleep(10);
               #else
                    msleep(10);
               #endif
               
             data = i2c_read_reg(0xec44);


		dci = i2c_read_reg(0xed64);
		dcq = i2c_read_reg(0xed68);

		if(dci & BIT(29)){
                 dci = dci - (1<<30);
		}
                dci = dci>>(10+10);

		if(dcq & BIT(29)){
            dcq = dcq - (1<<30);
		}
        dcq = dcq>>(10+10);
		

		if((num == -1)){
			if((data & BIT(24))){
				code_i = data & 0xff;
				code_q = (data >> 8) & 0xff;
				i2c_set_reg_fragment(0xff0003b0,7,0,code_i);
				i2c_set_reg_fragment(0xff0003b0,22,16,code_q);
				i2c_write_reg(0xec44,0x00000000);

                           #ifdef RUN_PT
				    TRACE("i=0x%x,  q=0x%x, reg03b0=0x%x\n",code_i,code_q,i2c_read_reg(0xff0003b0));
                            #else
                               printk("i=0x%x,  q=0x%x, reg03b0=0x%x\n",code_i,code_q,i2c_read_reg(0xff0003b0));
                            #endif
                
                            rx_dc_dbg_info->code_i = code_i;
                            rx_dc_dbg_info->code_q = code_q;
                            rx_dc_dbg_info->rd_reg = i2c_read_reg(0xff0003b0);
                            rx_dc_dbg_info ++;

				if(max_j++ > 1024)
        		         {
        		                #ifdef RUN_PT
                                        TRACE("overflow max_j %d\n",max_j);
                                    #else
                                        printk("overflow max_j %d\n",max_j);
                                    #endif
                                    
                                    return;
        		         }
                
				*dbg_inf_cnt = max_j;
			}
			else{
                           #ifdef RUN_PT
				    TRACE("RXDC Fail 0,, num %d\n",num);
                            #else
                               printk("RXDC Fail 0,, num %d\n",num);
                            #endif
                            
			    break;
			}
		}else{
			if(data & BIT(16)){
				code_i = data & 0xff;
				code_q = (data >> 8) & 0xff;
				i2c_set_reg_fragment(0xff0003b0,7,0,code_i);
				i2c_set_reg_fragment(0xff0003b0,22,16,code_q);
				i2c_write_reg(0xec44,0x00000000);

                           #ifdef RUN_PT
				    TRACE("i=0x%x,  q=0x%x, reg03b0=0x%x\n",code_i,code_q,i2c_read_reg(0xff0003b0));
                           #else
                               printk("i=0x%x,  q=0x%x, reg03b0=0x%x\n",code_i,code_q,i2c_read_reg(0xff0003b0)); 
                           #endif
                           
				 rx_dc_dbg_info->code_i = code_i;
                            rx_dc_dbg_info->code_q = code_q;
                            rx_dc_dbg_info->rd_reg = i2c_read_reg(0xff0003b0);
                            rx_dc_dbg_info ++;
                
				if(max_j++ > 1024)
        		         {
                                    #ifdef RUN_PT
                                        TRACE("overflow max_j %d\n",max_j);
                                    #else
                                        printk("overflow max_j %d\n",max_j);
                                    #endif
                                    
                                    return;
        		         }
                
				*dbg_inf_cnt = max_j;
		   }
                else
		   {
                         #ifdef RUN_PT
                             TRACE("RXDC Fail 1 , num %d\n",num);
                         #else
                            printk("RXDC Fail 1 , num %d\n",num);
                         #endif
                         
				break;
		   }
	     }

              #ifdef RUN_PT  
		    TRACE("---> rxdc calibration result: num= %d dci=%d,  dcq=%d, codei=%d,  codeq=%d\n",num, dci, dcq, code_i, code_q);
              #else
                 printk("---> rxdc calibration result: num= %d dci=%d,  dcq=%d, codei=%d,  codeq=%d\n",num, dci, dcq, code_i, code_q);
              #endif

                rx_dc_rst->num = num;
                rx_dc_rst->dci = dci;
                rx_dc_rst->dcq = dcq;
                rx_dc_rst->code_i = code_i;
                rx_dc_rst->code_q = code_q;
                rx_dc_rst++;

		if(max_i++ > 1024)
		{
		    #ifdef RUN_PT
                     TRACE("overflow max_i %d\n",max_i);
                  #else
                    printk("overflow max_i %d\n",max_i);
                  #endif
                  
                  return;
		}

		*rst_cnt = max_i;
	}

}


unsigned int do_rx_rc_calb(struct calb_rst_out* calb_val)
{
	unsigned int rg_out = 0;
	/*unsigned int rx_rc_set[][2] = {{0xff0003b8, 0x02800000}, 
								   {0xff0003bc, 0x02800000},
								   {0xff0003c0, 0x02800280},
								   {0xff0003c4, 0x00000280},
								   {0xff0003c8, 0x02800280},
								   {0xff000308, 0x00019c01},
								   {0xff0003b4, 0x00100000},
								   {0xff0003b4, 0x80100000}								 
								   };
	
	*/

	unsigned int origin_rg008;
    unsigned int origin_rg3b8;
	unsigned int origin_rg3bc;
	unsigned int origin_rg3c0;
	unsigned int origin_rg3c4;
	unsigned int origin_rg3c8;
	unsigned int origin_rg308;
	unsigned int origin_rg3b4;


       #ifdef RUN_PT
         char path[1024] = CALB_OUTPUT_PATH ;
         FILE *fp = NULL;

        sprintf(path,"%s%s",g_CurrentPath,CALB_OUTPUT_PATH );
        	
        fp = fopen((char *)path, "a+");
        if (!fp){
        		TRACE(" output file open fail\r\n");
        		return -1;
        	}
        	
        fprintf(fp, "rxrc log........\n");
      #else
        printk("rxrc log........\n");
      #endif
       
	new_get_reg(0xff000008, &origin_rg008);
	new_get_reg(0xff0003b8, &origin_rg3b8);
	new_get_reg(0xff0003bc, &origin_rg3bc);
	new_get_reg(0xff0003c0, &origin_rg3c0);
	new_get_reg(0xff0003c4, &origin_rg3c4);
	new_get_reg(0xff0003c8, &origin_rg3c8);
	new_get_reg(0xff000308, &origin_rg308);
    new_get_reg(0xff0003b4, &origin_rg3b4);

	//for(int i = 0; i < sizeof(rx_rc_set) / (sizeof(unsigned int) * 2); i++)
	//{
	//	new_set_reg(rx_rc_set[i][0], rx_rc_set[i][1]);
	//}
    
	new_set_reg(0xff0003b8, 0x02800000); 
	new_set_reg(0xff0003bc, 0x02800000);
	new_set_reg(0xff0003c0, 0x02800280);
	new_set_reg(0xff0003c4, 0x00000280);
	new_set_reg(0xff0003c8, 0x02800280);
	new_set_reg(0xff000308, 0x00019c01);
	new_set_reg(0xff0003b4, 0x00100000);

	i2c_set_reg_fragment(0xff000008,1,0, 3);  // rx mode

       i2c_set_reg_fragment(0xff000100,0,0,0);
	i2c_set_reg_fragment(0xff000100,0,0,1);

	new_set_reg(0xff0003b4, 0x80100000);

	new_get_reg(0xff0003d4, &rg_out);

       #ifdef RUN_PT
	    TRACE("-------------------------->RC OUT 0x%x\n", rg_out);	
       #else
           printk("-------------------------->RC OUT 0x%x\n", rg_out);	
       #endif
       
       calb_val->rx_rc.rxrc_out = rg_out;
	
	if(1){
		new_set_reg(0xff000008, origin_rg008);
		new_set_reg(0xff0003b8, origin_rg3b8);
		new_set_reg(0xff0003bc, origin_rg3bc);
		new_set_reg(0xff0003c0, origin_rg3c0);
		new_set_reg(0xff0003c4, origin_rg3c4);
		new_set_reg(0xff0003c8, origin_rg3c8);
		new_set_reg(0xff000308, origin_rg308);
		new_set_reg(0xff0003b4, origin_rg3b4);
	}

       #ifdef RUN_PT
	    fprintf(fp, "rxrc out 0x%x:\n",rg_out);
	
	    fclose(fp);

	    TRACE("RX RC calib finish \n");
        #else
            printk("RX RC calib finish \n");
        #endif
        
	return rg_out;
}



void TestRxDC_417(struct calb_rst_out* calb_val,
				  struct rx_dc_result* rx_dc_rst,
				  unsigned int * rst_cnt, 
				  struct rx_dc_dbg * rx_dc_dbg_info, 
				  unsigned int * dbg_inf_cnt,
				  unsigned int bw_idx,
				  unsigned int tia_idx,
				  unsigned int lpf_idx )
{
	unsigned int data;
	unsigned int tmp;
	unsigned int rx_dc_rdy;
	unsigned int unlock_cnt = 0;
	unsigned int TIME_OUT_CNT = 300;

	int num = 0;
	unsigned int code_i = 0;
	unsigned int code_q = 0;
	int dci = 0;
	int dcq = 0;
	int max_i = 0;
	int max_j = 0;
	int tmp1 = 0;
	int tmp2 = 0;
	unsigned int base_addr_I;
	unsigned int base_addr_Q;
	unsigned int offset_addr_I;
	unsigned int offset_addr_Q;
	unsigned int fe_gain;
	unsigned int lpf_gain;

	unsigned int origin_rg008;
	unsigned int origin_rg308;
	unsigned int origin_rg318;

	bool rx_dc_manual;
	rx_dc_manual = 0;
	
    new_get_reg(0xff000008, &origin_rg008);
	new_get_reg(0xff000308, &origin_rg308);
	new_get_reg(0xff000318, &origin_rg318);

	//rx dc
	i2c_set_reg_fragment(0xff000318, 6, 6, 0);   //LNA DISABLE
    if(rx_dc_manual == 0)
		new_set_reg(0xff0003b0, 0x00400040); //set manual mode to 0
	else
		new_set_reg(0xff0003b0, 0x01400140); //set manual mode to 1

	fe_gain = 8 + tia_idx;
       lpf_gain = lpf_idx + 1;

	i2c_set_reg_fragment(0xff000308, 15, 12, fe_gain);  //Rx FE gain
	i2c_set_reg_fragment(0xff000308, 11,  8, lpf_gain); //Rx lpf gain

	i2c_set_reg_fragment(0xff000008, 1, 0, 3); //rx mode 
    i2c_set_reg_fragment(0xff000100, 0, 0, 0); //do sx calib
	i2c_set_reg_fragment(0xff000100, 0, 0, 1);
        
	base_addr_I = 0xff000380 + tia_idx*12 + (lpf_idx/4)*4;
	base_addr_Q = 0xff000398 + tia_idx*12 + (lpf_idx/4)*4;

	offset_addr_I = (lpf_idx%4)*8;
	offset_addr_Q = (lpf_idx%4)*8;
    
	new_set_reg(0xe008, 0x00000000); //disable esti; tx,rx
	new_set_reg(0xe018, 0x00010000); //calib mode:enable:0, mode:RX DC
	new_set_reg(0xec44, 0x00000000); //read response bypass to 0, soft read ready response to 0
	new_set_reg(0xed04, 0x00004000); //rx dc calc count:2^14
	new_set_reg(0xe008, 0x11111100); //enable esti; open tx,rx
    
	for(num = 6; num >= -1; num--)
    {
		tmp1 = (num != -1) ? num: 0;
		tmp2 = (num != -1) ? 1  : 0;
        
		if(rx_dc_manual == 0){
			i2c_set_reg_fragment(base_addr_I, tmp1+offset_addr_I, tmp1+offset_addr_I, tmp2);// I 
			i2c_set_reg_fragment(base_addr_Q, tmp1+offset_addr_Q, tmp1+offset_addr_Q, tmp2);// Q
		}else{
            i2c_set_reg_fragment(0xff0003b0, tmp1, tmp1, tmp2);//I
			i2c_set_reg_fragment(0xff0003b0, tmp1+16, tmp1+16, tmp2);//Q
		}	

            #ifdef RUN_PT
		TRACE("base_addr_I 0x%x, tmp1+offset_addr_I 0x%x, tmp1+offset_addr_I 0x%x, tmp2 0x%x\n",
			(unsigned int)base_addr_I, (unsigned int)(tmp1+offset_addr_I), (unsigned int)(tmp1+offset_addr_I), (unsigned int)tmp2);
		TRACE("read base_addr_I 0x%x, value 0x%x\n", (unsigned int)base_addr_I, i2c_read_reg(base_addr_I));
		TRACE("read base_addr_Q 0x%x, value 0x%x\n", (unsigned int)base_addr_Q, i2c_read_reg(base_addr_Q));
             #endif
             
            new_set_reg(0xec44, 0x80000000);//read response bypass to 0, soft read ready response to 1
		
		tmp = (num != -1) ? 16 : 24;

		data = i2c_read_reg(0xec44);
		rx_dc_rdy = data & BIT(tmp);

		while(unlock_cnt < TIME_OUT_CNT){
			if(rx_dc_rdy == 0){
				data = i2c_read_reg(0xec44);
				rx_dc_rdy = data & BIT(tmp);

				unlock_cnt = unlock_cnt + 1;

                           #ifdef RUN_PT
				    TRACE("unlock_cnt:%d, RX DC NOT DONE...\n",unlock_cnt);
                           #else
                               //printk("unlock_cnt:%d, RX DC NOT DONE...\n",unlock_cnt); 
                           #endif
                
			}else{
			       #ifdef RUN_PT
				    TRACE("RX DC ready\n");
                            #else
                               printk("RX DC ready\n"); 
                            #endif
				break;
			}
		}


		dci = i2c_read_reg(0xed64);
		dcq = i2c_read_reg(0xed68);

		if(dci & BIT(29)){
			dci = dci - (1<<30);
		}

		dci = dci>>(5+14);

		if(dcq & BIT(29)){
			dcq = dcq - (1<<30);
		}
		dcq = dcq>>(5+14);

		code_i = data & 0xff;
		code_q = (data >> 8) & 0xff;

		if(rx_dc_manual == 0){
			i2c_set_reg_fragment(base_addr_I, offset_addr_I+6, offset_addr_I, code_i);// I 
			i2c_set_reg_fragment(base_addr_Q, offset_addr_Q+6, offset_addr_Q, code_q);// Q
		}else{
			i2c_set_reg_fragment(0xff0003b0,6,0,code_i);
			i2c_set_reg_fragment(0xff0003b0,22,16,code_q);
		}

   		new_set_reg(0xec44, 0x00000000);

		//TRACE("i=0x%x,  q=0x%x, reg03b0=0x%x\n",code_i,code_q,i2c_read_reg(0xff0003b0));
		rx_dc_dbg_info->code_i = code_i;
		rx_dc_dbg_info->code_q = code_q;
		//rx_dc_dbg_info->rd_reg = i2c_read_reg(0xff0003b0);
		rx_dc_dbg_info ++;

		if(max_i++ > 1024 || max_j++ > 1024){
                   #ifdef RUN_PT
        			TRACE("overflow: max_i %d, max_j %d\n",max_i, max_j);
                    #else
                          printk("overflow: max_i %d, max_j %d\n",max_i, max_j);
                    #endif
                    
                    return;
		}

		*rst_cnt = max_i;
		*dbg_inf_cnt = max_j;

             #ifdef RUN_PT
        		TRACE("----------------------> rx dc out: num = %d dci = %d,  dcq = %d, codei = %d,  codeq = %d\n",
                                        num, dci, dcq, code_i, code_q);
              #else
                    printk("----------------------> rx dc out: num = %d dci = %d,  dcq = %d, codei = %d,  codeq = %d\n",
                                        num, dci, dcq, code_i, code_q);
             
              #endif
              
		rx_dc_rst->num = num;
		rx_dc_rst->dci = dci;
		rx_dc_rst->dcq = dcq;
		rx_dc_rst->code_i = code_i;
		rx_dc_rst->code_q = code_q;
		rx_dc_rst++;
	}//end of for(num = 6;num >= -1;num--) 

	calb_val->rx_dc[bw_idx][tia_idx][lpf_idx].rxdc_i = dci;
	calb_val->rx_dc[bw_idx][tia_idx][lpf_idx].rxdc_q = dcq;
	calb_val->rx_dc[bw_idx][tia_idx][lpf_idx].rxdc_codei=code_i;
	calb_val->rx_dc[bw_idx][tia_idx][lpf_idx].rxdc_codeq = code_q;
       #ifdef RUN_PT
        	TRACE("RX DC calib finish \n");
       #else
             printk("RX DC calib finish \n");
       #endif
	new_set_reg(0xe008, 0x00111100);//disable esti; open tx,rx

	if(1){
		new_set_reg(0xff000008,origin_rg008);
		new_set_reg(0xff000308,origin_rg308);
		new_set_reg(0xff000318,origin_rg318);
	}
}

void do_rxdc_for_txirr(void)
{
	unsigned int data;
	unsigned int tmp;
	unsigned int rx_dc_rdy;
	unsigned int unlock_cnt = 0;
	unsigned int TIME_OUT_CNT = 300;

	int num = 0;
	unsigned int code_i = 0;
	unsigned int code_q = 0;
	int dci = 0;
	int dcq = 0;
	int max_i = 0;
	int max_j = 0;
	int tmp1 = 0;
	int tmp2 = 0;

	unsigned int origin_rg008;
	unsigned int origin_rg308;
	unsigned int origin_rg01c;
	unsigned int origin_rg208;
	unsigned int origin_rg314;
	unsigned int origin_e008;
	unsigned int origin_e018;
	unsigned int origin_ec44;
	unsigned int origin_ed04;
	unsigned int origin_e410;

	bool rx_dc_manual;
	rx_dc_manual = 1;
#ifdef RUN_PT
	char path[1024] = CALB_OUTPUT_PATH ;
	FILE *fp = NULL;
	
	sprintf(path,"%s%s",g_CurrentPath,CALB_OUTPUT_PATH);
	
	fp = fopen((char *)path, "a+");
	if (!fp){
		TRACE(" output file open fail\r\n");
		return ;
	}
	fprintf(fp, "rx dc for txirr start........\r\n");
#endif

      new_get_reg(0xff000008, &origin_rg008);
	new_get_reg(0xff000308, &origin_rg308);
	new_get_reg(0xff00001c, &origin_rg01c);
	new_get_reg(0xff000208, &origin_rg208);
	new_get_reg(0xff000314, &origin_rg314);
	new_get_reg(0xe008, &origin_e008);
	new_get_reg(0xe018, &origin_e018);
	new_get_reg(0xec44, &origin_ec44);
	new_get_reg(0xed04, &origin_ed04);
	new_get_reg(0xe410, &origin_e410);
	
	i2c_set_reg_fragment(0xff00001c, 7, 0, 0xff);
	i2c_set_reg_fragment(0xff000208, 0, 0, 1);

	i2c_set_reg_fragment(0xff000308, 7,  0, 0x01);//BW 20M
	i2c_set_reg_fragment(0xff000308, 15,  8, 0x1a);//0x19
           
	i2c_set_reg_fragment(0xff000314, 27, 12, 0x7ffe);

	i2c_set_reg_fragment(0xff000008, 1, 0, 2);//set tx mode
    i2c_set_reg_fragment(0xff000100, 0, 0, 0);//do sx calib
	i2c_set_reg_fragment(0xff000100, 0, 0, 1);
	//i2c_set_reg_fragment(0xff000008, 1, 0, 2);

    new_set_reg(0xff0003b0, 0x01400140); //set manual mode to 1

	new_set_reg(0xe008, 0x00000000); //disable esti; tx,rx
	new_set_reg(0xe018, 0x00010000); //calib mode:enable:0, mode:RX DC
	new_set_reg(0xec44, 0x00000000); //read response bypass to 0, soft read ready response to 0
	new_set_reg(0xed04, 0x00004000); //rx dc calc count:2^14
	new_set_reg(0xe410, 0x80000002); //const
	new_set_reg(0xe008, 0x11111100); //enable esti; open tx,rx
    
	for(num = 6; num >= -1; num--)
    {
		tmp1 = (num != -1) ? num: 0;
		tmp2 = (num != -1) ? 1  : 0;
        
        i2c_set_reg_fragment(0xff0003b0, tmp1, tmp1, tmp2);//I
		i2c_set_reg_fragment(0xff0003b0, tmp1+16, tmp1+16, tmp2);//Q

#ifdef RUN_PT
		TRACE("0xff0003b0, tmp1 0x%x, tmp1 0x%x, tmp2 0x%x\n",(unsigned int)tmp1, (unsigned int)tmp1, (unsigned int)tmp2);
		TRACE("0xff0003b0, tmp1+16 0x%x, tmp1+16 0x%x, tmp2 0x%x\n",(unsigned int)(tmp1+16), (unsigned int)(tmp1+16), (unsigned int)tmp2);
		TRACE("read 0xff0003b0, value 0x%x\n", i2c_read_reg(0xff0003b0));
#else
		printk("0xff0003b0, tmp1 0x%x, tmp1 0x%x, tmp2 0x%x\n",(unsigned int)tmp1, (unsigned int)tmp1, (unsigned int)tmp2);
		printk("0xff0003b0, tmp1+16 0x%x, tmp1+16 0x%x, tmp2 0x%x\n",(unsigned int)(tmp1+16), (unsigned int)(tmp1+16), (unsigned int)tmp2);
		printk("read 0xff0003b0, value 0x%x\n", i2c_read_reg(0xff0003b0));

#endif
             
        new_set_reg(0xec44, 0x80000000);//read response bypass to 0, soft read ready response to 1
		
		tmp = (num != -1) ? 16 : 24;

		data = i2c_read_reg(0xec44);
		rx_dc_rdy = data & BIT(tmp);

		while(unlock_cnt < TIME_OUT_CNT){
			if(rx_dc_rdy == 0){
				data = i2c_read_reg(0xec44);
				rx_dc_rdy = data & BIT(tmp);

				unlock_cnt = unlock_cnt + 1;

#ifdef RUN_PT
				TRACE("unlock_cnt:%d, RX DC NOT DONE...\n",unlock_cnt);
#else
                printk("unlock_cnt:%d, RX DC NOT DONE...\n",unlock_cnt); 
#endif
                
			}else{
#ifdef RUN_PT
				TRACE("RX DC ready\n");
#else
                printk("RX DC ready\n"); 
#endif
				break;
			}
		}


		dci = i2c_read_reg(0xed64);
		dcq = i2c_read_reg(0xed68);

		if(dci & BIT(29)){
			dci = dci - (1<<30);
		}

		dci = dci>>(5+14);

		if(dcq & BIT(29)){
			dcq = dcq - (1<<30);
		}
		dcq = dcq>>(5+14);

		code_i = data & 0xff;
		code_q = (data >> 8) & 0xff;

		i2c_set_reg_fragment(0xff0003b0,6,0,code_i);
		i2c_set_reg_fragment(0xff0003b0,22,16,code_q);

   		new_set_reg(0xec44, 0x00000000);

#ifdef RUN_PT
		TRACE("code_i=0x%x, code_q=0x%x, reg03b0=0x%x\n",code_i,code_q,i2c_read_reg(0xff0003b0));
#else
		printk("code_i=0x%x, code_q=0x%x, reg03b0=0x%x\n",code_i,code_q,i2c_read_reg(0xff0003b0));
#endif

		if(max_i++ > 1024 || max_j++ > 1024){
#ifdef RUN_PT
        	TRACE("overflow: max_i %d, max_j %d\n",max_i, max_j);
#else
            printk("overflow: max_i %d, max_j %d\n",max_i, max_j);
#endif
                    
            return;
		}


#ifdef RUN_PT
        TRACE("----------------------> rx dc out: num = %d dci = %d,  dcq = %d, codei = %d,  codeq = %d\n",
                                        num, dci, dcq, code_i, code_q);
		fprintf(fp, "rxdc calibration result: num= %d dci=%d,  dcq=%d, codei=%d,  codeq=%d\r\n",
			                            num, dci, dcq, code_i, code_q);
#else
        printk("----------------------> rx dc out: num = %d dci = %d,  dcq = %d, codei = %d,  codeq = %d\n",
                                        num, dci, dcq, code_i, code_q);
             
#endif
              
	}//end of for(num = 6;num >= -1;num--) 

#ifdef RUN_PT
    TRACE("RX DC for txirr calib finish \n");
	fprintf(fp, "rx dc for txirr end........\r\n");

#else
        printk("RX DC calib finish \n");
#endif
	new_set_reg(0xe008, 0x00111100);//disable esti; open tx,rx

	if(1){
		new_set_reg(0xff000008, origin_rg008);
		new_set_reg(0xff000308, origin_rg308);
		new_set_reg(0xff00001c, origin_rg01c);
		new_set_reg(0xff000208, origin_rg208);
		new_set_reg(0xff000314, origin_rg314);
	}
	if(1){
		new_set_reg(0xe008, origin_e008);
		new_set_reg(0xe018, origin_e018);
		new_set_reg(0xec44, origin_ec44);
		new_set_reg(0xed04, origin_ed04);
		new_set_reg(0xe410, origin_e410);
	}

#ifdef RUN_PT	
	fclose(fp);
#endif

}

void do_rxdc_for_txdpd(void)
{
	unsigned int data;
	unsigned int tmp;
	unsigned int rx_dc_rdy;
	unsigned int unlock_cnt = 0;
	unsigned int TIME_OUT_CNT = 300;

	int num = 0;
	unsigned int code_i = 0;
	unsigned int code_q = 0;
	int dci = 0;
	int dcq = 0;
	int max_i = 0;
	int max_j = 0;
	int tmp1 = 0;
	int tmp2 = 0;

    unsigned int origin_008 = 0;
	unsigned int origin_01c = 0;
	unsigned int origin_164 = 0;
	unsigned int origin_200 = 0;
	unsigned int origin_204 = 0;
	unsigned int origin_208 = 0;
	unsigned int origin_308 = 0;
	unsigned int origin_314 = 0;
	unsigned int origin_3e0 = 0;
	unsigned int origin_3e8 = 0;


	unsigned int origin_e008;
	unsigned int origin_e018;
	unsigned int origin_ec44;
	unsigned int origin_ed04;
	unsigned int origin_e410;

	bool rx_dc_manual;
	rx_dc_manual = 1;

#ifdef RUN_PT
	char path[1024] = CALB_OUTPUT_PATH ;
	FILE *fp = NULL;
	
	sprintf(path,"%s%s",g_CurrentPath,CALB_OUTPUT_PATH);
	
	fp = fopen((char *)path, "a+");
	if (!fp){
		TRACE(" output file open fail\r\n");
		return ;
	}
	fprintf(fp, "rx dc for txdpd start........\r\n");
#endif

      new_get_reg(0xff000008,&origin_008);
	new_get_reg(0xff00001c,&origin_01c);
	new_get_reg(0xff000164,&origin_164);
	new_get_reg(0xff000200,&origin_200);
	new_get_reg(0xff000204,&origin_204);
	new_get_reg(0xff000208,&origin_208);
	//new_get_reg(0xff00020c,&origin_20c);
	new_get_reg(0xff000308,&origin_308);
	new_get_reg(0xff000314,&origin_314);
	//new_get_reg(0xff0003f0,&origin_3f0);
	new_get_reg(0xff0003e0,&origin_3e0);
	new_get_reg(0xff0003e8,&origin_3e8);

	new_set_reg(0xff00001c,0x000000ff);//enable RX LDO in mode2
    new_set_reg(0xff000164,0x00001bff);//enable DPD LO in mode2
    
	new_set_reg(0xff000200,0x03639000);//tx dcoc i/q,txm_ct(default)
	//new_set_reg(0xff000204,0x0c8811c0);//tx adb close
	new_set_reg(0xff000204,0x19fc01c0);//tx adb on:19fc01c0
	new_set_reg(0xff000208,0x00110360);//tx dcoc i/q,txm_ct(default)
	//new_set_reg(0xff00020c,0x00001077);//tx v2i.mixr pa gain(default)
    
	new_set_reg(0xff000308,0x00017b1d);// 0x00017c1d, FE+LPF gain, BW:160M, ADC 320M

	new_set_reg(0xff000314,0x03ffe619);// RX mode2 control
	//new_set_reg(0xff0003f0,0x00003508);// tx lpf gain
    new_set_reg(0xff0003e0,0x01abb7fd);// GM/dpd atten
	new_set_reg(0xff0003e8,0x00000006);// close ADC notch filter

	i2c_set_reg_fragment(0xff000008,1,0,2);// set tx mode to enable dpd loop
	i2c_set_reg_fragment(0xff000100,0,0,0);// triger sx freq lock
	i2c_set_reg_fragment(0xff000100,0,0,1);
	//i2c_set_reg_fragment(0xff000008,1,0,2);

    new_set_reg(0xff0003b0, 0x01400140); //set manual mode to 1

	new_get_reg(0xe008, &origin_e008);
	new_get_reg(0xe018, &origin_e018);
	new_get_reg(0xec44, &origin_ec44);
	new_get_reg(0xed04, &origin_ed04);
	new_get_reg(0xe410, &origin_e410);

	new_set_reg(0xe008, 0x00000000); //disable esti; tx,rx
	new_set_reg(0xe018, 0x00010000); //calib mode:enable:0, mode:RX DC
	new_set_reg(0xec44, 0x00000000); //read response bypass to 0, soft read ready response to 0
	new_set_reg(0xed04, 0x00004000); //rx dc calc count:2^14
	new_set_reg(0xe410, 0x80000002); //const
	new_set_reg(0xe008, 0x11111100); //enable esti; open tx,rx
    
	for(num = 6; num >= -1; num--)
    {
		tmp1 = (num != -1) ? num: 0;
		tmp2 = (num != -1) ? 1  : 0;
        
        i2c_set_reg_fragment(0xff0003b0, tmp1, tmp1, tmp2);//I
		i2c_set_reg_fragment(0xff0003b0, tmp1+16, tmp1+16, tmp2);//Q

#ifdef RUN_PT
		TRACE("0xff0003b0, tmp1 0x%x, tmp1 0x%x, tmp2 0x%x\n",(unsigned int)tmp1, (unsigned int)tmp1, (unsigned int)tmp2);
		TRACE("0xff0003b0, tmp1+16 0x%x, tmp1+16 0x%x, tmp2 0x%x\n",(unsigned int)(tmp1+16), (unsigned int)(tmp1+16), (unsigned int)tmp2);
		TRACE("read 0xff0003b0, value 0x%x\n", i2c_read_reg(0xff0003b0));
#endif
             
        new_set_reg(0xec44, 0x80000000);//read response bypass to 0, soft read ready response to 1
		
		tmp = (num != -1) ? 16 : 24;

		data = i2c_read_reg(0xec44);
		rx_dc_rdy = data & BIT(tmp);

		while(unlock_cnt < TIME_OUT_CNT){
			if(rx_dc_rdy == 0){
				data = i2c_read_reg(0xec44);
				rx_dc_rdy = data & BIT(tmp);

				unlock_cnt = unlock_cnt + 1;

#ifdef RUN_PT
				TRACE("unlock_cnt:%d, RX DC NOT DONE...\n",unlock_cnt);
#else
                printk("unlock_cnt:%d, RX DC NOT DONE...\n",unlock_cnt); 
#endif
                
			}else{
#ifdef RUN_PT
				TRACE("RX DC ready\n");
#else
                printk("RX DC ready\n"); 
#endif
				break;
			}
		}

		dci = i2c_read_reg(0xed64);
		dcq = i2c_read_reg(0xed68);

		if(dci & BIT(29)){
			dci = dci - (1<<30);
		}

		dci = dci>>(5+14);

		if(dcq & BIT(29)){
			dcq = dcq - (1<<30);
		}
		dcq = dcq>>(5+14);

		code_i = data & 0xff;
		code_q = (data >> 8) & 0xff;

		i2c_set_reg_fragment(0xff0003b0,6,0,code_i);
		i2c_set_reg_fragment(0xff0003b0,22,16,code_q);

   		new_set_reg(0xec44, 0x00000000);

#ifdef RUN_PT
		TRACE("code_i=0x%x, code_q=0x%x, reg03b0=0x%x\n",code_i,code_q,i2c_read_reg(0xff0003b0));
#endif

		if(max_i++ > 1024 || max_j++ > 1024){
#ifdef RUN_PT
        	TRACE("overflow: max_i %d, max_j %d\n",max_i, max_j);
#else
            printk("overflow: max_i %d, max_j %d\n",max_i, max_j);
#endif
                    
            return;
		}

#ifdef RUN_PT
        TRACE("----------------------> rx dc out: num = %d dci = %d,  dcq = %d, codei = %d,  codeq = %d\n",
                                        num, dci, dcq, code_i, code_q);
		fprintf(fp, "rxdc calibration result: num= %d dci=%d,  dcq=%d, codei=%d,  codeq=%d\r\n",
			                            num, dci, dcq, code_i, code_q);
#else
        printk("----------------------> rx dc out: num = %d dci = %d,  dcq = %d, codei = %d,  codeq = %d\n",
                                        num, dci, dcq, code_i, code_q);
             
#endif

	}//end of for(num = 6;num >= -1;num--) 

#ifdef RUN_PT
    TRACE("RX DC for txdpd calib finish \n");
	fprintf(fp, "rx dc for txdpd end........\r\n");

#else
        printk("RX DC calib finish \n");
#endif
	new_set_reg(0xe008, 0x00111100);//disable esti; open tx,rx

	if(1){
		new_set_reg(0xff000008,origin_008);
	    new_set_reg(0xff00001c,origin_01c);
	    new_set_reg(0xff000164,origin_164);
	    //new_set_reg(0xff000200,origin_200);
		//new_set_reg(0xff000204,origin_204);
		//new_set_reg(0xff000208,origin_208);
		//new_set_reg(0xff00020c,origin_20c);
		
		new_set_reg(0xff000308,origin_308);
		new_set_reg(0xff000314,origin_314);
		//new_set_reg(0xff0003f0,origin_3f0);
		new_set_reg(0xff0003e0,origin_3e0);
		new_set_reg(0xff0003e8,origin_3e8);
	}
	if(1){
		new_set_reg(0xe008, origin_e008);
		new_set_reg(0xe018, origin_e018);
		new_set_reg(0xec44, origin_ec44);
		new_set_reg(0xed04, origin_ed04);
		new_set_reg(0xe410, origin_e410);
	}

#ifdef RUN_PT	
	fclose(fp);
#endif

}
void TxCalbCfg(int chn,int bw)
{
	unsigned int txiqdc_gain[] = {0x773,0x772,0x770,0x733,0x732,0x730,0x713,0x712,0x710};
	
	//unsigned int tx_dpd_gain[] = {0x773,0x733,0x713};

	unsigned int all_chn[] = {0x5a, 0x0a, 0x32, 0x82, 0xa5, 0xbe, 0x1e, 0x46, 0x6e, 0x96};
	//unsigned int all_chn[] = {0x0a};
	 //set RF register
	int gain_idx = 0;//max gain(773)
	int chn_idx = 0;
	int bw_idx = 0;
	bool dpd_bypass = 0;
       int ck = 0;
	unsigned int coeff_I[15];
	unsigned int coeff_Q[15];

	//find channel idx
	int i;
	int delta_chn;
	//bw
	unsigned int orig_b22c;

	//set TX DC/IQ digital gain
	unsigned int dci = 0;
	unsigned int dcq = 0;	
	unsigned int alpha = 0;
	unsigned int theta = 0;


	chn_idx = 1;
	for(i = 0; i < sizeof(all_chn)/sizeof(all_chn[0]); i++){
		delta_chn = all_chn[i]-chn;
		if( delta_chn >= -6 && delta_chn <= 6){
			chn_idx = i;
			break;
		}
	}
    
	
	new_get_reg(0xb22c, &orig_b22c);
    if(orig_b22c >= 0xa0000000)
		bw_idx = 2;
	else if(orig_b22c >= 0x90000000)
		bw_idx = 1;
	else if(orig_b22c >= 0x80000000)
		bw_idx = 0;

	//bw_idx = (bw == 20) ? 0 :(bw == 40) ? 1 : (bw == 80) ? 2 : 0;
    
   

    i2c_set_reg_fragment(0xff00020c,  2,  0, (txiqdc_gain[gain_idx]>>8)&0x7);   //Tx PA
	i2c_set_reg_fragment(0xff00020c,  6,  4, (txiqdc_gain[gain_idx]>>4)&0x7);   //Tx MXR
	i2c_set_reg_fragment(0xff0003f0, 13, 12, (txiqdc_gain[gain_idx])&0x7);      //TX Lpf
    
	//set channel
    i2c_set_reg_fragment(0xff000118, 23, 16, all_chn[chn_idx] & 0xff);

	//set RF tx mode
	i2c_set_reg_fragment(0xff000008, 1, 0, 2);
	i2c_set_reg_fragment(0xff000100, 0, 0, 0);// triger sx freq lock
	i2c_set_reg_fragment(0xff000100, 0, 0, 1);

	i2c_set_reg_fragment(0xe4b4, 31, 31, 1);//set TX DC/IQ comp manual to 1
	
	dci   = calb_val.tx_dc_irr[chn_idx][gain_idx].tx_dc_i;
	dcq   = calb_val.tx_dc_irr[chn_idx][gain_idx].tx_dc_q;
	alpha = calb_val.tx_dc_irr[chn_idx][gain_idx].tx_alpha;
	theta = calb_val.tx_dc_irr[chn_idx][gain_idx].tx_theta;

	i2c_set_reg_fragment(0xe4b4,  7, 0, dci   & 0xff); //set TX DC_I
	i2c_set_reg_fragment(0xe4b4, 15, 8, dcq   & 0xff); //set TX DC_Q
	i2c_set_reg_fragment(0xe4b8,  7, 0, theta & 0xff); //set TX theta
	i2c_set_reg_fragment(0xe4b8, 15, 8, alpha & 0xff); //set TX alpha

	i2c_set_reg_fragment(0xe488, 0, 0, 0);  //TX iqmm bypass set 0

	//set TX DPD comp digital coeff

	for( ck = 0; ck < 15; ck++){
		coeff_I[ck] = calb_val.tx_dpd[chn_idx][bw_idx][gain_idx].tx_dpd_coeff_i[ck];
		coeff_Q[ck] = calb_val.tx_dpd[chn_idx][bw_idx][gain_idx].tx_dpd_coeff_q[ck];
	}

	i2c_set_reg_fragment(0xe4f4, 11,  0, coeff_I[0] & 0xfff);
	i2c_set_reg_fragment(0xe4f4, 27, 16, coeff_Q[0] & 0xfff);
	i2c_set_reg_fragment(0xe4f8, 11,  0, coeff_I[1] & 0xfff);
	i2c_set_reg_fragment(0xe4f8, 27, 16, coeff_Q[1] & 0xfff);
	i2c_set_reg_fragment(0xe4fc, 11,  0, coeff_I[2] & 0xfff);
	i2c_set_reg_fragment(0xe4fc, 27, 16, coeff_Q[2] & 0xfff);
	i2c_set_reg_fragment(0xe500, 11,  0, coeff_I[3] & 0xfff);
	i2c_set_reg_fragment(0xe500, 27, 16, coeff_Q[3] & 0xfff);
	i2c_set_reg_fragment(0xe504, 11,  0, coeff_I[4] & 0xfff);
	i2c_set_reg_fragment(0xe504, 27, 16, coeff_Q[4] & 0xfff);
	i2c_set_reg_fragment(0xe508, 11,  0, coeff_I[5] & 0xfff);
	i2c_set_reg_fragment(0xe508, 27, 16, coeff_Q[5] & 0xfff);
	i2c_set_reg_fragment(0xe50c, 11,  0, coeff_I[6] & 0xfff);
	i2c_set_reg_fragment(0xe50c, 27, 16, coeff_Q[6] & 0xfff);
	i2c_set_reg_fragment(0xe510, 11,  0, coeff_I[7] & 0xfff);
	i2c_set_reg_fragment(0xe510, 27, 16, coeff_Q[7] & 0xfff);
	i2c_set_reg_fragment(0xe514, 11,  0, coeff_I[8] & 0xfff);
	i2c_set_reg_fragment(0xe514, 27, 16, coeff_Q[8] & 0xfff);
	i2c_set_reg_fragment(0xe518, 11,  0, coeff_I[9] & 0xfff);
	i2c_set_reg_fragment(0xe518, 27, 16, coeff_Q[9] & 0xfff);
	i2c_set_reg_fragment(0xe51c, 11,  0, coeff_I[10]& 0xfff);
	i2c_set_reg_fragment(0xe51c, 27, 16, coeff_Q[10]& 0xfff);
	i2c_set_reg_fragment(0xe520, 11,  0, coeff_I[11]& 0xfff);
	i2c_set_reg_fragment(0xe520, 27, 16, coeff_Q[11]& 0xfff);
	i2c_set_reg_fragment(0xe524, 11,  0, coeff_I[12]& 0xfff);
	i2c_set_reg_fragment(0xe524, 27, 16, coeff_Q[12]& 0xfff);
	i2c_set_reg_fragment(0xe528, 11,  0, coeff_I[13]& 0xfff);
	i2c_set_reg_fragment(0xe528, 27, 16, coeff_Q[13]& 0xfff);
	i2c_set_reg_fragment(0xe52c, 11,  0, coeff_I[14]& 0xfff);
	i2c_set_reg_fragment(0xe52c, 27, 16, coeff_Q[14]& 0xfff);

	i2c_set_reg_fragment(0xe4f0, 24, 24, 1);//set dpd comp type to 1:coeff
	
	if(0){
		if(dpd_bypass)
			i2c_set_reg_fragment(0xe4f0, 26, 26, 1);//set dpd comp bypass to 1
		else
			i2c_set_reg_fragment(0xe4f0, 26, 26, 0);//set dpd comp bypass to 0
	}

	i2c_set_reg_fragment(0xe4f0, 28, 28, 1);//set dpd comp bypass man to 1

	i2c_set_reg_fragment(0xe4f0, 31, 31, 1);//set dpd comp manual mode to 1
		
	new_set_reg(0xe008, 0x00111100); //disable esti, enable tx, rx
    new_set_reg(0xe410, 0x00000000); //select ofdm signal
    
	new_set_reg(0xe5b8, 0x00000008);//bb gain
	i2c_set_reg_fragment(0xe4b8, 31, 24, 0x80);//set txpwctrl gain
	i2c_set_reg_fragment(0xe4b8, 16, 16, 1);//set txpwctrl gain manual to 1
}

void RxCalbCfg(int chn,int bw)
{
	//unsigned int all_chn[] = {0x5a, 0x0a, 0x32, 0x82, 0xa5, 0xbe, 0x1e, 0x46, 0x6e, 0x96};
	unsigned int all_chn[] = {0x0a};
	
	int chn_idx = 0;
	int bw_idx = 0;
	bool irr_bypass = 0;
       int k = 0;
	//find channel idx
	int i;
	int delta_chn;
	 //bw
	unsigned int orig_b22c;
	unsigned int coeff_eup[4];
	unsigned int coeff_pup[4];
	unsigned int coeff_elow[4];
	unsigned int coeff_plow[4];
	unsigned int tia_idx;
	unsigned int lpf_idx;

	chn_idx = 1;
	for(i = 0; i < sizeof(all_chn)/sizeof(all_chn[0]); i++){
		delta_chn = all_chn[i]-chn;
		if( delta_chn >= -6 && delta_chn <= 6){
			chn_idx = i;
			break;
		}
	}

   
	new_get_reg(0xb22c, &orig_b22c);
    if(orig_b22c >= 0xa0000000)
		bw_idx = 2;
	else if(orig_b22c >= 0x90000000)
		bw_idx = 1;
	else if(orig_b22c >= 0x80000000)
		bw_idx = 0;

	//bw_idx = (bw == 20) ? 0 :(bw == 40) ? 1 : (bw == 80) ? 2 : 0;
    
    //set RF register
    //rx rc & rx dc is auto selected according to channel from rf register(already done in calib)

	//set rx fe+lpf gain
	i2c_set_reg_fragment(0xff000308,  0,  0, 1);   //set man mode to 1
	i2c_set_reg_fragment(0xff000308, 15,  8, 0x9c);//set rx fe+lpf gain
	i2c_set_reg_fragment(0xff000308, 16, 16, 0);   //set rx fe+lpf to auto gain
    
	//set rx dc according to bw
	for(tia_idx = 0; tia_idx < 2; tia_idx++){
		for(lpf_idx = 0; lpf_idx < 12; lpf_idx++){
			set_rxdc_calb(&calb_val, bw_idx, tia_idx, lpf_idx);
		}
	}

	//set RF bw & adc
	if(bw_idx == 0){
		i2c_set_reg_fragment(0xff000308, 3, 2, 0x0);//RF band:20M
		i2c_set_reg_fragment(0xff000308, 4, 4, 0);  // adc 160M
	}else if(bw_idx == 1){
	   i2c_set_reg_fragment(0xff000308, 3, 2, 0x1); //RF band:40M
	   i2c_set_reg_fragment(0xff000308, 4, 4, 0);   //adc 160M
	}else if(bw_idx == 2){
		i2c_set_reg_fragment(0xff000308, 3, 2, 0x2);//RF band:80M
		i2c_set_reg_fragment(0xff000308, 4, 4, 1);  //adc 320
	}
    
	//set RX LNA
	//auto selected according to channel from rf register(already done in calib)
	//new_set_reg(0xff000320, 0x00551c23);//RX LNA man set to 12 for 5050
    
	//set channel
    i2c_set_reg_fragment(0xff000118, 23, 16, all_chn[chn_idx]&0xff);
    
	//set RF rx mode
	i2c_set_reg_fragment(0xff000008, 1, 0, 3);
	// triger sx freq lock(do sx calib)
	i2c_set_reg_fragment(0xff000100, 0, 0, 0);
	i2c_set_reg_fragment(0xff000100, 0, 0, 1);

    //set rx irr comp digital coeff
	if(0){
		if(irr_bypass)
			new_set_reg(0xe82c, 0x00000a11);//set RXIRR comp bypass 1, manual mode to 1
		else
			new_set_reg(0xe82c, 0x00000a01);//set RXIRR comp bypass 0, manual mode to 1
	}

	
    
	for( k = 0; k < 4; k++){
		coeff_eup[k]  = calb_val.rx_irr[chn_idx][bw_idx].coeff_eup[k];
		coeff_pup[k]  = calb_val.rx_irr[chn_idx][bw_idx].coeff_pup[k];
		coeff_elow[k] = calb_val.rx_irr[chn_idx][bw_idx].coeff_elow[k];
		coeff_plow[k] = calb_val.rx_irr[chn_idx][bw_idx].coeff_plow[k];
	}

	i2c_set_reg_fragment(0xe80c, 14,  0, coeff_eup[0] &0x7fff);
	i2c_set_reg_fragment(0xe80c, 30, 16, coeff_eup[1] &0x7fff);
	i2c_set_reg_fragment(0xe810, 14,  0, coeff_eup[2] &0x7fff);
	i2c_set_reg_fragment(0xe810, 30, 16, coeff_eup[3] &0x7fff);
	i2c_set_reg_fragment(0xe814, 14,  0, coeff_pup[0] &0x7fff);
	i2c_set_reg_fragment(0xe814, 30, 16, coeff_pup[1] &0x7fff);
	i2c_set_reg_fragment(0xe818, 14,  0, coeff_pup[2] &0x7fff);
	i2c_set_reg_fragment(0xe818, 30, 16, coeff_pup[3] &0x7fff);
	i2c_set_reg_fragment(0xe81c, 14,  0, coeff_elow[0]&0x7fff);
	i2c_set_reg_fragment(0xe81c, 30, 16, coeff_elow[1]&0x7fff);
	i2c_set_reg_fragment(0xe820, 14,  0, coeff_elow[2]&0x7fff);
	i2c_set_reg_fragment(0xe820, 30, 16, coeff_elow[3]&0x7fff);
	i2c_set_reg_fragment(0xe824, 14,  0, coeff_plow[0]&0x7fff);
	i2c_set_reg_fragment(0xe824, 30, 16, coeff_plow[1]&0x7fff);
	i2c_set_reg_fragment(0xe828, 14,  0, coeff_plow[2]&0x7fff);
	i2c_set_reg_fragment(0xe828, 30, 16, coeff_plow[3]&0x7fff);
}

void TrswchCfg(int chn,int bw)
{
    //set RF reg
	unsigned int txiqdc_gain[] = {0x773,0x772,0x770,0x733,0x732,0x730,0x713,0x712,0x710};
	
	//unsigned int tx_dpd_gain[] = {0x773,0x733,0x713};

	//unsigned int all_chn[] = {0x5a, 0x0a, 0x32, 0x82, 0xa5, 0xbe, 0x1e, 0x46, 0x6e, 0x96};
	unsigned int all_chn[] = {0x0a};
	int ck  = 0;
       int k = 0;
	int chn_idx = 0;
	int bw_idx = 0;
	unsigned int dpd_bypass = 0;
   
	unsigned int coeff_I[15];
	unsigned int coeff_Q[15];
   //bw
	unsigned int orig_b22c;
	int gain_idx = 0;//max gain(773)
   //TX digital reg
	//set TX DC/IQ digital comp value
	unsigned int dci = 0;
	unsigned int dcq = 0;	
	unsigned int alpha = 0;
	unsigned int theta = 0;
//RX digital reg 
    unsigned int irr_bypass = 0;

	unsigned int coeff_eup[4];
	unsigned int coeff_pup[4];
	unsigned int coeff_elow[4];
	unsigned int coeff_plow[4];


	//find channel idx and bw
	int i;
	int delta_chn;
	chn_idx = 1;
	for(i = 0; i < sizeof(all_chn)/sizeof(all_chn[0]); i++){
		delta_chn = all_chn[i]-chn;
		if( delta_chn >= -6 && delta_chn <= 6){
			chn_idx = i;
			break;
		}
	}
    
	
	new_get_reg(0xb22c, &orig_b22c);
    if(orig_b22c >= 0xa0000000)
		bw_idx = 2;
	else if(orig_b22c >= 0x90000000)
		bw_idx = 1;
	else if(orig_b22c >= 0x80000000)
		bw_idx = 0;
	//bw_idx = (bw == 20) ? 0 :(bw == 40) ? 1 : (bw == 80) ? 2 : 0;
    
    //set RF register
	//TX RF reg

    
    i2c_set_reg_fragment(0xff00020c,  2,  0, (txiqdc_gain[gain_idx]>>8)&0x7);   //Tx PA
	i2c_set_reg_fragment(0xff00020c,  6,  4, (txiqdc_gain[gain_idx]>>4)&0x7);   //Tx MXR
	i2c_set_reg_fragment(0xff0003f0, 13, 12, (txiqdc_gain[gain_idx])&0x7);      //TX Lpf

	//RX RF reg  
    //rx rc & rx dc is auto selected according to channel from rf register(already done in calib)

	//set rx fe+lpf gain
	i2c_set_reg_fragment(0xff000308,  0,  0, 1);   //set man mode to 1
	i2c_set_reg_fragment(0xff000308, 15,  8, 0x9c);//set rx fe+lpf gain
	i2c_set_reg_fragment(0xff000308, 16, 16, 0);   //set rx fe+lpf to auto gain
    
	//set RF bw & adc
	if(bw_idx == 0){
		i2c_set_reg_fragment(0xff000308, 3, 2, 0x0);//RF band:20M
		i2c_set_reg_fragment(0xff000308, 4, 4, 0);  // adc 160M
	}else if(bw_idx == 1){
	   i2c_set_reg_fragment(0xff000308, 3, 2, 0x1); //RF band:40M
	   i2c_set_reg_fragment(0xff000308, 4, 4, 0);   //adc 160M
	}else if(bw_idx == 2){
		i2c_set_reg_fragment(0xff000308, 3, 2, 0x2);//RF band:80M
		i2c_set_reg_fragment(0xff000308, 4, 4, 1);  //adc 320
	}
    
	//set RX LNA
	//auto selected according to channel from rf register(already done in calib)
	//new_set_reg(0xff000320, 0x00551c23);//RX LNA man set to 12 for 5050

	//set channel
	i2c_set_reg_fragment(0xff000118, 23, 16, all_chn[i] & 0xff);

	i2c_set_reg_fragment(0xff000008, 1, 0, 2);//set tx mode
	//triger sx freq lock(do sx calib)
    i2c_set_reg_fragment(0xff000100, 0, 0, 0);
	i2c_set_reg_fragment(0xff000100, 0, 0, 1);

    i2c_set_reg_fragment(0xff000008,  4,  4, 0);//set RG_WF_MODE_MAN_MODE to 0
    i2c_set_reg_fragment(0xff000008,  9,  8, 2);//set TX mode to 2 in auto mode
	i2c_set_reg_fragment(0xff000008, 13, 12, 3);//set RX mode to 3 in auto mode
    

    

	i2c_set_reg_fragment(0xe4b4, 31, 31, 1);//set TX DC/IQ comp manual to 1
	
	dci   = calb_val.tx_dc_irr[chn_idx][gain_idx].tx_dc_i;
	dcq   = calb_val.tx_dc_irr[chn_idx][gain_idx].tx_dc_q;
	alpha = calb_val.tx_dc_irr[chn_idx][gain_idx].tx_alpha;
	theta = calb_val.tx_dc_irr[chn_idx][gain_idx].tx_theta;

	i2c_set_reg_fragment(0xe4b4,  7, 0, dci   & 0xff); //set TX DC_I
	i2c_set_reg_fragment(0xe4b4, 15, 8, dcq   & 0xff); //set TX DC_Q
	i2c_set_reg_fragment(0xe4b8,  7, 0, theta & 0xff); //set TX theta
	i2c_set_reg_fragment(0xe4b8, 15, 8, alpha & 0xff); //set TX alpha

	new_set_reg(0xe488, 0x000a0010);//TX iqmm bypass set 0

	//set TX DPD digital comp coeff
	for( ck = 0; ck < 15; ck++){
		coeff_I[ck] = calb_val.tx_dpd[chn_idx][bw_idx][gain_idx].tx_dpd_coeff_i[ck];
		coeff_Q[ck] = calb_val.tx_dpd[chn_idx][bw_idx][gain_idx].tx_dpd_coeff_q[ck];
	}

	i2c_set_reg_fragment(0xe4f4, 11,  0, coeff_I[0] & 0xfff);
	i2c_set_reg_fragment(0xe4f4, 27, 16, coeff_Q[0] & 0xfff);
	i2c_set_reg_fragment(0xe4f8, 11,  0, coeff_I[1] & 0xfff);
	i2c_set_reg_fragment(0xe4f8, 27, 16, coeff_Q[1] & 0xfff);
	i2c_set_reg_fragment(0xe4fc, 11,  0, coeff_I[2] & 0xfff);
	i2c_set_reg_fragment(0xe4fc, 27, 16, coeff_Q[2] & 0xfff);
	i2c_set_reg_fragment(0xe500, 11,  0, coeff_I[3] & 0xfff);
	i2c_set_reg_fragment(0xe500, 27, 16, coeff_Q[3] & 0xfff);
	i2c_set_reg_fragment(0xe504, 11,  0, coeff_I[4] & 0xfff);
	i2c_set_reg_fragment(0xe504, 27, 16, coeff_Q[4] & 0xfff);
	i2c_set_reg_fragment(0xe508, 11,  0, coeff_I[5] & 0xfff);
	i2c_set_reg_fragment(0xe508, 27, 16, coeff_Q[5] & 0xfff);
	i2c_set_reg_fragment(0xe50c, 11,  0, coeff_I[6] & 0xfff);
	i2c_set_reg_fragment(0xe50c, 27, 16, coeff_Q[6] & 0xfff);
	i2c_set_reg_fragment(0xe510, 11,  0, coeff_I[7] & 0xfff);
	i2c_set_reg_fragment(0xe510, 27, 16, coeff_Q[7] & 0xfff);
	i2c_set_reg_fragment(0xe514, 11,  0, coeff_I[8] & 0xfff);
	i2c_set_reg_fragment(0xe514, 27, 16, coeff_Q[8] & 0xfff);
	i2c_set_reg_fragment(0xe518, 11,  0, coeff_I[9] & 0xfff);
	i2c_set_reg_fragment(0xe518, 27, 16, coeff_Q[9] & 0xfff);
	i2c_set_reg_fragment(0xe51c, 11,  0, coeff_I[10]& 0xfff);
	i2c_set_reg_fragment(0xe51c, 27, 16, coeff_Q[10]& 0xfff);
	i2c_set_reg_fragment(0xe520, 11,  0, coeff_I[11]& 0xfff);
	i2c_set_reg_fragment(0xe520, 27, 16, coeff_Q[11]& 0xfff);
	i2c_set_reg_fragment(0xe524, 11,  0, coeff_I[12]& 0xfff);
	i2c_set_reg_fragment(0xe524, 27, 16, coeff_Q[12]& 0xfff);
	i2c_set_reg_fragment(0xe528, 11,  0, coeff_I[13]& 0xfff);
	i2c_set_reg_fragment(0xe528, 27, 16, coeff_Q[13]& 0xfff);
	i2c_set_reg_fragment(0xe52c, 11,  0, coeff_I[14]& 0xfff);
	i2c_set_reg_fragment(0xe52c, 27, 16, coeff_Q[14]& 0xfff);

	i2c_set_reg_fragment(0xe4f0, 24, 24, 1);//set dpd comp type to 1:coeff
	
	if(dpd_bypass)
		i2c_set_reg_fragment(0xe4f0, 26, 26, 1);//set dpd comp bypass to 1
	else
		i2c_set_reg_fragment(0xe4f0, 26, 26, 0);//set dpd comp bypass to 0

	i2c_set_reg_fragment(0xe4f0, 28, 28, 1);//set dpd comp bypass man to 1

	i2c_set_reg_fragment(0xe4f0, 31, 31, 1);//set dpd comp manual mode to 1
		
	

    //set rx irr digital comp coeff
	if(irr_bypass)
		new_set_reg(0xe82c, 0x00000a11);//set RXIRR comp bypass 1, manual mode to 1
	else
		new_set_reg(0xe82c, 0x00000a01);//set RXIRR comp bypass 0, manual mode to 1

	
    
	for( k = 0; k < 4; k++){
		coeff_eup[k]  = calb_val.rx_irr[chn_idx][bw_idx].coeff_eup[k];
		coeff_pup[k]  = calb_val.rx_irr[chn_idx][bw_idx].coeff_pup[k];
		coeff_elow[k] = calb_val.rx_irr[chn_idx][bw_idx].coeff_elow[k];
		coeff_plow[k] = calb_val.rx_irr[chn_idx][bw_idx].coeff_plow[k];
	}

	i2c_set_reg_fragment(0xe80c, 14,  0, coeff_eup[0] &0x7fff);
	i2c_set_reg_fragment(0xe80c, 30, 16, coeff_eup[1] &0x7fff);
	i2c_set_reg_fragment(0xe810, 14,  0, coeff_eup[2] &0x7fff);
	i2c_set_reg_fragment(0xe810, 30, 16, coeff_eup[3] &0x7fff);
	i2c_set_reg_fragment(0xe814, 14,  0, coeff_pup[0] &0x7fff);
	i2c_set_reg_fragment(0xe814, 30, 16, coeff_pup[1] &0x7fff);
	i2c_set_reg_fragment(0xe818, 14,  0, coeff_pup[2] &0x7fff);
	i2c_set_reg_fragment(0xe818, 30, 16, coeff_pup[3] &0x7fff);
	i2c_set_reg_fragment(0xe81c, 14,  0, coeff_elow[0]&0x7fff);
	i2c_set_reg_fragment(0xe81c, 30, 16, coeff_elow[1]&0x7fff);
	i2c_set_reg_fragment(0xe820, 14,  0, coeff_elow[2]&0x7fff);
	i2c_set_reg_fragment(0xe820, 30, 16, coeff_elow[3]&0x7fff);
	i2c_set_reg_fragment(0xe824, 14,  0, coeff_plow[0]&0x7fff);
	i2c_set_reg_fragment(0xe824, 30, 16, coeff_plow[1]&0x7fff);
	i2c_set_reg_fragment(0xe828, 14,  0, coeff_plow[2]&0x7fff);
	i2c_set_reg_fragment(0xe828, 30, 16, coeff_plow[3]&0x7fff);

	//set digital to TX&RX switch mode
	new_set_reg(0xe008, 0x00000000); //disable esti, enable tx, rx
    new_set_reg(0xe410, 0x00000000); //select ofdm signal
    
	new_set_reg(0xe5b8, 0x00000008);//bb gain
	i2c_set_reg_fragment(0xe4b8, 31, 24, 0x80);//set txpwctrl gain
	i2c_set_reg_fragment(0xe4b8, 16, 16, 1);//set txpwctrl gain manual to 1
}


#ifdef RUN_PT
#define PI 3.1415926
void do_txdpd_lut_calb(void)
{
    unsigned int origin_008 = 0;
	unsigned int origin_01c = 0;
	unsigned int origin_164 = 0;
	unsigned int origin_200 = 0;
	unsigned int origin_204 = 0;
	unsigned int origin_208 = 0;
	unsigned int origin_308 = 0;
	unsigned int origin_314 = 0;
	unsigned int origin_3e0 = 0;
	unsigned int origin_3e8 = 0;

	 unsigned int const_min  = 0x008;//0x004;0x008;0x00a
	unsigned int const_step = 0x012;//0x010;0x010;0x012 
	//unsigned int const_max  = const_step*31 + const_min;

    unsigned int dci_tmp = 0;
	unsigned int dcq_tmp = 0;
	int dci = 0;
	int dcq = 0;
	unsigned int data;
	unsigned int rx_dc_rdy;
	unsigned int unlock_cnt = 0;
	unsigned int TIME_OUT_CNT = 300;
    unsigned int const_value = 0;

	double tx_i[32] = {0};
	double tx_q[32] = {0};
	double rx_i[32] = {0};
	double rx_q[32] = {0};
	double abs_tx[32] = {0};
	double abs_rx[32] = {0};
	double angle_rx[32] = {0};
	double theta[32] = {0};
	double pa_slope[32] = {0};
	double pa_phase[32] = {0};
	double DPD_AM[32] = {0};
	double DPD_PM[32] = {0};
	double PA_table_I[32] = {0};
	double PA_table_Q[32] = {0};
	double DPD_table_I[32] = {0};
	double DPD_table_Q[32] = {0};
	int DPD_table_I_tmp[32] = {0};
	int DPD_table_Q_tmp[32] = {0};
    unsigned int DPD_table_rtl_I[32] = {0};
	unsigned int DPD_table_rtl_Q[32] = {0};
	double common_gain = 1.0;
	double common_phase = 0.0;

	//set lut coeff to register
	unsigned int addr;
       int ck = 0; 

	bool tx_dpd_manual = 0;

#ifdef RUN_PT
	char path[1024] = CALB_OUTPUT_PATH ;
	FILE *fp = NULL;
	
	sprintf(path,"%s%s",g_CurrentPath,CALB_OUTPUT_PATH);
	
	fp = fopen((char *)path, "a+");
	if (!fp){
		TRACE(" output file open fail\r\n");
		return ;
	}
	
	fprintf(fp, "dpd lut log........\n");
#else
	printk("dpd lut log........\n");
#endif

       new_get_reg(0xff000008,&origin_008);
	new_get_reg(0xff00001c,&origin_01c);
	new_get_reg(0xff000164,&origin_164);
	new_get_reg(0xff000200,&origin_200);
	new_get_reg(0xff000204,&origin_204);
	new_get_reg(0xff000208,&origin_208);
	//new_get_reg(0xff00020c,&origin_20c);
	new_get_reg(0xff000308,&origin_308);
	new_get_reg(0xff000314,&origin_314);
	//new_get_reg(0xff0003f0,&origin_3f0);
	new_get_reg(0xff0003e0,&origin_3e0);
	new_get_reg(0xff0003e8,&origin_3e8);
    
	new_set_reg(0xff00001c,0x000000ff);//enable RX LDO in mode2
    new_set_reg(0xff000164,0x00001bff);//enable DPD LO in mode2
    
	new_set_reg(0xff000200,0x03639000);//tx dcoc i/q,txm_ct(default)
	new_set_reg(0xff000204,0x0c8811c0);//tx adb close
	new_set_reg(0xff000208,0x00110360);//tx dcoc i/q,txm_ct(default)
	//new_set_reg(0xff00020c,0x00001077);//tx v2i.mixr pa gain(default)
    
	//new_set_reg(0xff000308,0x00017c1d);// FE+LPF gain, BW:160M, ADC 320M
	new_set_reg(0xff000308,0x00019b01);// FE+LPF gain, BW:160M, ADC 320M
	
	
	new_set_reg(0xff000314,0x03ffe619);// RX mode2 control
	//new_set_reg(0xff0003f0,0x00003508);// tx lpf gain
    new_set_reg(0xff0003e0,0x01abb7fd);// GM/dpd atten:0x01abb7fd(polynominal)
	new_set_reg(0xff0003e8,0x00000006);// close ADC notch filter

	i2c_set_reg_fragment(0xff000008,1,0,2);// set tx mode to enable dpd loop
	i2c_set_reg_fragment(0xff000100,0,0,0);// triger sx freq lock
	i2c_set_reg_fragment(0xff000100,0,0,1);
	//i2c_set_reg_fragment(0xff000008,1,0,2);


   

	//digital reg
	new_set_reg(0xe410, 0x80000002); //0:select OFDM signal;1:signal tone;2:constant signal
    new_set_reg(0x0000e5b8,0x00000008);
    i2c_set_reg_fragment(0xe4b8, 31, 24, 0x80); //set pwtrl gain

    i2c_set_reg_fragment(0xe4f0, 7, 0, 0x80);//dpd in gain 
    i2c_set_reg_fragment(0xe4f0, 24, 24, 1);//set dpd comp type to 1:coeff
    i2c_set_reg_fragment(0xe4f0, 26, 26, 1);//set dpd comp bypass to 1
    i2c_set_reg_fragment(0xe4f0, 28, 28, 1);//set dpd comp bypass man to 1
    if (tx_dpd_manual == 0)
        i2c_set_reg_fragment(0xe4f0, 31, 31, 0);//set dpd comp manual mode to 0
    else
        i2c_set_reg_fragment(0xe4f0, 31, 31, 1);//set dpd comp manual mode to 1
	new_set_reg(0xe530, 0x10000002);//set DPD LUT low&high threshold

	i2c_set_reg_fragment(0x8010, 28, 28, 1);//disable agc cca detect
	
	for( ck = 0; ck < 32; ck++){
		new_set_reg(0xe008, 0x00000000); //disable esti; tx,rx
		new_set_reg(0xe018, 0x00010000); //calib mode:enable:0, mode:RX DC
		new_set_reg(0xec44, 0x00000000); //read response bypass to 0, soft read ready response to 0
		new_set_reg(0xed04, 0x00004000); //rx dc calc count:2^14
		new_set_reg(0xe008, 0x11111100); //enable esti; open tx,rx

		if(ck >= 0 && ck < 10){
			const_step = 20;
		}else if(ck >= 10 && ck < 20){
			const_step = 16;
		}else{
			const_step = 14;
		}
        const_value = const_min + ck*const_step;
		i2c_set_reg_fragment(0xe410, 14,  4, const_value&0x7ff); //i component
		i2c_set_reg_fragment(0xe410, 26, 16, const_value&0x7ff); //q component

        new_set_reg(0xec44, 0x80000000);//read response bypass to 0, soft read ready response to 1
		

		new_get_reg(0xec44, &data);
		rx_dc_rdy = data & BIT(16);

		while(unlock_cnt < TIME_OUT_CNT){
			if(rx_dc_rdy == 0){
				new_get_reg(0xec44, &data);
				rx_dc_rdy = data & BIT(16);

				unlock_cnt = unlock_cnt + 1;
                           #ifdef RUN_PT
        				TRACE("unlock_cnt:%d, RX DC NOT DONE...\n",unlock_cnt);
                           #else
                                 //printk("unlock_cnt:%d, RX DC NOT DONE...\n",unlock_cnt);
                           #endif
                           
			}else{
				unlock_cnt = 0;
                           #ifdef RUN_PT
				    TRACE("RX DC ready\n");
                           #else
                              printk("RX DC ready\n");
                           #endif
                           
				break;
			}
		} 
        new_get_reg(0xed64, &dci_tmp);
		new_get_reg(0xed68, &dcq_tmp);
        dci = dci_tmp;
		dcq = dcq_tmp;

		if(dci & BIT(29)){
			dci = dci - (1<<30);
		}

		dci = dci>>(5+14);

		if(dcq & BIT(29)){
			dcq = dcq - (1<<30);
		}
		dcq = dcq>>(5+14);
        
		new_set_reg(0xec44, 0x00000000);

		
		//for dpd lut process
        tx_i[ck] = const_value;
		tx_q[ck] = const_value;
		rx_i[ck] = dci;
		rx_q[ck] = dcq;

		abs_tx[ck] = sqrt(tx_i[ck]*tx_i[ck]*1.0 + tx_q[ck]*tx_q[ck]*1.0);
		abs_rx[ck] = sqrt(rx_i[ck]*rx_i[ck]*1.0 + rx_q[ck]*rx_q[ck]*1.0);
		angle_rx[ck] = atan2(rx_q[ck], rx_i[ck])/PI*180.0;

             #ifdef RUN_PT
		TRACE("---> rx dc for look-up table: ck = %2d , dci = %d,  dcq = %d, abs_tx = %f, abs_rx = %f, angle_rx = %f\n",
                    ck, dci, dcq, abs_tx[ck], abs_rx[ck], angle_rx[ck]);
		fprintf(fp, "---> rx dc for look-up table: ck = %2d dci = %3d  dcq = %3d abs_tx = %f abs_rx = %f angle_rx = %f\n",
                    ck, dci, dcq, abs_tx[ck], abs_rx[ck], angle_rx[ck]);
             #endif
             
	}

       #ifdef RUN_PT
	    fprintf(fp, "rx_dci=[");
       #endif
       
	for( ck = 0; ck < 32; ck++){
        #ifdef RUN_PT
            fprintf(fp, "%.1f ",rx_i[ck]);
        #endif
	}

       #ifdef RUN_PT
	    fprintf(fp, "]\n");
	    fprintf(fp, "rx_dcq=[");
        #endif
        
	for( ck = 0; ck < 32; ck++){

            #ifdef RUN_PT
                fprintf(fp, "%.1f ",rx_q[ck]);
            #endif
	}

       #ifdef RUN_PT
        	fprintf(fp, "]\n");
        #endif
	
    common_gain = abs_rx[1]/abs_tx[1];
    common_phase = angle_rx[1];

	for( ck = 0; ck < 32; ck++){
		theta[ck] = angle_rx[ck] - common_phase;
		theta[ck] = theta[ck]/180.0*PI;
	}
    
	//calculate DPD LUT
	for( ck = 0; ck < 32; ck++){
		pa_slope[ck] = abs_rx[ck]/abs_tx[ck];
		pa_slope[ck] = pa_slope[ck]/common_gain;
		pa_phase[ck] = theta[ck];

		DPD_AM[ck] = 1.0/pa_slope[ck];
		DPD_PM[ck] = -1*pa_phase[ck];
	}

	for( ck = 0; ck < 32; ck++){
		PA_table_I[ck] = pa_slope[ck]*cos(pa_phase[ck]);
		PA_table_Q[ck] = pa_slope[ck]*sin(pa_phase[ck]);
		DPD_table_I[ck] = DPD_AM[ck]*cos(DPD_PM[ck]);
		DPD_table_Q[ck] = DPD_AM[ck]*sin(DPD_PM[ck]);

		DPD_table_I_tmp[ck] = (int)(DPD_table_I[ck]*(1<<8));
		DPD_table_Q_tmp[ck] = (int)(DPD_table_Q[ck]*(1<<8));

		if(DPD_table_I_tmp[ck]>=0){
            DPD_table_rtl_I[ck] = DPD_table_I_tmp[ck];
		}else{
            DPD_table_rtl_I[ck] = DPD_table_I_tmp[ck] + (1<<10);
		}

		if(DPD_table_Q_tmp[ck]>=0){
            DPD_table_rtl_Q[ck] = DPD_table_Q_tmp[ck];
		}else{
            DPD_table_rtl_Q[ck] = DPD_table_Q_tmp[ck] + (1<<10);
		}

             #ifdef RUN_PT
        		fprintf(fp, "rx dc for look-up table: pa_slope = %f  pa_phase = %f\n",pa_slope[ck], pa_phase[ck]);
             #endif
	}

	
    
	for( ck = 0; ck < 32; ck++){
		addr = 0x0000e534 + (ck*4); 
		i2c_set_reg_fragment(addr,  9,  0, DPD_table_rtl_I[ck]&0x3ff);
		i2c_set_reg_fragment(addr, 25, 16, DPD_table_rtl_Q[ck]&0x3ff);
             #ifdef RUN_PT
        		fprintf(fp,"sh rw.sh 0x0000%.4x 0x%.4x%.4x\n", addr, DPD_table_rtl_Q[ck], DPD_table_rtl_I[ck]);
             #endif
	}
    
    #ifdef RUN_PT
        fclose(fp);
    #endif
    
	new_set_reg(0xe008, 0x00111100);
    new_set_reg(0xe410, 0x00000000);
	new_set_reg(0xe4f0, 0x90008080);
    i2c_set_reg_fragment(0x8010, 28, 28, 1);//enable agc cca detect

	//set RF reg to initial value
	if(1){
		new_set_reg(0xff000008,origin_008);
	    new_set_reg(0xff00001c,origin_01c);
	    new_set_reg(0xff000164,origin_164);
	    new_set_reg(0xff000200,origin_200);
		new_set_reg(0xff000204,origin_204);
		new_set_reg(0xff000208,origin_208);
		//new_set_reg(0xff00020c,origin_20c);
		
		new_set_reg(0xff000308,origin_308);
		new_set_reg(0xff000314,origin_314);
		//new_set_reg(0xff0003f0,origin_3f0);
		new_set_reg(0xff0003e0,origin_3e0);
		new_set_reg(0xff0003e8,origin_3e8);
	}

}
#endif


// do rx dc test through all code

void TestRxDCAll_417(struct calb_rst_out* calb_val,
				     struct rx_dc_result* rx_dc_rst,
				     unsigned int * rst_cnt, 
				     struct rx_dc_dbg * rx_dc_dbg_info, 
				     unsigned int * dbg_inf_cnt, 
				     unsigned int tia_idx,
				     unsigned int lpf_idx )
{
	unsigned int data;
	unsigned int tmp;
	unsigned int rx_dc_rdy;
	unsigned int unlock_cnt = 0;
	unsigned int TIME_OUT_CNT = 300;

	int num;
	unsigned int code_i = 0;
	unsigned int code_q = 0;

	int dci = 0;
	int dcq = 0;
	int max_i = 0;
	int max_j = 0;

	unsigned int base_addr_I;
	unsigned int base_addr_Q;
	unsigned int offset_addr_I;
	unsigned int offset_addr_Q;
	unsigned int fe_gain;
	unsigned int lpf_gain;

	unsigned int origin_rg008;
	unsigned int origin_rg308;
	unsigned int origin_rg318;

	bool rx_dc_manual;
	rx_dc_manual = 1;
	
       new_get_reg(0xff000008, &origin_rg008);
	new_get_reg(0xff000308, &origin_rg308);
	new_get_reg(0xff000318, &origin_rg318);

	//rx dc
	i2c_set_reg_fragment(0xff000318, 6, 6, 0);   //LNA DISABLE
      if(rx_dc_manual == 0)
		new_set_reg(0xff0003b0, 0x00400040); //set manual mode to 0
	else
		new_set_reg(0xff0003b0, 0x01400140); //set manual mode to 1

	fe_gain = 8 + tia_idx;
       lpf_gain = lpf_idx + 1;

	i2c_set_reg_fragment(0xff000308, 15, 12, fe_gain);  //Rx FE gain
	i2c_set_reg_fragment(0xff000308, 11,  8, lpf_gain); //Rx lpf gain

	i2c_set_reg_fragment(0xff000008, 1, 0, 3); //rx mode 
       i2c_set_reg_fragment(0xff000100, 0, 0, 0); //do sx calib
	i2c_set_reg_fragment(0xff000100, 0, 0, 1);
        
	base_addr_I = 0xff000380 + tia_idx*12 + (lpf_idx/4)*4;
	base_addr_Q = 0xff000398 + tia_idx*12 + (lpf_idx/4)*4;

	offset_addr_I = (lpf_idx%4)*8;
	offset_addr_Q = (lpf_idx%4)*8;

	for(num = 0; num <= 127; num++)
      {
	    new_set_reg(0xe008, 0x00000000); //disable esti; tx,rx
	    new_set_reg(0xe018, 0x00010000); //calib mode:enable:0, mode:RX DC
	    new_set_reg(0xec44, 0x00000000); //read response bypass to 0, soft read ready response to 0
	    new_set_reg(0xed04, 0x00000400); //rx dc calc count:1024
	    new_set_reg(0xe008, 0x11111100); //enable esti; open tx,rx
    

		code_i = num;
		code_q = num;

		if(rx_dc_manual == 0){
			i2c_set_reg_fragment(base_addr_I, 6+offset_addr_I, offset_addr_I, num);// I 
			i2c_set_reg_fragment(base_addr_Q, 6+offset_addr_Q, offset_addr_Q, num);// Q
		}else{
			i2c_set_reg_fragment(0xff0003b0,  6,  0, num);
		    i2c_set_reg_fragment(0xff0003b0, 22, 16, num);
		}	
			
		new_set_reg(0xec44, 0x80000000);//read response bypass to 0, soft read ready response to 1
		
		tmp = (num != -1) ? 16 : 24;

		data = i2c_read_reg(0xec44);
		rx_dc_rdy = data & BIT(tmp);

		while(unlock_cnt < TIME_OUT_CNT){
			if(rx_dc_rdy == 0){
				data = i2c_read_reg(0xec44);
				rx_dc_rdy = data & BIT(tmp);

				unlock_cnt = unlock_cnt + 1;
                           #ifdef RUN_PT
				    TRACE("unlock_cnt:%d, RX DC NOT DONE...\n",unlock_cnt);
                           #else
                              // printk("unlock_cnt:%d, RX DC NOT DONE...\n",unlock_cnt); 
                           #endif
			}else{
			        #ifdef RUN_PT
        				TRACE("RX DC ready\n");
                            #else
                           	printk("RX DC ready\n"); 
                            #endif 
				break;
			}
		}

		dci = i2c_read_reg(0xed64);
		dcq = i2c_read_reg(0xed68);

		if(dci & BIT(29)){
			dci = dci - (1<<30);
		}

		dci = dci>>(5+10);

		if(dcq & BIT(29)){
			dcq = dcq - (1<<30);
		}
		dcq = dcq>>(5+10);

   		new_set_reg(0xec44, 0x00000000);

		//TRACE("i=0x%x,  q=0x%x, reg03b0=0x%x\n",code_i,code_q,i2c_read_reg(0xff0003b0));
		rx_dc_dbg_info->code_i = code_i;
		rx_dc_dbg_info->code_q = code_q;
		//rx_dc_dbg_info->rd_reg = i2c_read_reg(0xff0003b0);
		rx_dc_dbg_info ++;

		if(max_i++ > 1024 || max_j++ > 1024){
                    #ifdef RUN_PT
        			TRACE("overflow: max_i %d, max_j %d\n",max_i, max_j);
                    #else
                           printk("overflow: max_i %d, max_j %d\n",max_i, max_j); 
                    #endif
			return;
		}

		*rst_cnt = max_i;
		*dbg_inf_cnt = max_j;
              #ifdef RUN_PT
		    TRACE("----------------------> rx dc out: num = %d dci = %d,  dcq = %d, codei = %d,  codeq = %d\n",
		                num, dci, dcq, code_i, code_q);
              #else
                 printk("----------------------> rx dc out: num = %d dci = %d,  dcq = %d, codei = %d,  codeq = %d\n",
                            num, dci, dcq, code_i, code_q);              
              #endif
              
		rx_dc_rst->num = num;
		rx_dc_rst->dci = dci;
		rx_dc_rst->dcq = dcq;
		rx_dc_rst->code_i = code_i;
		rx_dc_rst->code_q = code_q;
		rx_dc_rst++;
	}//end of for(num = 0;num <= 127;num++) 

       #ifdef RUN_PT
        	TRACE("RX DC calib finish \n");
       #else
            printk("RX DC calib finish \n");
       #endif

	new_set_reg(0xe008, 0x00111100);//disable esti; open tx,rx

	if(1){
		new_set_reg(0xff000008,origin_rg008);
		new_set_reg(0xff000308,origin_rg308);
		new_set_reg(0xff000318,origin_rg318);
	}
}



/*
void TestTXDCALL1(int test_type)
{
	int num;
	unsigned long long pwr;
	unsigned int pwr_low;
	unsigned int pwr_high;
	unsigned int num_tmp = 0;
	unsigned int data0 = 0;
	unsigned int data1 = 0;
	unsigned int dc_rdy = 0;
	unsigned int irr_rdy = 0;
	unsigned int dci = 0;
	unsigned int dcq = 0;	
	unsigned int alpha = 0;
	unsigned int theta = 0;
	int          step;

	new_set_reg(0xe008,0x00000000);
    new_set_reg(0xe008,0x11111100);
    new_set_reg(0xe408,0x00200000);
	new_set_reg(0xe40c,0x90200000);//single tone auto
	new_set_reg(0xe410,0x00000001);
    new_set_reg(0xec50,0x00000000);//response
    new_set_reg(0xe488,0x00000010);
    new_set_reg(0xe018,0x00010100);
	
	new_set_reg(0xed00,0x00001fff);//wait
	new_set_reg(0xed04,0x00004000);//calc:16384
	new_set_reg(0xec38,0x00000004);//alpha factor
   

	new_set_reg(0xec54, 0x78787878);//max
	new_set_reg(0xec58, 0x88888888);//min

	step = 2;

	if(step == 2)
	    new_set_reg(0xec5c, 0x00002222);//step
	else
        new_set_reg(0xec5c, 0x00004444);//step
	
   	new_set_reg(0xe018,0x00010100);

	for(num = 0; num <= ((240/step)*2 + (240/step)*2) + 5; num = num + 1)
	{
		if(num == (240/step)*2 + 1)
		{
			new_set_reg(0xff000308,0x00019c00);//set RF RX FE+LPF gain
		}
		
		new_set_reg(0xec50,0x80000000);
      
		new_get_reg(0xed6c, &pwr_low);
		new_get_reg(0xed70, &pwr_high);
        
		pwr = (unsigned long long)pwr_high << 32;
		pwr |= pwr_low;
		pwr = pwr >> 14;
		new_set_reg(0xec50,0x00000000);

		TRACE("num:%.3d,  pwr %lld\n",num, pwr);
	}

	new_set_reg(0xe008, 0x00111100);
	i2c_set_reg_fragment(0xe4b4, 31,31, 1);
	new_get_reg(0xec4c, &data0);
	new_get_reg(0xec50, &data1);
	dc_rdy = (data0 >> 24 ) & 0x1;
	irr_rdy = (data1 >> 24) & 0x1;
	
	i2c_set_reg_fragment(0xe4b4, 31,31, 1);

	if(dc_rdy == 0|| irr_rdy == 0)
	{
		TRACE("TX DC OR IRR ERR\n");
	}
	else
	{
		dci = data0 & 0xff;
		dcq =  (data0 >> 12) & 0xff;
		alpha = data1 & 0xff;
		theta = (data1 >> 12) & 0xff;

		i2c_set_reg_fragment(0xe4b4, 7,0, dci);
		i2c_set_reg_fragment(0xe4b4, 15,8, dcq);
		i2c_set_reg_fragment(0xe4b8, 7,0, theta);
		i2c_set_reg_fragment(0xe4b8, 15,8, alpha);

		TRACE("dci: 0x%x dcq: 0x%x alpha:0x%x  theta:0x%x\n", dci, dcq, alpha, theta);
	}
}
*/


#define TXDC_CALC_MAX_NUM (1)
void TestTXDCALL1(int test_type)
{
	int num;
	unsigned long long pwr;
	unsigned int pwr_low;
	unsigned int pwr_high;
	unsigned int data0 = 0;
	unsigned int data1 = 0;
	unsigned int dc_rdy = 0;
	unsigned int irr_rdy = 0;
	unsigned char dci = 0;
	unsigned char dcq = 0;	
	unsigned char alpha = 0;
	unsigned char theta = 0;
	
	unsigned char step[TXDC_CALC_MAX_NUM] = {12};
	unsigned int range_max =0x78787878;
	unsigned int range_min = 0x88888888;

	unsigned char range_max_dci =0x78;
	unsigned char range_min_dci = 0x88;
	unsigned char range_max_dcq =0x78;
	unsigned char range_min_dcq = 0x88;
	unsigned char range_max_alpha =0x78;
	unsigned char range_min_alpha = 0x88;
	unsigned char range_max_theta =0x78;
	unsigned char range_min_theta = 0x88;
       int i = 0;
       int range  = 0;
	int tmp =0;

        new_set_reg(0xe008,0x00000000);
        new_set_reg(0xe008,0x11111100);
        new_set_reg(0xe408,0x00200000);
        new_set_reg(0xe40c,0x90200000);//single tone auto
        new_set_reg(0xe410,0x00000001);
        new_set_reg(0xec50,0x00000000);//response
        new_set_reg(0xe488,0x00000010);
        new_set_reg(0xe018,0x00010100);

        new_set_reg(0xed00,0x00001fff);//wait
        new_set_reg(0xed04,0x00004000);//calc:16384
        new_set_reg(0xec38,0x00000004);//alpha factor

        new_set_reg(0xec54, range_max);//max
        new_set_reg(0xec58, range_min);//min

	i2c_set_reg_fragment(0xec5c, 19, 16, 0);//oper bnd


      for( i = 0; i < TXDC_CALC_MAX_NUM ; i++)
      {
	if( range != 0 )
	{
     // range_max =  dci + step[i-1];
     // range_min =  dci - step[i-1];
	
	 tmp = (char)dci + (char)step[i-1];
	 tmp = tmp > 127 ? 127 : (tmp < -128? -128: tmp);
	 range_max_dci = (unsigned char)tmp;
	 tmp = (char)dci - (char)step[i-1];
	 tmp = tmp > 127 ? 127 : (tmp < -128? -128: tmp);
	 range_min_dci = (unsigned char)tmp;


	 tmp = (char)dcq + (char)step[i-1];
	 tmp = tmp > 127 ? 127 : (tmp < -128? -128: tmp);
	 range_max_dcq = (unsigned char)tmp;
	 tmp = (char)dcq - (char)step[i-1];
	 tmp = tmp > 127 ? 127 : (tmp < -128? -128: tmp);
	 range_min_dcq = (unsigned char)tmp;

	 tmp = (char)alpha + (char)step[i-1];
	 tmp = tmp > 127 ? 127 : (tmp < -128? -128: tmp);
	 range_max_alpha= (unsigned char)tmp;
	 tmp = (char)alpha - (char)step[i-1];
	 tmp = tmp > 127 ? 127 : (tmp < -128? -128: tmp);
	 range_min_alpha= (unsigned char)tmp;


	 tmp = (char)theta + (char)step[i-1];
	 tmp = tmp > 127 ? 127 : (tmp < -128? -128: tmp);
	 range_max_theta= (unsigned char)tmp;
	 tmp = (char)theta - (char)step[i-1];
	 tmp = tmp > 127 ? 127 : (tmp < -128? -128: tmp);
	 range_min_theta= (unsigned char)tmp;
	 

	  new_set_reg(0xec54, (range_max_theta & 0xff) <<24| (range_max_alpha & 0xff )<< 16 | (range_max_dcq & 0xff)<< 8| (range_max_dci& 0xff));//max
	  new_set_reg(0xec58, (range_min_theta & 0xff)<<24| (range_min_alpha & 0xff)<< 16 | (range_min_dcq & 0xff)<< 8| (range_min_dci & 0xff));//min
	}
	
	range = (char)range_max_dci - (char) range_min_dci;
      #ifdef RUN_PT
          TRACE("i=%d range=%d max %d min %d\n", i, range, (char)range_max_dci,(char) range_min_dci);
       #else
          printk("i=%d range=%d max %d min %d\n", i, range, (char)range_max_dci,(char) range_min_dci); 
       #endif
       
	new_set_reg(0xec5c, (step[i]&0xf) << 12| (step[i]&0xf) << 8 | (step[i]&0xf) << 4 | (step[i]&0xf));
   	new_set_reg(0xe018,0x00010100);

	for(num = 0; num <= ((range/step[i])*2 + (range/step[i])*2) + 5; num = num + 1)
	{
		if(num == (range/step[i])*2 + 1)
		{
			new_set_reg(0xff000308,0x00019c00);//set RF RX FE+LPF gain
		}
		
		new_set_reg(0xec50,0x80000000);

		new_get_reg(0xed6c, &pwr_low);
		new_get_reg(0xed70, &pwr_high);
		pwr = (unsigned long long)pwr_high << 32;
		pwr |= pwr_low;
		pwr = pwr >> 14;
		
		new_set_reg(0xec50,0x00000000);
             #ifdef RUN_PT
		    TRACE("num:%.3d,  pwr %lld\n",num, pwr);
             #else
                 printk("num:%.3d,  pwr %lld\n",num, pwr);
             #endif
	}

	new_set_reg(0xe008, 0x00111100);
	i2c_set_reg_fragment(0xe4b4, 31,31, 1);
	new_get_reg(0xec4c, &data0);
	new_get_reg(0xec50, &data1);
	dc_rdy = (data0 >> 24 ) & 0x1;
	irr_rdy = (data1 >> 24) & 0x1;
	
	i2c_set_reg_fragment(0xe4b4, 31,31, 1);

	if(dc_rdy == 0|| irr_rdy == 0)
	{
	       #ifdef RUN_PT
		    TRACE("TX DC OR IRR ERR\n");
             #else
                 printk("TX DC OR IRR ERR\n"); 
             #endif
	}
	else
	{
		dci = data0 & 0xff;
		dcq =  (data0 >> 12) & 0xff;
		alpha = data1 & 0xff;
		theta = (data1 >> 12) & 0xff;

		i2c_set_reg_fragment(0xe4b4, 7,0, dci);
		i2c_set_reg_fragment(0xe4b4, 15,8, dcq);
		i2c_set_reg_fragment(0xe4b8, 7,0, theta);
		i2c_set_reg_fragment(0xe4b8, 15,8, alpha);

             #ifdef RUN_PT
		    TRACE("dci: 0x%x dcq: 0x%x alpha:0x%x  theta:0x%x\n", dci, dcq, alpha, theta);
             #else
                 printk("dci: 0x%x dcq: 0x%x alpha:0x%x  theta:0x%x\n", dci, dcq, alpha, theta);
             #endif
	}
 }
}

void TestTXDCALL(int test_type)
{
	int num;
	unsigned long long pwr;
	unsigned int pwr_low;
	unsigned int pwr_high;
	unsigned int num_tmp = 0;

        new_set_reg(0xe008,0x00000000);
        new_set_reg(0xe008,0x11111100);
        new_set_reg(0xe408,0x00200000);
        new_set_reg(0xe40c,0x10200000);
        new_set_reg(0xe410,0x00000001);
        new_set_reg(0xec50,0x00000000);//response
        new_set_reg(0xe488,0x00000010);
        new_set_reg(0xe018,0x00010100);

	i2c_set_reg_fragment(0xe4b4, 31,31, 1);

	new_set_reg(0xed00,0x00000150);//wait
	new_set_reg(0xed04,0x00002000);//calc:8192

	for(num = -128; num < 128; num = num + 4)
	//for(num = 0; num < 2; num++)
	{
		new_set_reg(0xe008,0x00000000);
		new_set_reg(0xe008,0x11111100);

		if(num < 0){
			num_tmp = num + 128;
		}else{
			num_tmp = num;
		}

        if(test_type == 0){
	       i2c_set_reg_fragment(0xe4b4, 7, 0, num_tmp);//set DC_I
		}else if(test_type == 1){
		   i2c_set_reg_fragment(0xe4b4, 15, 8, num_tmp);//set DC_I
		}else if(test_type == 3){
			i2c_set_reg_fragment(0xe4b8,  7, 0, num_tmp);//set alpha
		}else if(test_type == 4){
			i2c_set_reg_fragment(0xe4b8, 15, 8, num_tmp);//set alpha
		}
		
		new_set_reg(0xe018,0x00010100);

             new_set_reg(0xec50,0x80000000);
        
		new_get_reg(0xed6c, &pwr_low);
		new_get_reg(0xed70, &pwr_high);
        
		pwr = (unsigned long long)pwr_high << 32;
		pwr |= pwr_low;

		pwr = pwr >> 13;

		new_set_reg(0xec50,0x00000000);
             #ifdef RUN_PT
        		TRACE("num:%.5d,  pwr %ld\n",num, pwr);
             #else
                    printk("num:%.5d,  pwr %lld\n",num, pwr);
             #endif
	}
}

void TestRxDCAll(void)
{

	int datai, dataq;
	int num;
	unsigned int value_i,value_q;

	i2c_write_reg(0xff0003b0,0x01400140);
	i2c_write_reg(0xff000308,0x00019c01);
	value_i = value_q = 0x40;
	i2c_set_reg_fragment(0xff0003b0,7,0,value_i);
	i2c_set_reg_fragment(0xff0003b0,22,16,value_q);

	i2c_write_reg(0xed04,0x00000400);
	for(num = 0; num < 128;num++)
	//for(num = 60; num < 90;num++)
	{	
		i2c_write_reg(0xec44, 0x00000000);
		i2c_write_reg(0xe008, 0x10101000);

		i2c_write_reg(0xe018, 0x00010100);
		i2c_write_reg(0xe018, 0x00010000);

		i2c_write_reg(0xe008,0x11111100);

		i2c_set_reg_fragment(0xff0003b0, 6, 0, num);
		i2c_set_reg_fragment(0xff0003b0, 22, 16, num);

		i2c_write_reg(0xec44,0x80000000);
		datai = i2c_read_reg(0xed64);
		dataq = i2c_read_reg(0xed68);

		
		if(datai & BIT(29)){
                    datai = datai - (1<<30);
		}
            datai = datai>>20;

		if(dataq & BIT(29)){
                    dataq = dataq - (1<<30);
		}
            dataq = dataq>>20;
		
             #ifdef RUN_PT
        		TRACE("---> num = %3d,   %d    %d\n",num, datai, dataq);
             #else
                    printk("---> num = %3d,   %d    %d\n",num, datai, dataq);
             #endif
	}
}


void TestRxLNA(struct rx_lna_output1* rx_lna_out1, unsigned int *out_cnt1,struct rx_lna_output2* rx_lna_out2, unsigned int *out_cnt2)
{
      unsigned int code=0;
	unsigned int data=0;
	unsigned int code_out=0;

	unsigned int pwr=0;
	unsigned int pwr_out=0;


	unsigned char chn_idx = 0;
       int max_i = 0;
	int max_k = 0;

	unsigned char freq_start = 20;
	unsigned char freq_end = 180 + 1;


    //RX LNA
    new_set_reg(0xff000208,0x10110360);
    new_set_reg(0xff000164,0x000017ff);
    new_set_reg(0xff000314,0x03fff9d7);
    new_set_reg(0xff00001c,0x000000ff);
    new_set_reg(0xff000008,0x00002312);
    //new_set_reg(0xff000308,0x00012401);  
	new_set_reg(0xff000308,0x00012101);
    new_set_reg(0xff000378,0x00022522);

	
    new_set_reg(0xff00020c,0x00001077);
    new_set_reg(0xff0003f0,0x00000508);

    //digital
	new_set_reg(0xe5b8, 0x0000000a);
	new_set_reg(0xec38, 0x00000005); //set LNA alpha factor
	new_set_reg(0xed00, 0x00001000); //set LNA wait count(4096)
	new_set_reg(0xed04, 0x00001000); //set LNA calc count(4096)

	new_set_reg(0xff000118, 0x000a0751);
	new_set_reg(0xff00011c, 0x000014f0);       
	new_set_reg(0xff000180, 0x00000701); 
	new_set_reg(0xff00018c, 0x17828900); 
	new_set_reg(0xff0001a0, 0x00001010); 

	i2c_set_reg_fragment(0xff000320, 12, 12, 1);//set RF LNA man mode to 1
    

	for(chn_idx = freq_start; chn_idx < freq_end; chn_idx = chn_idx + 20)
	{
		i2c_set_reg_fragment(0xff000118, 23, 16, chn_idx);//set RF frequency
		
		i2c_set_reg_fragment(0xff00011c,  0,  0, 0);//0:sx auto channel; 1: sx manual channel 
             i2c_set_reg_fragment(0xff000118,  0,  0, 0);//0(1/4 * VCO):PDIV to digital
             i2c_set_reg_fragment(0xff000118,  7,  4, 5);//3(80M); 5(40M):osc
             i2c_set_reg_fragment(0xff00018c, 25, 25, 0);//0(1/4 * VCO):FDIV to analog
            i2c_set_reg_fragment(0xff0001a0, 12, 12, 1);//1(double osc bypass: 40M); 0:80M 

		
		new_set_reg(0xff000100,0x00003f10);
		new_set_reg(0xff000100,0x00003f11);

		new_set_reg(0xe008, 0x00000000);//disable esti,tx,rx
		
		new_set_reg(0xe018, 0x00010400);//calib mode:enable:1, mode:RX LNA 4

		new_set_reg(0xe408, 0x00200000);//close tone1
		//new_set_reg(0xe40c, 0x10200000);//open tone2
		new_set_reg(0xe40c, 0x90066666);//open tone2(1M)
		new_set_reg(0xe410, 0x00000001);//1: select single tone 
		new_set_reg(0xec48, 0x00000000);//response

		new_set_reg(0xe008, 0x11111100);

		for(code = 0; code < 16; code ++)
		{
			i2c_set_reg_fragment(0xff000320, 11, 8, code);
			
			i2c_set_reg_fragment(0xec48, 31, 31, 1);
			
			data = i2c_read_reg(0xec48);
			
			if((data & BIT(8))){
				
				i2c_set_reg_fragment(0xec48, 31, 31, 0);
	                
				new_get_reg(0xed74, &pwr);
				
				pwr_out = pwr;//<17+12.1+12>

                           #ifdef RUN_PT
				    TRACE("code:%.2d,  pwr %d\n",code, pwr_out);// TO OUTPUT
                            #else
                               printk("code:%.2d,  pwr %d\n",code, pwr_out);// TO OUTPUT 
                            #endif
                            
                           rx_lna_out1->code = code;
                           rx_lna_out1->pwr = pwr;
				rx_lna_out1++;

                           if(max_i++ > 1024)
				{
                                #ifdef RUN_PT
                                    TRACE("overflow max_i=%d\n", max_i);
                                 #else
                                    printk("overflow max_i=%d\n", max_i);
                                 #endif
                                 
					return;
				}
				*out_cnt1 = max_i;

			}
                    else
			{
                            #ifdef RUN_PT
                                TRACE("RXLNA Fail 1, code %d\n",code);
                            #else
                                printk("RXLNA Fail 1, code %d\n",code);
                            #endif
                            
				break;
			}
		}

             data = i2c_read_reg(0xec48);
		if(data & BIT(16)){
			code_out = i2c_get_reg_fragment(0xec48, 3, 0);
		}else{
		       #ifdef RUN_PT
        			TRACE("RXLNA Fail 0, code %d\n",code);
                     #else
                           printk("RXLNA Fail 0, code %d\n",code); 
                     #endif
		}
		
		i2c_set_reg_fragment(0xff000320, 11, 8, code_out);

             #ifdef RUN_PT
		    TRACE("chn_idx：%d, RXLNA code out %d\n",chn_idx, code_out); // TO OUTPUT
		#else
                 printk("chn_idx：%d, RXLNA code out %d\n",chn_idx, code_out); // TO OUTPUT
            #endif
            
              rx_lna_out2->chn_idx = chn_idx;
              rx_lna_out2->code_out = code_out;
		rx_lna_out2++;

		if(max_k++ > 1024)
		{
                    #ifdef RUN_PT
                        TRACE("overflow max_k=%d\n", max_k);
                    #else
                        printk("overflow max_k=%d\n", max_k);
                    #endif
			return;
		}
		*out_cnt2 = max_k;

		new_set_reg(0xe008, 0x11111100);
		new_set_reg(0xe40c, 0x90066666);
	}
}


void TestRxLNA_417(struct calb_rst_out* calb_val,
				   bool tx_dciq_manual,
				   unsigned int chn,
				   unsigned int chn_idx,
				   struct rx_lna_output1* rx_lna_out1, 
				   unsigned int *out_cnt1,
				   struct rx_lna_output2* rx_lna_out2, 
				   unsigned int *out_cnt2)
{
	unsigned int code=0;
	unsigned int data=0;
	unsigned int code_out=0;

	unsigned int pwr=0;
	unsigned int pwr_out=0;

	int max_i = 0;
	int max_k = 0;

	unsigned int origin_rg008 =0;
	unsigned int origin_rg164 =0;
	unsigned int origin_rg01c =0;
	unsigned int origin_rg208 =0;
	unsigned int origin_rg308 =0;
	unsigned int origin_rg314 =0;
	unsigned int origin_rg320 =0;
    unsigned int origin_rg3f0 =0;

	//digital
    unsigned int origin_e008;
	unsigned int origin_b234;
	unsigned int origin_e018;
	unsigned int origin_e40c;
	unsigned int origin_e410;
	unsigned int origin_e4f0;
	unsigned int origin_e82c;
	unsigned int origin_e5b8;
      unsigned int origin_ec38;
	unsigned int origin_ed00;
	unsigned int origin_ed04;
	unsigned int origin_ec48;

	unsigned int tmp = 0;
	bool rx_lna_manual = 0;

	unsigned int rx_lna_ready = 0;
      unsigned int unlock_cnt = 0;
	unsigned int TIME_OUT_CNT = 300;
         //[90 10 50 130 165 190 30 70 110 150]
      unsigned int chn_idx_tmp = 0;


	new_get_reg(0xff000008, &origin_rg008);
	new_get_reg(0xff00001c, &origin_rg01c);
	new_get_reg(0xff000164, &origin_rg164);
	new_get_reg(0xff000208, &origin_rg208);
	new_get_reg(0xff000308, &origin_rg308);
	new_get_reg(0xff000314, &origin_rg314);
	new_get_reg(0xff000320, &origin_rg320);
	new_get_reg(0xff0003f0, &origin_rg3f0);

	i2c_set_reg_fragment(0xff00001c,  7,  0, 0xff);
	i2c_set_reg_fragment(0xff000164, 10, 10, 1);
	i2c_set_reg_fragment(0xff000208, 28, 28, 1);
	i2c_set_reg_fragment(0xff000308, 15,  8, 0x25);//0x26
	i2c_set_reg_fragment(0xff000314, 31,  0, 0x03fff9d7);

	if(rx_lna_manual == 0){
		i2c_set_reg_fragment(0xff000320, 12, 12, 0);//set RF LNA man mode to 0
	}else{
		i2c_set_reg_fragment(0xff000320, 12, 12, 1);//set RF LNA man mode to 1
	}
	i2c_set_reg_fragment(0xff0003f0, 13, 12, 0);//set tx lpf gain

	i2c_set_reg_fragment(0xff000008, 1, 0, 2);//set tx mode
	i2c_set_reg_fragment(0xff000100, 0, 0, 0);//do sx calib
       i2c_set_reg_fragment(0xff000100, 0, 0, 1);

	//i2c_set_reg_fragment(0xff00011c,  0,  0, 0);//0:sx auto channel; 1: sx manual channel 
	//i2c_set_reg_fragment(0xff000118,  0,  0, 0);//0(1/4 * VCO):PDIV to digital
	//i2c_set_reg_fragment(0xff000118,  7,  4, 5);//3(80M); 5(40M):osc
	//i2c_set_reg_fragment(0xff00018c, 25, 25, 0);//0(1/4 * VCO):FDIV to analog
	//i2c_set_reg_fragment(0xff0001a0, 12, 12, 1);//1(double osc bypass: 40M); 0:80M 

	

	new_get_reg(0xe008, &origin_e008);
	new_get_reg(0xb234, &origin_b234);
	new_get_reg(0xe018, &origin_e018);
	new_get_reg(0xe40c, &origin_e40c);
	new_get_reg(0xe410, &origin_e410);
	new_get_reg(0xe4f0, &origin_e4f0);
	origin_e82c=i2c_get_reg_fragment(0xe82c, 4, 4);
	new_get_reg(0xe5b8, &origin_e5b8);
	new_get_reg(0xec38, &origin_ec38);
	new_get_reg(0xed00, &origin_ed00);
	new_get_reg(0xed04, &origin_ed04);
	new_get_reg(0xec48, &origin_ec48);


	new_set_reg(0xe008, 0x00000000);//disable esti,tx,rx
    new_set_reg(0xb234, 0x00000001);//close open spectrum,must be closed in single tone mode
	new_set_reg(0xe018, 0x00010400);//calib mode:enable:1, mode:RX LNA 4

	//new_set_reg(0xe408, 0x00200000);//close tone1
	new_set_reg(0xe40c, 0x90066666);//open tone2(1M)
	new_set_reg(0xe410, 0x00000001);//1: select single tone 

	new_set_reg(0xe4f0, 0x15008080);   //TX DPD bypass 
	i2c_set_reg_fragment(0xe82c, 4, 4, 1); //RX IRR bypass

	if(tx_dciq_manual){
        i2c_set_reg_fragment(0xe4b4, 31, 31, 1);//set TX DC/IQ comp manual to 1

	   
		if(chn_idx <= 4)
			chn_idx_tmp = chn_idx;
		else if(chn_idx == 5)
			chn_idx_tmp = 1;
		else if(chn_idx == 6)
			chn_idx_tmp = 2;
		else if(chn_idx == 7)
			chn_idx_tmp = 0;
		else if(chn_idx == 8)
			chn_idx_tmp = 3;
		else if(chn_idx == 9)
			chn_idx_tmp = 4;

        i2c_set_reg_fragment(0xe4b4,  7,  0, calb_val->tx_dc_irr[chn_idx_tmp][2].tx_dc_i);//set TX DCI    using tx gain:770(idx:2)
		i2c_set_reg_fragment(0xe4b4, 15,  8, calb_val->tx_dc_irr[chn_idx_tmp][2].tx_dc_q);//set TX DCQ
		i2c_set_reg_fragment(0xe4b8,  7,  0, calb_val->tx_dc_irr[chn_idx_tmp][2].tx_theta);//set TX theta
        i2c_set_reg_fragment(0xe4b8, 15,  8, calb_val->tx_dc_irr[chn_idx_tmp][2].tx_alpha);//set TX alpha
	}

	if(chn >= 100 && chn <= 180){
		//new_set_reg(0xe5b8, 0x00000004);
		new_set_reg(0xe5b8, 0x00000002);
	}else{
		new_set_reg(0xe5b8, 0x00000002);
	}

	new_set_reg(0xec38, 0x00000005); //set LNA alpha factor
	new_set_reg(0xed00, 0x00001000); //set LNA wait count(4096)
	new_set_reg(0xed04, 0x00001000); //set LNA calc count(4096)

	new_set_reg(0xec48, 0x00000000);//close soft response

	new_set_reg(0xe008, 0x11111100);//enable esti,tx,rx

	for(code = 0; code < 16; code ++){
		if(rx_lna_manual == 0){
			//[190 10 30 50 70 90 110 130 150 165]
			if(chn >= 180){//if chn180=4900; if chn180=5900, not support, need to change code
				tmp = 0;
				i2c_set_reg_fragment(0xff00031c, tmp+3, tmp, code);
			 }else if(chn >= 0 && chn < 140){
				tmp = ( (int)((chn/10)/2) + 1) * 4 ;
				i2c_set_reg_fragment(0xff00031c, tmp+3, tmp, code);
			}else if(chn >= 140 && chn < 180){
				tmp = ( (int)((chn/10)/2) - 7) * 4 ;
				i2c_set_reg_fragment(0xff000320, tmp+3, tmp, code);
			}
		}else{
		    i2c_set_reg_fragment(0xff000320, 11, 8, code);
		}

		i2c_set_reg_fragment(0xec48, 31, 31, 1);//open soft response

		data = i2c_read_reg(0xec48);

		rx_lna_ready = data & BIT(8);
	   		
		while(unlock_cnt < TIME_OUT_CNT){
			if(rx_lna_ready == 0){
				data = i2c_read_reg(0xec48);

		        rx_lna_ready = data & BIT(8);
	            
				unlock_cnt = unlock_cnt + 1;

                           #ifdef RUN_PT
        				TRACE("unlock_cnt:%d, RX LNA NOT DOING...\n", unlock_cnt);
                           #else
                                 //printk("unlock_cnt:%d, RX LNA NOT DOING...\n", unlock_cnt);
                           #endif
                
			}else{
			        #ifdef RUN_PT
				        TRACE("RX LNA READY\n");
                            #else
                                   //printk("RX LNA READY\n"); 
                            #endif
                    
				break;
			}
		}

		i2c_set_reg_fragment(0xec48, 31, 31, 0);
			
		new_get_reg(0xed74, &pwr);
				
		pwr_out = pwr;//<17.1>

              #ifdef RUN_PT
        		TRACE("code:%.2d,  pwr %d\n",code, pwr_out);
               #else
                    printk("code:%.2d,  pwr %d\n",code, pwr_out);
               #endif
               
		rx_lna_out1->code = code;
		rx_lna_out1->pwr = pwr;
		rx_lna_out1++;
		if(max_i++ > 1024)
		{
                    #ifdef RUN_PT
                        TRACE("overflow max_i=%d\n", max_i);
                    #else
                        printk("overflow max_i=%d\n", max_i);
                    #endif
			return;
		}
		*out_cnt1 = max_i;
	}//end of for(code = 0; code < 16; code ++)
    
	unlock_cnt = 0;
	data = i2c_read_reg(0xec48);
	rx_lna_ready = data & BIT(16);

	while(unlock_cnt < TIME_OUT_CNT){
		if(rx_lna_ready == 0){
			data = i2c_read_reg(0xec48);

	        rx_lna_ready = data & BIT(16);
            
			unlock_cnt = unlock_cnt + 1;
                    #ifdef RUN_PT
        			TRACE("unlock_cnt:%d, RX LNA NOT DOING...\n", unlock_cnt);
                    #else
                           //printk("unlock_cnt:%d, RX LNA NOT DOING...\n", unlock_cnt);
                    #endif
		}else{
                     #ifdef RUN_PT
                            TRACE("RX LNA READY\n");
                     #else
                           // printk("RX LNA READY\n");
                     #endif
                     
                    break;
		}
	}

	code_out = i2c_get_reg_fragment(0xec48, 3, 0);
    
	if(rx_lna_manual == 0){
		//[190 10 30 50 70 90 110 130 150 165]
		if(chn >= 180){//if chn180=4900; if chn180=5900, not support, need to change code
			tmp = 0;
			i2c_set_reg_fragment(0xff00031c, tmp+3, tmp, code_out);
		}else if(chn >= 0 && chn < 140){
			tmp = ( (int)((chn/10)/2) + 1) * 4 ;
			i2c_set_reg_fragment(0xff00031c, tmp+3, tmp, code_out);
		}else if(chn >= 140 && chn < 180){
			tmp = ( (int)((chn/10)/2) - 7) * 4 ;
			i2c_set_reg_fragment(0xff000320, tmp+3, tmp, code_out);
		}
	}else{
		i2c_set_reg_fragment(0xff000320, 11, 8, code_out);
	}

	new_set_reg(0xe008, 0x00111100);//disable esti,open tx, rx

	calb_val->rx_lna[chn_idx].rx_lna_code = code_out;
       #ifdef RUN_PT
	    TRACE("chn_idx：%d, RXLNA code out %d\n", chn_idx, code_out);
        #else
           printk("chn_idx：%d, RXLNA code out %d\n", chn_idx, code_out); 
        #endif
        
	rx_lna_out2->chn_idx = chn_idx;
	rx_lna_out2->code_out = code_out;
	rx_lna_out2++;

	if(max_k++ > 1024){
             #ifdef RUN_PT
		    TRACE("overflow max_k=%d\n", max_k);
             #else
                 printk("overflow max_k=%d\n", max_k);
             #endif
             
		return;
	}

	*out_cnt2 = max_k;
    
	//set RF reg to initial value
	if(1){
		new_set_reg(0xff000008,origin_rg008);
		new_set_reg(0xff00001c,origin_rg01c);
		new_set_reg(0xff000164,origin_rg164);
		new_set_reg(0xff000208,origin_rg208);
		new_set_reg(0xff000308,origin_rg308);
		new_set_reg(0xff000314,origin_rg314);
		//new_set_reg(0xff000320,origin_rg320);
		new_set_reg(0xff0003f0,origin_rg3f0);
	}
    
	//set digital reg to initial value
	if(1){
		new_set_reg(0xe008, origin_e008);
		new_set_reg(0xb234, origin_b234);
		new_set_reg(0xe018, origin_e018);
		new_set_reg(0xe40c, origin_e40c);
		new_set_reg(0xe410, origin_e410);
		new_set_reg(0xe4f0, origin_e4f0);
		i2c_set_reg_fragment(0xe82c, 4, 4, origin_e82c&0x1);
		new_set_reg(0xe5b8, origin_e5b8);
		new_set_reg(0xec38, origin_ec38);
		new_set_reg(0xed00, origin_ed00);
		new_set_reg(0xed04, origin_ed04);
		new_set_reg(0xec48, origin_ec48);
	}
}



#ifndef RUN_PT
int t9023_dpd_tx_func(void *param)
{
        
        struct hal_private *hal_priv = hal_get_priv();
        int len = 0;
        struct NullDataCmd null_data;
        unsigned int pwr_save = 0;
        unsigned int pwr_flag = 0;
        unsigned int qos = 0;
        unsigned int ac = 0;
        unsigned char phony_macaddr[] = {0x00,0x22,0x33, 0x44, 0x55,0x66};
        unsigned int   phony_sn = 0;
        unsigned reg_chk = 0;
        
        memset(&null_data, 0, sizeof(struct NullDataCmd));
        
        null_data.vid = 0;
        null_data.pwr_save= pwr_save;
        null_data.pwr_flag = pwr_flag;
        null_data.staid =1 ;
       
        null_data.bw = CHAN_BW_20M;//CHAN_BW_40M;
        null_data.rate = WIFI_11AC_MCS5;//WIFI_11AC_MCS5;
        qos = 1; // for ht/vht
        new_set_reg(0x0000b22c, 0x80000000);// rf auto switch

        if (qos == 0)
        {
            null_data.qos = 0;
            null_data.tid = 0;
            len = sizeof(struct wifi_frame) + FCS_LEN;
        }
        else
        {
            null_data.qos = qos;
            null_data.tid = WME_AC_TO_TID(ac);
            len = sizeof(struct wifi_qos_frame) + FCS_LEN;
          
        }
        
        memcpy(&null_data.dest_addr, phony_macaddr, WIFINET_ADDR_LEN);
     
        while(!hal_priv->hi_dpd_tx_thread_quit)
            {	   
		   if(phony_sn > 4095)
		   {
                        phony_sn = 0;
		   }
                null_data.sn =phony_sn;
                phony_sn++;

                new_get_reg(0x0000b22c, &reg_chk);
                printk("reg_b22c 0x%x\n", reg_chk);
                    
		   hal_priv->hal_ops.phy_send_null_data(null_data, len);
                 udelay(10);
            }

        printk("t9023_dpd_tx_thread stop, last sn %d\n", null_data.sn);
        complete_and_exit(&hal_priv->hi_dpd_tx_thread_completion, 0);

        return 0;
}
#endif


void set_txm_ct(unsigned int ch)
{
	//TXM cap tune frequency
	unsigned int txm_ct_chn[9]  = {10, 27, 47, 67, 87, 107, 127, 147, 167};
	unsigned int txm_ct_vaule[9]= { 2 , 4,  6, 8,  10, 12,  12,  14,  15};
	int ck = 0; 
	//set txm ct according to frequency
    int txm_ct_chidx = 0;
    int min_delta_chn = 1000;
	int delta_chn = 0 ;
	int tmp = 0;
	for(ck = 0; ck < sizeof(txm_ct_chn)/sizeof(txm_ct_chn[0]); ck++){
		tmp = txm_ct_chn[ck]- ch;
		delta_chn = abs(tmp);
		if( delta_chn < min_delta_chn){
			min_delta_chn = delta_chn;
			txm_ct_chidx = ck;
		}
	}
    i2c_set_reg_fragment(0xff000200, 3, 0, txm_ct_vaule[txm_ct_chidx]&0xf);
}

void t9023_channel_switch(int p_chn)
{
    //set RF reg
    unsigned int origin_308 = 0;
    unsigned int origin_118 = 0;
    unsigned int txiqdc_gain[] = {0x773,0x772,0x770,0x733,0x732,0x730,0x713,0x712,0x710};
    unsigned int dpd_bypass = 1;
    unsigned int irr_bypass = 1;
    
    //set RF register
    //TX RF reg
    int gain_idx = 0;//max gain(773)

    i2c_set_reg_fragment(0xff00020c,  2,  0, (txiqdc_gain[gain_idx]>>8)&0x7);   //Tx PA
    i2c_set_reg_fragment(0xff00020c,  6,  4, (txiqdc_gain[gain_idx]>>4)&0x7);   //Tx MXR
    i2c_set_reg_fragment(0xff0003f0, 13, 12, (txiqdc_gain[gain_idx])&0x7);      //TX Lpf

    //RX RF reg  
    //rx rc & rx dc is auto selected according to channel from rf register(already done in calib)

    //set rx fe+lpf gain
    i2c_set_reg_fragment(0xff000308,  0,  0, 1);   //set man mode to 1
    i2c_set_reg_fragment(0xff000308, 15,  8, 0x9c);//set rx fe+lpf gain
    i2c_set_reg_fragment(0xff000308, 16, 16, 0);   //set rx fe+lpf to auto gain

    //set RX LNA
    //auto selected according to channel from rf register(already done in calib)
    //new_set_reg(0xff000320, 0x00551c23);//RX LNA man set to 12 for 5050

    //set channel
    i2c_set_reg_fragment(0xff000118, 23, 16, p_chn & 0xff);

    i2c_set_reg_fragment(0xff000008, 1, 0, 2);//set tx mode
    //triger sx freq lock(do sx calib)
    i2c_set_reg_fragment(0xff000100, 0, 0, 0);
    i2c_set_reg_fragment(0xff000100, 0, 0, 1);

    i2c_set_reg_fragment(0xff000008,  4,  4, 0);//set RG_WF_MODE_MAN_MODE to 0
    i2c_set_reg_fragment(0xff000008,  9,  8, 2);//set TX mode to 2 in auto mode
    i2c_set_reg_fragment(0xff000008, 13, 12, 3);//set RX mode to 3 in auto mode


    //TX digital reg
    //set TX DC/IQ digital comp value


    //set TX DPD digital comp coeff
    i2c_set_reg_fragment(0xe4f0, 24, 24, 1);//set dpd comp type to 1:coeff

    if(dpd_bypass)
    	i2c_set_reg_fragment(0xe4f0, 26, 26, 1);//set dpd comp bypass to 1
    else
    	i2c_set_reg_fragment(0xe4f0, 26, 26, 0);//set dpd comp bypass to 0

    i2c_set_reg_fragment(0xe4f0, 28, 28, 1);//set dpd comp bypass man to 1

    i2c_set_reg_fragment(0xe4f0, 31, 31, 0);//set dpd comp manual mode to 0
    	
    //RX digital reg 


    //set rx irr digital comp coeff
    if(irr_bypass)
    	new_set_reg(0xe82c, 0x00000a10);//set RXIRR comp bypass 1, manual mode to 0
    else
    	new_set_reg(0xe82c, 0x00000a00);//set RXIRR comp bypass 0, manual mode to 0

    //set digital to TX&RX switch mode
    new_set_reg(0xe008, 0x00000000); //disable esti, enable tx, rx
    new_set_reg(0xe410, 0x00000000); //select ofdm signal

    new_set_reg(0xe5b8, 0x00000008);//bb gain
    i2c_set_reg_fragment(0xe4b8, 31, 24, 0x80);//set txpwctrl gain
    i2c_set_reg_fragment(0xe4b8, 16, 16, 1);//set txpwctrl gain manual to 1

    new_get_reg(0xff000308, &origin_308);
    new_get_reg(0xff000118, &origin_118);
    i2c_set_reg_fragment(0x8010, 28, 28, 0);//enable agc cca detect
#ifdef RUN_PT
    TRACE("rf_reg_308 0x%x, rf_reg_118 0x%x\n", origin_308, origin_118);
#else
	printk("rf_reg_308 0x%x, rf_reg_118 0x%x\n", origin_308, origin_118);
#endif
}

void t9023_bw_switch(int bw_idx)
{
    //set RF bw & adc
    if (bw_idx == 0) {
        i2c_set_reg_fragment(0xff000308, 3, 2, 0x0);//RF band:20M
        i2c_set_reg_fragment(0xff000308, 4, 4, 0);  // adc 160M

    } else if (bw_idx == 1) {
        i2c_set_reg_fragment(0xff000308, 3, 2, 0x1); //RF band:40M
        i2c_set_reg_fragment(0xff000308, 4, 4, 0);   //adc 160M

    } else if (bw_idx == 2) {
        i2c_set_reg_fragment(0xff000308, 3, 2, 0x2);//RF band:80M
        i2c_set_reg_fragment(0xff000308, 4, 4, 1);  //adc 320
    } else {
        #ifdef RUN_PT
            TRACE("bw_idx invalide%d", bw_idx);
        #else
            printk("%s(%d) bw_idx invalide%d", __func__, __LINE__, bw_idx);
        #endif
    }
}


void do_one_gain( unsigned int calb_item)
{
    unsigned int chn = 149;
    unsigned int chn_tmp;
    unsigned int origin_20c = 0;
	unsigned int origin_3f0 = 0;

	unsigned int tia_idx = 0;
	unsigned int lpf_idx = 0;
	unsigned int bw_idx = 0;
	unsigned int origin_3b0 = 0;
#ifdef RUN_PT
	char path[1024] = ALL_CALB_OUTPUT_PATH ;
	FILE *fp = NULL;
	
	sprintf(path,"%s%s",g_CurrentPath,ALL_CALB_OUTPUT_PATH);
	
	fp = fopen((char *)path, "a+");
	if (!fp){
		TRACE(" output file open fail\r\n");
		return ;
	}
#endif

	if((calb_item >> 0) & 0x1)
	{
		init_t9023();
	      i2c_set_reg_fragment(0xff000118,23,16,chn&0xff);

	      set_txm_ct(chn);  
	}


	if((calb_item >> 1) & 0x1)
	{
      		do_rx_rc_calb(&calb_val);
           
	}


	if((calb_item >> 2) & 0x1)
	{
	// do rx-dc

	for(bw_idx = 0; bw_idx < 3; bw_idx++){
        reset_rxdc_rf_reg();
    
		for(tia_idx = 0; tia_idx < 2; tia_idx++)
		{
			for(lpf_idx = 0; lpf_idx < 12; lpf_idx++)
			{ 
				do_rxdc_calb(&calb_val, bw_idx, tia_idx, lpf_idx);
#ifdef RUN_PT
				fprintf(fp, "%d %d %d %d %d %d %d\n",
						bw_idx, 
						tia_idx, 
						lpf_idx, 
						calb_val.rx_dc[bw_idx][tia_idx][lpf_idx].rxdc_i, 
						calb_val.rx_dc[bw_idx][tia_idx][lpf_idx].rxdc_q, 
						calb_val.rx_dc[bw_idx][tia_idx][lpf_idx].rxdc_codei,
						calb_val.rx_dc[bw_idx][tia_idx][lpf_idx].rxdc_codeq);
#endif
			}
		}
	 }

#ifdef RUN_PT
	 fclose(fp);
#endif	


}


if((calb_item >> 3) & 0x1)
{
    new_get_reg(0xff0003b0, &origin_3b0);
	new_get_reg(0xff00020c, &origin_20c);
	new_get_reg(0xff0003f0, &origin_3f0);

	new_set_reg(0xff00020c, 0x00001077);//TX V2I gain(0), MIX gain(7), PA gain(7)
	//new_set_reg(0xff00020c, 0x00001033);//TX V2I gain(0), MIX gain(7), PA gain(7)
	new_set_reg(0xff0003f0, 0x00003508);//TX LPF gain(3)
    
	do_rxdc_for_txirr();
	do_tx_dc_irr_calb(&calb_val, 0, 0);
	new_set_reg(0xff00020c, origin_20c);
	new_set_reg(0xff0003f0, origin_3f0);
	new_set_reg(0xff0003b0, origin_3b0);
}

#if 0	
//
if((calb_item >> 4) & 0x1)
{
	new_get_reg(0xff00020c,&origin_20c);
	new_get_reg(0xff0003f0,&origin_3f0);

	new_set_reg(0xff00020c,0x00001077);//tx v2i.mixr pa gain(default)
    //new_set_reg(0xff00020c,0x00001017);//tx v2i.mixr pa gain(default)
      new_set_reg(0xff0003f0,0x00003508);// tx lpf gain

      do_txdpd_lut_calb();

	new_set_reg(0xff00020c,origin_20c);
	new_set_reg(0xff0003f0,origin_3f0);
	 printk("%s(%d)calb 0x%x\n", __func__,__LINE__, calb_item);
}
#endif

if((calb_item >> 5) & 0x1)
{
	new_get_reg(0xff000118,&chn_tmp);
      chn = (chn_tmp>>16) & 0xff; 
	set_rxdc_calb(&calb_val, 0, 0, 4);
      do_rx_lna_calb(&calb_val,
		            0, //tx dc/iq comp manual
		           chn, //channel num
			    1);//channel idx
     
}

if((calb_item >> 6) & 0x1)
{
	for( bw_idx = 0; bw_idx < 1; bw_idx++){
		set_rxdc_calb(&calb_val, bw_idx, 0, 4);
		  do_rx_irr_calb(&calb_val, 
		       0, //tx dc/iq comp manual 
		       0, //chn_idx 
		       bw_idx);//bw_idx		       
	}
}

   
#ifdef RUN_PT
   TRACE("----> all calb done!\n");
#else

   t9023_channel_switch(chn); // chn, bw
   t9023_bw_switch(0);
   printk("----> all calb done! calb_item: 0x%x\n", calb_item);
#endif

}



 
void t9023_top(unsigned int calb_item)
{	
    do_one_gain(calb_item);//(0xa1, 0x0);
    
}

#if 0
extern _TCHAR ipaddr[];
int check_rx_irr(unsigned int chn_idx, unsigned int bw_idx)
{
	unsigned int channel[] = {0x0a};
	unsigned int rx_irr_bw[]= {0,1,2};
	unsigned int rf_bw = 0;
	int hr = 0;
	int ret = 0;
	unsigned int origin_118;
    unsigned int origin_008;
	unsigned int origin_308;
	int cnt_max;
	double irr;

	unsigned int coeff_eup0;
	unsigned int coeff_eup1;
	unsigned int coeff_eup2;
	unsigned int coeff_eup3;
	unsigned int coeff_pup0;
	unsigned int coeff_pup1;
	unsigned int coeff_pup2;
	unsigned int coeff_pup3;
	unsigned int coeff_elow0;
	unsigned int coeff_elow1;
	unsigned int coeff_elow2;
	unsigned int coeff_elow3;
	unsigned int coeff_plow0;
	unsigned int coeff_plow1;
	unsigned int coeff_plow2;
	unsigned int coeff_plow3;
	double pwr_list_tone[40];
	double pwr_list_img[40];
	int pass_idx = 0;
    rf_bw = rx_irr_bw[bw_idx];

	
	char path[1024] = CALB_OUTPUT_PATH ;
	FILE *fp = NULL;
	
	sprintf(path,"%s%s",g_CurrentPath,CALB_OUTPUT_PATH);
	
	fp = fopen((char *)path, "a+");
	if (!fp){
		TRACE(" output file open fail\r\n");
		return -1;
	}
	
	fprintf(fp, "Check Rx IRR Start........\n");

	new_get_reg(0xff000118, &origin_118);
	new_get_reg(0xff000008, &origin_008);
	new_get_reg(0xff000008, &origin_308);

	if((hr = LP_Init(1,1))!=ERR_OK) // 实始化Matlab库
	{
		TRACE("Initialize  Fail \r\n ");
		return -1;
	}
	if((hr=LP_InitTesterN(ipaddr,1))!=ERR_OK)
	{
		TRACE("Cann't Connect IQtester !!\r\n ");
		return -1;
	}
	
	int selectedModules[1] = {1};
	char strVersion[4096]={0};
	LP_SetTesterMode(UP_TO_80MHZ_SIGNAL, selectedModules, 1);
	LP_GetVersion(strVersion,4096);

	TRACE("Connect IQtester.... !!\r\n ");
	TRACE("正在努力连接仪器中，请您稍等...!!\r\n ");
	TRACE("Connect IQtester ok !!\r\n ");

	//set channel
	i2c_set_reg_fragment(0xff000118, 23, 16, channel[chn_idx] & 0xff);

	i2c_set_reg_fragment(0xff000008, 1, 0, 3);//set rx mode
    i2c_set_reg_fragment(0xff000100, 0, 0, 0);//do sx calib
    i2c_set_reg_fragment(0xff000100, 0, 0, 1);

	i2c_set_reg_fragment(0xff000308,  0,  0, 1);//set man mode to 1
	i2c_set_reg_fragment(0xff000308, 15,  8, 0x25);//0x26
	i2c_set_reg_fragment(0xff000308, 16, 16, 1);//fix rx fe+lpf gain

    if(rf_bw == RF_BW_20M){
		i2c_set_reg_fragment(0xff000308,3,2,RF_BW_20M);
		i2c_set_reg_fragment(0xff000308,4,4,0);// adc 160M
	}else if(rf_bw == RF_BW_40M){
	   i2c_set_reg_fragment(0xff000308,3,2,RF_BW_40M);
	   i2c_set_reg_fragment(0xff000308,4,4,0);//adc 160M
	}else if(rf_bw == RF_BW_80M){
		i2c_set_reg_fragment(0xff000308,3,2,RF_BW_80M);
		i2c_set_reg_fragment(0xff000308,4,4,1);//adc 320
	}
    
	//get rx irr result
	coeff_eup0 = calb_val.rx_irr[chn_idx][bw_idx].coeff_eup[0];
	coeff_eup1 = calb_val.rx_irr[chn_idx][bw_idx].coeff_eup[1];
	coeff_eup2 = calb_val.rx_irr[chn_idx][bw_idx].coeff_eup[2];
	coeff_eup3 = calb_val.rx_irr[chn_idx][bw_idx].coeff_eup[3];

	coeff_pup0 = calb_val.rx_irr[chn_idx][bw_idx].coeff_pup[0];
	coeff_pup1 = calb_val.rx_irr[chn_idx][bw_idx].coeff_pup[1];
	coeff_pup2 = calb_val.rx_irr[chn_idx][bw_idx].coeff_pup[2];
	coeff_pup3 = calb_val.rx_irr[chn_idx][bw_idx].coeff_pup[3];

	coeff_elow0 = calb_val.rx_irr[chn_idx][bw_idx].coeff_elow[0];
	coeff_elow1 = calb_val.rx_irr[chn_idx][bw_idx].coeff_elow[1];
	coeff_elow2 = calb_val.rx_irr[chn_idx][bw_idx].coeff_elow[2];
	coeff_elow3 = calb_val.rx_irr[chn_idx][bw_idx].coeff_elow[3];

	coeff_plow0 = calb_val.rx_irr[chn_idx][bw_idx].coeff_plow[0];
	coeff_plow1 = calb_val.rx_irr[chn_idx][bw_idx].coeff_plow[1];
	coeff_plow2 = calb_val.rx_irr[chn_idx][bw_idx].coeff_plow[2];
	coeff_plow3 = calb_val.rx_irr[chn_idx][bw_idx].coeff_plow[3];

	i2c_set_reg_fragment(0xe80c, 14,  0, coeff_eup0);
	i2c_set_reg_fragment(0xe80c, 30, 16, coeff_eup1);
	i2c_set_reg_fragment(0xe810, 14,  0, coeff_eup2);
	i2c_set_reg_fragment(0xe810, 30, 16, coeff_eup3);
	i2c_set_reg_fragment(0xe814, 14,  0, coeff_pup0);
	i2c_set_reg_fragment(0xe814, 30, 16, coeff_pup1);
	i2c_set_reg_fragment(0xe818, 14,  0, coeff_pup2);
	i2c_set_reg_fragment(0xe818, 30, 16, coeff_pup3);
	i2c_set_reg_fragment(0xe81c, 14,  0, coeff_elow0);
	i2c_set_reg_fragment(0xe81c, 30, 16, coeff_elow1);
	i2c_set_reg_fragment(0xe820, 14,  0, coeff_elow2);
	i2c_set_reg_fragment(0xe820, 30, 16, coeff_elow3);
	i2c_set_reg_fragment(0xe824, 14,  0, coeff_plow0);
	i2c_set_reg_fragment(0xe824, 30, 16, coeff_plow1);
	i2c_set_reg_fragment(0xe828, 14,  0, coeff_plow2);
	i2c_set_reg_fragment(0xe828, 30, 16, coeff_plow3);
	
	if(rf_bw == RF_BW_20M){
		cnt_max = 10;
	}else if(rf_bw == RF_BW_40M){
		cnt_max = 20;
	}else if(rf_bw == RF_BW_80M){
		cnt_max = 40;
	}  
	
	for (int rate = 1; rate < cnt_max; rate++)
	{
		//IQ send single tone
		double expected_freq   = 1e6 * (WIFIChannel2Freq(channel[chn_idx])+ rate);
		double gain = -10;

		fprintf(fp, "rf_bw = %d, Freq = %.2f MHz\n", rf_bw, double(expected_freq/1e6));

		if((hr = LP_SetVsg(expected_freq, gain, IQV_PORT_RIGHT ))!=ERR_OK){	 // 设置仪器VSG的参数
			TRACE("\n\nERROR: %s\n", LP_GetErrorString(hr));
			return -1;
		}

		LP_ScpiCommandSet("VSG1;MOD:STAT OFF");
		LP_ScpiCommandSet("VSG1;POW:STAT ON");

		pwr_list_tone[rate] = get_spectrum_snr_pwr(~rate+1);
		pwr_list_img[rate] = get_spectrum_snr_pwr(rate);
		fprintf(fp, "SingleTone rate = %d M, tone power= %.2f, image power= %.2f\n", rate, pwr_list_tone[rate], pwr_list_img[rate]);

		irr = pwr_list_tone[rate] - pwr_list_img[rate];
		if (irr >= 45 ){
			ret = PASS_RESULT;
			pass_idx++;
			fprintf(fp, "SingleTone rate = %d M, irr = %.2f, test is pass!\n", rate, irr);
		}else{
			ret = -1;
			fprintf(fp, "SingleTone rate = %d M, irr = %.2f, test is fail!\n", rate, irr);
		}
	}

	for (int rate = 1; rate < cnt_max; rate++)
	{  
		//IQ send single tone
		double expected_freq   = 1e6 * (WIFIChannel2Freq(channel[chn_idx]) - rate);
		double gain = -10;

		fprintf(fp, "rf_bw = %d, Freq = %.2f MHz\n", rf_bw, double(expected_freq/1e6));

		if((hr = LP_SetVsg(expected_freq, gain, IQV_PORT_RIGHT ))!=ERR_OK){	 // 设置仪器VSG的参数
			TRACE("\n\nERROR: %s\n", LP_GetErrorString(hr));
			return -1;
		}

		LP_ScpiCommandSet("VSG1;MOD:STAT OFF");
		LP_ScpiCommandSet("VSG1;POW:STAT ON");

		pwr_list_tone[rate] = get_spectrum_snr_pwr(rate);
		pwr_list_img[rate] = get_spectrum_snr_pwr(~rate+1);
		fprintf(fp, "SingleTone rate = %d M, tone power= %.2f, image power= %.2f\n", ~rate+1, pwr_list_tone[rate], pwr_list_img[rate]);

		irr = pwr_list_tone[rate] - pwr_list_img[rate];
		if (irr >= 45 ){
			ret = PASS_RESULT;
			pass_idx++;
			fprintf(fp, "SingleTone rate = %d M, irr = %.2f, test is pass!\n", ~rate+1, irr);
		}else{
			ret = -1;
			fprintf(fp, "SingleTone rate = %d M, irr = %.2f, test is fail!\n", ~rate+1, irr);
		}
	}

	if(pass_idx == 2 * (cnt_max - 1))
		fprintf(fp, "All rate IRR test is pass!\n");

	new_set_reg(0xff000118, origin_118);
	new_set_reg(0xff000008, origin_008);
	new_set_reg(0xff000008, origin_308);

	LP_ScpiCommandSet("VSG1;MOD:STAT ON");
	LP_ScpiCommandSet("VSG1;POW:STAT OFF");

	fprintf(fp, "Check Rx IRR finish \n");
    fclose(fp);
	LP_ConClose();
	LP_Term();
	return ret;   //PASS_RESULT
}

double get_spectrum_snr_pwr(int rate)
{
    int pwr_db;
	float power;
	double f;
	double fs;
	double tmp;
	int data;
    unsigned int reg_data;
	unsigned int adda_db_vld;
	unsigned int unlock_cnt = 0;
	unsigned int TIME_OUT_CNT = 300;
	unsigned int rg_ed5c;

	f = rate;
	fs = 80;
	if(f > 0){
		tmp = f*2/fs * pow(2.0,24);
	}else{
		tmp = f*2/fs * pow(2.0,24) + pow(2.0,25);
	}
	
	data = floor(tmp);
	//dec2hex
    reg_data = data;

	i2c_set_reg_fragment(0xe82c, 4, 4, 0); 
	i2c_set_reg_fragment(0xec38, 3, 0, 7);      //factor for alpha fliter
	i2c_set_reg_fragment(0xec38, 28, 4, reg_data);  //angle base for spectrum analyasis
	i2c_set_reg_fragment(0xec38, 31, 31, 1);    //rg_spectrum_ana_angle_man_en

	i2c_set_reg_fragment(0xe018, 11, 8, 6);  //rf calibration mode manual value :0:RX DC, 1: TX DC/IQ, 2: TX TSSI, 3: TX DPD, 4: RX LNA, 5: RX IRR, 6: RX OFDM

    i2c_set_reg_fragment(0xed00, 23, 0, 0x1000);   //wait_count

	i2c_set_reg_fragment(0xed58, 6, 4, 7);    //rg_snr_esti_calctime
	i2c_set_reg_fragment(0xed58, 10, 8, 7);   //rg_snr_alpha
    i2c_set_reg_fragment(0xed58, 0, 0, 1); 

    new_set_reg(0xe008, 0x11110011);   //recv enable manual mode

	new_get_reg(0xed5c, &rg_ed5c);
	adda_db_vld = rg_ed5c & BIT(12);

	while(unlock_cnt < TIME_OUT_CNT){
		if(adda_db_vld == 0){
			new_get_reg(0xed5c, &rg_ed5c);
			adda_db_vld = rg_ed5c & BIT(12);

			unlock_cnt = unlock_cnt + 1;
#ifdef RUN_PT
    		TRACE("unlock_cnt:%d, Power estimate NOT DONE...\n",unlock_cnt);
#else
            printk("unlock_cnt:%d, Power estimate NOT DONE...\n",unlock_cnt);
#endif
                       
		}else{
			unlock_cnt = 0;
#ifdef RUN_PT
			TRACE("Power estimate ready\n");
#else
            printk("Power estimate ready\n");
#endif                       
			break;
		}
	}

	pwr_db = i2c_get_reg_fragment(0xed5c, 24, 16);

	TRACE("\n\nspectrum snr pwr_db: 0x%x\n", pwr_db);

	if(pwr_db & BIT(8)){
		power = (pwr_db - pow(2.0, 9))/4;
	}else{
		power = pwr_db/4;
	}

	TRACE("\n\nspectrum snr pwr_db: %.2f\n", power);
	i2c_set_reg_fragment(0xed58, 0, 0, 0); 
	new_set_reg(0xe008, 0x00000000);

	return power;
}

int check_rx_rc(unsigned int chn_idx, unsigned int bw_idx)
{
    unsigned int channel[] = {0x0a};
	unsigned int rx_irr_bw[]= {0,1,2};
	unsigned int rf_bw = 0;
	int hr = 0;
	int ret = 0;
    rf_bw = rx_irr_bw[bw_idx];
	double pwr_list_tone[100];
	int cnt_max;

	unsigned int origin_118;
	unsigned int origin_008;
	unsigned int origin_308;

	
	char path[1024] = CALB_OUTPUT_PATH ;
	FILE *fp = NULL;
	
	sprintf(path,"%s%s",g_CurrentPath,CALB_OUTPUT_PATH);
	
	fp = fopen((char *)path, "a+");
	if (!fp){
		TRACE(" output file open fail\r\n");
		return -1;
	}
	
	fprintf(fp, "Check Rx RC Start........\n");
    
	new_get_reg(0xff000118, &origin_118);
	new_get_reg(0xff000008, &origin_008);
	new_get_reg(0xff000008, &origin_308);

	if((hr = LP_Init(1,1))!=ERR_OK) // 实始化Matlab库
	{
		TRACE("Initialize  Fail \r\n ");
		return -1;
	}
	if((hr=LP_InitTesterN(ipaddr,1))!=ERR_OK)
	{
		TRACE("Cann't Connect IQtester !!\r\n ");
		return -1;
	}
	
	int selectedModules[1] = {1};
	char strVersion[4096]={0};
	LP_SetTesterMode(UP_TO_80MHZ_SIGNAL, selectedModules, 1);
	LP_GetVersion(strVersion,4096);

	TRACE("Connect IQtester.... !!\r\n ");
	TRACE("正在努力连接仪器中，请您稍等...!!\r\n ");
	TRACE("Connect IQtester ok !!\r\n ");

	//set channel
	i2c_set_reg_fragment(0xff000118, 23, 16, channel[chn_idx] & 0xff);

	i2c_set_reg_fragment(0xff000008, 1, 0, 3);//set rx mode
    i2c_set_reg_fragment(0xff000100, 0, 0, 0);//do sx calib
    i2c_set_reg_fragment(0xff000100, 0, 0, 1);

	//i2c_set_reg_fragment(0xff000308,  0,  0, 1);//set man mode to 1
	i2c_set_reg_fragment(0xff000308, 15,  8, 0x9c);//RG_WF_RX_GAIN_MAN
	i2c_set_reg_fragment(0xff000308, 16, 16, 1);//fix rx fe+lpf gain

	if(rf_bw == RF_BW_20M){
		i2c_set_reg_fragment(0xff000308, 7,  0, 0x01);
		cnt_max = 40;
	}else if(rf_bw == RF_BW_40M){
		i2c_set_reg_fragment(0xff000308, 7,  0, 0x05);
		cnt_max = 40;
	}else if(rf_bw == RF_BW_80M){
		i2c_set_reg_fragment(0xff000308, 7,  0, 0x19);
		cnt_max = 60;
	}

	 for (int rate = cnt_max - 1; rate > 0; rate--)
	{
		//IQ send single tone
		double expected_freq   = 1e6 * (WIFIChannel2Freq(channel[chn_idx])- rate);
		double gain = -70;

		fprintf(fp, "rf_bw = %d, Freq = %.2f MHz\n", rf_bw, double(expected_freq/1e6));

		if((hr = LP_SetVsg(expected_freq, gain, IQV_PORT_RIGHT ))!=ERR_OK){	 // 设置仪器VSG的参数
			TRACE("\n\nERROR: %s\n", LP_GetErrorString(hr));
			return -1;
		}

		LP_ScpiCommandSet("VSG1;MOD:STAT OFF");
	    LP_ScpiCommandSet("VSG1;POW:STAT ON");

		pwr_list_tone[rate] = get_spectrum_snr_pwr(~rate + 1);
		fprintf(fp, "singletone rate = %dM, power = %.2f\n", ~rate + 1, pwr_list_tone[rate]);
	}

	 for (int rate = 1; rate < cnt_max; rate++)
	{
		//IQ send single tone
		double expected_freq   = 1e6 * (WIFIChannel2Freq(channel[chn_idx])+ rate);
		double gain = -70;

		fprintf(fp, "rf_bw = %d, Freq = %.2f MHz\n", rf_bw, double(expected_freq/1e6));

		if((hr = LP_SetVsg(expected_freq, gain, IQV_PORT_RIGHT ))!=ERR_OK){	 // 设置仪器VSG的参数
			TRACE("\n\nERROR: %s\n", LP_GetErrorString(hr));
			return -1;
		}

		LP_ScpiCommandSet("VSG1;MOD:STAT OFF");
	    LP_ScpiCommandSet("VSG1;POW:STAT ON");

		pwr_list_tone[rate] = get_spectrum_snr_pwr(rate);
		fprintf(fp, "singletone rate = %dM, power = %.2f\n", rate, pwr_list_tone[rate]);
	}

	new_set_reg(0xff000118, origin_118);
	new_set_reg(0xff000008, origin_008);
	new_set_reg(0xff000308, origin_308);
	
	fprintf(fp, "Check Rx RC finish \n");
    fclose(fp);
	LP_ConClose();
	LP_Term();
	return ret;

}

double get_tone_power(double freq)
{
    int hr = 0;
	double rfAmplDb = 20;
	double power;

    if((hr=LP_SetVsa(1e6 * freq, rfAmplDb, g_IQV_PORT))!=ERR_OK){// 设置仪器VSA的参数
		TRACE("\n\n LP_SetVsa ERROR: %s\n", LP_GetErrorString(hr));
		return -1;
	}
	LP_ScpiCommandSet("VSA1;SRAT 10000000");   //set sampling rate 10M
	LP_ScpiCommandSet("VSA1;RLEVel:AUTO");

	IQ_Vsa_GPRF_analyze(&power);
	TRACE("Freq = %.2f MHz, power = %.2f dBm\n", freq, power);
	
    return power;
}

int check_tx_dcirr(unsigned int chn_idx)
{
    unsigned int channel[] = {0x0a};
	int hr = 0;
	int rate;
	double LO_PWR;
	double DES_PWR;
	double IMG_PWR;
	double IRR;
	double LOFT;
	double fs;
	double tmp;
	int data;
	unsigned reg_data;
	double expected_freq;
	
	unsigned int origin_118;
	unsigned int origin_008;
	unsigned int origin_20c;
	unsigned int origin_3f0;
	unsigned int origin_e40c;
	unsigned int origin_e410;
	unsigned int origin_b22c;
	unsigned int origin_ec38;
	unsigned int origin_e008;
	unsigned int origin_e018;
	unsigned int origin_e4b8;

	
	char path[1024] = CALB_OUTPUT_PATH ;
	FILE *fp = NULL;
	
	sprintf(path,"%s%s",g_CurrentPath,CALB_OUTPUT_PATH);
	
	fp = fopen((char *)path, "a+");
	if (!fp){
		TRACE(" output file open fail\r\n");
		return -1;
	}
	
	fprintf(fp, "Check TX DC&IRR Start........\n");
    
	new_get_reg(0xff000118, &origin_118);
	new_get_reg(0xff000008, &origin_008);
	new_get_reg(0xff00020c, &origin_20c);
	new_get_reg(0xff0003f0, &origin_3f0);
	new_get_reg(0xe40c, &origin_e40c);
	new_get_reg(0xe410, &origin_e410);
	new_get_reg(0xb22c, &origin_b22c);
	new_get_reg(0xec38, &origin_ec38);
	new_get_reg(0xe008, &origin_e008);
	new_get_reg(0xe018, &origin_e018);
	new_get_reg(0xe4b8, &origin_e4b8);

	if((hr = LP_Init(1,1))!=ERR_OK) // 实始化Matlab库
	{
		TRACE("Initialize  Fail \r\n ");
		return -1;
	}
	if((hr=LP_InitTesterN(ipaddr,1))!=ERR_OK)
	{
		TRACE("Cann't Connect IQtester !!\r\n ");
		return -1;
	}
	
	int selectedModules[1] = {1};
	char strVersion[4096]={0};
	LP_SetTesterMode(UP_TO_80MHZ_SIGNAL, selectedModules, 1);
	LP_GetVersion(strVersion,4096);

	TRACE("Connect IQtester.... !!\r\n ");
	TRACE("正在努力连接仪器中，请您稍等...!!\r\n ");
	TRACE("Connect IQtester ok !!\r\n ");

	//set channel
	i2c_set_reg_fragment(0xff000118, 23, 16, channel[chn_idx] & 0xff);

	i2c_set_reg_fragment(0xff000008, 1, 0, 2);//set tx mode
    i2c_set_reg_fragment(0xff000100, 0, 0, 0);//do sx calib
    i2c_set_reg_fragment(0xff000100, 0, 0, 1);

    new_set_reg(0xb22c, 0xa1000000);   //set BW 80M
	new_set_reg(0xff00020c, 0x00001077);//TX V2I gain(0), MIX gain(7), PA gain(7)
	new_set_reg(0xff0003f0, 0x00003508);//TX LPF gain(3)
	i2c_set_reg_fragment(0xe4b8, 31, 24, 40);

	for(rate = 1; rate < 10; rate++)
    {
		fs = 80;
		tmp = rate*2/fs * pow(2.0,24);		
		data = floor(tmp);
		//dec2hex
		reg_data = data;

		i2c_set_reg_fragment(0xe40c, 31, 31, 1);
		i2c_set_reg_fragment(0xe40c, 23, 0, reg_data);  //open  tone2
		new_set_reg(0xe410, 0x00000001);//1: select single tone 

	    expected_freq   = WIFIChannel2Freq(channel[chn_idx]);
        
		if(rate >= 4){
			LO_PWR = get_tone_power(expected_freq);
		    fprintf(fp, "Freq = %.2f MHz, LO leakage power= %.2f dBm\r\n", expected_freq, LO_PWR);

			DES_PWR = get_tone_power(expected_freq + rate);
			fprintf(fp, "Desired signal Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq + rate, DES_PWR);
			
			IMG_PWR = get_tone_power(expected_freq - rate);
			fprintf(fp, "Image Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq - rate, IMG_PWR);
		}else if(rate == 1){
			LO_PWR = get_tone_power(expected_freq- rate - 2.5);  //expected_freq- rate - 2.5
		    fprintf(fp, "LO leakage Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq, LO_PWR);

            DES_PWR = get_tone_power(expected_freq);
			fprintf(fp, "Desired signal Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq + rate, DES_PWR);
			
			IMG_PWR = get_tone_power(expected_freq - rate - 3.5);
			fprintf(fp, "Image Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq - rate, IMG_PWR);

		}else{
			LO_PWR = get_tone_power(expected_freq- rate);
		    fprintf(fp, "LO leakage Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq, LO_PWR);

            DES_PWR = get_tone_power(expected_freq);
			fprintf(fp, "Desired signal Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq + rate, DES_PWR);
			
			IMG_PWR = get_tone_power(expected_freq - rate * 2);
			fprintf(fp, "Image Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq - rate, IMG_PWR);

		}

		LOFT = LO_PWR - DES_PWR;
		IRR = DES_PWR - IMG_PWR;
		fprintf(fp, "Freq = %.2f MHz, tone rate = %dM, LOFT = %.2fdB, IRR = %.2fdB\r\n\r\n",expected_freq, rate, LOFT, IRR);

	}

	for(rate = 1; rate < 10; rate++)
	{
	    fs = 80;
		tmp = rate*2/fs * pow(2.0,24);		
		data = floor(tmp);
		//dec2hex
		reg_data = data;

		i2c_set_reg_fragment(0xe40c, 31, 31, 0);
		i2c_set_reg_fragment(0xec38, 31, 31, 1);
		i2c_set_reg_fragment(0xec38, 28, 4, reg_data);
		new_set_reg(0xe008, 0x11111111);
        i2c_set_reg_fragment(0xe018, 16, 16, 1);
		i2c_set_reg_fragment(0xe018, 11, 8, 0);

		if(rate >= 4){
			LO_PWR = get_tone_power(expected_freq);
		    fprintf(fp, "LO leakage Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq, LO_PWR);

			DES_PWR = get_tone_power(expected_freq - rate);
			fprintf(fp, "Desired signal Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq + rate, DES_PWR);
			
			IMG_PWR = get_tone_power(expected_freq + rate);
			fprintf(fp, "Image Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq - rate, IMG_PWR);
		}else if(rate == 1){
			LO_PWR = get_tone_power(expected_freq + rate + 2.5);
		    fprintf(fp, "LO leakage Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq, LO_PWR);

            DES_PWR = get_tone_power(expected_freq);
			fprintf(fp, "Desired signal Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq - rate, DES_PWR);
			
			IMG_PWR = get_tone_power(expected_freq + rate + 3.5);
			fprintf(fp, "Image Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq + rate, IMG_PWR);

		}else{
			LO_PWR = get_tone_power(expected_freq + rate);
		    fprintf(fp, "LO leakage Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq, LO_PWR);

            DES_PWR = get_tone_power(expected_freq);
			fprintf(fp, "Desired signal Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq - rate, DES_PWR);
			
			IMG_PWR = get_tone_power(expected_freq + rate * 2);
			fprintf(fp, "Image Freq = %.2f MHz, power= %.2f dBm\r\n", expected_freq + rate, IMG_PWR);

		}

		LOFT = LO_PWR - DES_PWR;
		IRR = DES_PWR - IMG_PWR;
		fprintf(fp, "Freq = %.2f MHz, tone rate = %dM, LOFT = %.2fdB, IRR = %.2fdB\r\n\r\n", expected_freq, ~rate + 1, LOFT, IRR);

	}

	new_set_reg(0xff000118, origin_118);
	new_set_reg(0xff000008, origin_008);
	new_set_reg(0xff00020c, origin_20c);
	new_set_reg(0xff0003f0, origin_3f0);
	new_set_reg(0xe40c, origin_e40c);
	new_set_reg(0xe410, origin_e410);
	new_set_reg(0xb22c, origin_b22c);
	new_set_reg(0xec38, origin_ec38);
	new_set_reg(0xe008, origin_e008);
	new_set_reg(0xe018, origin_e018);
	new_set_reg(0xe4b8, origin_e4b8);
	
	fprintf(fp, "Check TX DC&IRR finish \n");
    fclose(fp);
	LP_ConClose();
	LP_Term();
	return 0;
}

int check_tx_dpd(TEST_PARAM * pTestParam, unsigned int chn_idx, unsigned int bw_idx, unsigned int gain_idx)
{
    unsigned int channel[] = {0x0a};
	unsigned int rx_irr_bw[]= {20,40,80};
	int ret = 0;
	int hr = 0;
	unsigned int coeff_I[15];
	unsigned int coeff_Q[15];
	unsigned int ck = 0;

	unsigned int origin_118;
    unsigned int origin_008;
	unsigned int origin_e410;
    unsigned int origin_e4f0;

	pTestParam->channel = channel[chn_idx];
    pTestParam->bandwidth= rx_irr_bw[bw_idx];
    pTestParam->bss_bandwidth = rx_irr_bw[bw_idx];

	new_get_reg(0xff000118, &origin_118);
	new_get_reg(0xff000008, &origin_008);
	new_get_reg(0xe410, &origin_e410);
	new_get_reg(0xe4f0, &origin_e4f0);

    if((hr = LP_Init(1,1))!=ERR_OK) // 实始化Matlab库
	{
		TRACE("Initialize  Fail \r\n ");
		return -1;
	}
	if((hr=LP_InitTesterN(ipaddr,1))!=ERR_OK)
	{
		TRACE("Cann't Connect IQtester !!\r\n ");
		return -1;
	}
	
	int selectedModules[1] = {1};
	char strVersion[4096]={0};
	LP_SetTesterMode(UP_TO_80MHZ_SIGNAL, selectedModules, 1);
	LP_GetVersion(strVersion,4096);

	TRACE("Connect IQtester.... !!\r\n ");
	TRACE("正在努力连接仪器中，请您稍等...!!\r\n ");
	TRACE("Connect IQtester ok !!\r\n ");

    //set channel
	//i2c_set_reg_fragment(0xff000118, 23, 16, channel[chn_idx] & 0xff);

	//i2c_set_reg_fragment(0xff000008, 1, 0, 2);//set tx mode
    //i2c_set_reg_fragment(0xff000100, 0, 0, 0);//do sx calib
    //i2c_set_reg_fragment(0xff000100, 0, 0, 1);

	i2c_set_reg_fragment(0x8010, 28, 28, 0);//enable agc cca detect
	//new_set_reg(0xe410, 0x00000000); //0:select OFDM signal;1:signal tone 

	//DPD OFF
	i2c_set_reg_fragment(0xe4f0, 7, 0, 0x80);//dpd in gain 
	i2c_set_reg_fragment(0xe4f0, 24, 24, 1);//set dpd comp type to 1:coeff
	i2c_set_reg_fragment(0xe4f0, 26, 26, 1);//set dpd comp bypass to 1
	i2c_set_reg_fragment(0xe4f0, 28, 28, 1);//set dpd comp bypass man to 1
	i2c_set_reg_fragment(0xe4f0, 31, 31, 1);//set dpd comp manual mode to 1
    
	DutTest_EVM_MASK(pTestParam);

    //DPD ON
	i2c_set_reg_fragment(0xe4f0, 7, 0, 0x80);//dpd in gain 
    i2c_set_reg_fragment(0xe4f0, 24, 24, 1);//set dpd comp type to 1:coeff
	i2c_set_reg_fragment(0xe4f0, 26, 26, 0);//set dpd comp bypass to 1
	i2c_set_reg_fragment(0xe4f0, 28, 28, 1);//set dpd comp bypass man to 1
    i2c_set_reg_fragment(0xe4f0, 31, 31, 1);//set dpd comp manual mode to 1

    DutTest_EVM_MASK(pTestParam);

	new_set_reg(0xff000118, origin_118);
	new_set_reg(0xff000008, origin_008);
	new_set_reg(0xe410, origin_e410);
	new_set_reg(0xe4f0, origin_e4f0);

	LP_ConClose();
	LP_Term();
	return 0;
}

#endif
#endif//RF_T9023