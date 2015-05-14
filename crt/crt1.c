#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[], char* envp[]);

void _start(void) {

	uint64_t temp_argc=1, offset_1=8, offset_2=16;
        char **argv;
        char **envp;
        int argc=1,res;
	uint64_t user_user_stack=0;
	
	__asm __volatile(       "movq %%rsp,%%rbx \n\t"
                                "addq %4,%%rbx \n\t"
//				"movq %%rbx,%2\n\t"
                                "movq (%%rbx),%1 \n\t"
                                "addq %3,%%rbx\n\t"
				"movq %%rbx,%2\n\t"
                                "movq %%rbx,%0 \n\t"
                                :"=r"(argv),
                                 "=r"(temp_argc),
                                 "=r"(user_user_stack)
                                :"r"(offset_1),
                                 "r"(offset_2)
  //                               "r"(hop)
                                :"memory","rbx");
        argc = temp_argc;
        uint64_t x  = 8*(argc+1);

        __asm __volatile(       "movq %2,%%rbx \n\t"
                                "addq %1,%%rbx \n\t"
                                "movq %%rbx,%0 \n\t"
                                :"=r"(envp)
                                :"r"(x),
                                 "r"(argv)
                                :"memory","rbx");
	res = main(argc, argv, envp);
	exit(res);
}
