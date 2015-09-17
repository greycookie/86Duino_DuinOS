
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core
OBJS := Time86.o DateStrings.o
TARGETS := libTime86.a

.PHONY : everything clean all

everything: $(TARGETS)

clean:
	rm $(OBJS) $(TARGETS)

all: clean everything

$(TARGETS): $(OBJS)
	$(AR) rcs $(TARGETS) $?

Time86.o: Time86.cpp Time86.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

DateStrings.o: DateStrings.cpp
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)