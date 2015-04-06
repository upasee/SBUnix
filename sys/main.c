#include <sys/sbunix.h>
#include <sys/gdt.h>
#include <sys/tarfs.h>
#include <sys/idt.h>
#include <sys/pic.h>
#include <sys/port.h>
#include <sys/pic.h>
#include <string.h>
#include <sys/paging.h>

void start(uint32_t* modulep, void* physbase, void* physfree)
{
	interrupt_init();
        reload_idt();
	
	IRQ_clear_mask(0);
	int div = 1193;
	outb(0x43,0x36);
	outb(0x40, div & 0xff);
	outb(0x43,div >> 8);
	interrupt_enable();

	IRQ_clear_mask(1);

	while(modulep[0] != 0x9001) 
		modulep += modulep[1]+2;


	// map memory for first level of the page table
	uint64_t *pml4=pml4_init(physfree);

	// Mark all the pages as used
	setPageVal();

	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) 
	{
		if (smap->type == 1 && smap->length != 0) 
		{
			// Mark all available pages as free
			markAvailablePages(smap);
			printf("Available Physical Memory [%x-%x]\n", smap->base, smap->base + smap->length);
		}
	}

	// Mark all the pages used by the kernel as used
	markKernelUsed();

	// Create a linked list of all free pages in physical memory
	createPageFreeList();

	size_t nPages = (kern_pages(physbase,physfree))/0x1000;

	// Load the kernel from kernel lower address to kernel higher address
	vm_setup(pml4, nPages);
	
	void *cr3val = physicalAddr((void *)pml4);
	
	// Load cr3
	__asm __volatile(	"movq %0,%%cr3" 
			 	: 
				: "r" (cr3val)
				: "memory","cc"
			);

	printf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
	while(1);
	// kernel starts here*/
}

#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;
struct tss_t tss;

void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
	register char *s, *v;
	__asm__(
		"movq %%rsp, %0;"
		"movq %1, %%rsp;"
		:"=g"(loader_stack)
		:"r"(&stack[INITIAL_STACK_SIZE])
	);
	
	PIC_remap(0x20,0x28);
	reload_gdt();
	setup_tss();
	PIC_remap(0x20,0x28);
	start(
		(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
		&physbase,
		(void*)(uint64_t)loader_stack[4]
	);
	s = "!!!!! start() returned !!!!!";
	for(v = (char*)0xb8000; *s; ++s, v += 2) *v = *s;
	while(1);
}
