#include <sys/sbunix.h>
#include <sys/paging.h>
#include <sys/elf.h>
#include <sys/gdt.h>
#include <sys/process.h>
#include <sys/string.h>
#include <sys/trap.h>

struct tss_t tss;
uint64_t *kernel_pml4;
uint64_t kernel_cr3;

void createProcesses(char *elfStart, char *elfEnd)
{
        struct posix_header_ustar *ph_init= (struct posix_header_ustar *)elfStart;
	struct elfHdr *binary;
	struct task_struct *task;
        char *size;
        uint64_t sizeInt = 0;
	uint64_t prev_size=(uint64_t)elfStart;
	int status_flag=0;

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
			if(strcmp(ph_init->name,"bin/sbush") == 0)
				status_flag=1;
			else
				status_flag=0;
			prev_size += sizeof(struct posix_header_ustar) + sizeInt;
			size = (char *)ph_init->size;
			sizeInt = atoi(size,8);
			sizeInt = roundUp((void *)sizeInt, 512);
			binary = (struct elfHdr *)(( uint64_t )ph_init + (uint64_t )sizeof(struct posix_header_ustar));
			if(isElf(binary) && checkElfSize(ph_init->size))
			{
				task_alloc(&task, 0,status_flag,ph_init->name);
				loadElf(binary, task, kernel_pml4);
				loadcr3((void *)task->cr3);
				copyElf(binary);
				trap_frame_init(task,(uint64_t)task->stack->vm_end,(uint64_t)binary->e_entry);
				copy_trap_frame((uint64_t)task->kstack,(struct UTrapframe *)&task->tf);
				loadcr3((void *)kernel_cr3);
				struct FileDesc *fd0 = (struct FileDesc *)kmalloc(sizeof(struct FileDesc));
                                struct FileDesc *fd1 = (struct FileDesc *)kmalloc(sizeof(struct FileDesc));
                                struct FileDesc *fd2 = (struct FileDesc *)kmalloc(sizeof(struct FileDesc));
                                fd0->fd = 0;
                                fd0->perm = PTE_P|PTE_U|PTE_W;
                                fd0->open = 1;
                                fd1->fd = 1;
                                fd1->perm = PTE_P|PTE_U|PTE_W;
                                fd1->open = 1;
                                fd2->fd = 2;
                                fd2->perm = PTE_P|PTE_U|PTE_W;
                                task->open_fd[0] = fd0;
                                task->open_fd[1] = fd1;
                                task->open_fd[2] = fd2;

			}
		}
	}
}

void mem_alloc(void *pml4, void *mem, int perm)
{

        uint64_t *table1 = page_walk(pml4, mem, true, perm);
        if(!(*table1 & PTE_P))
        {
                struct Page *pp = phys_page_alloc();
                *table1 = physAddrFromPage(pp)|PTE_P|PTE_W|perm;
        }
        else
        {
                *table1 = *table1|perm;
        }
}

void loadElf(struct elfHdr *binary, struct task_struct *task, uint64_t *kernel_pml4e)
{
	struct progHdr *phdr = (struct progHdr *)((uint64_t)binary + binary->e_phoff);
	uint64_t num = binary->e_phnum;
	struct vm_area_struct *vms;
	vms = (struct vm_area_struct *)kmalloc(sizeof(struct vm_area_struct)*(num+2));
	int k, j = 0;
	if(j);
	for (k=0; k<num; k++){
		if (phdr->p_type == 1)
		{
			uint64_t size = phdr->p_memsz;
			int nPages = size/4096;
			if (size % 4096 != 0)
				nPages = nPages + 1;
			void *addr = (void *)phdr->p_vaddr;
			vms[j].vm_start = phdr->p_vaddr;
			vms[j].vm_end = vms[j].vm_start + size;
			if (j != 0)
				vms[j-1].next_vma = &vms[j];
			vms[j].file = NULL;
			vms[j].mm = task->mm;
			j++;
			int i;
			for (i=0; i<= nPages; i++)
			{
				uint64_t *table;
				table = page_walk(task->pml4e, addr, true, PTE_P|PTE_W|PTE_U);
				struct Page *phys_page = phys_page_alloc();
				*table = physAddrFromPage(phys_page)|PTE_P|PTE_W|PTE_U;
				addr = addr+ 0x1000;
			}

		}

		phdr = (struct progHdr *)((uint64_t)phdr + binary->e_phentsize);
	}
	uint64_t *stack1 = (uint64_t *)vms + ((num+2)*sizeof(struct vm_area_struct));
	uint64_t stack = (uint64_t)roundUp((void *)stack1, 0x1000);
	mem_alloc(kernel_pml4e,(void *)stack, PTE_P|PTE_W);
	task->kstack = (uint64_t *)(stack + 0x1000 - 0x8);
	uint64_t envvars = 0x802a0000;
        mem_alloc(task->pml4e, (void  *)envvars, PTE_P|PTE_W|PTE_U);
	uint64_t userstack = 0x8029f000;
	mem_alloc(task->pml4e, (void  *)userstack, PTE_P|PTE_W|PTE_U);
	vms[j].vm_start = userstack;
	vms[j].vm_end = userstack + 0x1000 - 0x8;
	vms[j].mm = task->mm;
	vms[j].file = NULL;
	task->stack = &vms[j];
	vms[j-1].next_vma = &vms[j];
	j++;
//	uint64_t userheap = 0x8028a000;
//	uint64_t userheap = 0x8029a000;
	uint64_t userheap = 0x80296000;
	task->heap_hard_limit = userheap + (5*0x1000) - 0x8;
	uint64_t i=0;
	for(i=userheap;i<task->heap_hard_limit;i+=0x1000)
	{
		mem_alloc(task->pml4e, (void  *)i, PTE_P|PTE_W|PTE_U);
	}
//	mem_alloc(task->pml4e, (void  *)userheap, PTE_P|PTE_W|PTE_U);
	vms[j].vm_start = userheap;
	vms[j].vm_end = task->heap_hard_limit;
//	vms[j].vm_end = userheap;
	vms[j].mm = task->mm;
	vms[j].file = NULL;
	task->heap = &vms[j];
	vms[j-1].next_vma = &vms[j];
	task->vma_list = &vms[0];
	task->vma_count = j+1;
	task->mm->vma_list = task->vma_list;
	task->heap_end = task->heap->vm_start;
//	task->heap_hard_limit = userheap + (11*0x1000) - 0x8;
//	task->heap_hard_limit = userheap + (5*0x1000) - 0x8;
}

void loadcr3(void *cr31)
{
                __asm __volatile(       "movq %0,%%cr3"
                                :
                                : "r" (cr31)
                                : "memory","cc"
                                );

}

void copyElf(struct elfHdr *binary)
{
        uint64_t num = binary->e_phnum;
        struct progHdr *phdr = (struct progHdr *)((uint64_t)binary + binary->e_phoff);
        uint64_t k=0;
        for (k=0; k<num; k++)
        {
                if (phdr->p_type == 1)
                        {
                                memcpy((void *)phdr->p_vaddr, (void *)(binary+phdr->p_offset), phdr->p_filesz);
				if (phdr->p_memsz > phdr->p_filesz)
					memset1((void *)(phdr->p_vaddr + phdr->p_filesz), 0, phdr->p_memsz-phdr->p_filesz);
                        }

                phdr = (struct progHdr *)((uint64_t)phdr + binary->e_phentsize);

        }
}

uint64_t * getPml4(uint64_t *kernel_pml4)
{
        struct Page *p_pml4 = phys_page_alloc();
        p_pml4->refcount++;
        uint64_t phys_addr = physAddrFromPage(p_pml4);
        uint64_t *virt_addr = (uint64_t *)(phys_addr+(uint64_t)KERNBASE);
        uint64_t *pml = virt_addr;
        uint64_t va = roundUp((void *)KERNBASE, 0x1000);
        pml[pml4_index((void *)va)] = kernel_pml4[pml4_index((void *)va)]|PTE_U;
        return pml;
}

int taskListCount;
struct task_struct *taskFreeList;

struct task_struct *task_create()
{
        static struct task_struct *taskStruct;
        taskStruct = (struct task_struct *)kmalloc(sizeof(struct task_struct));
        static struct mm_struct *mmStruct;
        mmStruct = (struct mm_struct *)kmalloc(sizeof(struct mm_struct));
	taskListCount++;
	taskStruct->pid = taskListCount;
	taskStruct->status = INIT;
	taskStruct->pml4e = getPml4(kernel_pml4);
	mmStruct->task = taskStruct;
	taskStruct->mm = mmStruct;
	taskStruct->cr3 = (uint64_t)physicalAddr((void *)taskStruct->pml4e);

        if (!taskFreeList)
        {
                taskFreeList = taskStruct;
                taskFreeList->next_task = NULL;
        }
        else
        {
                struct task_struct *temp = taskFreeList;
                while(temp->next_task)
                        temp = temp->next_task;
                temp->next_task = taskStruct;
        }
        return taskStruct;

}

uint64_t task_alloc(struct task_struct **task, int parentid, int status_flag, char *name)
{
	struct task_struct *tsk = NULL;
	if (tsk == NULL)
        {
                tsk = task_create();
        }
        tsk->parent_pid = parentid;
	strcpy(tsk->name,name);
	if(status_flag == 1)
	        tsk->status = RUNNABLE;
        *task = tsk;
        return 0;

}

void setenv(struct task_struct *tsk, char *argv[], char *envp[])
{
        uint64_t stk = tsk->stack->vm_end;
        void * addr = (void *)0x802a00f0;
        void * addrold = (void *)0x802a00f0;
        void * addr1 = (void *)0x802a04f0;
        int i=0;
        while(argv[i]!= NULL)
        {
                strcpy((void *)addr1, argv[i]);
                *(uint64_t *)addr = (uint64_t)addr1;
                addr = (void *)((uint64_t)addr+ 0x8);
                addr1 = (void *)((uint64_t)addr1+ 0x8);
                i++;
        }
        uint64_t argc = i;
        memset1(addr, 0, 0x8);
        memset1(addr1, 0, 0x8);
        addr = addr + 0x8;
        addr1 = addr1 + 0x8;
        i=0;
        while(envp[i]!=NULL)
        {
                 strcpy((void *)addr1, envp[i]);
                *(uint64_t *)addr = (uint64_t)addr1;
                addr = (void *)((uint64_t)addr+ 0x8);
                addr1 = (void *)((uint64_t)addr1+ 0x8);
                i++;
        }
        memset1(addr, 0, 0x8);
        memset1(addr1, 0, 0x8);
        memcpy((void *)0x802a0008, addrold, 0x100);
        __asm __volatile(       "movq %1,   %%rcx \n\t"
                                "movq %0, 8(%%rcx)\n\t"
                                :
                                :"r"(argc),
                                "r"(stk)
                                :"memory", "rbx", "rcx");
}

void task_free(uint64_t pid)
{
        struct task_struct *tsk;
        tsk = taskfrompid(pid);
        uint64_t i=0;
        struct vm_area_struct *list = tsk->vma_list;
        while (list)
        {
                uint64_t start = roundDown((void *)list->vm_start, 0x1000);
                uint64_t end = roundUp((void *)list->vm_end, 0x1000);
                for (i= start; i< end; i+=0x1000)
                {
                        uint64_t *table;
                        table = page_walk(tsk->pml4e, (void *)i, false ,PTE_U);
                        if (table != 0)
                        {
                                if(*table & PTE_P)
                                {

                                        page_unmap(pid, (void *)i);
                                }
                        }
                }
                list = list->next_vma;
        }
}


void sys_ps()
{
	struct task_struct *temp = taskFreeList;
	struct ps *ps_temp;
	kprintf("PID   PARENT   CMD \n");
	while(temp)
	{
		if(temp->status != EXIT && temp->status != INIT)
		{
			ps_temp = (struct ps *)kmalloc(sizeof(struct ps));
			ps_temp->id = temp->pid;
			ps_temp->parent_id = temp->parent_pid;
			strcpy(ps_temp->ps_name,temp->name);
			kprintf("%d \t %d \t %s\n",ps_temp->id,ps_temp->parent_id,ps_temp->ps_name);
		}
		temp=temp->next_task;
	}
}

int sys_kill(int pid)
{
        struct task_struct *tsk = taskfrompid(pid);
        task_free(pid);
        tsk->status = EXIT;
        return 0;
}

