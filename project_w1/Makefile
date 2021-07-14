TARGET=vlsicomm.ko

#PROJECT=PROJECT_W1
#export PROJECT
export PROJ_NAME

all: firmware driver

firmware:
	make -C firmware

driver:
	make -C vmac
	cp vmac/$(TARGET) ./
   

clean :
	make clean -C firmware
	make clean -C vmac
	rm -rf $(TARGET)

.PHONY:firmware driver
