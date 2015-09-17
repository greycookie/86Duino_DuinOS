
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core
OBJS := Block.o EEPROM.o SPIFlash.o
TARGETS := libEEPROM.a

.PHONY : everything clean all

everything: $(TARGETS)

clean:
	rm $(OBJS) $(TARGETS)

all: clean everything

$(TARGETS): $(OBJS)
	$(AR) rcs $(TARGETS) $?

Block.o: Block.cpp Block.h SPIFlash.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

EEPROM.o: EEPROM.cpp EEPROM.h Block.h SPIFlash.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

SPIFlash.o: SPIFlash.cpp SPIFlash.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)