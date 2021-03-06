#include<sys/sbunix.h>
#include<sys/paging.h>
#include<sys/process.h>
#include<sys/idt.h>
#include<sys/sched.h>
#include<sys/port.h>
#include<sys/string.h>
#include<sys/file.h>

volatile int scan_flag;
volatile int flag1;

static uint8_t map[256]=
{
	[0x01] = 0x1B,
	[0x02] = '1', 
	[0x03] = '2',
	[0x04] = '3',
	[0x05] = '4',
	[0x06] = '5',
	[0x07] = '6',
	[0x08] = '7',
	[0x09] = '8',
	[0x0a] = '9',
	[0x0b] = '0',
	[0x0c] = '-',
	[0x0d] = '=',
	[0x0e] = '\b',
	[0x0f] = '\t',
	[0x10] = 'q',
	[0x11] = 'w',
	[0x12] = 'e',
	[0x13] = 'r',
	[0x14] = 't',
	[0x15] = 'y',
	[0x16] = 'u',
	[0x17] = 'i',
	[0x18] = 'o',
	[0x19] = 'p',
	[0x1a] = '[',
	[0x1b] = ']',
	[0x1c] = '\n',
	[0x1e] = 'a',
	[0x1f] = 's',
	[0x20] = 'd',
	[0x21] = 'f',
	[0x22] = 'g',
	[0x23] = 'h',
	[0x24] = 'j',
	[0x25] = 'k',
	[0x26] = 'l',
	[0x27] = ';',
	[0x28] = '\'',
	[0x29] = '`',
	[0x2b] = '\\',
	[0x2c] = 'z',
	[0x2d] = 'x',
	[0x2e] = 'c',
	[0x2f] = 'v',
	[0x30] = 'b',
	[0x31] = 'n',
	[0x32] = 'm',
	[0x33] = ',',
	[0x34] = '.',
	[0x35] = '/',
	[0x37] = '*',
	[0x39] = ' '
};

static uint8_t shiftmap[256] = 
{
	[0x08] = '&',
	[0x05] = '$',
        [0x10] = 'Q',
        [0x11] = 'W',
        [0x12] = 'E',
        [0x13] = 'R',
        [0x14] = 'T',
        [0x15] = 'Y',
        [0x16] = 'U',
        [0x17] = 'I',
        [0x18] = 'O',
        [0x19] = 'P',
        [0x1a] = '{',
        [0x1b] = '}',
        [0x1e] = 'A',
        [0x1f] = 'S',
        [0x20] = 'D',
        [0x21] = 'F',
        [0x22] = 'G',
        [0x23] = 'H',
        [0x24] = 'J',
        [0x25] = 'K',
	[0x26] = 'L',
        [0x27] = ':',
        [0x28] = '"',
        [0x29] = '~',
        [0x2b] = '|',
        [0x2c] = 'Z',
        [0x2d] = 'X',
        [0x2e] = 'C',
        [0x2f] = 'V',
        [0x30] = 'B',
        [0x31] = 'N',
        [0x32] = 'M',
        [0x33] = ',',
        [0x34] = '.',
        [0x35] = '?',

};

int timer_ticks = 0, sec;
void interrupt_handler()
{
	outb(0x20,0x20);
	timer_ticks++;
	if (timer_ticks % 18 == 0)
	{
		struct task_struct *sleeping = taskFreeList;
                while(sleeping)
                {
                        if (sleeping->status == SLEEPING)
                        {
                                sleeping->sleep_seconds--;
                                if (sleeping->sleep_seconds == 0)
                                        sleeping->status = RUNNABLE;
                        }
			sleeping = sleeping->next_task;
                }
		sec = timer_ticks/18;
		char *string = itoa(sec,10);
		volatile char *video = (volatile char*)0xFFFFFFFF800B8F98;
		while( *string != 0 )
		{
			*video++ = *string++;
			*video++ = 7;
		}
		if(scan_flag != 1)
			sched();
	}
}

void interrupt_handler2()
{
	kprintf("IN THE HANDLER\n");
	while(1);
}

void temp_handler()
{
	kprintf("In temp handler");
	while(1);
}

uint64_t getcr2()
{
        uint64_t cr2;
         __asm __volatile(      "movq %%cr2,%0"
                                :"=r"(cr2)
                                :
                                : "memory","cc"
                                );
	 return cr2;
}

void pf_handler()
{
	uint64_t cr2 = getcr2();

	loadcr3((void *)current_task->cr3);
	uint64_t *fault_va = (uint64_t *)roundDown((void *) cr2, 0x1000);
	if (fault_va <= (uint64_t *)0x8029fff8 && fault_va >=(uint64_t *)0x8029b000)
        {
                mem_alloc(current_task->pml4e, (void  *)(fault_va), PTE_P|PTE_W|PTE_U);
                current_task->stack->vm_start = (uint64_t)fault_va;
                return;

        }
        uint64_t temp = 0x802a1000;
	uint64_t *table1 = page_walk(current_task->pml4e, (void *)cr2, false, 0);
	if(*table1 & PTE_P)
	{
		if (*table1 & PTE_COW)
		{
			mem_alloc(current_task->pml4e, (void  *)temp, PTE_P|PTE_W|PTE_U);
			loadcr3((void *)current_task->cr3);
			memcpy((void *)(temp), (void *)(fault_va), 0x1000);
			page_map(current_task->pid, (void *)(temp), current_task->pid, (void *)(fault_va), PTE_P|PTE_U|PTE_W);
			page_unmap(current_task->pid, (void *)temp);
			*table1 = *table1 & PTE_NCOW;
			*table1 = *table1 | PTE_W;
			return;
	        }
	}
	while(1);
}

volatile struct buf *scan_buf;

void kbd_handler()
{
        outb(0x20,0x20);
        unsigned char c = 0x0;
        uint8_t d;
        c = inb(0x60);
        if (c & 0x80)
                return;
        if (c == 0x2a)
        {
                unsigned char u;
                u = inb(0x60);
                d = shiftmap[u];
        }
        else
        {
                d = map[c];
        }
        if(scan_buf->pos == 4095)
        {
                memset1(&(scan_buf->pp_buf), 0, 0x1000);
                scan_buf->pos = 0;
        }
        kprintf("%c", d);
        if (c == 0x1c)
        {
                flag1 = 1;
        }
        if (flag1 == 0)
        {
                scan_buf->pp_buf[scan_buf->pos] = d;
                scan_buf->pos++;
        }
        char string[4096];
        int i=0;
        volatile char *video = (volatile char*)0xFFFFFFFF800B8F90;
        *video++ = ' ';
        *video++ = 7;
        *video++ = ' ';
        *video++ = 7;
        video = video -4;
        if (d == '\n')
        {
                string[0] = '\\';
               string[1] = 'n';
                string[2] = '\0';
                i = 3;
        }
        else if(d == '\b')
        {
                string[0] =  '\\';
                string[1] = 'b';
                string[2] = '\0';
                i = 3;
        }
        else if(d == '\t')
        {
                string[0] = '\\';
                string[1] = 't';
                string[2] = '\0';
                i = 3;
        }
        else
        {
                string[0] = d;
                string[1] = '\0';
                i = 2;
        }
        video = (volatile char*)0xFFFFFFFF800B8F90;

        i=0;
        while( string[i] != '\0' )
        {
                *video++ = string[i];
                i++;
                *video++ = 7;
        }

}

