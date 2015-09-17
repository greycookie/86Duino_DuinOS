
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE := -I.. -I..\..\..\core
OBJS := WSS.o

.PHONY : everything clean all

everything: $(OBJS)

clean:
	rm $(OBJS)

all: clean everything

WSS.o: WSS.cpp WSS.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

