#include "Arduino.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

bool get_BIOS_version(char *version);
bool get_mem_size(unsigned long *size);
void help(void);

int main()
{
	char bios_version[16];
	unsigned long mem_size;

	printf("86INFO v1.1, author: PING-HUI WEI, E-mail: greycookie@dmp.com.tw, 2016/03/18\n");

	printf("CPU:\t\t\t ");
	switch(vx86_CpuID())
	{
		case CPU_VORTEX86SX:
			printf("Vortex86SX\n");
			break;
		case CPU_VORTEX86DX_UNKNOWN:
			printf("Vortex86DX_unknown\n");
			break;
		case CPU_VORTEX86DX_A:
			printf("Vortex86DX_A\n");
			break;
		case CPU_VORTEX86DX_C:
			printf("Vortex86DX_C\n");
			break;
		case CPU_VORTEX86DX_D:
			printf("Vortex86DX_D\n");
			break;
		case CPU_VORTEX86MX:
			printf("Vortex86MX\n");
			break;
		case CPU_VORTEX86MX_PLUS:
			printf("Vortex86MX_plus\n");
			break;
		case CPU_VORTEX86DX2:
			printf("Vortex86DX2\n");
			break;
		case CPU_VORTEX86DX3:
			printf("Vortex86DX3\n");
			break;
		case CPU_VORTEX86EX:
			printf("Vortex86EX\n");
			break;
		case CPU_UNSUPPORTED:
		default:
			printf("unsupported\n");
	}

	printf("CPU Frequency:\t\t %ldMHz\n", vx86_CpuCLK());
	printf("CPU Temperature:\t %f °C\n", cpuTemperature());

	printf("Memory Size:\t\t");
	if(get_mem_size(&mem_size) == false)
		printf("get memory size error!\n");
	else
		printf(" %luMB\n", mem_size);

	printf("Memory Frequency:\t %luMHz\n", vx86_DramCLK());

	printf("BIOS Version:\t\t");
	if(get_BIOS_version(bios_version) == false)
		printf("get bios version error!\n");
	else
		printf(" %s\n", bios_version);

	return EXIT_SUCCESS;
} 

bool get_BIOS_version(char *version)
{
	int i;
	FILE *fp;

	fp = fopen("A:\\_BVER.V86", "r");
	if(fp == NULL)
		return false;

	fread(version, sizeof(char), 16, fp);
	fclose(fp);

	version[5] = '\0';
	if(strcmp("Guava", version) != 0)
		return false;
	version[5] = ' ';

	return true;
}

bool get_mem_size(unsigned long *size)
{
	unsigned char memory_bank_register;

	memory_bank_register = nb_Read8(0x6D);
	memory_bank_register &= 0x0f;

	*size = pow(2, memory_bank_register+1);
	return true;
}