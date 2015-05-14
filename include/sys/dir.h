#ifndef _DIR_H
#define _DIR_H

#include <sys/defs.h>

extern char *current_dir;

int64_t sys_getdents(uint64_t fd, char *buff,int count);
int parse_root(char *input, char *parent, char delim);
int sys_getcwd(char *buff, int size);
int sys_chdir(char *path);

#endif
