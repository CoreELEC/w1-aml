import re
import matplotlib.pyplot as plt
import os
import sys

file_name = "sta_full_log_2019_03_06_15_25.txt"

file_path = os.getcwd() + "\\" + file_name
print("plot start---> ")
log = open(file_path,"r")
line_list = log.readlines()

x = []
y = []
for line in line_list:
    if("/sec" in line and "bits/sec" in line):
        res=list(filter(None,line.split(" ")))        
        i = 0
        for e in res:
            if(e == "Mbits/sec" or e == "bits/sec"):
               y.append(float(res[i-1])) 
            if(e == "Kbits/sec"):
               y.append(float(res[i-1])/1000) 
            i = i +1

for j in range(len(y)):
    x.append(j)
    j = j+1
                    
plt.text(-10,300,"X:time\nY:MCS_Mbps")
plt.plot(x,y,'--,')
plt.scatter(x,y,marker='.')
plt.show()

log.close()           
print("<---plot end")
