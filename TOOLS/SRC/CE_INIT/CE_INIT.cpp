#include "Arduino.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void enable_audio_in_pci(void);
void disable_audio_in_crsbar(void);

int main(int argc, char *argv[])
{
	int crossbarBase;

	if(argc != 2)
	{
		printf("Error\n");
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
		sb_Write16(SB_CROSSBASE, CROSSBARBASE | 0x01);

	io_outpb(CROSSBARBASE + 0x28, 0xD0);
	io_outpb(CROSSBARBASE + 0x85, 0x02);
	io_outpdw(CROSSBARBASE + 0x98, 0x08080808);
	io_outpdw(CROSSBARBASE + 0x9C, 0x08080808);

	if(strcmp(argv[1], "E") == 0 || strcmp(argv[1], "e") == 0)
	{
		enable_audio_in_pci();
	}
	else if(strcmp(argv[1], "D") == 0 || strcmp(argv[1], "d") == 0)
	{
		disable_audio_in_crsbar();
	}
	else
		printf("Error!\n");

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
	io_outpb(CROSSBARBASE + 0x2C, 0xff);
}