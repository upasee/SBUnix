#include<stdlib.h>
#include<syscall.h>

void yield()
{
	syscall_0(SYS_yield);
/*         __asm __volatile("movq %0,%%rdi \n\t"
			  "movq %1,%%rax \n\t"
			  "int $0x80 \n\t"
			 :
			 :"r"(a1),
			  "r"(n)
			 :"cc","memory"); */
}

