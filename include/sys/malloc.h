#ifndef _MALLOC_H
#define _MALLOC_H

#include <sys/defs.h>
#include <stdlib.h>
#include <sys/elf.h>
#include <sys/tarfs.h>








uint64_t sys_brk(uint64_t end_data_segment);


#endif

