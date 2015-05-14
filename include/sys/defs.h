#ifndef _DEFS_H
#define _DEFS_H

#define NULL ((void*)0)
#define true 1
#define false 0

typedef unsigned long  uint64_t;
typedef          long   int64_t;
typedef unsigned int   uint32_t;
typedef          int    int32_t;
typedef unsigned short uint16_t;
typedef          short  int16_t;
typedef unsigned char   uint8_t;
typedef uint64_t      uintptr_t;
typedef int64_t	       intptr_t;
typedef int 		   bool;   

/* Elf data structure definitions */
typedef uint64_t      Elf64_Off;
typedef uint16_t     Elf64_Half;
typedef uint32_t     Elf64_Word;
typedef uint64_t     Elf64_Addr;
typedef uint64_t    Elf64_Xword;
	
#endif
