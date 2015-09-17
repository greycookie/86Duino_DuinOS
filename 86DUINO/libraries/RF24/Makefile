
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core -I..\SPI -I.\\
OBJS := RF24.o
TARGETS := libRF24.a

.PHONY : everything clean all
everything: $(TARGETS)

clean:
	rm $(OBJS) $(TARGETS)

all: clean everything


$(TARGETS): $(OBJS)
	$(AR) rcs $(TARGETS) $?

RF24.o: RF24.cpp RF24.h nRF24L01.h RF24_config.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)