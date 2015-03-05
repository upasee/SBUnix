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

//void t_divide(void);
extern void t_kdb(void);
extern void t_timer(void);

void setidt(struct segment_gate_descriptor *gate, void *offset,int n) {
	gate[n].gd_offset1 = (uint64_t)(offset) & 0xffff;
	gate[n].gd_css = 0x8;
	gate[n].gd_ist = 0;
	gate[n].gd_xx1 = 0;
	gate[n].gd_type = 0xe;
	gate[n].gd_zero = 0;
	gate[n].gd_dpl = 0;
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
//	setidt(idt,t_divide,0);
	setidt(idt,t_timer,MASTER_OFFSET);
	setidt(idt,t_kdb,MASTER_OFFSET+1);
}

void interrupt_enable()
{
	__asm("sti");
}
