import serial
import threading
import time
import os
import queue
import math
import  os
import re
#        self.STA_COM.write(chr(0x03).encode())

class dyn_bw_test(object):
    def __init__(self,STA_COM):
        self.STA_COM = serial.Serial(STA_COM, 115200, timeout=1)
        self.timestamp = time.strftime('%Y_%m_%d_%H_%M', time.localtime(time.time()))
        self.log_file = 
        self.directory = os.path.abspath(os.curdir) + self.timestamp
        self.get_reg_cmd="iw dev wlan0 vendor send 0xc3 0xc4 0x09 0x00 0xa0 0x02 0x74\r\n"
    def com_recv(serial):
        while True:
            data = serial.read_all()
            if data == '':
              continue
            else:
              break
            sleep(0.02)
        return data
    
    def init_n5182(self):
        print("init n5182")

    def connect_n5182(self):
        print("connect n5182")
        
    def start_n5182(self, bw, chn):
        print("start n5182")

    def stop_n5182(self):
        print("stop n5182")

    def log_rts_cts_info(self):
        self.STA_COM.write(self.get_reg_cmd.encode())
        a = self.STA_COM.read(1000).decode()
        b = re.findall(r"data=&(.+)",a)
        print("data %s"% b[0])
            
    def check_throughput(self):
        print("top ctrl")
        
    def top_ctrl(self):
        print("top ctrl")

    def close_com(self):
        if self.STA_COM.isOpen():
            self.STA_COM.close()
            print("close com")

if __name__ == "__main__":
    obj_dyn_bw = dyn_bw_test("COM5")
    obj_dyn_bw.init_n5182()
    obj_dyn_bw.connect_n5182()
    for i in range(0, 100,1):
        obj_dyn_bw.start_n5182(1,149)
        obj_dyn_bw.log_rts_cts_info()
        obj_dyn_bw.stop_n5182()

    obj_dyn_bw.close_com()
    
    

