
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE := -I.. -I..\..\..\core -I..\..\SPI
OBJS := Adafruit_GFX.o Adafruit_ST7735.o glcdfont.o

.PHONY : everything clean all

everything: $(OBJS)

clean:
	rm $(OBJS)

all: clean everything
	
Adafruit_GFX.o: Adafruit_GFX.cpp Adafruit_GFX.h glcdfont.c
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

Adafruit_ST7735.o: Adafruit_ST7735.cpp Adafruit_ST7735.h Adafruit_GFX.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

glcdfont.o: glcdfont.c
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

