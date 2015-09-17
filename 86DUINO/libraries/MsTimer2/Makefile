
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core -I.\\
OBJS := MsTimer2.o
TARGETS := libMsTimer2.a

.PHONY : everything clean all

everything: $(TARGETS)

clean:
	rm $(OBJS) $(TARGETS)

all: clean everything

$(TARGETS): $(OBJS)
	$(AR) rcs $(TARGETS) $?

MsTimer2.o: MsTimer2.cpp MsTimer2.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)