#include <sys/sbunix.h>
#include <sys/paging.h>
#include <sys/elf.h>
#include <sys/process.h>
#include <sys/string.h>


struct task_struct *current_task;

uint64_t sys_brk(uint64_t end_data_segment)
{
/*
        if (end_data_segment > current_task->heap_hard_limit)
        {
                kprintf("end segment more than limit\n");
                return -1;
        }
        else if (end_data_segment > current_task->heap->vm_start)
        {
                if (end_data_segment < current_task->heap->vm_end)
                        current_task->heap_end = end_data_segment;
                else
                {
                        mem_alloc(current_task->pml4e, (void  *)(current_task->heap->vm_end + 0x8), PTE_P|PTE_W|PTE_U);
                        current_task->heap->vm_end = current_task->heap->vm_end + 0x1000;
                        current_task->heap_end = end_data_segment;
                }
        }
	current_task->tf.reg_rax = (uint64_t)current_task->heap_end;
        return (uint64_t)current_task->heap_end;
*/
//	if (current_task->heap->vm_end < end_data_segment)
	if (current_task->heap_end < end_data_segment)
        {
 //               current_task->heap->vm_end = end_data_segment;
                current_task->heap_end = end_data_segment;
        }
        current_task->tf.reg_rax = (uint64_t)current_task->heap_end;
        return (uint64_t)current_task->heap_end;

}


