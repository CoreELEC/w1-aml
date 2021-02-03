import re
import matplotlib.pyplot as plt
import os
import sys
import string

file_name = "rx_ok_log_2019_03_09_15_33_cutoff.txt"

file_path = os.getcwd() + "\\" + file_name
print("plot start---> ")
log = open(file_path,"r")
line_list = log.readlines()

x = []
y = []
z = []
for line in line_list:
    res=list(filter(None,line.split(" ")))
    i = 0
    for e in res:
        if(e == "hirq_rx_ok,"):
            m = res[i+1]            
            y.append(float(m.strip()))
        i = i +1

    for e in res:
        if "data=" in e:
            n = e[-11:-1]
            z.append(int(n,16))
            #print(z)
            
for j in range(len(y)):
    x.append(j)
    j = j+1

                
#plt.text(-50,-100,"X:time\nY:MCS_Mbps")
plt.plot(x,y,'--,',label='hst_rxok')
plt.scatter(x,y,marker='.')
plt.plot(x,z,'--,',label='fw_rxok')
plt.scatter(x,z,marker='.')

plt.xlabel('time')
plt.ylabel('cnt')

plt.title('rx int in host and fw')

plt.legend()

plt.show()

log.close()           
print("<---plot end")
