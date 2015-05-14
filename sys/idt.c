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

extern void t_divide(void);
extern void t_kbd(void);
extern void t_timer(void);
extern void t_pf(void);
extern void t_gpf(void);
extern void t_align(void);
extern void t_stack(void);
extern void t_seg(void);
extern void t_syscall(void);

void setidt(struct segment_gate_descriptor *gate, void *offset,int n, int dpl) {
	gate[n].gd_offset1 = (uint64_t)(offset) & 0xffff;
	gate[n].gd_css = 0x8;
	gate[n].gd_ist = 0;
	gate[n].gd_xx1 = 0;
	gate[n].gd_type = 0xe;
	gate[n].gd_zero = 0;
	gate[n].gd_dpl = dpl;
	gate[n].gd_p = 1;
	gate[n].gd_offset2 = ((uint64_t) offset >> 16) & 0xffff;
	gate[n].gd_offset3 = ((uint64_t) offset >> 32) & 0xffffffff;
	gate[n].gd_xx3 = 0;
}

void _x86_64_asm_lidt(struct idtr_t* idtr, uint64_t cs_idx, uint64_t ds_idx)
{
	
	__asm __volatile( "lidt (%0)" : : "r" (idtr));
}

void reload_idt() {
	_x86_64_asm_lidt(&idtr, 8, 16);
}

void interrupt_init()
{
	setidt(idt,t_divide,0,0);
	setidt(idt,t_timer,MASTER_OFFSET,0);
	setidt(idt,t_kbd,MASTER_OFFSET+1,0);
	setidt(idt,t_pf,14,0);
	setidt(idt,t_gpf,13,0);
	setidt(idt,t_align,17,0);
	setidt(idt,t_stack,12,0);
	setidt(idt,t_seg,11,0);
	setidt(idt,t_syscall,128,3);
}

void interrupt_enable()
{
	__asm("sti");
}
