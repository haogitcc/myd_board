CFLAGS=-g -Wall -static
#CC=gcc
#AR=ar

# make for im287
#CC=/home/shen/imx28/toolschain/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/bin/arm-fsl-linux-gnueabi-gcc
#AR=/home/shen/imx28/toolschain/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/bin/arm-fsl-linux-gnueabi-ar

# make for y2
CC=/home/shen/arm-toolschain/gcc-linaro-4.9-2014.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
AR=/home/shen/arm-toolschain/gcc-linaro-4.9-2014.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-ar

RANLIB=ranlib
LIBS=-L./ -lcgic

all: libcgic.a cgictest.cgi capture setIp.cgi getTagCount.cgi setWifi.cgi

install: libcgic.a
	cp libcgic.a /usr/local/lib
	cp cgic.h /usr/local/include
	@echo libcgic.a is in /usr/local/lib. cgic.h is in /usr/local/include.

libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a

#mingw32 and cygwin users: replace .cgi with .exe

cgictest.cgi: cgictest.o libcgic.a app_sys_setting.o ind_cfg.o ind_string.o mid_mutex.o 
	$(CC) -o cgictest.cgi cgictest.o app_sys_setting.o ind_cfg.o ind_string.o mid_mutex.o  ${LIBS} -lpthread

#cgictest.cgi: cgictest.o libcgic.a
#	$(CC) cgictest.o -o cgictest.cgi ${LIBS} -lpthread

getTagCount.cgi: getTagCount.o libcgic.a
	$(CC) getTagCount.o -o getTagCount.cgi ${LIBS}

setIp.cgi: setIp.o libcgic.a app_sys_setting.o ind_cfg.o ind_string.o mid_mutex.o 
	$(CC) -o setIp.cgi setIp.o app_sys_setting.o ind_cfg.o ind_string.o mid_mutex.o  ${LIBS} -lpthread

setWifi.cgi: setWifi.o libcgic.a app_sys_setting.o ind_cfg.o ind_string.o mid_mutex.o 
	$(CC) -o setWifi.cgi setWifi.o app_sys_setting.o ind_cfg.o ind_string.o mid_mutex.o  ${LIBS} -lpthread


capture: capture.o libcgic.a
	$(CC) capture.o -o capture ${LIBS}

clean:
	rm -f *.o *.a cgictest.cgi capture cgicunittest

test:
	gcc -D UNIT_TEST=1 cgic.c -o cgicunittest
	./cgicunittest
