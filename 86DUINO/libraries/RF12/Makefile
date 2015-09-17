
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core -I.\\
OBJS := PRF12.o Ports.o PortsBMP085.o PortsLCD.o PortsSHT11.o RBlock.o REEPROM.o RF12.o RF12sio.o RSPIFlash.o
TARGETS := libRF12.a

.PHONY : everything clean all

everything: $(TARGETS)

clean:
	rm $(OBJS) $(TARGETS)

all: clean everything

$(TARGETS): $(OBJS) 
	$(AR) rcs $(TARGETS) $?

PRF12.o: PRF12.cpp JeeLib.h Ports.h RF12.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

Ports.o: Ports.cpp Ports.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

PortsBMP085.o: PortsBMP085.cpp Ports.h PortsBMP085.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

PortsLCD.o: PortsLCD.cpp PortsLCD.h Ports.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

PortsSHT11.o: PortsSHT11.cpp Ports.h PortsSHT11.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RBlock.o: RBlock.cpp RBlock.h RSPIFlash.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

REEPROM.o: REEPROM.cpp REEPROM.h RBlock.h RSPIFlash.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RF12.o: RF12.cpp RF12.h REEPROM.h RBlock.h RSPIFlash.h RF12.h Ports.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RF12sio.o: RF12sio.cpp JeeLib.h Ports.h RF12.h RF12sio.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RSPIFlash.o: RSPIFlash.cpp RSPIFlash.h 
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)