
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE := -I.. -I..\..\..\core
OBJS := spi_drv.o wSPI.o server_drv.o wifi_drv.o

.PHONY : everything clean all

everything: $(OBJS)

clean:
	rm $(OBJS)

all: clean everything

spi_drv.o: spi_drv.cpp spi_drv.h debug.h wifi_spi.h wl_definitions.h wSPI.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

wSPI.o: wSPI.cpp wSPI.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

server_drv.o: server_drv.cpp server_drv.h wifi_spi.h wl_definitions.h spi_drv.h wl_types.h debug.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

wifi_drv.o: wifi_drv.cpp spi_drv.h wifi_spi.h wl_definitions.h wifi_drv.h wl_types.h debug.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE)

