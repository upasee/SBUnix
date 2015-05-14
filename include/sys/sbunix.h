#ifndef __SBUNIX_H
#define __SBUNIX_H

#include <sys/defs.h>

void kprintf(const char *fmt, ...);
void write_string(int colour, const char *string, int size);
void read_string(const char *string, int size);

#endif
