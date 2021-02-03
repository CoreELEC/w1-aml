import re
import matplotlib.pyplot as plt
import os
import sys
import serial
import time
import os



class rpt_get_log(object):
    def __init__(self, com_num):
        self.COM_X = serial.Serial(com_num, 115200, timeout=1)
        self.timestamp = time.strftime('%Y_%m_%d_%H_%M', time.localtime(time.time()))
        self.file_name = "rx_ok_log_%s.txt" % self.timestamp
        self.file_path = os.getcwd() + "\\" + self.file_name
        self.cmd = "iw dev wlan0 vendor send 0xc3 0xc4 0x99 0x28 0x0 0x0 0x0\r\n"
        self.cmd_rdfw = "iw dev wlan0 vendor send 0xc3 0xc4 0x09 0x00 0x00 0x00 0x7c\r\n"
    def get_log_by_timer(self):
        print("log start---> ")
        test_time = 0
        log_file = open(self.file_path,"w")
        
        self.COM_X.write(chr(0x03).encode())
        while test_time < 120:
            self.COM_X.write(chr(0x03).encode())
            self.COM_X.write(self.cmd.encode())
            while True:
                line = self.COM_X.readline().decode()
                if "hirq_rx_ok" in line:
                    log_file.write(line)
                    break
                else:
                    continue

            self.COM_X.write(chr(0x03).encode())
            self.COM_X.write(self.cmd_rdfw.encode())
            while True:
                line = self.COM_X.readline().decode()
                if "data=" in line:
                    log_file.write(line)
                    break
                else:
                    continue
            test_time = test_time +1
            time.sleep(1)
            print(test_time)
        log_file.close()
        
        if self.COM_X != None:
            self.COM_X.close()
            print("close uart")
     
if __name__ == "__main__":
    o_rpt_get_log = rpt_get_log("COM4")
    o_rpt_get_log.get_log_by_timer()


