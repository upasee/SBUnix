#include<sys/sbunix.h>
#include<sys/idt.h>
#include<sys/gdt.h>
#include<sys/syscall.h>
#include<sys/process.h>
#include<sys/sched.h>
#include<sys/trap.h>
#include<sys/fork.h>
#include<sys/dir.h>
#include<sys/file.h>
#include<sys/malloc.h>

struct task_struct *current_task;
uint64_t task_no;
struct task_struct *taskFreeList;
struct tss_t tss;

void syscall_handler(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5)
{
	int ret=0;
	set_trap_frame(current_task);
	switch(n)
	{
		case SYS_write:
			sys_write(a1, (void *)a2, a3);
			break;
		case SYS_open:
			sys_open((char*)a1,a2);
			break;
		case SYS_yield:
			sched();
			break;
		case SYS_read:
                        sys_read(a1,(void *)a2, a3);
                        break;
		case SYS_close:
			sys_close(a1);
			break;
		case SYS_lseek:
                        sys_lseek(a1,a2,a3);
                        break;
		case SYS_brk:
                        sys_brk(a1);
                        break;
		case SYS_fork:
                        sys_fork();
                        break;
		case SYS_exit:
                        sys_exit();
			break;
		case SYS_getdents:
			sys_getdents(a1,(char *)a2, a3);
			break;
		case SYS_execve:
                        ret = (uint64_t)sys_exec((char *)a1, (char **)a2, (char **)a3);
                        break;
		case SYS_wait4:
                        ret = (uint64_t)sys_waitpid(a1, a2, a3);
                        break;
		case SYS_getcwd:
			sys_getcwd((char *)a1,a2);
			break;
		case SYS_chdir:
			sys_chdir((char *)a1);
			break;
		case SYS_pipe:
                        ret = sys_pipe((int *)a1);
                        break;
                case SYS_dup2:
                        ret = (uint64_t)sys_dup2(a1, a2);
                        break;
		case SYS_dup:
                        ret = (uint64_t)sys_dup(a1);
                        break;
		case SYS_ps:
                        sys_ps();
                        break;
		case SYS_nanosleep:
                        sys_sleep((int)a1);
                        break;
		case SYS_kill:
                        sys_kill((int)a1);
                        break;
	}

	if(n!=SYS_yield)
        {
                copy_trap_frame((uint64_t)current_task->kstack,&current_task->tf);
                switch_to_user((uint64_t)current_task->kstack);
        }
	kprintf("ret = %d\n",ret);
}

