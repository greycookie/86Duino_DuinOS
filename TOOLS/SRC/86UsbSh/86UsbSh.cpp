#include "Arduino.h"
#include <stdio.h>
#include <conio.h>
#include <unistd.h>

typedef struct
{
	char *name;
	int size; 
}command;

void to_lowercase(char *str);
bool do_command(char *command);

FILE *fp;

int main(int argc, char **argv) 
{ 
	int i;
	int dont_support_command_num;
	int rt_val;
	int left, right, mid;
	int old_stderr, old_stdout;
	bool dont_support;
	char input_command[128];
	char buf_command[128];
	char path[128];
	char internal_command_prefix[] = "4dos.com /c ";

	command dont_support_command[] = { {"fdisk", 5}, {"format", 6} };

	Serial.begin(115200);

	fp = fopen("Z:\\tmp.txt", "w+");
	if(fp == NULL)
		return EXIT_FAILURE;

	old_stderr = dup(fileno(stderr));
	dup2(fileno(fp), fileno(stderr));
	old_stdout = dup(fileno(stdout));
	dup2(fileno(fp), fileno(stdout));

	dont_support = false;
	dont_support_command_num = sizeof(dont_support_command) / sizeof(command);

	while(kbhit() != 1)
	{
		if(!Serial)
		{
			while(!Serial)
				;
			getcwd(path, sizeof(path) / sizeof(char) - 1);

			Serial.println("================================================================================");
			Serial.println("                      ___   __   ____        _");
			Serial.println("                     ( _ ) / /_ |  _ \\ _   _(_)_ __   ___");
			Serial.println("                     / _ \\| '_ \\| | | | | | | | '_ \\ / _ \\");
			Serial.println("                    | (_) | (_) | |_| | |_| | | | | | (_) |");
			Serial.println("                     \\___/ \\___/|____/ \\____|_|_| |_|\\___/");
			Serial.println("\n 86UsbSh v1.0, author: PING-HUI WEI, E-mail: greycookie@dmp.com.tw, 2015/09/17");
			Serial.println("================================================================================");
			Serial.println();
			Serial.print(path);
			Serial.println('>');
		}

		rt_val = Serial.available();
		if(rt_val > 127)
			Serial.println("Command is too long!");
		else if(rt_val > 0)
		{
			Serial.readBytes(input_command, rt_val);
			input_command[rt_val] = '\0';
			to_lowercase(input_command);

			for(i = 0; i < dont_support_command_num; i++)
			{
				if(strncmp(input_command, dont_support_command[i].name, dont_support_command[i].size) == 0)
				{
					dont_support = true;
					break;
				}
			}
			if(dont_support == true)
			{
				Serial.println("Don't support command!");
				dont_support = false;
				continue;
			}

			Serial.print(path);
			Serial.print('>');
			Serial.println(input_command);

			if(strncmp("dir", input_command, 3) == 0)
			{
				strcpy(buf_command, internal_command_prefix);
				strcat(buf_command, input_command);

				do_command(buf_command);
			}
			else
			{
				do_command(input_command);

				if(strncmp("cd", input_command, 2) == 0)
					getcwd(path, sizeof(path) / sizeof(char) - 1);
			}

			Serial.print(path);
			Serial.println('>');
		}
	}

	dup2(old_stderr, fileno(stderr));
	dup2(old_stdout, fileno(stdout));
	fclose(fp);
	Serial.end();

	return EXIT_SUCCESS; 
} 

void to_lowercase(char *str)
{
	do
	{
		*str = tolower(*str);
	}
	while(*(++str) != '\0');
}

bool do_command(char* command)
{
	int i;
	int rt_val;
	char buf[256];

	if(command == NULL)
		return false;

	system(command);

	fflush(fp);
	rewind(fp);
	while(feof(fp) == 0)
	{
		rt_val = fread(buf, sizeof(char), 255, fp);
		buf[rt_val] = '\0';
		Serial.print(buf);
	}

	ftruncate(fileno(fp), 0);
	rewind(fp);

	return true;
}