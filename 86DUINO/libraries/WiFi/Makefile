
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core -Iutility -I.\\
UTILITY_OBJS := utility\spi_drv.o utility\wSPI.o utility\server_drv.o utility\wifi_drv.o
OBJS := WiFiClient.o WiFi.o WiFiServer.o
TARGETS := libWiFi.a

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

WiFiClient.o: WiFiClient.cpp utility\wl_definitions.h utility\wl_types.h utility\socket.h utility\wl_definitions.h \
 WiFiClient.h WiFiServer.h utility\server_drv.h utility\wifi_spi.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

WiFi.o: WiFi.cpp utility\wifi_drv.h utility\wifi_spi.h utility\wl_definitions.h WiFi.h utility\wl_definitions.h utility\wl_types.h \
 WiFiClient.h WiFiServer.h utility\debug.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

WiFiServer.o: WiFiServer.cpp utility\server_drv.h utility\wifi_spi.h utility\wl_definitions.h utility\debug.h WiFi.h \
 utility\wl_definitions.h utility\wl_types.h WiFiClient.h WiFiServer.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)