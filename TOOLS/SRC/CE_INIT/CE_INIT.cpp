#include "Arduino.h"
#include "SPI.h"
#include "Ethernet.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SB_CROSSBASE 0x64
#define CRSBARBASE 0x0A00

void query_IP(void);
void enable_audio_in_pci(void);
void disable_audio_in_crsbar(void);

int main(int argc, char *argv[])
{
	int crossbarBase;

	if(argc != 2)
	{
		printf("Need board information\n");
		return 0;
	}

	if(strcmp(argv[1], "ONE_VGA") != 0 && strcmp(argv[1], "ONE") != 0 && strcmp(argv[1], "ZERO") != 0 && strcmp(argv[1], "EDUCAKE") != 0)
	{
		printf("Need board information\n");
		return 0;
	}

	if(io_Init() == false)
	{
		printf("io_Init error");
		return false;
	}

	//set corssbar Base Address
	crossbarBase = sb_Read16(SB_CROSSBASE) & 0xfffe;
	if(crossbarBase == 0 || crossbarBase == 0xfffe)
		sb_Write16(SB_CROSSBASE, CRSBARBASE | 0x01);

	io_outpdw(CRSBARBASE + 0x98, 0x08080808);
	io_outpdw(CRSBARBASE + 0x9C, 0x08080808);

	if(strcmp(argv[1], "ONE_VGA") == 0)
	{
		enable_audio_in_pci();
	}
	else if(strcmp(argv[1], "ONE") == 0 || strcmp(argv[1], "EDUCAKE") == 0)
	{
		enable_audio_in_pci();
		query_IP();
	}
	else
	{
		disable_audio_in_crsbar();
		query_IP();
	}

	io_Close();
	return 0;
}

void enable_audio_in_pci(void)
{
	//Enable Audio
	void *HDA_handle;

	HDA_handle = pci_Alloc(0x00, 0x0e, 0x00);
	pci_Out8(HDA_handle, 0x04, 0x06);
	pci_Free(HDA_handle);
}

void disable_audio_in_crsbar(void)
{
	io_outpb(CRSBARBASE + 0x2C, 0xff);
}

void query_IP(void)
{
	unsigned char mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };

	Serial.begin(115200);

	while(Serial.available() <= 0)
		;
	usb_FlushRxQueue(USBDEV);

	// start the Ethernet connection:
	if (Ethernet.begin((uint8_t*)mac) == 0)
	{
		Serial.println("Failed to configure Ethernet using DHCP");
	}
	else
	{
		// print your local IP address:
		Serial.print("My IP address: ");
		for (char thisByte = 0; thisByte < 4; thisByte++)
		{
			// print the value of each byte of the IP address:
			Serial.print(Ethernet.localIP()[thisByte], DEC);
			if(thisByte != 3)
				Serial.print("."); 
		}
		Serial.println();
	}
	delay(3);

	Serial.end();	
}