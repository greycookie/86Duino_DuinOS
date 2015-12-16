#include "Arduino.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void help(void);
bool string_to_uppercase(char *string);
char lowercase_to_uppercase(char lowercase);

int main(int argc, char **argv)
{
	bool is_read;
	char logic_level;
	unsigned int pin_num;

	if(argc != 3 && argc != 4)
		goto help;

	string_to_uppercase(argv[1]);
	if(strcmp(argv[1], "R") == 0)
		is_read = true;
	else if(strcmp(argv[1], "W") == 0)
		is_read = false;
	else
		goto help;

	if(argc == 4)
	{
		string_to_uppercase(argv[3]);
		if(strcmp(argv[3], "HIGH") == 0)
			logic_level = HIGH;
		else if(strcmp(argv[3], "LOW") == 0)
			logic_level = LOW;
		else
			goto help;
	}

	pin_num = atoi(argv[2]);
	//decive board version
	if(pin_num > 44 || pin_num < 0)
	{
		printf("pin num error!\n");
		return 0;
	}

	if(is_read)
	{
		pinMode(pin_num, INPUT);
		printf("pin %d logic level is %s\n", pin_num, digitalRead(pin_num) ? "HIGH" : "LOW");
	}
	else
	{
		pinMode(pin_num, OUTPUT);
		digitalWrite(pin_num, logic_level);
		printf("pin %d output logic level %s\n", pin_num, argv[3]);
	}

	return 0;

	help:
		help();
		return 0;
} 

bool string_to_uppercase(char *string)
{
	for(;*string != '\0'; string++)
	{
		*string = lowercase_to_uppercase(*string);

		if(*string == -1)
			return false;
	}

	return true;
}

char lowercase_to_uppercase(char lowercase)
{
	if(lowercase >= 'A' && lowercase <= 'Z')
		return lowercase;

	if(lowercase < 'a' || lowercase > 'z')
		return -1;

	return lowercase - 32;
}

void help(void)
{
	printf("86IO v1.0, author: PING-HUI WEI, E-mail: greycookie@dmp.com.tw, 2015/12/16\n");
	printf("86IO help information.\n");
	printf("Usage: 86IO [mode] [pin] [logic level]\n");
	printf("Example: 86IO R 8\n");
	printf("         86IO W 6 HIGH\n\n");
	printf("mode:\t\tcan type \"R\" or \"W\", reads the value from a specified digital pin when type \"R\" , write a HIGH or a LOW value to a digital pin when type \"W\"\n");
	printf("pin:\t\tthe number of the digital pin\n");
	printf("logic level:\tcan type \"HIGH\" or \"LOW\", assign output logic level\n");
}