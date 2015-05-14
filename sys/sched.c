#include<sys/sbunix.h>
#include<sys/idt.h>
#include<sys/gdt.h>
#include<sys/process.h>
#include<sys/sched.h>
#include<sys/trap.h>

struct task_struct *current_task;
struct task_struct *taskFreeList;
struct tss_t tss;

void sched()
{	
	struct task_struct *new_task = taskFreeList;
	struct task_struct *free = taskFreeList;
        if (current_task)
        {
                while(free)
                {
                        if (free->status == EXIT && free!=current_task)
                                task_free(free->pid);
                        free = free->next_task;
                }
        }
	if(!current_task)
	{
		struct task_struct *taskFirst = taskFreeList;
		while(taskFirst->status != RUNNABLE)
		{
			if(taskFirst->next_task!=NULL)
				taskFirst = taskFirst->next_task;
			else
			{
				kprintf("NO TASKS TO RUN 1\n");
				while(1);
			}
		}
		tss.rsp0 = (uint64_t )taskFirst->kstack;
		tss_flush();
		current_task = taskFirst;
		if (current_task->status == RUNNABLE)
			current_task->status = RUNNING;
		loadcr3((void *)taskFirst->cr3);
		copy_trap_frame((uint64_t)taskFirst->kstack,(struct UTrapframe *)&taskFirst->tf);
		switch_to_user((uint64_t)taskFirst->kstack);	
	}
	set_trap_frame(current_task);
	if (current_task->status == RUNNING)
		current_task->status = RUNNABLE;
	struct task_struct *temp = taskFreeList;
	if (!temp)
	{
		kprintf("NO TASKS TO RUN! \n");
		current_task = NULL;
	}
	while(temp->pid != current_task->pid)
	{
		if(temp->next_task)
		{
			temp = temp->next_task;
		}
	}
	int task_flag=0;
	if(temp->next_task)
        {
                temp = temp->next_task;
		while(temp->status != RUNNABLE)
		{
			if(temp->next_task!=NULL)
			{
				temp = temp->next_task;
			}
			else
			{
				task_flag = 1;
				break;
			}
		}
		if(task_flag == 0)
			new_task = temp;
	}
	else
		task_flag=1;
	if(task_flag == 1)
	{
		new_task = taskFreeList;
		while(new_task->status != RUNNABLE)
                {
                        if(new_task->next_task!=NULL)
                                new_task = new_task->next_task;
                        else
			{
                                kprintf("NO TASKS TO RUN\n");
				while(1);
			}
                }
	}
	if (new_task->status == RUNNABLE)
		new_task->status = RUNNING;
	tss.rsp0 = (uint64_t)new_task->kstack;
	current_task = new_task;
	loadcr3((void *)new_task->cr3);
	copy_trap_frame((uint64_t)new_task->kstack,(struct UTrapframe *)&new_task->tf);
	switch_to_user((uint64_t)new_task->kstack);
}
