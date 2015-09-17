
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core -Iutility -I.\\
UTILITY_OBJS := utility\twi.o
OBJS := Wire.o
TARGETS := libWire.a

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

Wire.o: Wire.cpp Wire.h utility\twi.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)