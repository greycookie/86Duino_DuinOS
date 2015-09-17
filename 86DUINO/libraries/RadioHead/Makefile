
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core -I.\\ -I..\SPI
OBJS := RHCRC.o RHDatagram.o RHGenericDriver.o RHGenericSPI.o RHHardwareSPI.o RHMesh.o RHNRFSPIDriver.o RHReliableDatagram.o RHRouter.o \
 RHSPIDriver.o RHSoftwareSPI.o RH_ASK.o RH_NRF24.o RH_NRF905.o RH_RF22.o RH_RF24.o RH_RF69.o RH_RF95.o RH_Serial.o RH_TCP.o
TARGETS := libRadioHead.a

.PHONY : everything clean all

everything: $(TARGETS)

clean:
	rm $(OBJS) $(TARGETS)

all: clean everything

$(TARGETS): $(OBJS)
	$(AR) rcs $(TARGETS) $?

RHCRC.o: RHCRC.cpp RHCRC.h RadioHead.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RHDatagram.o: RHDatagram.cpp RHDatagram.h RHGenericDriver.h RadioHead.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RHGenericDriver.o: RHGenericDriver.cpp RHGenericDriver.h RadioHead.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RHGenericSPI.o: RHGenericSPI.cpp RHGenericSPI.h RadioHead.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RHHardwareSPI.o: RHHardwareSPI.cpp RHHardwareSPI.h RHGenericSPI.h RadioHead.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RHMesh.o: RHMesh.cpp RHMesh.h RHRouter.h RHReliableDatagram.h RHDatagram.h RHGenericDriver.h RadioHead.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RHNRFSPIDriver.o: RHNRFSPIDriver.cpp RHNRFSPIDriver.h RHGenericDriver.h RadioHead.h RHHardwareSPI.h RHGenericSPI.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RHReliableDatagram.o: RHReliableDatagram.cpp RHReliableDatagram.h RHDatagram.h RHGenericDriver.h RadioHead.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RHRouter.o: RHRouter.cpp RHRouter.h RHReliableDatagram.h RHDatagram.h RHGenericDriver.h RadioHead.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RHSPIDriver.o: RHSPIDriver.cpp RHSPIDriver.h RHGenericDriver.h RadioHead.h RHHardwareSPI.h RHGenericSPI.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RHSoftwareSPI.o: RHSoftwareSPI.cpp RHSoftwareSPI.h RHGenericSPI.h RadioHead.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RH_ASK.o: RH_ASK.cpp RH_ASK.h RHGenericDriver.h RadioHead.h RHCRC.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RH_NRF24.o: RH_NRF24.cpp RH_NRF24.h RHGenericSPI.h RadioHead.h RHNRFSPIDriver.h RHGenericDriver.h RHHardwareSPI.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RH_NRF905.o: RH_NRF905.cpp RH_NRF905.h RHGenericSPI.h RadioHead.h RHNRFSPIDriver.h RHGenericDriver.h RHHardwareSPI.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RH_RF22.o: RH_RF22.cpp RH_RF22.h RHGenericSPI.h RadioHead.h RHSPIDriver.h RHGenericDriver.h RHHardwareSPI.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RH_RF24.o: RH_RF24.cpp RH_RF24.h RHGenericSPI.h RadioHead.h RHSPIDriver.h RHGenericDriver.h RHHardwareSPI.h radio_config_Si4460.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RH_RF69.o: RH_RF69.cpp RH_RF69.h RHGenericSPI.h RadioHead.h RHSPIDriver.h RHGenericDriver.h RHHardwareSPI.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RH_RF95.o: RH_RF95.cpp RH_RF95.h RHSPIDriver.h RHGenericDriver.h RadioHead.h RHHardwareSPI.h RHGenericSPI.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RH_Serial.o: RH_Serial.cpp RH_Serial.h RHGenericDriver.h RadioHead.h RHCRC.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

RH_TCP.o: RH_TCP.cpp RadioHead.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)