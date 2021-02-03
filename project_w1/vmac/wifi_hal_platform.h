#ifndef __PLATFORM_WIFI_AML__
#define __PLATFORM_WIFI_AML__



#ifdef HAL_FPGA_VER
#define OWNER_NAME "AML_WIFI"

#define GPIOX_6     234  //GET ( kernel dts print)
#define GPIOX_7     235
#define GPIOX_19    247
#define GPIOY_15    226 //IRQ TEST USE OK
#define GPIOX_20    248  //used to clk_sel,replace before GPIOY_15

void platform_wifi_reset_cpu(void);
int platform_wifi_request_gpio_irq (void *data);
void platform_wifi_free_gpio_irq (void *data);

void platform_wifi_clk_source_sel(int is_ssv_clk);
void platform_wifi_reset (void);

extern enum irqreturn  hal_irq_top(int irq, void *dev_id);

void hi_change_sram_concurrent_mode(void);
void set_wifi_baudrate (unsigned int apb_clk);
int amlhal_resetmac(void);
int amlhal_resetsdio(void);
unsigned char hal_set_sys_clk_for_fpga(void);
#endif
unsigned char hal_set_sys_clk(int clockdiv);
unsigned char hal_set_sys_clk_Core(unsigned int addr, unsigned int value);
unsigned char hal_download_wifi_fw_img(void);
extern void aml_wifi_set_mac_addr(void);

#endif
