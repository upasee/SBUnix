#include<stdio.h>
#include<sys/syscall.h>
#include<syscall.h>
#include<stdlib.h>
#define BUF_SIZE 1024

static char dir_buf[300];
static int nread = 0;
static int bpos = 0;
static int reclen=0;

void *opendir(const char *dirname)
{
        int fd;
        struct DIR *dp;
        if (((fd = open(dirname, O_DIRECTORY)) < 0) || ((dp = malloc(sizeof(struct DIR))) == NULL))
        {
                return NULL;
        }
        dp->fd = fd;
        return (void *)dp;
}

struct dirent *readdir(void *dp)
{
        struct DIR *dir = (struct DIR*)dp;
	bpos = bpos+reclen;
        struct dirent *d;
	if(bpos >= nread)
                bpos = 0;
        if(bpos == 0)
	{
                nread = syscall_3(SYS_getdents,(uint64_t)dir->fd,(uint64_t)dir_buf,300);
	}
        if(nread == 0 || nread == 1)
                return NULL;
        d = (struct dirent*)((uintptr_t)dir_buf+(uintptr_t)bpos);
	reclen = d->d_reclen;
        return d;
}

int closedir(void *dir)
{
        struct DIR *d = (struct DIR*)dir;
        close(d->fd);
        return 0;
}

char *getcwd(char *buf, size_t size)
{
        syscall_2(SYS_getcwd, (uint64_t)buf, (uint64_t)size);
        return buf;
}

int chdir(const char* path)
{
        int r;
        r = syscall_1(SYS_chdir, (uint64_t)path);
        if (r < 0)
        {
                printf("chdir failed\n");
                return -1;
        }
        return 0;
}
