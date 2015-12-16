#include "Arduino.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLOCK_OUT_REG 0x44

void help(void);
char lowercase_to_uppercase(char lowercase);

int main(int argc, char **argv)
{
	char option_choose;

	if(argc != 2 || strlen(argv[1]) > 1)
		goto help;

	option_choose = argv[1][0];
	option_choose = lowercase_to_uppercase(option_choose);
	switch(option_choose)
	{
		case 'A':
			io_outpb(CROSSBARBASE + 0x9A, 0x01);
			io_outpb(CROSSBARBASE + 0x9B, 0x01);
			printf("Switch TX1\\RX1 to GPIO.\n");
			break;
		case 'B':
			io_outpb(CROSSBARBASE + 0x9A, 0x08);
			io_outpb(CROSSBARBASE + 0x9B, 0x08);			
			printf("Switch TX1\\RX1 to UART.\n");
			break;
		case 'C':
			io_outpb(CROSSBARBASE + 0x9E, 0x01);
			io_outpb(CROSSBARBASE + 0x9F, 0x01);
			printf("Switch TX2\\RX2 to GPIO.\n");
			break;
		case 'D':
			io_outpb(CROSSBARBASE + 0x9E, 0x08);
			io_outpb(CROSSBARBASE + 0x9F, 0x08);		
			printf("Switch TX2\\RX2 to UART.\n");
			break;
		case 'E':
			io_outpb(CROSSBARBASE + 0x9C, 0x01);
			io_outpb(CROSSBARBASE + 0x9D, 0x01);
			printf("Switch TX3\\RX3 to GPIO.\n");		
			break;
		case 'F':
			io_outpb(CROSSBARBASE + 0x9C, 0x08);
			io_outpb(CROSSBARBASE + 0x9D, 0x08);
			printf("Switch TX3\\RX3 to UART.\n");		
			break;
		case 'G':
			io_outpb(CROSSBARBASE + 0x20, 0x1B);
			printf("Switch SPICS to WATCHDOG OUT.\n");
			break;
		case 'H':
			sb_Write8(CLOCK_OUT_REG, 0x01);
			io_outpb(CROSSBARBASE + 0x20, 0x24);
			printf("Switch SPICS to CLOCK OUT - 14.318MHz.\n");
			break;
		case 'I':
			sb_Write8(CLOCK_OUT_REG, 0x02);
			io_outpb(CROSSBARBASE + 0x20, 0x24);
			printf("Switch SPICS to CLOCK OUT - 24MHz.\n");
			break;
		case 'J':
			sb_Write8(CLOCK_OUT_REG, 0x03);
			io_outpb(CROSSBARBASE + 0x20, 0x24);
			printf("Switch SPICS to CLOCK OUT - 25MHz.\n");
			break;
		case 'K':
			sb_Write8(CLOCK_OUT_REG, 0x04);
			io_outpb(CROSSBARBASE + 0x20, 0x24);
			printf("Switch SPICS to CLOCK OUT - 48MHz.\n");
			break;
		case 'L':
			sb_Write8(CLOCK_OUT_REG, 0x05);
			io_outpb(CROSSBARBASE + 0x20, 0x24);
			printf("Switch SPICS to CLOCK OUT - 50MHz.\n");
			break;
		case 'M':
			sb_Write8(CLOCK_OUT_REG, 0x06);
			io_outpb(CROSSBARBASE + 0x20, 0x24);
			printf("Switch SPICS to CLOCK OUT - ISA CLOCK.\n");
			break;
		case 'N':
			io_outpb(CROSSBARBASE + 0x20, 0x1A);
			printf("Switch SPICS to SPEAKER.\n");	
			break;
		case 'O':
			io_outpb(CROSSBARBASE + 0x20, 0x09);	
			printf("Switch SPICS to SPICS.\n");	
			break;
		case 'P':
			io_outpb(CROSSBARBASE + 0x21, 0x1C);
			printf("Switch SPICLK to TXEN1.\n");
			break;
		case 'Q':
			io_outpb(CROSSBARBASE + 0x21, 0x0A);
			printf("Switch SPICLK to SPICLK.\n");
			break;
		case 'R':
			io_outpb(CROSSBARBASE + 0x22, 0x1D);
			printf("Switch SPIDI to TXEN2.\n");
			break;
		case 'S':
			io_outpb(CROSSBARBASE + 0x22, 0x0C);
			printf("Switch SPIDI to SPIDI.\n");
			break;
		case 'T':
			io_outpb(CROSSBARBASE + 0x23, 0x1E);
			printf("Switch SPIDO to TXEN3.\n");
			break;
		case 'U':
			io_outpb(CROSSBARBASE + 0x23, 0x0B);
			printf("Switch SPIDO to SPIDO.\n");
			break;	
		default:
			goto help;
	}

	return 0;

	help:
		help();
		return 0;
} 

void help(void)
{
	printf("86CRSBAR v1.0, author: PING-HUI WEI, E-mail: greycookie@dmp.com.tw, 2015/12/16\n");
	printf("86CRSBAR help information.\n");
	printf("Usage: 86CRSBAR [mode]\n");
	printf("Example: 86CRSBAR G\n\n");

	printf("mode:\t\tcan type \"A\" ~ \"U\", the following list describes that mode meaning.\n\n");
	
	printf("86CRSBAR mode:\n");
	printf("[A] TX1\\RX1 -> GPIO\n");
	printf("[B] TX1\\RX1 -> TX1\\RX1\n");
	printf("[C] TX2\\RX2 -> GPIO\n");
	printf("[D] TX2\\RX2 -> TX2\\RX2\n");
	printf("[E] TX3\\RX3 -> GPIO\n");
	printf("[F] TX3\\RX3 -> TX3\\RX3\n");
	printf("[G] SPICS -> WATCHDOG OUT\n");
	printf("[H] SPICS -> CLOCK OUT - 14.318MHz\n");
	printf("[I] SPICS -> CLOCK OUT - 24MHz\n");
	printf("[J] SPICS -> CLOCK OUT - 25MHz\n");
	printf("[K] SPICS -> CLOCK OUT - 48MHz\n");
	printf("[L] SPICS -> CLOCK OUT - 50MHz\n");
	printf("[M] SPICS -> CLOCK OUT - ISA Clock\n");
	printf("[N] SPICS -> SPEAKER\n");
	printf("[O] SPICS -> SPICS\n");
	printf("[P] SPICLK -> TXEN1\n");
	printf("[Q] SPICLK -> SPICLK\n");
	printf("[R] SPIDI -> TXEN2\n");
	printf("[S] SPIDI -> SPIDI\n");
	printf("[T] SPIDO -> TXEN3\n");
	printf("[U] SPIDO -> SPIDO\n");
}

char lowercase_to_uppercase(char lowercase)
{
	if(lowercase >= 'A' && lowercase <= 'Z')
		return lowercase;

	if(lowercase < 'a' || lowercase > 'z')
		return -1;

	return lowercase - 32;
}