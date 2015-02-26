#include <sys/idt.h>
#include <sys/sbunix.h>

#define MAX_IDT 256

struct segment_gate_descriptor idt[MAX_IDT] = { {0} };

struct idtr_t {
        uint16_t limit;
        uint64_t base;
}__attribute__((packed));

static struct idtr_t idtr = {
       	.limit = sizeof(idt),
        .base = (uint64_t)idt,
};

void t_divide(void)
{
	printf("It didn't come here");
	while(1);
}

void setidt(struct segment_gate_descriptor *gate, void *offset) {
	gate->gd_offset1 = (uint64_t)(offset) & 0xffff;
	gate->gd_css = 0x8;
	gate->gd_ist = 0;
	gate->gd_xx1 = 0;
	gate->gd_type = 0xe;
	gate->gd_zero = 0;
	gate->gd_dpl = 0;
	gate->gd_p = 1;
	gate->gd_offset2 = ((uint64_t) offset >> 16) & 0xffff;
	gate->gd_offset3 = ((uint64_t) offset >> 32) & 0xffffffff;
	gate->gd_xx3 = 0;
}

void _x86_64_asm_lidt(struct idtr_t* idtr)
{
	__asm __volatile("lidt (%0)" 
			 : 
			 : "r" (idtr)
			 :"cc","memory");
}

void reload_idt() {
	_x86_64_asm_lidt(&idtr);
}

void interrupt_init()
{
	setidt(idt,&t_divide);
//	setidt(idt[32],t_timer);
}


