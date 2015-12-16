#include "Arduino.h"
#include <stdio.h>
#include <stdlib.h>

#define SECTORSIZE 4096

int _sector_offset = 8L * 1024 * 1024 - (256L * 1024) + (63 * SECTORSIZE);

char clock_array[5][6] = {{0x30, 0x03, 0x0F, 0x02, 0x8F, 0x07},	
                          {0x48, 0x03, 0x23, 0x02, 0x7F, 0x07},	
                          {0x90, 0x53, 0x23, 0x02, 0x9F, 0x07},	
                          {0xA0, 0x53, 0x23, 0x1A, 0x4F, 0x08},	
                          {0x50, 0x42, 0x23, 0x1A, 0xBF, 0x07}};
/*****************  Modify SPI Flash to Set CPU Clock *************************/
void write_spi_byte(unsigned short iobase, unsigned char n) {
	io_outpb(iobase, n);
	while((io_inpb(iobase + 3) & 0x10) == 0);
}

unsigned char read_spi_byte(unsigned short iobase) {
	io_outpb(iobase + 1, 0); //triggle SPI read
	while((io_inpb(iobase + 3) & 0x20) == 0);
	return io_inpb(iobase + 1); // read SPI data
}

void enable_cs(unsigned short iobase) {
	io_outpb(iobase + 4, 0);
}

void disable_cs(unsigned short iobase) {
	io_outpb(iobase + 4, 1);
}

void write_spi_24bit_addr(unsigned short iobase, unsigned long addr) {
	write_spi_byte(iobase, (unsigned char)((addr >> 16) & 0xff));
	write_spi_byte(iobase, (unsigned char)((addr >> 8) & 0xff));
	write_spi_byte(iobase, (unsigned char)((addr) & 0xff));
}

static unsigned char reg_sb_c4     = 0;
static unsigned long reg_nb_40     = 0L;
static unsigned char spi_ctrl_reg  = 0;
static unsigned long spi_base      = 0L;
static unsigned long flash_size    = 0L;
static unsigned long EEPROM_offset = 0L;
void set_flash_writable(void) {
	io_DisableINT();
	
	reg_sb_c4 = sb_Read8(0xc4);
	reg_nb_40 = nb_Read(0x40);
	spi_base = reg_nb_40 & 0x00fffeL;
	spi_ctrl_reg = io_inpb(spi_base + 2);
	
	sb_Write8(0xc4, reg_sb_c4 | 1);
	nb_Write(0x40, reg_nb_40 | 1);
	
	io_outpdw(spi_base + 2, ((spi_ctrl_reg | 0x20) & 0xf0) | 0x0c);
}

static void set_flash_unwritable(void) {
	io_outpdw(spi_base + 2, spi_ctrl_reg);
	nb_Write(0x40, reg_nb_40);
	sb_Write8(0xc4, reg_sb_c4);
	
	io_EnableINT();
}

void writee(unsigned long int in_addr, unsigned char in_value) {
	unsigned char s;
  	int wip_cnt = 0;
  	
	set_flash_writable();
	
	//mxic write enable
	enable_cs(spi_base);
	write_spi_byte(spi_base, 6);
	disable_cs(spi_base);
	
	//write data
	enable_cs(spi_base);
	write_spi_byte(spi_base, 0x02); //PAGE PROGRAM
	write_spi_24bit_addr(spi_base,_sector_offset + in_addr-0x3f000);//address
	write_spi_byte(spi_base, in_value);
	disable_cs(spi_base);
  
	//wait wip
	enable_cs(spi_base);
	write_spi_byte(spi_base, 5); //RDSR
	while(1)
	{
		s = read_spi_byte(spi_base);
		if((s&1) == 0) //WIP == 0
		{
			wip_cnt++;
			if(wip_cnt >= 3) break;
		}
		else
			wip_cnt = 0;
	}
	disable_cs(spi_base);
  
	//mxic write disable
	enable_cs(spi_base);
	write_spi_byte(spi_base, 4);
	disable_cs(spi_base);
	
	set_flash_unwritable();
}

unsigned char readd(unsigned long int in_addr) {
	unsigned char value;
	set_flash_writable();
	enable_cs(spi_base);
	write_spi_byte(spi_base, 0x03); //READ
	write_spi_24bit_addr(spi_base, _sector_offset + in_addr-0x3f000); //address
	value = read_spi_byte(spi_base);
	disable_cs(spi_base);
	set_flash_unwritable();
	return value;
}

void reset(void) {
	unsigned char s;
	int wip_cnt = 0;
	
	set_flash_writable();
	
	//mxic write enable
	enable_cs(spi_base);
	write_spi_byte(spi_base, 6);
	disable_cs(spi_base);
	
	//reset command and address
	enable_cs(spi_base);
	write_spi_byte(spi_base, 0x20);
	write_spi_24bit_addr(spi_base, _sector_offset);
	disable_cs(spi_base);
  
	//wait wip
	enable_cs(spi_base);
	write_spi_byte(spi_base, 5); //RDSR
	while(1)
	{
		s = read_spi_byte(spi_base);
		if((s&1) == 0) //WIP == 0
		{
			wip_cnt ++;
			if(wip_cnt >= 3) break;
		}
		else
			wip_cnt = 0;
	}
	disable_cs(spi_base);
  
	//mxic write disable
	enable_cs(spi_base);
	write_spi_byte(spi_base, 4);
	disable_cs(spi_base);
	
	set_flash_unwritable();
}

void adjust_clock(char *arr) {
    int i;
    char temp[4096];
    
	for(i=0; i<4096; i++) temp[i] = readd(0x3f000+i);
    
		
	temp[0xfb6] = arr[0];
	temp[0xfb7] = arr[1];
	temp[0xfbb] = arr[2];
	temp[0xfbc] = arr[3];
	temp[0xfbd] = arr[4];
	temp[0xfbf] = arr[5];
	
	reset();

	for(i=0; i<4096; i++) 
		writee(0x3f000+i, temp[i]);
}

void help()
{
	printf("86CLOCK v1.0, author: PING-HUI WEI, E-mail: greycookie@dmp.com.tw, 2015/12/16\n");
	printf("86CLOCK help information.\n");
	printf("Usage: 86CLOCK [CPU clock]\n");
	printf("Example: 86CLOCK 500\n\n");

	printf("CPU clock: can type 200, 300, 400, 444 or 500, setting CPU clock frequency\n");
}
						  
int main(int argc, char* argv[]) 
{
	int cpu_clock;
	int clock_no;
	
	if(argc == 1)
	{
		printf("Current CPU info:\n");
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
		printf("Memory Frequency:\t %luMHz\n", vx86_DramCLK());
		return 0;
	}
	
	if(argc != 2)
		goto help;

	cpu_clock = atoi(argv[1]);

	switch(cpu_clock)
	{
		case 200:
			clock_no = 0;
			break;
		case 300:
			clock_no = 1;
			break;
		case 400:
			clock_no = 2;
			break;
		case 444:
			clock_no = 3;
			break;
		case 500:
			clock_no = 4;
			break;
		default:
			goto help;
	}

	printf("Current CPU freq:\t%ldMHz\n", vx86_CpuCLK());
	printf("Modify to:\t\t%ldMHz\n\n", cpu_clock);

	adjust_clock(clock_array[clock_no]);

	printf("Setting complete.\n");
	printf("Now, reboot 86Duino.\n");

	return 0;

help:
	help();
	return 0;	
}
