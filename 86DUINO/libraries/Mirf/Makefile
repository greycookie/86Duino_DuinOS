
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core -I..\SPI
OBJS := Mirf.o MirfHardwareSpiDriver.o MirfSpiDriver.o
TARGETS := libMirf.a

.PHONY : everything clean all

everything: $(TARGETS)

clean:
	rm $(OBJS) $(TARGETS)

all: clean everything

$(TARGETS): $(OBJS) $(UTILITY_LIB)
	$(AR) rcs $(TARGETS) $?

Mirf.o: Mirf.cpp Mirf.h nRF24L01.h MirfSpiDriver.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

MirfHardwareSpiDriver.o: MirfHardwareSpiDriver.cpp MirfHardwareSpiDriver.h MirfSpiDriver.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

MirfSpiDriver.o: MirfSpiDriver.cpp MirfSpiDriver.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)