#include "Arduino.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void help(void);

int main(int argc, char **argv)
{
	unsigned char input_pin;
	unsigned int pin_num = A0;

	if(argc != 2)
		goto help;

	if(argv[1][0] != 'A' && argv[1][0] != 'a')
	{
		if(argv[1][0] < '0' || argv[1][0] > '6')
			goto help;
		else
			input_pin = argv[1][0];
	}
	else
	{
		if(argv[1][1] < '0' || argv[1][1] > '6')
			goto help;
		else
			input_pin = argv[1][1];
	}

	switch(input_pin)
	{
		case '1':
			pin_num += 1;
			break;
		case '2':
			pin_num += 2;
			break;
		case '3':
			pin_num += 3;
			break;
		case '4':
			pin_num += 4;
			break;
		case '5':
			pin_num += 5;
			break;
		case '6':
			pin_num += 6;
			break;
	}

	printf("analog pin %s reading value is %d\n", argv[1], analogRead(pin_num));

	return 0;

	help:
		help();
		return 0;
} 

void help(void)
{
	printf("86ADC v1.0, author: PING-HUI WEI, E-mail: greycookie@dmp.com.tw, 2015/12/16\n");
	printf("86ADC help information.\n");
	printf("Usage: 86ADC [pin]\n");
	printf("Example: 86ADC A2\n\n");
	printf("pin:\tcan type A0 ~ A6 or 0 ~ 6, the number of the analog input pin to read from\n");
}