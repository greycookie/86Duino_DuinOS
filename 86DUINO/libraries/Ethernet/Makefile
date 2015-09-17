
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core -Iutility -I.\\
UTILITY_OBJS := utility\SwsSock.o
OBJS := Dhcp.o Dns.o Ethernet.o EthernetClient.o EthernetServer.o EthernetUdp.o
TARGETS := libEthernet.a

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

Dhcp.o: Dhcp.cpp utility\SwsSock.h Dhcp.h EthernetUdp.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

Dns.o: Dns.cpp utility\SwsSock.h EthernetUdp.h Dns.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

Ethernet.o: Ethernet.cpp utility\SwsSock.h Ethernet.h EthernetClient.h EthernetServer.h Dhcp.h EthernetUdp.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

EthernetClient.o: EthernetClient.cpp utility\SwsSock.h Ethernet.h EthernetClient.h EthernetServer.h Dhcp.h EthernetUdp.h Dns.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

EthernetServer.o: EthernetServer.cpp utility\SwsSock.h EthernetClient.h EthernetServer.h Ethernet.h Dhcp.h EthernetUdp.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

EthernetUdp.o: EthernetUdp.cpp Ethernet.h EthernetClient.h EthernetServer.h Dhcp.h EthernetUdp.h Dns.h utility\SwsSock.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)