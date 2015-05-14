#include<stdlib.h>
#include<syscall.h>
#include<sys/syscall.h>

ssize_t write(int fd, const void *buf, size_t count)
{
        ssize_t r = 0;
        r=syscall_3(SYS_write,(uint64_t)fd,(uint64_t)buf,(uint64_t)count);
        return r;
}

