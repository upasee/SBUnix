#ifndef _PROCESS_H
#define _PROCESS_H

#include <sys/defs.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/elf.h>
#include <sys/tarfs.h>

extern uint64_t *kernel_pml4;
extern uint64_t kernel_cr3;

extern char *elfStart;
extern char *elfEnd;

enum
{
	INIT=0,
        RUNNABLE,
        RUNNING,
        BLOCKED,
        WAITING,
        EXIT,
	SLEEPING
};

struct mm_struct
{
	struct task_struct *task;
	struct vm_area_struct *vma_list;
		
};

struct ps
{
	uint64_t id;
	uint64_t parent_id;
	char ps_name[100];
};

struct vm_area_struct
{
	struct mm_struct *mm;
	unsigned long vm_start;
	unsigned long vm_end;
	struct vm_area_struct *next_vma;
	struct file *file;		
};

/* DO NOT TOUCH! Like EVER! */

struct UTrapframe {
	int64_t reg_r15;
        int64_t reg_r14;
        int64_t reg_r13;
        int64_t reg_r12;
        int64_t reg_r11;
        int64_t reg_r10;
        int64_t reg_r9;
        int64_t reg_r8;
        int64_t reg_rsi;
        int64_t reg_rdi;
        int64_t reg_rbp;
        int64_t reg_rdx;
        int64_t reg_rcx;
        int64_t reg_rbx;
        int64_t reg_rax;
	int64_t utf_rip;
        int64_t utf_cs;
        int64_t utf_flags;
        int64_t utf_rsp;
        int64_t utf_ds;
} __attribute__((packed));

struct task_struct
{
        struct task_struct *next_task;
        struct UTrapframe tf;
	struct FileDesc *open_fd[20];
        uint64_t pid;
        uint64_t parent_pid;
        unsigned int status;
        uint64_t *pml4e;
        uint64_t cr3;
        struct mm_struct *mm;
        struct vm_area_struct *heap;
        struct vm_area_struct *stack;
	struct vm_area_struct *vma_list;
        uint64_t *kstack;
	uint64_t vma_count;
        uint64_t heap_end;
        uint64_t heap_hard_limit;
	char name[100];
	uint64_t sleep_seconds;
        uint64_t background;
};


extern struct task_struct *taskStructs;
extern struct task_struct *taskFreeList;
extern int taskListCount;
extern struct task_struct *current_task;

void page_map(uint64_t srcpid, void *srcva,  uint64_t dstpid, void *dstva, int perm);
void page_unmap(uint64_t pid, void *va);
void createProcesses(char *elfStart, char *elfEnd);
void mem_alloc(void *pml4, void *mem, int perm);
void loadElf(struct elfHdr *binary, struct task_struct *task, uint64_t *kernel_pml4e);
void loadcr3(void *cr31);
void copyElf(struct elfHdr *binary);
uint64_t * getPml4(uint64_t *kernel_pml4);
uint64_t task_alloc(struct task_struct **task, int parentid, int status_flag, char *name);
struct task_struct *taskfrompid(uint64_t pid);
void setenv(struct task_struct *tsk, char *argv[], char *envp[]);
void task_free(uint64_t pid);
void sys_ps();
int sys_kill(int pid);
#endif
