#coding=utf-8
import os
import re

def get_files(dir, suffix):   
    res = []
    for root, directory, files in os.walk(dir):  
        for filename in files:
            name, suf = os.path.splitext(filename)
            if suf == suffix:
                res.append(os.path.join(root, filename)) #make up the string to be a path 
    return res

match_list = []
ctend_list = []
mline  =  []
mlines = []
i = 0
z = 0;

#e.g: "#define"  
ipattern = "#define"
os.system('grep -nr "%s" . > ../1' % (ipattern))
os.system('mv ../1 ./')

f = open("1","r")
clines = f.readlines()
for cline in clines:
	tag = re.findall(r"\s\w+\s+",cline) #e.g ../../x.h:120  #define WIFI_MODE (0) 
	for itag in tag:
		imatch = itag.strip()
		match_list.append(imatch)
f.close()

for file in get_files("./", '.h' ):  # find *.h 
    f = open(file, "r")
    mlines = f.readlines()
    for mline in mlines:
      ctend_list.append(mline)
    i+= 1
    print ("*.h file done", i)
    f.close()

for file in get_files("./", '.c' ):  #find *.c
    f = open(file, "r")
    mlines = f.readlines()
    for mline in mlines:
      ctend_list.append(mline)
    i+= 1
    print ("*.c file done", i)
    f.close()

fp = open("./no_used_macro.txt","w")
for mx in match_list:
  z = 0
  for cx in ctend_list:
    if mx in cx:
      z+= 1;
    else:
      continue
  if z == 1:
    fp.write(mx + "\n")
    print ("no used", mx)
    #os.system("sed -i '/%s/d' *.h" % (mx)) # do delt, better to confirm before delt
fp.close();

os.system('rm 1')





