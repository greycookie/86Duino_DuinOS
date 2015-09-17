
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core -I.\\
OBJS := SoftwareSerial.o
TARGETS := libSoftwareSerial.a

.PHONY : everything clean all

everything: $(TARGETS)

clean:
	rm $(OBJS) $(TARGETS)

all: clean everything

$(TARGETS): $(OBJS)
	$(AR) rcs $(TARGETS) $?

SoftwareSerial.o: SoftwareSerial.cpp SoftwareSerial.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)