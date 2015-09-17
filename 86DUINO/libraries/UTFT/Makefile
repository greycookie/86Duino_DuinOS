
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core
OBJS := UTFT.o Fonts.o
TARGETS := libUTFT.a

.PHONY : everything clean all

everything: $(TARGETS)

clean:
	rm $(OBJS) $(TARGETS)

all: clean everything

$(TARGETS): $(OBJS)
	$(AR) rcs $(TARGETS) $?


UTFT.o: UTFT.cpp UTFT.h hardware\HW_One.h tft_drivers\hx8347a\initlcd.h \
 tft_drivers\ili9327\initlcd.h tft_drivers\ssd1289\initlcd.h \
 tft_drivers\ili9325c\initlcd.h tft_drivers\ili9325d\default\initlcd.h \
 tft_drivers\ili9325d\alt\initlcd.h tft_drivers\hx8340b\8\initlcd.h \
 tft_drivers\hx8340b\s\initlcd.h tft_drivers\st7735\initlcd.h \
 tft_drivers\pcf8833\initlcd.h tft_drivers\s1d19122\initlcd.h \
 tft_drivers\hx8352a\initlcd.h tft_drivers\ssd1963\480\initlcd.h \
 tft_drivers\ssd1963\800\initlcd.h tft_drivers\ssd1963\800alt\initlcd.h \
 tft_drivers\s6d1121\initlcd.h tft_drivers\ili9320\initlcd.h \
 tft_drivers\ili9481\initlcd.h tft_drivers\s6d0164\initlcd.h \
 tft_drivers\st7735s\initlcd.h tft_drivers\ili9341\s4p\initlcd.h \
 tft_drivers\ili9341\s5p\initlcd.h tft_drivers\r61581\initlcd.h \
 tft_drivers\ili9486\initlcd.h tft_drivers\cpld\initlcd.h \
 tft_drivers\hx8347a\setxy.h tft_drivers\hx8352a\setxy.h \
 tft_drivers\ili9327\setxy.h tft_drivers\ssd1289\setxy.h \
 tft_drivers\ili9325c\setxy.h tft_drivers\ili9325d\default\setxy.h \
 tft_drivers\ili9325d\alt\setxy.h tft_drivers\hx8340b\8\setxy.h \
 tft_drivers\hx8340b\s\setxy.h tft_drivers\st7735\setxy.h \
 tft_drivers\s1d19122\setxy.h tft_drivers\pcf8833\setxy.h \
 tft_drivers\ssd1963\480\setxy.h tft_drivers\ssd1963\800\setxy.h \
 tft_drivers\ssd1963\800alt\setxy.h tft_drivers\s6d1121\setxy.h \
 tft_drivers\ili9320\setxy.h tft_drivers\ili9481\setxy.h \
 tft_drivers\s6d0164\setxy.h tft_drivers\st7735s\setxy.h \
 tft_drivers\ili9341\s4p\setxy.h tft_drivers\ili9341\s5p\setxy.h \
 tft_drivers\r61581\setxy.h tft_drivers\ili9486\setxy.h \
 tft_drivers\cpld\setxy.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

Fonts.o: Fonts.c
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)