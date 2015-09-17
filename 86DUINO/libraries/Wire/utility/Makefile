
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE := -I.. -I..\..\..\core
OBJS := twi.o

.PHONY : everything clean all

everything: $(OBJS)

clean:
	rm $(OBJS)

all: clean everything

twi.o: twi.cpp twi.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

