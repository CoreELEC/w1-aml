#ifdef RF_D_TX_REG
#else
#define RF_D_TX_REG


#define RF_D_TX_REG_BASE                          (0x200)

#define RG_TX_A0                                  (RF_D_TX_REG_BASE + 0x0)
// Bit 3   :0      rg_wf5g_txm_ct                 U     RW        default = 'h7
// Bit 5   :4      rg_wf5g_txm_dcoc_i             U     RW        default = 'h0
// Bit 7   :6      rg_wf5g_txm_dcoc_q             U     RW        default = 'h0
// Bit 13  :12     rg_wf5g_txm_lobuf_ict          U     RW        default = 'h1
// Bit 18  :15     rg_wf5g_txm_v2i_vcm_gain       U     RW        default = 'h7
// Bit 23  :21     rg_wf5g_txm_vb3                U     RW        default = 'h3
// Bit 26  :24     rg_wf5g_txm_vb_lo              U     RW        default = 'h3
typedef union RG_TX_A0_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_WF5G_TXM_CT : 4;
    unsigned int RG_WF5G_TXM_DCOC_I : 2;
    unsigned int RG_WF5G_TXM_DCOC_Q : 2;
    unsigned int rsvd_0 : 4;
    unsigned int RG_WF5G_TXM_LOBUF_ICT : 2;
    unsigned int rsvd_1 : 1;
    unsigned int RG_WF5G_TXM_V2I_VCM_GAIN : 4;
    unsigned int rsvd_2 : 2;
    unsigned int RG_WF5G_TXM_VB3 : 3;
    unsigned int RG_WF5G_TXM_VB_LO : 3;
    unsigned int rsvd_3 : 5;
  } b;
} RG_TX_A0_FIELD_T;

#define RG_TX_A1                                  (RF_D_TX_REG_BASE + 0x4)
// Bit 6   :4      rg_wf5g_txpa_vb2               U     RW        default = 'h4
// Bit 9   :7      rg_wf5g_adb_cbank_tune         U     RW        default = 'h3
// Bit 13  :10     rg_wf5g_adb_core_dcib_tune     U     RW        default = 'h4
// Bit 17  :14     rg_wf5g_adb_core_dcvb_tune     U     RW        default = 'h0
// Bit 21  :18     rg_wf5g_adb_core_ed_gain       U     RW        default = 'h2
// Bit 22          rg_wf5g_adb_ed_en              U     RW        default = 'h0
// Bit 23          rg_wf5g_adb_en                 U     RW        default = 'h1
// Bit 26  :24     rg_wf5g_adb_ibias_cgm          U     RW        default = 'h2
// Bit 29  :27     rg_wf5g_adb_ibias_ptat         U     RW        default = 'h2
typedef union RG_TX_A1_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int rsvd_0 : 4;
    unsigned int RG_WF5G_TXPA_VB2 : 3;
    unsigned int RG_WF5G_ADB_CBANK_TUNE : 3;
    unsigned int RG_WF5G_ADB_CORE_DCIB_TUNE : 4;
    unsigned int RG_WF5G_ADB_CORE_DCVB_TUNE : 4;
    unsigned int RG_WF5G_ADB_CORE_ED_GAIN : 4;
    unsigned int RG_WF5G_ADB_ED_EN : 1;
    unsigned int RG_WF5G_ADB_EN : 1;
    unsigned int RG_WF5G_ADB_IBIAS_CGM : 3;
    unsigned int RG_WF5G_ADB_IBIAS_PTAT : 3;
    unsigned int rsvd_1 : 2;
  } b;
} RG_TX_A1_FIELD_T;

#define RG_TX_A2                                  (RF_D_TX_REG_BASE + 0x8)
// Bit 0           rg_wf5g_iqcal_en               U     RW        default = 'h0
// Bit 3   :1      rg_wf5g_pwr_ctrl_atten         U     RW        default = 'h0
// Bit 4           rg_wf5g_pwr_ctrl_en            U     RW        default = 'h0
// Bit 7   :5      rg_wf5g_txm_dect_vblo          U     RW        default = 'h3
// Bit 10  :8      rg_wf5g_tx_ib_i                U     RW        default = 'h3
// Bit 16  :13     rg_wf5g_tx_vrct_d              U     RW        default = 'h8
// Bit 20  :17     rg_wf5g_tx_vrct_u              U     RW        default = 'h8
// Bit 21          rg_wf5g_tp_en                  U     RW        default = 'h0
// Bit 26  :22     rg_wf5g_tp_sel                 U     RW        default = 'h0
// Bit 28          rg_wf5g_txm_rx_iq_iq_cal_en     U     RW        default = 'h0
typedef union RG_TX_A2_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_WF5G_IQCAL_EN : 1;
    unsigned int RG_WF5G_PWR_CTRL_ATTEN : 3;
    unsigned int RG_WF5G_PWR_CTRL_EN : 1;
    unsigned int RG_WF5G_TXM_DECT_VBLO : 3;
    unsigned int RG_WF5G_TX_IB_I : 3;
    unsigned int rsvd_0 : 2;
    unsigned int RG_WF5G_TX_VRCT_D : 4;
    unsigned int RG_WF5G_TX_VRCT_U : 4;
    unsigned int RG_WF5G_TP_EN : 1;
    unsigned int RG_WF5G_TP_SEL : 5;
    unsigned int rsvd_1 : 1;
    unsigned int RG_WF5G_TXM_RX_IQ_IQ_CAL_EN : 1;
    unsigned int rsvd_2 : 3;
  } b;
} RG_TX_A2_FIELD_T;

#define RG_TX_A3                                  (RF_D_TX_REG_BASE + 0xc)
// Bit 2   :0      rg_wf5g_txpa_gain_man          U     RW        default = 'h7
// Bit 6   :4      rg_wf5g_txm_gain_man           U     RW        default = 'h7
// Bit 9   :8      rg_wf5g_txm_v2i_gain_man       U     RW        default = 'h0
// Bit 12          rg_wf5g_tx_gain_man_mode       U     RW        default = 'h1
typedef union RG_TX_A3_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_WF5G_TXPA_GAIN_MAN : 3;
    unsigned int rsvd_0 : 1;
    unsigned int RG_WF5G_TXM_GAIN_MAN : 3;
    unsigned int rsvd_1 : 1;
    unsigned int RG_WF5G_TXM_V2I_GAIN_MAN : 2;
    unsigned int rsvd_2 : 2;
    unsigned int RG_WF5G_TX_GAIN_MAN_MODE : 1;
    unsigned int rsvd_3 : 19;
  } b;
} RG_TX_A3_FIELD_T;

#define RG_TX_A4                                  (RF_D_TX_REG_BASE + 0x10)
// Bit 0           rg_m0_wf5g_txm_en              U     RW        default = 'h0
// Bit 1           rg_m0_wf5g_txm_v2i_en          U     RW        default = 'h0
// Bit 2           rg_m0_wf5g_txpa_en             U     RW        default = 'h0
// Bit 3           rg_m0_wf5g_tx_ib_en            U     RW        default = 'h0
// Bit 4           rg_m0_wf5g_tx_vrct_en          U     RW        default = 'h0
typedef union RG_TX_A4_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_M0_WF5G_TXM_EN : 1;
    unsigned int RG_M0_WF5G_TXM_V2I_EN : 1;
    unsigned int RG_M0_WF5G_TXPA_EN : 1;
    unsigned int RG_M0_WF5G_TX_IB_EN : 1;
    unsigned int RG_M0_WF5G_TX_VRCT_EN : 1;
    unsigned int rsvd_0 : 27;
  } b;
} RG_TX_A4_FIELD_T;

#define RG_TX_A5                                  (RF_D_TX_REG_BASE + 0x14)
// Bit 0           rg_m1_wf5g_txm_en              U     RW        default = 'h0
// Bit 1           rg_m1_wf5g_txm_v2i_en          U     RW        default = 'h0
// Bit 2           rg_m1_wf5g_txpa_en             U     RW        default = 'h0
// Bit 3           rg_m1_wf5g_tx_ib_en            U     RW        default = 'h0
// Bit 4           rg_m1_wf5g_tx_vrct_en          U     RW        default = 'h0
typedef union RG_TX_A5_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_M1_WF5G_TXM_EN : 1;
    unsigned int RG_M1_WF5G_TXM_V2I_EN : 1;
    unsigned int RG_M1_WF5G_TXPA_EN : 1;
    unsigned int RG_M1_WF5G_TX_IB_EN : 1;
    unsigned int RG_M1_WF5G_TX_VRCT_EN : 1;
    unsigned int rsvd_0 : 27;
  } b;
} RG_TX_A5_FIELD_T;

#define RG_TX_A6                                  (RF_D_TX_REG_BASE + 0x18)
// Bit 0           rg_m2_wf5g_txm_en              U     RW        default = 'h1
// Bit 1           rg_m2_wf5g_txm_v2i_en          U     RW        default = 'h1
// Bit 2           rg_m2_wf5g_txpa_en             U     RW        default = 'h1
// Bit 3           rg_m2_wf5g_tx_ib_en            U     RW        default = 'h1
// Bit 4           rg_m2_wf5g_tx_vrct_en          U     RW        default = 'h1
typedef union RG_TX_A6_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_M2_WF5G_TXM_EN : 1;
    unsigned int RG_M2_WF5G_TXM_V2I_EN : 1;
    unsigned int RG_M2_WF5G_TXPA_EN : 1;
    unsigned int RG_M2_WF5G_TX_IB_EN : 1;
    unsigned int RG_M2_WF5G_TX_VRCT_EN : 1;
    unsigned int rsvd_0 : 27;
  } b;
} RG_TX_A6_FIELD_T;

#define RG_TX_A7                                  (RF_D_TX_REG_BASE + 0x1c)
// Bit 0           rg_m3_wf5g_txm_en              U     RW        default = 'h0
// Bit 1           rg_m3_wf5g_txm_v2i_en          U     RW        default = 'h0
// Bit 2           rg_m3_wf5g_txpa_en             U     RW        default = 'h0
// Bit 3           rg_m3_wf5g_tx_ib_en            U     RW        default = 'h0
// Bit 4           rg_m3_wf5g_tx_vrct_en          U     RW        default = 'h0
typedef union RG_TX_A7_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_M3_WF5G_TXM_EN : 1;
    unsigned int RG_M3_WF5G_TXM_V2I_EN : 1;
    unsigned int RG_M3_WF5G_TXPA_EN : 1;
    unsigned int RG_M3_WF5G_TX_IB_EN : 1;
    unsigned int RG_M3_WF5G_TX_VRCT_EN : 1;
    unsigned int rsvd_0 : 27;
  } b;
} RG_TX_A7_FIELD_T;

#define RG_TX_A8                                  (RF_D_TX_REG_BASE + 0x20)
// Bit 31  :0      rg_wf5g_tx_rsv                 U     RW        default = 'h0
typedef union RG_TX_A8_FIELD
{
  unsigned int data;
  struct
  {
    unsigned int RG_WF5G_TX_RSV : 32;
  } b;
} RG_TX_A8_FIELD_T;

#endif

