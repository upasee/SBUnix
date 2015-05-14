#include <sys/sbunix.h>
#include <sys/gdt.h>
#include <sys/paging.h>
#include <sys/process.h>
#include <sys/string.h>
#include <sys/trap.h>
#include <sys/sched.h>
#include <sys/file.h>

struct task_struct *current_task;
uint64_t *kernel_pml4;
uint64_t kernel_cr3;
struct task_struct *taskFreeList;
char *elfStart;
char *elfEnd;
struct tss_t tss;
void copy_file_descriptors(struct task_struct *parent, struct task_struct *child);

uint64_t sys_fork()
{
        struct task_struct *task = current_task;
        struct task_struct *newtask;
        task_alloc(&newtask, task->pid,0,task->name);
        newtask->pml4e = getPml4(kernel_pml4);
        newtask->cr3 = (uint64_t)physicalAddr((void *)newtask->pml4e);
        struct task_struct *temp = taskFreeList;
        while (temp->next_task)
        {
                temp = temp->next_task;
        }
        newtask->next_task = NULL;
        newtask->status = INIT;
        uint64_t kstack;
        kstack = (uint64_t)kmalloc(0x1000);
        newtask->kstack = (uint64_t *)(kstack + 0x1000 - 0x8);
        copy_trapframe_parentToChild(task, newtask);
        static struct mm_struct *mmStruct;
        mmStruct = (struct mm_struct *)kmalloc(sizeof(struct mm_struct));
        mmStruct->task = newtask;
        newtask->mm = mmStruct;
        uint64_t i;
        struct vm_area_struct *vms;
        vms = (struct vm_area_struct *)kmalloc(sizeof(struct vm_area_struct)*(task->vma_count));
        struct vm_area_struct *parent_list = task->vma_list;
        int j = 0;
        while (parent_list)
        {
             uint64_t start = roundDown((void *)parent_list->vm_start, 0x1000);
                uint64_t end = roundUp((void *)parent_list->vm_end, 0x1000);
                for (i= start; i< end; i+=0x1000)
                {
                        if (i >= 0x80296000 && i < 0x802a0000)
				continue;
/*                        if (i >= 0x8029a000 && i < 0x8029b000)
                                continue;
                        if (i >= 0x8029f000 && i < 0x802a0000)
                                continue;
 */                       uint64_t *table;
                        table = page_walk(task->pml4e, (void *)i, false ,PTE_U);
                        if (table != 0)
                        {
                                if(*table & PTE_P)
                                {
                                        if (*table & PTE_W)
                                        {
                                                *table  = *table & PTE_NW;
                                                *table = *table | PTE_COW;
                                                page_map(task->pid, (void *)i, newtask->pid, (void *)i, PTE_P|PTE_U|PTE_COW);
                                        }
                                        else
                                        {
                                                page_map(task->pid, (void *)i, newtask->pid, (void *)i, PTE_P|PTE_U);
                                        }

                                }
                        }
                }
                vms[j].vm_start = parent_list->vm_start;
                vms[j].vm_end = parent_list->vm_end;
                if (j != 0)
                        vms[j-1].next_vma = &vms[j];
                vms[j].file = NULL;
                vms[j].mm = newtask->mm;
                j++;
                parent_list = parent_list->next_vma;
        }
        uint64_t envvars = 0x802a0000;
        mem_alloc(task->pml4e, (void  *)envvars, PTE_P|PTE_W|PTE_U);
        uint64_t userstack = 0x8029f000;
        uint64_t temp1 = 0x802a0000;
        mem_alloc(newtask->pml4e, (void  *)userstack, PTE_P|PTE_W|PTE_U);
        vms[j].vm_start = userstack;
        vms[j].vm_end = userstack + 0x1000 - 0x8;
	vms[j].mm = task->mm;
        vms[j].file = NULL;
        newtask->stack = &vms[j];
        page_map(task->pid, (void *)(task->stack->vm_start), newtask->pid, (void *)(temp1), PTE_P|PTE_U|PTE_W);
        loadcr3((void *)newtask->cr3);
        memcpy((void *)(newtask->stack->vm_start), (void *)(temp1), 0x1000);
        page_unmap(newtask->pid, (void *)temp1);
        loadcr3((void *)kernel_cr3);
        vms[j-1].next_vma = &vms[j];
        j++;
        uint64_t userheap = 0x80296000;
        uint64_t temp2= 0x80289000;
	i=0;
	newtask->heap_hard_limit = userheap + (5*0x1000) - 0x8;
        for(i=userheap;i<newtask->heap_hard_limit;i+=0x1000)
        {
                mem_alloc(newtask->pml4e, (void  *)i, PTE_P|PTE_W|PTE_U);
        }
        mem_alloc(newtask->pml4e, (void  *)userheap, PTE_P|PTE_W|PTE_U);
        vms[j].vm_start = userheap;
        vms[j].vm_end = newtask->heap_hard_limit;
        vms[j].mm = task->mm;
        vms[j].file = NULL;
        newtask->heap = &vms[j];
        page_map(task->pid, (void *)(task->heap->vm_start), newtask->pid, (void *)(temp2), PTE_P|PTE_U|PTE_W);
        loadcr3((void *)newtask->cr3);
        memcpy((void *)(newtask->heap->vm_start), (void *)(temp2), 0x1000);
        page_unmap(newtask->pid, (void *)temp2);
        loadcr3((void *)kernel_cr3);
        vms[j-1].next_vma = &vms[j];
        newtask->vma_list = &vms[0];
        newtask->vma_count = j+1;
        newtask->mm->vma_list = newtask->vma_list;
        newtask->heap_end = newtask->heap->vm_start;
//        newtask->heap_hard_limit = userheap + (5*0x1000) - 0x8;
        newtask->tf.reg_rax = 0;
        task->tf.reg_rax = newtask->pid;
	copy_file_descriptors(current_task,newtask);
        copy_trap_frame((uint64_t)newtask->kstack,&newtask->tf);
        newtask->status = RUNNABLE;
        newtask->parent_pid = current_task->pid;
	loadcr3((void*)current_task->cr3);
        return newtask->pid;
}

void copy_file_descriptors(struct task_struct *parent, struct task_struct *child)
{
	int i=0;
	for(i=0;i<20;i++)
	{
		if(parent->open_fd[i] != 0)
		{
			if(child->open_fd[i] == 0)
			{
				child->open_fd[i] = (struct FileDesc *)kmalloc(sizeof(struct FileDesc));
			}
			memcpy(child->open_fd[i],parent->open_fd[i],sizeof(struct FileDesc));
		}
	}
}

struct task_struct *taskfrompid(uint64_t pid)
{
        struct task_struct *temp = taskFreeList;
        while(temp)
        {
                if (temp->pid == pid)
                        return temp;
                temp = temp->next_task;
        }
        return NULL;
}

uint64_t sys_getpid()
{
        return current_task->pid;
}

uint64_t sys_getppid()
{
        return current_task->parent_pid;
}

uint64_t sys_exec(char *filename, char *argv[], char *envp[])
{
        struct posix_header_ustar *ph_init= (struct posix_header_ustar *)elfStart;
        struct elfHdr *binary;
        char *size;
        uint64_t sizeInt = 0;
        uint64_t prev_size=(uint64_t)elfStart;

        while((uint64_t)ph_init < (uint64_t)elfEnd)
        {
                ph_init = (struct posix_header_ustar *)(prev_size + sizeInt);
                prev_size += sizeof(struct posix_header_ustar) + sizeInt;
                size = (char *)ph_init->size;
                sizeInt = atoi(size,8);
                sizeInt=roundUp((void *)sizeInt, 512);
                if(strcmp(ph_init->name,"bin/")==0)
                        break;
        }
        while(strncmp(ph_init->name,"bin/",4) == 0)
        {
                ph_init = (struct posix_header_ustar *)(prev_size + sizeInt);
                if(strncmp(ph_init->name,"bin/",4) == 0)
                {
                        prev_size += sizeof(struct posix_header_ustar) + sizeInt;
                        size = (char *)ph_init->size;
                        sizeInt = atoi(size,8);
                        sizeInt = roundUp((void *)sizeInt, 512);
                        binary = (struct elfHdr *)(( uint64_t )ph_init + (uint64_t )sizeof(struct posix_header_ustar));
			loadcr3((void *)current_task->cr3);
                        if (strcmp(ph_init->name, filename) == 0)
                        {
				if (filename[strlen(filename) - 1] == '&')
                                        current_task->background = 1;
                                loadElf(binary, current_task, current_task->pml4e);
                                loadcr3((void *)current_task->cr3);
                                copyElf(binary);
                                trap_frame_init(current_task,(uint64_t)current_task->stack->vm_end,(uint64_t)binary->e_entry);
				strcpy(current_task->name,filename);
                                setenv(current_task, argv, envp);
                                tss.rsp0 = (uint64_t)current_task->kstack;
                                return 0;
                        }

                }
        }
        return -1;
}

uint64_t sys_waitpid(uint64_t pid, uint64_t status, uint64_t options)
{
        struct task_struct *tsk = taskFreeList;
        while(tsk)
        {
                if(tsk->parent_pid == current_task->pid && tsk->background != 1)
                {
                        current_task->status = WAITING;
                        sched();
                        return tsk->pid;
                }
                tsk = tsk->next_task;
        }
        return -1;
}

void sys_exit()
{
        int ppid = current_task->parent_pid;
        struct task_struct *tsk = taskFreeList;
        struct task_struct *parent;
        parent = taskfrompid(ppid);
        if (ppid != 0)
        {
        if (parent->status == WAITING)
        {
                while(tsk)
                {
                        if((tsk->parent_pid == ppid) && (tsk->status == RUNNABLE) && tsk->pid != current_task->pid)
                                break;
                        tsk = tsk->next_task;
                }
                if (tsk == NULL)
                        parent->status = RUNNABLE;
        }
        }
        current_task->status = EXIT;
        sched();
}

uint64_t sys_dup(int fdnum)
{
        if (fdnum >= 20)
                return -1;
        if(current_task->open_fd[fdnum] != 0 && current_task->open_fd[fdnum]->open == 1)
        {
                int fd = 0;
                while(current_task->open_fd[fd] != 0)
                {
                        fd++;
                }
                current_task->open_fd[fd] = (struct FileDesc *)kmalloc(sizeof(struct FileDesc));
                current_task->open_fd[fd]->f = current_task->open_fd[fdnum]->f;
                current_task->open_fd[fd]->offset = current_task->open_fd[fdnum]->offset;
                current_task->open_fd[fd]->fd = fd;
                current_task->open_fd[fd]->perm = current_task->open_fd[fdnum]->perm;
                current_task->open_fd[fd]->open = current_task->open_fd[fdnum]->open;
                return fd;
        }
        return -1;
}

uint64_t sys_dup2(int oldfd, int newfd)
{
        if (oldfd >=20 || newfd >=20)
                return -1;
        if (current_task->open_fd[oldfd] != 0/* && current_task->open_fd[oldfd]->open == 1*/)
        {
                sys_close(newfd);
                current_task->open_fd[oldfd]->f = current_task->open_fd[newfd]->f;
                current_task->open_fd[oldfd]->offset = current_task->open_fd[newfd]->offset;
                current_task->open_fd[oldfd]->fd = newfd;
                current_task->open_fd[oldfd]->perm = current_task->open_fd[newfd]->perm;
                current_task->open_fd[oldfd]->open = current_task->open_fd[newfd]->open;
                return newfd;
        }
        return -1;
}



uint64_t sys_pipe(int pipe[])
{
        int fd = 3;
        while(current_task->open_fd[fd] != 0 && current_task->open_fd[fd]->open == 1)
        {
              fd++;
        }
        if (fd > 20)
                return -1;
        pipe[0] = fd;
        fd++;
        while(current_task->open_fd[fd] != 0 && current_task->open_fd[fd]->open == 1)
        {
                fd++;
        }
	if (fd > 20)
                return -1;
        loadcr3((void *)current_task->cr3);
        pipe[1] = fd;
        char buf[4096];
        current_task->open_fd[pipe[0]] = (struct FileDesc *)kmalloc(sizeof(struct FileDesc));
        struct File *f = (struct File *)kmalloc(sizeof(struct File));
        f->type = F_PIPE;
        f->start_addr = buf;
        current_task->open_fd[pipe[0]]->f = f;
        current_task->open_fd[pipe[0]]->open = 1;
        current_task->open_fd[pipe[0]]->perm = PTE_P|PTE_U;
        current_task->open_fd[pipe[0]]->offset = 0;
        current_task->open_fd[pipe[1]] = (struct FileDesc *)kmalloc(sizeof(struct FileDesc));
        current_task->open_fd[pipe[1]]->f = f;
        current_task->open_fd[pipe[1]]->open = 1;
        current_task->open_fd[pipe[1]]->perm = PTE_P|PTE_U|PTE_W;
        current_task->open_fd[pipe[1]]->offset = 0;
        current_task->tf.reg_rax = 0;
        return 0;
}

int sys_sleep(int seconds)
{
        current_task->sleep_seconds = seconds;
        current_task->status = SLEEPING;
        sched();
	return 0;
}

