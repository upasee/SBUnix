#include<stdlib.h>
#include<syscall.h>
#include<sys/syscall.h>

int open(const char *pathname, int flags)
{
        int r = 0;
        r=(int)syscall_2(SYS_open,(uint64_t)pathname,(uint64_t)flags);
        return r;
}

int close(int fd)
{
        int r;
        r = syscall_1(SYS_close,(uint64_t)fd);
        return r;
}

ssize_t read(int fd, void *buf, size_t count)
{
        ssize_t size;
        size = syscall_3(SYS_read, (uint64_t)fd, (uint64_t)buf, (uint64_t)count);
        return size;
}

off_t lseek(int fildes, off_t offset, int whence)
{
        int r;
        r = syscall_3(SYS_lseek, fildes, (uint64_t)offset, (uint64_t)whence);
        if (r < 0)
                printf("lseek failed\n");
        return r;
}

int dup(int oldfd)
{
        int r;
        r = syscall_1(SYS_dup, (uint64_t)oldfd);
        if (r < 0)
                printf("dup failed\n");
        return r;
}

int dup2(int oldfd, int newfd)
{
        int r;
        r = syscall_2(SYS_dup2, (uint64_t)oldfd, (uint64_t)newfd);
        if (r < 0)
                printf("dup2 failed\n");
        return r;
}

int pipe(int filedes[2])
{
        int r;
        r = syscall_1(SYS_pipe, (uint64_t)filedes);
        if (r < 0)
        {
                printf("pipe failed\n");
        }
        return r;
}

int ps()
{
	syscall_0(SYS_ps);
	return 0;
}
