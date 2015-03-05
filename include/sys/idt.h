#ifndef _IDT_H
#define _IDT_H

#include <sys/defs.h>

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */

#define MASTER_OFFSET	0x20		
#define IRQ_SLAVE       2       // IRQ at which slave connects to master

void reload_idt();
void interrupt_init();
void interrupt_enable();

void interrupt_handler();

//void t_divide(void);
//void t_kdb(void);
//void t_timer(void);

struct segment_gate_descriptor {
        uint64_t gd_offset1:16;         /* Segment offset (Lower 15-0 bits) */
        uint64_t gd_css:16;             /* Code segment selector */
        uint64_t gd_ist:3;              /* Arguments for int/trap gates */
        uint64_t gd_xx1:5;              /* Reserved */
        uint64_t gd_type:4;             /* Segment type */
        uint64_t gd_zero:1;             /* Must be zero */
        uint64_t gd_dpl:2;              /* Descriptor Privilege Level  */
        uint64_t gd_p:1;                /* Present bit */
        uint64_t gd_offset2:16;         /* Segment offset (16-31 bits) */
        uint64_t gd_offset3:32;         /* Segment offset (32-63 bits) */
        uint64_t gd_xx3:32;             /* Reserved */
}__attribute__((packed));

#endif
