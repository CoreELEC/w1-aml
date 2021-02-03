1. only 802.11b support short/long preamble and 1M rate only support long preamble.
2. 11b mode working sequence:
	a. insmod
	b. if up
	c. disable ampdu and amsdu through iw cmd (0x01,0x02)
	d. set preamble type through iw cmd (0x32)
	e. set data rate in both side (for udp, only set Tx side)
	f. run iperf in both side.
3. 