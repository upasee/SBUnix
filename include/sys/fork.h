#ifndef _FORK_H
#define _FORK_H

#include <sys/defs.h>
#include <stdlib.h>
#include <sys/elf.h>
#include <sys/tarfs.h>

uint64_t sys_fork();
uint64_t sys_exec(char *filename, char *argv[], char *envp[]);
uint64_t sys_dup(int fdnum);
uint64_t sys_dup2(int oldfd, int newfd);
uint64_t sys_waitpid(uint64_t pid, uint64_t status, uint64_t options);
void sys_exit();
uint64_t sys_pipe(int pipe[]);
int sys_sleep(int seconds);
#endif
