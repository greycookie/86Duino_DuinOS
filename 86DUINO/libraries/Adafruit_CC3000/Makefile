
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core -Iutility -I.\\ -I..\SPI
UTILITY_OBJS := utility\cc3000_common.o utility\debug.o utility\evnt_handler.o utility\hci.o utility\netapp.o utility\nvmem.o utility\security.o utility\sntp.o utility\socket.o utility\wlan.o 
OBJS := Adafruit_CC3000.o Adafruit_CC3000_Server.o ccspi.o
TARGETS := libAdafruit_CC3000.a

.PHONY : everything clean all utility

everything: utility $(TARGETS)

clean:
	rm $(OBJS) $(TARGETS)
	make -C utility clean

all: clean everything

utility:
	make -C utility

$(TARGETS): $(OBJS) $(UTILITY_OBJS)
	$(AR) rcs $(TARGETS) $?

Adafruit_CC3000.o: Adafruit_CC3000.cpp Adafruit_CC3000.h utility\cc3000_common.h utility\data_types.h utility\wlan.h \
 utility\netapp.h ccspi.h Adafruit_CC3000_Server.h utility\evnt_handler.h utility\socket.h utility\hci.h utility\security.h \
 utility\nvmem.h utility\socket.h utility\debug.h utility\sntp.h \

	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

Adafruit_CC3000_Server.o: Adafruit_CC3000_Server.cpp Adafruit_CC3000_Server.h Adafruit_CC3000.h utility\cc3000_common.h \
	utility\data_types.h utility\wlan.h utility\netapp.h ccspi.h utility\socket.h

	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)	

ccspi.o: ccspi.cpp ccspi.h utility\wlan.h utility\cc3000_common.h utility\data_types.h utility\hci.h utility\netapp.h \
 utility\evnt_handler.h utility\socket.h utility\debug.h

	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)
