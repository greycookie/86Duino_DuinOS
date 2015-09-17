
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core -I.\\
OBJS := VirtualWire.o
TARGETS := libVirtualWire.a

.PHONY : everything clean all

everything: $(TARGETS)

clean:
	rm $(OBJS) $(TARGETS)

all: clean everything

$(TARGETS): $(OBJS)
	$(AR) rcs $(TARGETS) $?

VirtualWire.o: VirtualWire.cpp VirtualWire.h VirtualWire_Config.h VWutil\crc16.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)