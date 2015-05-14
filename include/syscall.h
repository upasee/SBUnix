#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <sys/defs.h>
#include <sys/syscall.h>
#include <stdio.h>

static __inline int64_t syscall_0(uint64_t n) 
{
	int64_t ret;
        __asm __volatile(	"movq %1,%%rdi \n\t"
                        	"int $0x80 \n\t"
                        	"movq %%rax,%0\n\t"
                        	:"=r"(ret)
                        	:"r"(n)
                        	:"cc","memory","rax"
                	);
        return ret;
}

static __inline int64_t syscall_1(uint64_t n, uint64_t a1) {
	int64_t ret;
	__asm __volatile(	"movq %1,%%rdi \n\t"
                        	"movq %2,%%rsi \n\t"
                      		"int $0x80 \n\t"
                        	"movq %%rax,%0\n\t"
                        	:"=r"(ret)
                        	:"r"(n),
                         	 "r"(a1)
                        	:"rax","rdi","rsi"
                	);
	return ret;
}

static __inline int64_t syscall_2(uint64_t n, uint64_t a1, uint64_t a2) {
	int64_t ret;
        __asm __volatile(       "movq %1,%%rdi \n\t"
                                "movq %2,%%rsi \n\t"
                                "movq %3,%%rdx \n\t"
                                "int $0x80 \n\t"
                                "movq %%rax,%0\n\t"
                                :"=r"(ret)
                                :"r"(n),
                                 "r"(a1),
                                 "r"(a2)
                                :"cc","memory","rdi","rsi","rax","rdx"
                        );
        return ret;

}

static __inline int64_t syscall_3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
	 int64_t ret;

        __asm __volatile(       "movq %1,%%rdi \n\t"
                                "movq %2,%%rsi \n\t"
                                "movq %3,%%rdx \n\t"
                                "movq %4,%%rcx \n\t"
                                "int $0x80 \n\t"
                                "movq %%rax,%0\n\t"
                                :"=r"(ret)
                                :"r"(n),
                                 "r"(a1),
                                 "r"(a2),
                                 "r"(a3)
                                :"cc","memory","rdi","rsi","rdx","rax","rcx"
                        );
        return ret;

}

static __inline int64_t syscall_4(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4) {
         int64_t ret;

        __asm __volatile(       "movq %1,%%rdi \n\t"
                                "movq %2,%%rsi \n\t"
                                "movq %3,%%rdx \n\t"
				"movq %4,%%rcx \n\t"
				"movq %5,%%r10 \n\t"
                                "int $0x80 \n\t"
                                "movq %%rax,%0\n\t"
                                :"=r"(ret)
                                :"r"(n),
                                 "r"(a1),
                                 "r"(a2),
				 "r"(a3),
                                 "r"(a4)
                                :"cc","memory","rdi","rsi","rdx","rcx","r10","rax"
                        );
        return ret;

}

#endif
