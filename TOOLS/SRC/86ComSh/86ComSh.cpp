#include "Arduino.h"
#include <stdlib.h>
#include <stdio.h>

#define CROSSBAR_REG_NUM 44
#define SB_CROSSBASE  (0x64)
#define CROSSBARBASE  (0x0A00)

int main(int argc, char* argv[])
{
	int crossbarBase, gpioBase;

	if(argc != 2)
	{
		printf("Parameters input error!\n");
		printf("Example: 86ComSh 1\n");
		return EXIT_FAILURE;
	}

	if(io_Init() == false)
		return false;

	//set corssbar Base Address
	crossbarBase = sb_Read16(SB_CROSSBASE) & 0xfffe;
	if(crossbarBase == 0 || crossbarBase == 0xfffe)
		sb_Write16(SB_CROSSBASE, CROSSBARBASE | 0x01);

	switch(argv[1][0])
	{
		case '1': 
			io_outpw(CROSSBARBASE + 0x9a, 0x0808);
			break;

		case '2': 
			io_outpw(CROSSBARBASE + 0x9e, 0x0808);
			break;

		case '3': 
			io_outpw(CROSSBARBASE + 0x9c, 0x0808);
			break;

		case '4': 
			break;

		default :
			printf("Assign comport error!\n");
			break;
	}

	io_Close();

	printf("================================================================================\n");
	printf("                      ___   __   ____        _\n");
	printf("                     ( _ ) / /_ |  _ \\ _   _(_)_ __   ___\n");
	printf("                     / _ \\| '_ \\| | | | | | | | '_ \\ / _ \\\n");
	printf("                    | (_) | (_) | |_| | |_| | | | | | (_) |\n");
	printf("                     \\___/ \\___/|____/ \\____|_|_| |_|\\___/\n");
	printf("\n 86ComSh v1.0, author: PING-HUI WEI, E-mail: greycookie@dmp.com.tw, 2015/09/17\n");
	printf("================================================================================\n");

	return EXIT_SUCCESS;
}

