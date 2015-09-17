
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core -I..\SPI
OBJS := NTP.o Spider_L3.o WebClient.o WebServer.o cc3000_common.o evnt_handler.o hci.o netapp.o nvmem.o security.o socket.o \
 spider_spi.o wlan.o
TARGETS := libSpiderL3S.a

.PHONY : everything clean all

everything: $(TARGETS)

clean:
	rm $(OBJS) $(TARGETS)

all: clean everything

$(TARGETS): $(OBJS)
	$(AR) rcs $(TARGETS) $?

NTP.o: NTP.cpp NTP.h Spider_L3.h cc3000_common.h data_types.h evnt_handler.h hci.h socket.h host_driver_version.h netapp.h nvmem.h \
 security.h wlan.h spider_spi.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

Spider_L3.o: Spider_L3.cpp Spider_L3.h cc3000_common.h data_types.h evnt_handler.h hci.h socket.h host_driver_version.h netapp.h nvmem.h \
 security.h wlan.h spider_spi.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

WebClient.o: WebClient.cpp WebClient.h Spider_L3.h cc3000_common.h data_types.h evnt_handler.h hci.h socket.h host_driver_version.h \
 netapp.h nvmem.h security.h wlan.h spider_spi.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

WebServer.o: WebServer.cpp WebServer.h Spider_L3.h cc3000_common.h data_types.h evnt_handler.h hci.h socket.h host_driver_version.h \
 netapp.h nvmem.h security.h wlan.h spider_spi.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

cc3000_common.o: cc3000_common.cpp cc3000_common.h data_types.h socket.h wlan.h evnt_handler.h hci.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

evnt_handler.o: evnt_handler.cpp cc3000_common.h data_types.h hci.h evnt_handler.h socket.h wlan.h netapp.h spider_spi.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

hci.o: hci.cpp cc3000_common.h data_types.h hci.h spider_spi.h evnt_handler.h socket.h wlan.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

netapp.o: netapp.cpp netapp.h data_types.h hci.h cc3000_common.h socket.h evnt_handler.h nvmem.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

nvmem.o: nvmem.cpp nvmem.h cc3000_common.h data_types.h hci.h socket.h evnt_handler.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

security.o: security.cpp security.h nvmem.h cc3000_common.h data_types.h 
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

socket.o: socket.cpp hci.h cc3000_common.h data_types.h socket.h evnt_handler.h netapp.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

spider_spi.o: spider_spi.cpp spider_spi.h hci.h cc3000_common.h data_types.h evnt_handler.h socket.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

wlan.o: wlan.cpp wlan.h cc3000_common.h data_types.h hci.h spider_spi.h socket.h nvmem.h security.h evnt_handler.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)