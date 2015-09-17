
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core
OBJS := Tlc5940.o
TARGETS := libTlc5940.a

.PHONY : everything clean all

everything: $(TARGETS)

clean:
	rm $(OBJS) $(TARGETS)

all: clean everything

$(TARGETS): $(OBJS)
	$(AR) rcs $(TARGETS) $?

Tlc5940.o: Tlc5940.cpp Tlc5940.h tlc_config.h pinouts\p86duino.h pinouts\pinfuc.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)