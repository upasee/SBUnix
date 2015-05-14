#include<stdlib.h>
#include<syscall.h>

void exit(int status)
{
        uint64_t n= (uint64_t)status;
        uint64_t a1=0x3c;

         __asm __volatile("movq %0,%%rdi \n\t"
			  "movq %1,%%rsi \n\t"
			  "int $0x80 \n\t"
			 :
			 :"r"(n),
			  "r"(a1)
			 :"cc","memory");
}

