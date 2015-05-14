#include<stdlib.h>
#include<string.h>
#include<stdio.h>

int main(int argc, char* argv[], char* envp[])
{
	int pid;
	if (argv[1]!= NULL && (strcmp(argv[1],"-9") == 0))
	{
		if (argv[2] != NULL)
		{
			pid = atoi(argv[2],10);
			kill(pid);
		}
	}
	return 0;
}
