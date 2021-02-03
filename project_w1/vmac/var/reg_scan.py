import serial
import time
import os
import sys

class scan_reg_mem(object):
    def __init__(self, com_num):
        self.COM_X = serial.Serial(com_num, 115200, timeout=1)
        self.ip_addr = "192.168.1.99"
        self.su_cmd = "su\r\n mount -o rw,remount /system\r\n\n"
        self.sys_init_cmd = "insmod /system/lib/8192cu.ko mac_addr=00:01:02:58:00:12\r\n"
        self.ifconfig_cmd = "ifconfig wlan0 %s up\r\n" % self.ip_addr
        self.module_addr_range={"top":[0x7000, 0x70a8],"mac":[0x0000, 0x03f4],"sdio":[0x5000, 0x5108],
                          "agc":[0x8000, 0x827c], "ofdm":[0x9000,0x929c], "dsss":[0xa000, 0xa090],
                          "phy":[0xb000, 0xb230],  "intf":[0xd000, 0xd118],
                          "uart":[0x3000, 0x3180],"co_exist":[0xc000, 0xc3b0]}
        self.module_name = ["top","mac","sdio","agc","ofdm","dsss","phy","intf","uart","co_exist"]
        self.addr_range=[]
        self.timestamp = time.strftime('%Y_%m_%d_%H_%M', time.localtime(time.time()))
        self.reg_scan_log = os.getcwd() + "\\reg_log_%s.txt" % self.timestamp
        self.fd_log = open(self.reg_scan_log,"w+")
        
    def sys_init(self):
        self.COM_X.write(self.su_cmd.encode())
        time.sleep(0.1)
        self.COM_X.write(self.sys_init_cmd.encode())
        time.sleep(0.1)
        self.COM_X.write(self.ifconfig_cmd.encode())
        
    def scan_module(self):
        while(1):
            rd_line = self.COM_X.readline().decode()
            if "wifi_mac_hardstart" in rd_line:
                break
            else:
                continue
            
        for i in self.module_name:
            self.addr_range = self.module_addr_range[i]
            for j in range(self.addr_range[0], self.addr_range[1]):
                    rd_reg_cmd = "iw dev wlan0 vendor send 0xc3 0xc4 0x09 0x00 0x00 0x%x 0x%x\r\n" % (j >>8,j &0xff)
                    self.COM_X.write(rd_reg_cmd.encode())
                    time.sleep(0.1)
                    line = self.COM_X.readline().decode()
                    if "Reg addr" in line:
                        self.fd_log.write(line)
                        print(line)
                    if "Reg data" in line:
                        self.fd_log.write(line)
                        print(line)
                    j = j + 4

        self.fd_log.close()

        if self.COM_X != None:
            self.COM_X.close()
            print("close uart")
                                
if __name__ == "__main__":
    o_scan_reg_mem = scan_reg_mem("COM4")
    o_scan_reg_mem.sys_init()
    o_scan_reg_mem.scan_module()
    
    
    

        
    
                                     
