#ifndef __PATCH_FI_CMD__
#define __PATCH_FI_CMD__

#include "fi_sdio.h"
#include "fi_cmd.h"

#define CHANNEL_SWITCH_CMD  0x25
#define DPD_MEMORY_CMD  0x26
#define FWLOG_MODE_CMD  0x27
#define CALI_PARAM_CMD  0x28
#define PHY_INTERFACE_CMD  0x29


#define TSSI_5G_CAL_NUM 4

enum patch_fw_event_type
{
    CHANNEL_SWITCH_EVENT = (TBTT_EVENT + 1),
    DPD_CALIBRATION_EVENT,
    TX_ERROR_EVENT,
};

/* channel switch flag */
enum {
    CHANNEL_RESTORE_FLAG = BIT(0),
    CHANNEL_CONNECT_FLAG = BIT(1),
    CHANNEL_NO_EVENT_FLAG = BIT(2),
};

typedef struct Channel_Switch
{
    unsigned char Cmd;
    unsigned char vid;
    unsigned char flag;
    unsigned char bw;
    unsigned int channel;
    /* center freq value */
    unsigned int pri_chan;
}Channel_Switch;

typedef struct DPD_Memory_Downlod
{
    unsigned char Cmd;
    unsigned char vid;
    unsigned char reserved[2];
} DPD_Memory_Downlod;

typedef struct Fwlog_Mode_Control
{
    unsigned char Cmd;
    unsigned char mode; //0x00:fw  0x01:host
    unsigned char reserved[2];
} Fwlog_Mode_Control;

// event
typedef struct channel_switch_event
{
    struct fw_event_basic_info basic_info;
    unsigned int channel;
    unsigned int bw;
    unsigned int done;
} channel_switch_event;

typedef struct tx_error_event
{
    struct fw_event_basic_info basic_info;
    unsigned int error_type;
    unsigned int frame_type;
} tx_error_event;

typedef struct Cali_Param
{
    unsigned char Cmd;
    unsigned char version;
    unsigned short cali_config;
    unsigned char freq_offset;
    unsigned char tssi_2g_offset;
    unsigned char tssi_5g_offset[TSSI_5G_CAL_NUM];
    unsigned char hdmi_enable;
    unsigned short hdmi_freq;
    unsigned char reserve;
} Cali_Param;

typedef struct Phy_Interface_Param
{
    unsigned char Cmd;
    unsigned char vid;
    unsigned char interface_enable;
    unsigned char reserve;
} Phy_Interface_Param;

#endif
