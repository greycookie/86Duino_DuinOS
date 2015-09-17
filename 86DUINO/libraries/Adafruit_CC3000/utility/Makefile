
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE := -I.. -I..\..\..\core -I..\..\SPI
OBJS := cc3000_common.o debug.o evnt_handler.o hci.o netapp.o nvmem.o security.o sntp.o socket.o wlan.o

.PHONY : everything clean all

everything: $(OBJS)

clean:
	rm $(OBJS)

all: clean everything

cc3000_common.o: cc3000_common.cpp cc3000_common.h data_types.h socket.h wlan.h evnt_handler.h hci.h debug.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

debug.o: debug.cpp debug.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

evnt_handler.o: evnt_handler.cpp cc3000_common.h data_types.h hci.h evnt_handler.h socket.h wlan.h netapp.h ..\ccspi.h debug.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

hci.o: hci.cpp cc3000_common.h data_types.h hci.h ..\ccspi.h wlan.h evnt_handler.h socket.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

netapp.o: netapp.cpp netapp.h data_types.h hci.h cc3000_common.h socket.h evnt_handler.h nvmem.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

nvmem.o: nvmem.cpp nvmem.h cc3000_common.h data_types.h hci.h socket.h evnt_handler.h debug.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

security.o: security.cpp security.h nvmem.h cc3000_common.h data_types.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

sntp.o: sntp.cpp sntp.h socket.h netapp.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

socket.o: socket.cpp hci.h cc3000_common.h data_types.h socket.h evnt_handler.h netapp.h debug.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

wlan.o: wlan.cpp wlan.h cc3000_common.h data_types.h hci.h ..\ccspi.h socket.h nvmem.h security.h evnt_handler.h debug.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

