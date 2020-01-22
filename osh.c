#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 80

typedef enum { false, true } bool;

char history[10][MAX_LINE];
int count = 0;
bool should_run = true;

void printHistory() 
{
	int i;

	if (count == 0)
		printf("No commands in history.\n");
	else
	{
		printf("Command History:\n");

		for (i = 0; i < count; i++)
			printf("%i: %s\n", i + 1, history[i]);
	}
}

void addHistory(char str[]) 
{
	if (str[0] == '!' || !strcmp(str, "history\n"))
		return;

	int i;

	if (count != 0)
		for (i = 9; i > 0; i--)
			strcpy(history[i], history[i - 1]);

	str[strlen(str) - 1] = ' ';

	strcpy(history[0], str);

	if (count < 10)
		count++;
}

int interpretInput(char input[], char* args[], bool* amp, bool nth) 
{
	int length, i;
	char* p, temp[MAX_LINE];

	if (strlen(input) < 1)
		return 0;

	length = strlen(input);
	if (nth) 
		input[length - 1] = '\0';
	else
		input[length] = '\0';

	if (!strcmp(input, "history")) 
	{
		printHistory();
		return 0;
	}
	else if (!strcmp(input, "exit")) 
	{
		should_run = false;
		return 0;
	}
	else if (!strcmp(input, "!!")) 
	{
		if (count > 0) 
		{
			strcpy(temp, history[0]);
			printf("Executing: %s\n", temp);
			interpretInput(temp, args, amp, false);
			return 1;
		}
		else
		{
			printf("No commands in history.\n");
			return 0;
		}
	}
	else if (input[0] == '!') 
	{
		i = input[1] - '1';
		if (i < count && i >= 0) 
		{
			strcpy(temp, history[i]);
			printf("Executing: %s\n", temp);
			interpretInput(temp, args, amp, false);
			return 1;
		}
		else 
		{
			printf("No such command in history.\n");
			return 0;
		}
	}

	p = strtok(input, " ");
	i = 0;
	while (p) 
	{
		args[i] = p + '\0';
		i++;
		p = strtok(NULL, " ");
	}

	if (!strcmp(args[i - 1], "&")) 
	{
		*amp = true;
		args[i - 1] = NULL;
	}
	else
		*amp = false;

	return 1;
}

int main() 
{
	char input[MAX_LINE], *args[MAX_LINE / 2 + 1], temp[MAX_LINE];
	pid_t pid;
	bool amp;

	while (should_run) 
	{
		printf("osh> ");
		fflush(stdout);

		fgets(input, MAX_LINE, stdin);
		strcpy(temp, input);

		if (interpretInput(input, args, &amp, true)) 
		{
			addHistory(temp);
			pid = fork();

			if (pid < 0)
				printf("Process creation failed.\n");
			else if (pid == 0)
				if (execvp(args[0], args) == -1)
				printf("Unable to execute command.\n");
			else
				if (!amp)
					wait(NULL);
		}
	}

	return 0;
}