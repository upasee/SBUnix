#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) 
{
	char buff[10];
	getcwd(buff,10);
	remove_slash_before(buff);
	struct DIR *d;
	if((d=(struct DIR *)opendir(buff)) != NULL)
	{
	}
	else
		return -1;
	struct dirent *dir;
	while((dir = readdir(d)) != NULL)
	{
		printf("%s\t",dir->d_name);
	}
	printf("\n");
	return 0;
}
