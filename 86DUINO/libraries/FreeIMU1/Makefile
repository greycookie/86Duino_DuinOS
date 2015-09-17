
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core -Iutility -I.\\ -I..\Wire
UTILITY_OBJS := utility\LSM330.o
OBJS := FreeIMU1.o
TARGETS := libFreeIMU1.a

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

FreeIMU1.o: FreeIMU1.cpp FreeIMU1.h utility\LSM330.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)