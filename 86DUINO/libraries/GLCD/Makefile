
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core -I.\\
OBJS := gText.o glcd.o glcd_Device.o
TARGETS := libGLCD.a

.PHONY : everything clean all

everything: $(TARGETS)

clean:
	rm $(OBJS) $(TARGETS)

all: clean everything

$(TARGETS): $(OBJS)
	$(AR) rcs $(TARGETS) $?

gText.o: gText.cpp include\gText.h include\Streaming.h include\glcd_Device.h glcd_Config.h config\ks0108-192x64_Panel.h \
 config\ks0108_Arduino.h device\ks0108_Device.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

glcd.o: glcd.cpp glcd.h include\gText.h include\Streaming.h include\glcd_Device.h glcd_Config.h config\ks0108-192x64_Panel.h \
 config\ks0108_Arduino.h device\ks0108_Device.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

glcd_Device.o: glcd_Device.cpp include\glcd_Device.h glcd_Config.h include\glcd_io.h include\arduino_io.h include\avrio.h \
 include\glcd_errno.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)