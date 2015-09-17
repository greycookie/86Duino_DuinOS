
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core -I.\\
OBJS := GSM3CircularBuffer.o GSM3MobileAccessProvider.o GSM3MobileCellManagement.o GSM3MobileClientProvider.o \
 GSM3MobileClientService.o GSM3MobileDataNetworkProvider.o GSM3MobileMockupProvider.o GSM3MobileNetworkProvider.o \
 GSM3MobileNetworkRegistry.o GSM3MobileSMSProvider.o GSM3MobileServerProvider.o GSM3MobileServerService.o GSM3MobileVoiceProvider.o \
 GSM3SMSService.o GSM3ShieldV1.o GSM3ShieldV1AccessProvider.o GSM3ShieldV1BandManagement.o GSM3ShieldV1BaseProvider.o \
 GSM3ShieldV1CellManagement.o GSM3ShieldV1ClientProvider.o GSM3ShieldV1DataNetworkProvider.o GSM3ShieldV1DirectModemProvider.o \
 GSM3ShieldV1ModemCore.o GSM3ShieldV1ModemVerification.o GSM3ShieldV1MultiClientProvider.o GSM3ShieldV1MultiServerProvider.o \
 GSM3ShieldV1PinManagement.o GSM3ShieldV1SMSProvider.o GSM3ShieldV1ScanNetworks.o GSM3ShieldV1ServerProvider.o \
 GSM3ShieldV1VoiceProvider.o GSM3SoftSerial.o GSM3VoiceCallService.o

#OBJS := GSM3CircularBuffer.o
TARGETS := libGSM.a

.PHONY : everything clean all

everything: $(TARGETS)

clean:
	rm $(OBJS) $(TARGETS)

all: clean everything

$(TARGETS): $(OBJS)
	$(AR) rcs $(TARGETS) $?

GSM3CircularBuffer.o: GSM3CircularBuffer.cpp GSM3CircularBuffer.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3MobileAccessProvider.o: GSM3MobileAccessProvider.cpp GSM3MobileAccessProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3MobileCellManagement.o: GSM3MobileCellManagement.cpp GSM3MobileCellManagement.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3MobileClientProvider.o: GSM3MobileClientProvider.cpp GSM3MobileClientProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3MobileClientService.o: GSM3MobileClientService.cpp GSM3MobileClientService.h GSM3MobileNetworkProvider.h \
 GSM3MobileAccessProvider.h GSM3ShieldV1ClientProvider.h GSM3MobileClientProvider.h GSM3ShieldV1BaseProvider.h \
 GSM3SoftSerial.h GSM3CircularBuffer.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3MobileDataNetworkProvider.o: GSM3MobileDataNetworkProvider.cpp GSM3MobileDataNetworkProvider.h GSM3MobileAccessProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3MobileMockupProvider.o: GSM3MobileMockupProvider.cpp GSM3MobileNetworkProvider.h GSM3MobileAccessProvider.h \
 GSM3MobileMockupProvider.h GSM3MobileVoiceProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3MobileNetworkProvider.o: GSM3MobileNetworkProvider.cpp GSM3MobileNetworkProvider.h GSM3MobileAccessProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3MobileNetworkRegistry.o: GSM3MobileNetworkRegistry.cpp GSM3MobileNetworkRegistry.h GSM3MobileNetworkProvider.h \
 GSM3MobileAccessProvider.h 
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3MobileSMSProvider.o: GSM3MobileSMSProvider.cpp GSM3MobileSMSProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3MobileServerProvider.o: GSM3MobileServerProvider.cpp GSM3MobileServerProvider.h GSM3MobileAccessProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3MobileServerService.o: GSM3MobileServerService.cpp GSM3MobileServerService.h GSM3MobileNetworkProvider.h GSM3MobileAccessProvider.h \
 GSM3MobileClientService.h  GSM3MobileServerProvider.h GSM3MobileClientProvider.h GSM3ShieldV1ServerProvider.h \
 GSM3ShieldV1BaseProvider.h GSM3SoftSerial.h GSM3CircularBuffer.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3MobileVoiceProvider.o: GSM3MobileVoiceProvider.cpp GSM3MobileVoiceProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3SMSService.o: GSM3SMSService.cpp GSM3SMSService.h GSM3ShieldV1MultiServerProvider.h GSM3MobileServerProvider.h \
 GSM3MobileAccessProvider.h GSM3ShieldV1BaseProvider.h GSM3SoftSerial.h GSM3CircularBuffer.h GSM3MobileNetworkProvider.h \
 GSM3ShieldV1SMSProvider.h GSM3ShieldV1ModemCore.h GSM3MobileSMSProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1.o: GSM3ShieldV1.cpp GSM3ShieldV1.h GSM3MobileNetworkProvider.h GSM3MobileAccessProvider.h \
 GSM3ShieldV1ModemCore.h GSM3SoftSerial.h GSM3CircularBuffer.h GSM3ShieldV1BaseProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1AccessProvider.o: GSM3ShieldV1AccessProvider.cpp GSM3ShieldV1AccessProvider.h GSM3MobileAccessProvider.h \
 GSM3ShieldV1ModemCore.h GSM3SoftSerial.h GSM3CircularBuffer.h GSM3ShieldV1BaseProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1BandManagement.o: GSM3ShieldV1BandManagement.cpp GSM3ShieldV1BandManagement.h GSM3ShieldV1DirectModemProvider.h \
 GSM3SoftSerial.h GSM3CircularBuffer.h GSM3MobileNetworkProvider.h GSM3MobileAccessProvider.h GSM3ShieldV1BaseProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1BaseProvider.o: GSM3ShieldV1BaseProvider.cpp GSM3ShieldV1BaseProvider.h GSM3SoftSerial.h GSM3CircularBuffer.h \
 GSM3ShieldV1ModemCore.h GSM3MobileAccessProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1CellManagement.o: GSM3ShieldV1CellManagement.cpp GSM3ShieldV1CellManagement.h GSM3ShieldV1ModemCore.h GSM3SoftSerial.h \
 GSM3CircularBuffer.h GSM3ShieldV1BaseProvider.h GSM3MobileAccessProvider.h GSM3MobileCellManagement.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1ClientProvider.o: GSM3ShieldV1ClientProvider.cpp GSM3ShieldV1ClientProvider.h GSM3MobileClientProvider.h \
 GSM3ShieldV1BaseProvider.h GSM3SoftSerial.h GSM3CircularBuffer.h GSM3ShieldV1ModemCore.h GSM3MobileAccessProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1DataNetworkProvider.o: GSM3ShieldV1DataNetworkProvider.cpp GSM3ShieldV1DataNetworkProvider.h \
 GSM3MobileDataNetworkProvider.h GSM3MobileAccessProvider.h GSM3ShieldV1BaseProvider.h GSM3SoftSerial.h GSM3CircularBuffer.h \
 GSM3ShieldV1ModemCore.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1DirectModemProvider.o: GSM3ShieldV1DirectModemProvider.cpp GSM3ShieldV1DirectModemProvider.h GSM3SoftSerial.h \
 GSM3CircularBuffer.h GSM3MobileNetworkProvider.h GSM3MobileAccessProvider.h GSM3ShieldV1BaseProvider.h GSM3ShieldV1ModemCore.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1ModemCore.o: GSM3ShieldV1ModemCore.cpp GSM3ShieldV1ModemCore.h GSM3SoftSerial.h GSM3CircularBuffer.h \
 GSM3ShieldV1BaseProvider.h GSM3MobileAccessProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1ModemVerification.o: GSM3ShieldV1ModemVerification.cpp GSM3ShieldV1ModemVerification.h GSM3ShieldV1AccessProvider.h \
 GSM3MobileAccessProvider.h GSM3ShieldV1ModemCore.h GSM3SoftSerial.h GSM3CircularBuffer.h GSM3ShieldV1BaseProvider.h \
 GSM3ShieldV1DirectModemProvider.h GSM3MobileNetworkProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1MultiClientProvider.o: GSM3ShieldV1MultiClientProvider.cpp GSM3ShieldV1MultiClientProvider.h GSM3MobileClientProvider.h \
 GSM3ShieldV1BaseProvider.h GSM3SoftSerial.h GSM3CircularBuffer.h GSM3ShieldV1ModemCore.h GSM3MobileAccessProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1MultiServerProvider.o: GSM3ShieldV1MultiServerProvider.cpp GSM3ShieldV1MultiServerProvider.h GSM3MobileServerProvider.h \
 GSM3MobileAccessProvider.h GSM3ShieldV1BaseProvider.h GSM3SoftSerial.h GSM3CircularBuffer.h GSM3ShieldV1ModemCore.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1PinManagement.o: GSM3ShieldV1PinManagement.cpp GSM3ShieldV1PinManagement.h GSM3ShieldV1AccessProvider.h \
 GSM3MobileAccessProvider.h GSM3ShieldV1ModemCore.h GSM3SoftSerial.h GSM3CircularBuffer.h GSM3ShieldV1BaseProvider.h \
 GSM3ShieldV1DirectModemProvider.h GSM3MobileNetworkProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1SMSProvider.o: GSM3ShieldV1SMSProvider.cpp GSM3ShieldV1SMSProvider.h GSM3ShieldV1ModemCore.h GSM3SoftSerial.h \
 GSM3CircularBuffer.h GSM3ShieldV1BaseProvider.h GSM3MobileAccessProvider.h GSM3MobileSMSProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1ScanNetworks.o: GSM3ShieldV1ScanNetworks.cpp GSM3ShieldV1ScanNetworks.h GSM3ShieldV1AccessProvider.h \
 GSM3MobileAccessProvider.h GSM3ShieldV1ModemCore.h GSM3SoftSerial.h GSM3CircularBuffer.h GSM3ShieldV1BaseProvider.h \
 GSM3ShieldV1DirectModemProvider.h GSM3MobileNetworkProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1ServerProvider.o: GSM3ShieldV1ServerProvider.cpp GSM3ShieldV1ServerProvider.h GSM3MobileServerProvider.h \
 GSM3MobileAccessProvider.h GSM3ShieldV1BaseProvider.h GSM3SoftSerial.h GSM3CircularBuffer.h GSM3ShieldV1ModemCore.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3ShieldV1VoiceProvider.o: GSM3ShieldV1VoiceProvider.cpp GSM3ShieldV1VoiceProvider.h GSM3MobileVoiceProvider.h \
 GSM3ShieldV1ModemCore.h GSM3SoftSerial.h GSM3CircularBuffer.h GSM3ShieldV1BaseProvider.h GSM3MobileAccessProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3SoftSerial.o: GSM3SoftSerial.cpp GSM3SoftSerial.h GSM3CircularBuffer.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

GSM3VoiceCallService.o: GSM3VoiceCallService.cpp GSM3VoiceCallService.h GSM3MobileNetworkProvider.h GSM3MobileAccessProvider.h \
 GSM3MobileVoiceProvider.h GSM3ShieldV1VoiceProvider.h GSM3ShieldV1ModemCore.h GSM3SoftSerial.h GSM3CircularBuffer.h \
 GSM3ShieldV1BaseProvider.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)