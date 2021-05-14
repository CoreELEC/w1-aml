#ifndef __T9026_TOP_H__
#define __T9026_TOP_H__

void t9026_top(void);
void t9026_channel_switch(int p_chn, unsigned char bw, unsigned char restore);
void rf_channel_restore(unsigned char p_chn, unsigned char bw);
void check_spectrum_snr(unsigned int channel, unsigned int bw_idx);
void rf_calibration_before_connect(unsigned char channel, unsigned char bw_idx, unsigned char vid);

void t9026_top_pt(unsigned char channel, unsigned char bw_idx);

#endif
