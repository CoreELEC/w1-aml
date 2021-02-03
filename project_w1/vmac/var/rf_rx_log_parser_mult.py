import re
import matplotlib.pyplot as plt
import os
import sys
#import serial
import time
import os
import datetime
import numpy as np

class rpt_get_log(object):
    def __init__(self):
        self.curr_path = os.path.abspath('0_agc.log')#os.path.abspath('.')#sys.argv[0]
        print (self.curr_path)
        self.rd_lines=[]
        self.idx = 0
        self.flag = 0
        #self.now_time = datetime.datetime.now().strftime('\%m\-\%d %H\:%M')
        self.file_name = ".sorted"
        self.tag_list = ["ucast","bcast","fcs","other","r_agc_fine_db","r_rx_ic_gain",\
                         "r_pow_sat_db","r_primary20_db","r_primary40_db","r_primary80_db",\
                         "r_rssi0_db","r_rssi1_db","r_rssi2_db","r_rssi3_db","r_ant_nfloor_qdb","r_snr_qdb"]
    
        self.data_array =[[0.0 for col in range(16)] for row in range(128)]
        self.array_row = [i for i in range(128)]
    def process_log(self):
        col = 0
        row = 0
        f = open(self.curr_path,"r")
        file_wt = open(self.curr_path+ self.file_name ,"w+")
        line = f.readline()
        while line:
            #get a clean line
            line = line.strip()

            #skip the space line
            if not len(line) or line.startswith('#'):
                line = f.readline()
                continue

            #filter the 'ucast' line
            s = line.split()
            #print (s)
            if "ucast" not in s:
                line = f.readline()
                continue
            
            for i, tag in enumerate(s):
                if tag in self.tag_list:
                    idx = s.index(tag) +1
                    self.data_array[row][col] = s[idx]
                    #print(self.data_array[col][row], col, row)
                    col +=1
                    
            row += 1
            col = 0
            line = f.readline()
            
        f.close()
        file_wt.close()
        row =0
        #a = np.array(self.data_array)
        #print( a[:,0])
        #print(self.data_array)

    def plot_data(self):
     a = np.array(self.data_array)
     color =['b','g','r','c','m','y','k']
     show_tag=["uct","bct","fcs","other","fine","gain","sat"]
     for i,clr in enumerate(color):
         s_label =  show_tag[i]
         plt.plot(self.array_row,a[:,i],label= str(s_label),color=clr,marker='.')

     plt.legend(loc='best', fontsize=8)
     plt.show()
     #print(self.array_row)
   
        
if __name__ == "__main__":
   _obj = rpt_get_log()
   _obj.process_log()
   _obj.plot_data()
    


