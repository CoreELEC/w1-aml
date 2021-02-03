import re
import matplotlib.pyplot as plt
import os
import sys
import serial
import time
import os
import datetime


class rpt_get_log(object):
    def __init__(self):
        self.curr_path = os.path.abspath('00-00-00-00-00-00_test_5g_1019.log')#os.path.abspath('.')#sys.argv[0]
        print (self.curr_path)
        self.rd_lines=[]
        self.idx = 0
        self.flag = 0
        #self.now_time = datetime.datetime.now().strftime('\%m\-\%d %H\:%M')
        self.file_name = ".sorted"
    def process_log(self):
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
            s = line.split()

            #filter the usefule line
            if "uct" not in s:
                line = f.readline()
                continue
            
            #location the element in list and get the dbm when rx pkt num < 900 
            uct_idx = s.index("uct")
            dbn_idx = uct_idx -1
            if int(s[uct_idx+1]) < 900:
                if self.flag == 0:
                    print (line.split()[1], line.split()[2],float(s[uct_idx-1]) + 0.5, int(s[uct_idx+1]) )
                    x = str(line.split()[1]) + " " + str(line.split()[2]) + " " + str(float(s[uct_idx-1]) + 0.5)  + " "  + str(int(s[uct_idx+1])) + "\r"
                    file_wt.write(x)
                    self.flag = 1
            else:
                self.flag = 0

            line = f.readline()
            
        f.close()
        file_wt.close()
        
if __name__ == "__main__":
   _obj = rpt_get_log()
   _obj.process_log()
    


