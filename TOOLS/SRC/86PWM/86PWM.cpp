#include "Arduino.h"
#include "TimerOne.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENABLE_PWM (0)
#define DISABLE_PWM (1)

#define MCM_MC (0)
#define MCM_MD (1)

void help(void);
void string_to_uppercase(char *string);
char lowercase_to_uppercase(char lowercase);

int main(int argc, char **argv)
{
	unsigned int mc;
	unsigned int md;
	unsigned int action;
	int duty;
	int period = 1000;
	unsigned int pin_num;

	if(argc < 3 || argc > 5)
		goto help;

	if((pin_num = atoi(argv[1])) != 0)
	{
		mc = arduino_to_mc_md[MCM_MC][pin_num];
	    md = arduino_to_mc_md[MCM_MD][pin_num];

	    if(mc == NOPWM || md == NOPWM)
			goto help;
		else
		{
			string_to_uppercase(argv[2]);
			if(strcmp(argv[2], "-D") == 0)
			{
				action = DISABLE_PWM;
			}
			else if(strcmp(argv[2], "-E") == 0)
			{
				if((duty = atoi(argv[3])) == 0)
					goto help;

				if(argc == 5)
					if((period = atoi(argv[4])) == 0)
						goto help;

				if(duty < 0 || duty > 1024)
					goto help;

				if(period < 1 || period > 21474836)
					goto help;

				action = ENABLE_PWM;
			}
			else
				goto help;
		}
	}
	else
		goto help;

	Timer1.initialize(1000);
	switch(action)
	{
		case ENABLE_PWM:
			Timer1.pwm(pin_num, duty, period);
			printf("Enable PWM on pin %d\n", pin_num);
			printf("Duty: %d%%, Period: %d microseconds\n", (int)(duty * 100.0 / 1024.0), period);
			break;
		case DISABLE_PWM:
			Timer1.pwm(pin_num, 1);
			Timer1.disablePwm(pin_num);
			printf("Disable PWM on pin %d\n", pin_num);
			break;
	}
	return 0;

	help:
		help();
		return 0;
} 

void string_to_uppercase(char *string)
{
	for(;*string != '\0'; string++)
		*string = lowercase_to_uppercase(*string);
}

char lowercase_to_uppercase(char lowercase)
{
	if(lowercase >= 'a' && lowercase <= 'z')
		return lowercase - 32;
	else
		return lowercase;
}

void help(void)
{
	printf("86PWM v1.0, author: PING-HUI WEI, E-mail: greycookie@dmp.com.tw, 2016/01/30\n");
	printf("86PWM help information.\n");
	printf("Usage: 86PWM <pin> [-D]\n");
	printf("       86PWM <pin> [-E <duty> <period>]\n");
	printf("Example: 86PWM 3 -E 256\n");
	printf("         86PWM 5 -E 256 2000\n");
	printf("         86PWM 11 -D\n\n");
	printf("Option:\n");
	printf("    D: disable PWM on <pin>\n");
	printf("    E: enable PWM on <pin>; <duty> set PWM duty in percent (0~1024, e.g. 512 = 50%%); <period> set PWM period, you can optionally specify the period (in microseconds), by default it is set at 1 millisecond.\n");
}