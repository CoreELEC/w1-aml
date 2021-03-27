#ifdef RF_D_TOP_REG
#else
#define RF_D_TOP_REG


#define RF_D_TOP_REG_BASE                         (0x0)

#define RG_TOP_A0                                 (RF_D_TOP_REG_BASE + 0x0)
// Bit 31  :0      rg_wf5g_top_rsv0               U     RW        default = 'h0
typedef union RG_TOP_A0_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_WF5G_TOP_RSV0 : 32;
  } b;
} RG_TOP_A0_FIELD_T;

#define RG_TOP_A1                                 (RF_D_TOP_REG_BASE + 0x4)
// Bit 31  :0      rg_wf5g_top_rsv1               U     RW        default = 'hffffffff
typedef union RG_TOP_A1_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_WF5G_TOP_RSV1 : 32;
  } b;
} RG_TOP_A1_FIELD_T;

#define RG_TOP_A2                                 (RF_D_TOP_REG_BASE + 0x8)
// Bit 1   :0      rg_wf_mode_man                 U     RW        default = 'h0
// Bit 4           rg_wf_mode_man_mode            U     RW        default = 'h1
// Bit 9   :8      rg_wf_tx_mode                  U     RW        default = 'h3
// Bit 13  :12     rg_wf_rx_mode                  U     RW        default = 'h2
typedef union RG_TOP_A2_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_WF_MODE_MAN : 2;
    unsigned int rsvd_0 : 2;
    unsigned int RG_WF_MODE_MAN_MODE : 1;
    unsigned int rsvd_1 : 3;
    unsigned int RG_WF_TX_MODE : 2;
    unsigned int rsvd_2 : 2;
    unsigned int RG_WF_RX_MODE : 2;
    unsigned int rsvd_3 : 18;
  } b;
} RG_TOP_A2_FIELD_T;

#define RG_TOP_A3                                 (RF_D_TOP_REG_BASE + 0xc)
// Bit 0           rg_wf_trx_hf_ldo_fc_sel        U     RW        default = 'h0
// Bit 1           rg_wf_trx_hf_ldo_fc_en_man_mode     U     RW        default = 'h0
// Bit 2           rg_wf_trx_hf_ldo_fc_en_man     U     RW        default = 'h0
// Bit 7   :3      rg_wf_trx_hf_ldo_vsel          U     RW        default = 'h10
// Bit 8           rg_wf_trx_hf_ldo_rc_ctrl       U     RW        default = 'h1
// Bit 9           rg_wf_trx_lf_ldo_fc_sel        U     RW        default = 'h0
// Bit 10          rg_wf_trx_lf_ldo_fc_en_man_mode     U     RW        default = 'h0
// Bit 11          rg_wf_trx_lf_ldo_fc_en_man     U     RW        default = 'h0
// Bit 16  :12     rg_wf_trx_lf_ldo_vsel          U     RW        default = 'h10
// Bit 17          rg_wf_trx_lf_ldo_rc_ctrl       U     RW        default = 'h1
// Bit 18          rg_wf_bg_ptat_en               U     RW        default = 'h1
// Bit 19          rg_wf_bg_ptat_cal_en           U     RW        default = 'h1
// Bit 21  :20     rg_wf_trx_ldo_fc_cnt           U     RW        default = 'h0
// Bit 31  :22     rg_wf_top_reserved3            U     RW        default = 'h0
typedef union RG_TOP_A3_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_WF_TRX_HF_LDO_FC_SEL : 1;
    unsigned int RG_WF_TRX_HF_LDO_FC_EN_MAN_MODE : 1;
    unsigned int RG_WF_TRX_HF_LDO_FC_EN_MAN : 1;
    unsigned int RG_WF_TRX_HF_LDO_VSEL : 5;
    unsigned int RG_WF_TRX_HF_LDO_RC_CTRL : 1;
    unsigned int RG_WF_TRX_LF_LDO_FC_SEL : 1;
    unsigned int RG_WF_TRX_LF_LDO_FC_EN_MAN_MODE : 1;
    unsigned int RG_WF_TRX_LF_LDO_FC_EN_MAN : 1;
    unsigned int RG_WF_TRX_LF_LDO_VSEL : 5;
    unsigned int RG_WF_TRX_LF_LDO_RC_CTRL : 1;
    unsigned int RG_WF_BG_PTAT_EN : 1;
    unsigned int RG_WF_BG_PTAT_CAL_EN : 1;
    unsigned int RG_WF_TRX_LDO_FC_CNT : 2;
    unsigned int RG_WF_TOP_RESERVED3 : 10;
  } b;
} RG_TOP_A3_FIELD_T;

#define RG_TOP_A4                                 (RF_D_TOP_REG_BASE + 0x10)
// Bit 6   :0      rg_wf_bg_ptat_cal_i            U     RW        default = 'h40
// Bit 7           rg_wf_bg_zt_en                 U     RW        default = 'h1
// Bit 8           rg_wf_bg_zt_cal_en             U     RW        default = 'h1
// Bit 15  :9      rg_wf_bg_zt_cal_i              U     RW        default = 'h40
// Bit 16          rg_bg_test_en                  U     RW        default = 'h0
// Bit 17          rg_bg_test_sel                 U     RW        default = 'h0
// Bit 20  :18     rg_wf_top_dc_test_sel          U     RW        default = 'h0
// Bit 31  :21     rg_wf_top_reserved4            U     RW        default = 'h0
typedef union RG_TOP_A4_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_WF_BG_PTAT_CAL_I : 7;
    unsigned int RG_WF_BG_ZT_EN : 1;
    unsigned int RG_WF_BG_ZT_CAL_EN : 1;
    unsigned int RG_WF_BG_ZT_CAL_I : 7;
    unsigned int RG_BG_TEST_EN : 1;
    unsigned int RG_BG_TEST_SEL : 1;
    unsigned int RG_WF_TOP_DC_TEST_SEL : 3;
    unsigned int RG_WF_TOP_RESERVED4 : 11;
  } b;
} RG_TOP_A4_FIELD_T;

#define RG_TOP_A5                                 (RF_D_TOP_REG_BASE + 0x14)
// Bit 0           rg_m0_wf_trx_hf_ldo_en         U     RW        default = 'h0
// Bit 1           rg_m0_wf_trx_hf_ldo_tx_en      U     RW        default = 'h0
// Bit 2           rg_m0_wf_trx_hf_ldo_rx1_en     U     RW        default = 'h0
// Bit 3           rg_m0_wf_trx_hf_ldo_rx2_en     U     RW        default = 'h0
// Bit 4           rg_m0_wf_trx_hf_ldo_lo_en      U     RW        default = 'h0
// Bit 5           rg_m0_wf_trx_lf_ldo_en         U     RW        default = 'h0
// Bit 6           rg_m0_wf_trx_lf_ldo_tx_en      U     RW        default = 'h0
// Bit 7           rg_m0_wf_trx_lf_ldo_rx_en      U     RW        default = 'h0
typedef union RG_TOP_A5_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_M0_WF_TRX_HF_LDO_EN : 1;
    unsigned int RG_M0_WF_TRX_HF_LDO_TX_EN : 1;
    unsigned int RG_M0_WF_TRX_HF_LDO_RX1_EN : 1;
    unsigned int RG_M0_WF_TRX_HF_LDO_RX2_EN : 1;
    unsigned int RG_M0_WF_TRX_HF_LDO_LO_EN : 1;
    unsigned int RG_M0_WF_TRX_LF_LDO_EN : 1;
    unsigned int RG_M0_WF_TRX_LF_LDO_TX_EN : 1;
    unsigned int RG_M0_WF_TRX_LF_LDO_RX_EN : 1;
    unsigned int rsvd_0 : 24;
  } b;
} RG_TOP_A5_FIELD_T;

#define RG_TOP_A6                                 (RF_D_TOP_REG_BASE + 0x18)
// Bit 0           rg_m1_wf_trx_hf_ldo_en         U     RW        default = 'h0
// Bit 1           rg_m1_wf_trx_hf_ldo_tx_en      U     RW        default = 'h0
// Bit 2           rg_m1_wf_trx_hf_ldo_rx1_en     U     RW        default = 'h0
// Bit 3           rg_m1_wf_trx_hf_ldo_rx2_en     U     RW        default = 'h0
// Bit 4           rg_m1_wf_trx_hf_ldo_lo_en      U     RW        default = 'h0
// Bit 5           rg_m1_wf_trx_lf_ldo_en         U     RW        default = 'h0
// Bit 6           rg_m1_wf_trx_lf_ldo_tx_en      U     RW        default = 'h0
// Bit 7           rg_m1_wf_trx_lf_ldo_rx_en      U     RW        default = 'h0
typedef union RG_TOP_A6_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_M1_WF_TRX_HF_LDO_EN : 1;
    unsigned int RG_M1_WF_TRX_HF_LDO_TX_EN : 1;
    unsigned int RG_M1_WF_TRX_HF_LDO_RX1_EN : 1;
    unsigned int RG_M1_WF_TRX_HF_LDO_RX2_EN : 1;
    unsigned int RG_M1_WF_TRX_HF_LDO_LO_EN : 1;
    unsigned int RG_M1_WF_TRX_LF_LDO_EN : 1;
    unsigned int RG_M1_WF_TRX_LF_LDO_TX_EN : 1;
    unsigned int RG_M1_WF_TRX_LF_LDO_RX_EN : 1;
    unsigned int rsvd_0 : 24;
  } b;
} RG_TOP_A6_FIELD_T;

#define RG_TOP_A7                                 (RF_D_TOP_REG_BASE + 0x1c)
// Bit 0           rg_m2_wf_trx_hf_ldo_en         U     RW        default = 'h1
// Bit 1           rg_m2_wf_trx_hf_ldo_tx_en      U     RW        default = 'h1
// Bit 2           rg_m2_wf_trx_hf_ldo_rx1_en     U     RW        default = 'h0
// Bit 3           rg_m2_wf_trx_hf_ldo_rx2_en     U     RW        default = 'h0
// Bit 4           rg_m2_wf_trx_hf_ldo_lo_en      U     RW        default = 'h1
// Bit 5           rg_m2_wf_trx_lf_ldo_en         U     RW        default = 'h1
// Bit 6           rg_m2_wf_trx_lf_ldo_tx_en      U     RW        default = 'h1
// Bit 7           rg_m2_wf_trx_lf_ldo_rx_en      U     RW        default = 'h0
typedef union RG_TOP_A7_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_M2_WF_TRX_HF_LDO_EN : 1;
    unsigned int RG_M2_WF_TRX_HF_LDO_TX_EN : 1;
    unsigned int RG_M2_WF_TRX_HF_LDO_RX1_EN : 1;
    unsigned int RG_M2_WF_TRX_HF_LDO_RX2_EN : 1;
    unsigned int RG_M2_WF_TRX_HF_LDO_LO_EN : 1;
    unsigned int RG_M2_WF_TRX_LF_LDO_EN : 1;
    unsigned int RG_M2_WF_TRX_LF_LDO_TX_EN : 1;
    unsigned int RG_M2_WF_TRX_LF_LDO_RX_EN : 1;
    unsigned int rsvd_0 : 24;
  } b;
} RG_TOP_A7_FIELD_T;

#define RG_TOP_A8                                 (RF_D_TOP_REG_BASE + 0x20)
// Bit 0           rg_m3_wf_trx_hf_ldo_en         U     RW        default = 'h1
// Bit 1           rg_m3_wf_trx_hf_ldo_tx_en      U     RW        default = 'h0
// Bit 2           rg_m3_wf_trx_hf_ldo_rx1_en     U     RW        default = 'h1
// Bit 3           rg_m3_wf_trx_hf_ldo_rx2_en     U     RW        default = 'h1
// Bit 4           rg_m3_wf_trx_hf_ldo_lo_en      U     RW        default = 'h1
// Bit 5           rg_m3_wf_trx_lf_ldo_en         U     RW        default = 'h1
// Bit 6           rg_m3_wf_trx_lf_ldo_tx_en      U     RW        default = 'h0
// Bit 7           rg_m3_wf_trx_lf_ldo_rx_en      U     RW        default = 'h1
typedef union RG_TOP_A8_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_M3_WF_TRX_HF_LDO_EN : 1;
    unsigned int RG_M3_WF_TRX_HF_LDO_TX_EN : 1;
    unsigned int RG_M3_WF_TRX_HF_LDO_RX1_EN : 1;
    unsigned int RG_M3_WF_TRX_HF_LDO_RX2_EN : 1;
    unsigned int RG_M3_WF_TRX_HF_LDO_LO_EN : 1;
    unsigned int RG_M3_WF_TRX_LF_LDO_EN : 1;
    unsigned int RG_M3_WF_TRX_LF_LDO_TX_EN : 1;
    unsigned int RG_M3_WF_TRX_LF_LDO_RX_EN : 1;
    unsigned int rsvd_0 : 24;
  } b;
} RG_TOP_A8_FIELD_T;

#endif

