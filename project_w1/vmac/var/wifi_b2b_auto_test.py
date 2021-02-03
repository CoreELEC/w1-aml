import serial
import threading
import time
import os
import queue
import math
def ping_function_decorator(function):
    def wrapper(self,*args,**kwargs):
        self.STA_COM.write(chr(0x03).encode())
        self.AP_COM.write(chr(0x03).encode())
        function(self,*args,**kwargs)
        self.AP_COM.write(self.ping_command.encode())
        time.sleep(2)
        for i in range(5):
            time.sleep(1)
            if not self.ping_result.empty():
                print("%s\n"%self.ping_result.get())
                break
            elif i > 3:
                self.STA_COM.write(chr(0x03).encode())
                self.AP_COM.write(chr(0x03).encode())
                print("Ping no pass!\n")
                self.ping_no_pass = True
                return False
        self.STA_COM.write(chr(0x03).encode())
        self.AP_COM.write(chr(0x03).encode())
    return wrapper
class B2B_function_test(object):
    def __init__(self, AP_COM, STA_COM):
        self.AP_COM = serial.Serial(AP_COM, 115200, timeout=1)
        self.STA_COM = serial.Serial(STA_COM, 115200, timeout=1)
        self.timestamp = time.strftime('%Y_%m_%d_%H_%M', time.localtime(time.time()))
        self.directory = "./" + self.timestamp
        if not os.path.exists(self.directory):
            os.mkdir(self.directory)
        self.noise_on = 0 #default is on
        self.noise_index = 0
        self.save_log_type = 0 # 1:TCP, 0:UDP for log save
        self.sta_tcp_result = open(self.directory + r"\sta_tcp_result.txt","w")
        self.sta_udp_result = open(self.directory + r"\sta_udp_result.txt","w")
        #self.noise_list = ["0x03", "0x13",  "0x23",  "0x33", "0x43","0x53", "0x63", "0x73"]
        self.noise_list = ["0x53", "0x63", "0x73"]
        #self.noise_list = [  "0x31", "0x41","0x51", "0x61", "0x71"]
        #self.noise_list = [ "0x41"]
        #self.noise_list = [ "0x71","0x71","0x71","0x71","0x71","0x71","0x71","0x71","0x71"]
        #self.ap_fullog_file_path = "E:\\jikai.mao\\ap_full_log_%s.txt" % self.timestamp
        #self.sta_fullog_file_path = "E:\\jikai.mao\\sta_full_log_%s.txt" % self.timestamp
        self.ap_fullog_file_path = self.directory + r"\ap_full_log_%s.txt" % self.timestamp
        self.sta_fullog_file_path = self.directory + r"\sta_full_log_%s.txt" % self.timestamp
        
        self.ap_fullog_file = open(self.ap_fullog_file_path,"w")
        self.sta_fullog_file = open(self.sta_fullog_file_path, "w")
        self.hold_test_parameters = []
        self.test_time = 5
        self.ap_log_flag = 1
        self.sta_log_flag = 1
        self.ping_no_pass = False
        self.ap_init_flag = queue.Queue()
        self.sta_init_flag = queue.Queue()
        self.ap_rmmod_flag = queue.Queue()
        self.sta_rmmod_flag = queue.Queue()
        self.connect_flag = queue.Queue()
        self.ap_hostapd_up_flag = queue.Queue()
        self.ping_result = queue.Queue()
        self.ap_ip_addr = "192.168.1.88"
        self.sta_ip_addr = "192.168.1.99"
        self.ping_command = "ping %s\n" % self.sta_ip_addr
        self.su_command = "su\r\n mount -o rw,remount /system\r\n\n"
        self.kill_hostapd = "killall hostapd\r\n"
        self.rmmod_command = "rmmod vlsicomm.ko\r\n"
        self.set_pri_channel_command = "iw dev wlan0 vendor send 0xc3 0xc4 0x0a 0x00 0x00 0xb2 0x2c "
        self.ap_init_command = "insmod /system/lib/vlsicomm_8e659_w1_b2b_1213_0948.ko " \
                               "mac_addr=00:01:02:58:00:12;ifconfig " \
                               "wlan0 %s up;\r\n" % self.ap_ip_addr
        self.sta_init_command = "insmod /system/lib/vlsicomm_8e659_w1_b2b_1213_0948.ko " \
                                "mac_addr=00:01:02:58:00:13;" \
                                "ifconfig wlan0 %s up;\r\n" % self.sta_ip_addr
        #100 is auto rate
        self.test_channel_rate = {"vht80M": [[153, ], [0,1,2,3,4,5,6,7,8,9]], "vht40M": [[153, ], [0,1,2,3,4,5,6,7,8,9]],                                     
                                  "vht20M": [[153, ], [0,1,2,3,4,5,6,7,8]], "ht40M": [[153, ], [0,1,2,3,4,5,6,7]],
                                  "ht20M": [[153, ], [0,1,2,3,4,5,6,7]], "11g": [[6, ], [6,9,12,18,24,36,48,54]],
                                  "11b": [[6, ], [1,2,5.5,11]]}
        self.vht_ht_pri_channel = {"vht80M": {153: "0xa2 0x00 0x00 0x00",
                                               157: "0xa1 0x00 0x00 0x00"},
                                   "vht40M": {153: "0x91 0x00 0x00 0x00",
                                               157: "0x92 0x00 0x00 0x00"},
                                   "vht20M": {153: "0x80 0x00 0x00 0x00",
                                               157: "0x80 0x00 0x00 0x00"},
                                   "ht40M": {153: "0x91 0x00 0x00 0x00",
                                              157: "0x92 0x00 0x00 0x00"},
                                   "ht20M": {153: "0x80 0x00 0x00 0x00",
                                              157: "0x80 0x00 0x00 0x00"},
                                   "11b": {6: "0x80 0x00 0x00 0x00",
                                           11: "0x80 0x00 0x00 0x00"},
                                   "11g": {6: "0x80 0x00 0x00 0x00",
                                           11: "0x80 0x00 0x00 0x00"}}
        self.hostapd_up_file_path = {153: "hostapd -B -d /system/etc/wifi/B2B_Verify_config/",
                                     157: "hostapd -B -d /system/etc/wifi/",
                                     6: "hostapd -B -d /system/etc/wifi/B2B_Verify_config/",
                                     11: "hostapd -B -d /system/etc/wifi/"}
        self.set_rate_command = {"vht": "iw dev wlan0 set bitrates vht-mcs-2.4 1:",
                                 "ht": "iw dev wlan0 set bitrates ht-mcs-2.4 ",
                                 "11b_11g": "iw dev wlan0 set bitrates legacy-2.4 "}
        self.iperf_command = {"TCP": {"AP": "iperf -c %s -i 1 -t %d\r\n" % (self.sta_ip_addr, self.test_time),
                                      "STA": "iperf -s -i 1\r\n"},
                              "UDP": {
                                  "AP": "iperf -c %s -i 1 -t %d -u -b 500M &\r\n" % (self.sta_ip_addr, self.test_time),
                                  "STA": "iperf -s -i 1 -u &\r\n"}}
                                 
    def ap_sta_init(self):
        self.AP_COM.write(self.su_command.encode())
        self.STA_COM.write(self.su_command.encode())
        time.sleep(1)
        self.AP_COM.write(self.ap_init_command.encode())
        for i in range(15):
            time.sleep(1)
            if not self.ap_init_flag.empty():
                print("AP %s initialization!\n"%self.ap_init_flag.get())
                self.ap_init_flag.queue.clear()
                break
            if i > 13 :
                print("AP failed initialization!\n")
                return False
        self.STA_COM.write(self.sta_init_command.encode())
        for i in range(15):
            time.sleep(1)
            if not self.sta_init_flag.empty():
                print("STA %s initialization!\n"%self.sta_init_flag.get())
                self.sta_init_flag.queue.clear()
                break
            if i > 13 :
                print("STA failed initialization!\n")
                return False
        time.sleep(1)
        return True
    def set_init_parameter(self, test_mode, ch):
        # ap hostapd up
        self.ap_hostapd_command = self.hostapd_up_file_path[ch] + "hostapd_%s.conf" % test_mode + "\r\n"
        self.AP_COM.write(self.ap_hostapd_command.encode())
        print(self.ap_hostapd_command)
        for i in range(15):
            time.sleep(1)
            if not self.ap_hostapd_up_flag.empty():
                print(self.ap_hostapd_up_flag.get() + "\n")
                self.ap_hostapd_up_flag.queue.clear()
                break
            if i > 13:
                print("AP hostapd up failed!\n")
                return False
        time.sleep(2)
        # set sta pri channel
        self.sta_pri_channel_command = self.set_pri_channel_command + self.vht_ht_pri_channel[test_mode][ch] + "\r\n"
        self.STA_COM.write(self.sta_pri_channel_command.encode())
        print("STA set pri channel-->%s"%self.sta_pri_channel_command)
        time.sleep(2)
        self.STA_COM.write(chr(0x03).encode())
        self.AP_COM.write(chr(0x03).encode())
        time.sleep(1)
        return True
    #@ping_function_decorator
    def set_test_rate(self, test_mode, rate):
        enable_auto_rate = "iw dev wlan0 vendor send 0xc3 0xc4 0x38 0x00 0x00 0x00 0x00"
        if "vht" in test_mode:
            self.rate_index = "vht"
            tmp_rate = 4
        elif "ht" in test_mode:
            self.rate_index = "ht"
            tmp_rate = 4
        elif "11g" in test_mode:
            self.rate_index = "11b_11g"
            tmp_rate = 12
        elif "11b" in test_mode:
            self.rate_index = "11b_11g"
            tmp_rate = 5.5
        else:
            print("Rate index error!\n")
       
        if( rate == 100 ):
            set_auto_rate = enable_auto_rate + "\r\n"
            self.set_test_rate_command = self.set_rate_command[self.rate_index] + str(tmp_rate) + "\r\n"
            print(self.set_test_rate_command)
            self.AP_COM.write( set_auto_rate.encode())
            time.sleep(1)
            self.STA_COM.write(self.set_test_rate_command.encode( ))
            time.sleep(1)
        else:
            self.set_test_rate_command = self.set_rate_command[self.rate_index] + str(rate) + "\r\n"
            print(self.set_test_rate_command)
            self.AP_COM.write(self.set_test_rate_command.encode())
            time.sleep(1)
            self.STA_COM.write(self.set_test_rate_command.encode())
            time.sleep(1)
             
    def send_iperf_command(self, type_data):
        print("-*-*-*-*-*-*-*-*-*-*-*-*- %s -*-*-*-*-*-*-*-*-*-*-*-"%type_data)
        self.send_sta_iperf_command = self.iperf_command[type_data]["STA"] + "\r\n"
        self.STA_COM.write(self.send_sta_iperf_command.encode())
        time.sleep(2)
        self.send_ap_iperf_command = self.iperf_command[type_data]["AP"] + "\r\n"
        self.AP_COM.write(self.send_ap_iperf_command.encode())
        time.sleep(3)
       
    def sta_connect_ap(self):
        set_noise_cmd = "iw dev wlan0 vendor send 0xc3 0xc4 0x0a 0x00 0x00 0xb0 0x78 0x00 0x00 0x07 "+ self.noise_list[self.noise_index] + "\r\n"
        set_phase_cmd = "iw dev wlan0 vendor send 0xc3 0xc4 0x0a 0x00 0x00 0xb0 0x78 0x00 0x00 0x07 0x03" + "\r\n"
        set_static_arp = "arp -s 192.168.1.99 00:01:02:58:00:13" + "\r\n"
        time.sleep(5)
        self.connect_command = "iw dev wlan0 connect lg_test\r\n"
        self.STA_COM.write(self.connect_command.encode())
        for i in range(100):
            time.sleep(1)
            if(i>50):
                self.STA_COM.write(chr(0x03).encode())
                self.STA_COM.write(self.connect_command.encode())
            if not self.connect_flag.empty():
                print(self.connect_flag.get() + "\n")
                self.connect_flag.queue.clear()
                #set ap is static arp
                self.AP_COM.write(chr(0x03).encode())
                self.AP_COM.write(set_static_arp.encode())
                time.sleep(2)
                
                #set ap phase
                self.AP_COM.write(chr(0x03).encode())
                self.AP_COM.write(set_phase_cmd.encode())
                time.sleep(2)
                
                if( self.noise_on == 1):
                    if self.save_log_type:
                        self.sta_udp_result.writelines(set_noise_cmd)
                    else:
                        self.sta_tcp_result.writelines(set_noise_cmd)

                    self.STA_COM.write(chr(0x03).encode())
                    self.STA_COM.write(set_noise_cmd.encode())
                    time.sleep(3)
                else:
                    self.STA_COM.write(chr(0x03).encode())    #set sta phase
                    self.STA_COM.write(set_phase_cmd.encode())
                   
                break
            elif i > 98:
                print("STA connected AP failed!\n")
                return False
        time.sleep(1)
        self.STA_COM.write(chr(0x03).encode())
        self.AP_COM.write(chr(0x03).encode())
        time.sleep(1)
        return True
    def rmmod_sta_ap(self):
        self.ap_killhostapd_command = "killall hostapd\r\n"
        self.rmmod_command = "rmmod vlsicomm.ko\r\n"
        self.STA_COM.write(self.rmmod_command.encode())
        for i in range(15):
            time.sleep(1)
            if not self.sta_rmmod_flag.empty():
                print(self.sta_rmmod_flag.get()+"\n")
                self.sta_rmmod_flag.queue.clear()
                break
            if i > 13:
                print("STA rmmod failed!\n")
                return False
        self.AP_COM.write(self.ap_killhostapd_command.encode())
        time.sleep(5)
        self.AP_COM.write(self.rmmod_command.encode())
        for i in range(15):
            time.sleep(1)
            if not self.ap_rmmod_flag.empty():
                print(self.ap_rmmod_flag.get()+"\n")
                self.ap_rmmod_flag.queue.clear()
                break
            if i > 13:
                print("AP rmmod failed!\n")
                return False
        return True
    def print_ap_log_info(self):
        while self.ap_log_flag:
            if self.AP_COM.isOpen():
                self.ap_comm_info = self.AP_COM.readline()
                if "wifi_mac_hardstart" in self.ap_comm_info.decode():
                    self.ap_init_flag.put("successful")
                elif "exit_aml_sdio" in self.ap_comm_info.decode():
                    self.ap_rmmod_flag.put("AP rmmod successfully!")
                elif "state SCAN->CONNECTED" in self.ap_comm_info.decode():
                    self.ap_hostapd_up_flag.put("AP hostapd up successfully!")
                elif "bytes from %s"%self.sta_ip_addr in self.ap_comm_info.decode():
                    self.ping_result.put("ping successfully")
                    #print(self.ap_comm_info.decode())
            self.ap_fullog_file.writelines(self.ap_comm_info.decode())

    def print_sta_log_info(self):
        throughput_arr = []
        throughput = 0
        count = 0;
        sum_square = 0;
        
        while self.sta_log_flag:
            if self.STA_COM.isOpen():
                self.sta_comm_info = self.STA_COM.readline()
                if "wifi_mac_hardstart" in self.sta_comm_info.decode():
                    self.sta_init_flag.put("successful")
                elif "ASSOC->CONNECTED" in self.sta_comm_info.decode():
                    self.connect_flag.put("STA connected AP successfully!")
                elif "Mbits/sec" in self.sta_comm_info.decode():
                    print(self.sta_comm_info.decode())
                elif "Kbits/sec" in self.sta_comm_info.decode():
                    print(self.sta_comm_info.decode())
                elif "exit_aml_sdio" in self.sta_comm_info.decode():
                    self.sta_rmmod_flag.put("STA rmmod successfully!")
            self.sta_fullog_file.writelines(self.sta_comm_info.decode())
            self.sta_fullog_file.flush()
            msg = self.sta_comm_info.decode()

            if( "bitrates" in msg ):
                msg = msg.replace("b'130|root@p200:/ # ", '')
                msg = msg.strip( '130\|root\@p200:\/ \#')
                msg = msg.strip('\r\r\n')
                continue
                
            result = ".0-" 
            if ( result not in msg ):
                continue
            if "datagrams received out-of-order" in msg:
                continue
            if "datagrams received" in msg:
                continue
            if "out-of-order" in msg:
                continue
            if "NOHZ" in msg:
                continue
            
            msg1 = msg
            msg1 = msg1.replace(".0- ", ".0-")

            split_res =  msg1.split( )

            throughput = float(split_res[6])
            if( "Kbits/sec" in msg ):
                throughput = throughput/1024


            if( " bits/sec" in msg ):
                throughput = throughput/(1024*1024)
                
            split_res = str(split_res[2]).split("-")
           

            #if throughput == 0:
            #  trig_dbg ="sh /system/lib/rw.sh 0x0000b100 0x01930008\r\n"
            #  self.STA_COM.write(trig_dbg.encode())
            #  self.AP_COM.write(trig_dbg.encode())
            #   trig_dbg ="iw dev wlan0 vendor send 0xc3 0xc4 0x99 0x2c 0x0 0x0 0x0\r\n"
            #   self.STA_COM.write(trig_dbg.encode())

             #ignore first second,because fist second may be have rx sram full to affect throughput
            if( (float(split_res[1]) - float(split_res[0]) <= 2.0 ) and (float(split_res[0]) == 0.0) ):
                continue 
                                                            
            if (float(split_res[1]) - float(split_res[0])) <= 2.0:
                throughput_arr.append(throughput)
                count = count+1
                #print("throughput=%f" %throughput)
                #print(throughput_arr)
                continue
            
            #delete delete the last second value,because the jitter is relatively large
            throughput_arr.pop()
            count = count-1
            throughput_sum = 0
            for j in range(len(throughput_arr)):
                throughput_sum += throughput_arr[j]
                
            throughput =  throughput_sum/len(throughput_arr)
            for j in range(len(throughput_arr)):
                sum_square += (throughput_arr[j]-throughput)*(throughput_arr[j]-throughput)
            print("sum_square =%f, throughput=%f" %(sum_square, throughput))
            if(count != len(throughput_arr)):
                print("error  len error\n")
                os._exit()
            standard_deviation = math.sqrt( sum_square/len(throughput_arr) )
            sum_square = 0;
            count = 0;
            throughput_arr.clear()
            del throughput_arr[:]
                
            if "[  4]" in msg:
                msg = msg.replace("[  4]", " ") #tcp iperv log has the log message
            if "[  3]" in msg: 
                msg = msg.replace("[  3]", " ") #udp iperv log has the log message  
            if "Mbits/sec" in msg or  "Kbits/sec" in msg:
                msg = msg.strip('\r\r\n')
                msg = msg.strip( '130\|root\@p200:\/ \#')
                if self.save_log_type:
                    self.sta_udp_result.writelines(msg + "  ∑:" + str(standard_deviation) + "\n")
                else:
                    self.sta_tcp_result.writelines(msg + "  ∑:" + str(standard_deviation) + "\n")
            self.sta_udp_result.flush()
            self.sta_tcp_result.flush()

            
    def print_sta_ap_thread(self):
        self.thread_arrg = []
        self.ap_print = threading.Thread(target=self.print_ap_log_info)
        self.sta_print = threading.Thread(target=self.print_sta_log_info)
        self.thread_arrg.append(self.ap_print)
        self.thread_arrg.append(self.sta_print)
        for t in self.thread_arrg:
            t.setDaemon(True)
            t.start()
    def start_test(self):
        print("-*-*-*-*-*-*-Wifi B2B Verify start-*-*-*-*-*-*-*-*\n")
        self.test_case = ["ht40M","vht20M"]#"11b","11g","ht20M","ht40M","vht20M","vht40M","vht80M"
        #self.print_sta_ap_thread()
        for case in self.test_case:
            self.sta_udp_result.writelines( case + "\n")
            self.sta_udp_result.flush()
            self.sta_tcp_result.writelines( case + "\n")
            self.sta_tcp_result.flush()
            for channel in self.test_channel_rate[case][0]:
                self.ap_sta_init()
                self.set_init_parameter(case, channel)
                if self.sta_connect_ap():
                    for rate in self.test_channel_rate[case][1]:
                        self.STA_COM.write(chr(0x03).encode())
                        self.AP_COM.write(chr(0x03).encode())
                        self.set_test_rate(case, rate)
                        if "vht" in case:
                            print("Test mode-->%s Test channel--> %d,test rate-->MCS%d"%(case,channel,rate))
                        elif "ht" in case:
                            print("Test mode-->%s Test channel--> %d,test rate-->MCS%d" % (case, channel, rate))
                        else:
                            print("Test mode-->%s Test channel--> %d,test rate-->Legacy %d" % (case,channel, rate))
                        #for data in ["TCP", "UDP"]:
                        self.AP_COM.write(chr(0x03).encode())
                        time.sleep(2)
                        self.STA_COM.write(chr(0x03).encode())
                        for data in ["TCP","UDP"]:
                            if data == "TCP":
                                self.save_log_type = 0
                                self.sta_tcp_result.writelines(self.set_test_rate_command.strip('\r\n')+ "--->")
                                self.sta_tcp_result.flush()
                            else:
                                self.save_log_type = 1
                                self.sta_udp_result.writelines(self.set_test_rate_command.strip('\r\n') + "--->")
                                self.sta_udp_result.flush()
 
                            self.send_iperf_command(data)
                            time.sleep(self.test_time+5)
                            self.AP_COM.write(chr(0x03).encode())
                            self.STA_COM.write(chr(0x03).encode())
                            self.AP_COM.write("killall iperf\r\n".encode())
                            self.STA_COM.write("killall iperf\r\n".encode())
                            time.sleep(2)
                            self.AP_COM.write(chr(0x03).encode())
                            self.STA_COM.write(chr(0x03).encode())
                            
                    self.rmmod_sta_ap()
                else:
                    #self.hold_test_parameters.append((case,channel,["TCP", "UDP"],self.test_channel_rate[case][1]))
                    self.hold_test_parameters.append((case,channel,["UDP"],self.test_channel_rate[case][1]))
                    print("Test mode-->%s Test channel-->%d\n"%(case,channel))
                    print("Parameters are saved!Wait for double checking!\n")
                    self.rmmod_sta_ap()
        if len(self.hold_test_parameters) > 0:
            for err_parameters in self.hold_test_parameters:
                self.double_check(err_parameters[0],err_parameters[1],err_parameters[2],err_parameters[3])
                self.hold_test_parameters.remove(err_parameters)
        else:
            self.close_file_function()
    def close_file_function(self):
        self.ap_log_flag = 1
        #self.sta_log_flag = 0
        #time.sleep(2)
        #self.ap_fullog_file.close()
        #self.sta_fullog_file.close()
        #self.AP_COM.close()
        #self.STA_COM.close()
        
    def double_check(self,tesmode,channel,typedata,ratelist):
        print("Now is checking error data.\n")
        self.print_sta_ap_thread()
        self.ap_sta_init()
        self.set_init_parameter(tesmode, channel)
        self.sta_connect_ap()
        for rate in ratelist:
            self.set_test_rate(tesmode, rate)
            if "vht" in tesmode:
                print("Test mode-->%s Test channel--> %d,test rate-->MCS%d" % (tesmode, channel, rate))
            elif "ht" in tesmode:
                print("Test mode-->%s Test channel--> %d,test rate-->MCS%d" % (tesmode, channel, rate))
            else:
                print("Test mode-->%s Test channel--> %d,test rate-->Legacy %d" % (tesmode, channel, rate))
            for data in typedata:
                self.AP_COM.write(chr(0x03).encode())
                self.STA_COM.write(chr(0x03).encode())
                self.send_iperf_command(data)
                time.sleep(self.test_time + 2)
                self.STA_COM.write(chr(0x03).encode())
                self.AP_COM.write(chr(0x03).encode())
        self.rmmod_sta_ap()
        self.close_file_function()
if __name__ == "__main__":
    b2b_test = B2B_function_test("COM3", "COM10")
    b2b_test.print_sta_ap_thread()
    if( b2b_test.noise_on == 0 ):
        b2b_test.start_test()
    else:
        for i in range(len(b2b_test.noise_list)):
            b2b_test.noise_index = i
            b2b_test.start_test()
    #b2b_test.double_check("ht_40",6,["TCP","UDP"],[48,54])


