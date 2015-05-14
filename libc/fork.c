#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include <sys/syscall.h>

struct timespec{
        long int tv_sec;
        long int tv_nsec;
};

struct timeval {
long int tv_sec;
long int tv_usec;
};

struct rusage {
    struct timeval ru_utime;
    struct timeval ru_stime;
    long   ru_maxrss;
    long   ru_ixrss;
    long   ru_idrss;
    long   ru_isrss;
    long   ru_minflt;
    long   ru_majflt;
    long   ru_nswap;
    long   ru_inblock;
    long   ru_oublock;
    long   ru_msgsnd;
    long   ru_msgrcv;
    long   ru_nsignals;
    long   ru_nvcsw;
    long   ru_nivcsw;
};


pid_t fork(void)
{
        int pid;
        pid = syscall_0(SYS_fork);
        return pid;
}

pid_t getpid()
{
        pid_t pid;
        pid = syscall_0(SYS_getpid);
        return pid;
}

pid_t getppid()
{
        pid_t ppid;
        ppid = syscall_0(SYS_getppid);
        return ppid;
}

pid_t waitpid(pid_t pid, int *status, int options)
{
	int new_pid;
	struct rusage *rusg;
	rusg = malloc(sizeof(struct rusage));
        new_pid = syscall_4(SYS_wait4, (uint64_t)pid, (uint64_t)status, (uint64_t)options, (uint64_t)rusg);
        return (pid_t)new_pid;
}

unsigned int sleep(unsigned int seconds)
{
        unsigned int timeleft;
        struct timespec *req, *rem;
        req = (struct timespec*)malloc(sizeof(struct timespec));
        rem = (struct timespec*)malloc(sizeof(struct timespec));
        req->tv_sec = seconds;
        req->tv_nsec = 0;
        rem->tv_sec = 0;
        rem->tv_nsec = 0;
        timeleft = syscall_2(SYS_nanosleep, (uint64_t)req, (uint64_t)rem);
        return timeleft;
}

unsigned int alarm(unsigned int seconds)
{
        unsigned int timerem;
        timerem = syscall_1(SYS_alarm, (uint64_t)seconds);
        return timerem;
}

int execve(const char *filename, char *const argv[], char *const envp[])
{
        int r;
        r = syscall_3(SYS_execve, (uint64_t)filename, (uint64_t)argv, (uint64_t)envp);
//	if (r < 0)
//		printf("execve failed\n");
        return r;
}

int kill(int pid)
{
        int r;
        r = syscall_1(SYS_kill, (uint64_t)pid);
        return r;
}

