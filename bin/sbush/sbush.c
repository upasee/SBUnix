#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char ps1[50];
static char home[50];
static char path[100];
static char ps1init[25];

/* Initialization of PS1 */
int ps1_init(char ps1[])
{
        getcwd(ps1,100);
        return 0;
}

/* setenv PATH */
void setpath(char *newpath)
{
	strcpy(path,newpath);
}

/* Change PS1 */
void setps1(char *newps1)
{
        strcpy(ps1init,newps1);
}

/* Executing binaries */
int appendPathAndExec(char *input[])
{
/*
	char *blah[] ={NULL};
	char *paths[50];
	char pathtemp[1024], buf[128];
	int i=0,j=0,r=0;
	j=tokenize(path,paths,':');
	for (i = 0; i<j; i++)
	{
		if (strncmp(paths[i],input[0], strlen(paths[i]))== 0)
		{
			r = execve(input[0], input, blah);
		}
		strcpy(pathtemp, paths[i]);
		strcat(pathtemp, "/");
		strcat(pathtemp, input[0]);
		r = execve(pathtemp, input,blah);
		if (r == 0)
		{
			break;
		}
	}
	if (r < 0)
	{
		getcwd(buf,128);
		for(i=0;buf[i]!='\0';i++)
		       pathtemp[i] = buf[i];
		pathtemp[i] = '\0';
		strcat(pathtemp, "/");
		strcat(pathtemp, input[0]);
		r = execve(pathtemp, input, blah);
	}
*/
	int r=0;
/*	char one[] = "abc";
	char two[] = "xyz";
	char *argv1[] = {one, two, NULL};
*/	char *envp1[] = {NULL};

	r=execve("bin/ls", input, envp1);
	return r;
}

/* Pipes */
/*
void pipe_exec(char *cmd[20][20],int count)
{
	int i=0,pipefd[2],status,pid,fwd=0,r=0;
	int temp_count = count;
	for(i=0;temp_count!=0;i++,temp_count--)
	{
		pipe(pipefd);
		pid=fork();
		if(pid == 0)
		{
			if(i!=0)
			{
				dup2(fwd,0);
				close(fwd);
			}
			if(i!=count-1)
				dup2(pipefd[1],1);
			close(pipefd[1]);
			close(pipefd[0]);
			r = appendPathAndExec(cmd[i]);
			if (r < 0)
			{
				badcommand = 1;
				break;
			}
		}
		else
		{
			waitpid(pid,&status,0);
			close(pipefd[1]);
			fwd = pipefd[0];
		}
	}
}
*/
/* Execution of any command */
void shell_exec(char str[])
{
	char *input[20], *localenv[2];
//	char *pipes[20], *pipetemp[20];// *cmd[20][20];
//	int pipeflag=0;
//	int i=0,j=0,x=0,k=0,y=0;
	char pathtemp[50];
	int noOfTok=tokenize(str,input,' ');

	/* Change directory */
	if(strcmp(input[0], "cd") == 0)
	{
		if (noOfTok == 1)
		{
			chdir(home);
			return;
		}

		if(input[1][0] == '~')
		{
			chdir(home);
			return;
		}

		chdir(input[1]);
		return;
	}

	/* echo environment variables and $PS1 */
	else if(strcmp(input[0],"echo")==0 && input[1] != NULL)
	{
		if(input[1][0] =='$')
		{
			if(strcmp(input[1],"$PATH")==0)
				printf("%s\n",path);
			else if(strcmp(input[1],"$PS1")==0)
				printf("%s\n",ps1init);
			else
				printf("\n");
			return;
		}
		else
		{
			printf(input[1]);
			printf("\n");	
			return;
		}
	}

	/* exit */
	else if(strcmp(input[0],"exit")==0)
	{
		exit(0);
	}


	/* export PATH and PS1 */
	else if(strcmp(input[0],"export")==0)
	{
		if(input[1] == NULL)
			return;
		tokenize(input[1],localenv,'=');
		if(strcmp(localenv[0],"PATH")==0)
			setpath(localenv[1]);
		else
			setps1(localenv[1]);
		return;
	}

	/* Scripts sbush */
        else if(strcmp(input[0],"sbush")==0)
        {
		int i=0,j=0;
                if(noOfTok == 1)
                {
                        printf("Please specify the file name\n");
                        return;
                }
		char src2[120];
		char *scr_temp = (char *)malloc(100);
		getcwd(src2,120);
		strcpy(scr_temp,src2);
		strcat(scr_temp, input[1]);
		remove_slash_before(scr_temp);		
                int fd = open(scr_temp,O_RDONLY);
                char *scr[20];
                char *buff;
                buff = (char *)malloc(100);
                read(fd,buff,100);
                j=tokenize(buff,scr,'\n');
                for(i=0;i<j;i++)
                {
                        shell_exec(scr[i]);
                }
                close(fd);
                return;
        }


	/* Check for pipes */
/*	for(i=0;str[i]!='\0';i++)
	{
		if(str[i] == '|')
		{
			pipeflag=1;
			break;
		}
	}
*/
	int i=0,j=0,r=0;
	char *env_temp[] ={NULL};
	char buf[50];
	int status;
	int pid = fork();
	if (pid == 0)
        {
		char *paths[50];
		j=tokenize(path,paths,':');
		for (i = 0; i<j; i++)
	        {
			if (strncmp(paths[i],input[0], strlen(paths[i]))== 0)
			{
				remove_slash_before(input[0]);
				r = execve(input[0], input, env_temp);
			}
			strcpy(pathtemp, paths[i]);
			remove_slash_before(pathtemp);
			strcat(pathtemp, input[0]);
			r = execve(pathtemp, input, env_temp);
			if (r == 0)
			{
				break;
			}
		}
		if(r<0)
		{
			getcwd(buf,128);	
			strcpy(pathtemp,buf);
			remove_slash_before(pathtemp);
			strcat(pathtemp, input[0]);
			r = execve(pathtemp, input, env_temp);
		}
        }


	/*	
		if(pipeflag == 1)
		{
			printf("Pipes\n");
			j=tokenize(str,pipes,'|');
			printf("total tokens are %d\n",j);
			for(x=0;x<j;x++)
			{
				k=tokenize(pipes[i],pipetemp,' ');
				for(y=0;y<k;y++)
				{
					printf("pipetemp[y] is %s\n",pipetemp);
					cmd[x][i]=pipetemp[y];
				}
				cmd[x][i]=NULL;
			}
			cmd[x][0]=NULL;
			pipe_exec(cmd,j);
		}
		else
		{
			printf("hello world - I am the forked child\n");
			char one[] = "abc";
			char two[] = "xyz";
			char *argv1[] = {one, two, NULL};
			char *envp1[] = {NULL};
			execve(str, argv1, envp1);
		}	
*/	else
	{
		waitpid(pid,&status,0);
	}
}

int main(int argc, char* argv[], char* envp[]) 
{
	strcpy(home,"/bin/");
	strcpy(ps1init,"sbush");
	strcpy(path,"/bin/");
	while(1)
	{
		ps1_init(ps1);
		printf("%s",ps1init);
		printf(":");
		printf("%s",ps1);
		char *str;
		str = malloc(30);
		printf("$ ");
		scanf("%s",str);
		shell_exec(str);	
	}
	return 0;
}
