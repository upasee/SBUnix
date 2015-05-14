#include <sys/sbunix.h>
#include <sys/trap.h>

#define IF 0x00000200

uint64_t read_eflags(void)
{
        uint64_t rflags;
        __asm __volatile("pushfq; popq %0" : "=r" (rflags));
        return rflags;
}

void copy_trapframe_parentToChild(struct task_struct *parent, struct task_struct *child)
{
	child->tf.reg_r15 = parent->tf.reg_r15;
	child->tf.reg_r14 = parent->tf.reg_r14;
	child->tf.reg_r13 = parent->tf.reg_r13;
	child->tf.reg_r12 = parent->tf.reg_r12;
	child->tf.reg_r11 = parent->tf.reg_r11;
	child->tf.reg_r10 = parent->tf.reg_r10;
	child->tf.reg_r9 = parent->tf.reg_r9;
	child->tf.reg_r8 = parent->tf.reg_r8;
	child->tf.reg_rsi = parent->tf.reg_rsi;
	child->tf.reg_rdi = parent->tf.reg_rdi;
	child->tf.reg_rbp = parent->tf.reg_rbp;
	child->tf.reg_rdx = parent->tf.reg_rdx;
	child->tf.reg_rcx = parent->tf.reg_rcx;
	child->tf.reg_rbx = parent->tf.reg_rbx;
	child->tf.reg_rax = parent->tf.reg_rax;
	child->tf.utf_rip = parent->tf.utf_rip;
	child->tf.utf_cs = parent->tf.utf_cs;
	child->tf.utf_flags = parent->tf.utf_flags;
	child->tf.utf_rsp = parent->tf.utf_rsp;
	child->tf.utf_ds = parent->tf.utf_ds;
}

void trap_frame_init(struct task_struct *task, uint64_t stack, uint64_t elf_addr)
{
        struct UTrapframe *tf = (struct UTrapframe *)&task->tf;
        __asm __volatile(       "movq %%rsp, %%rbx \n\t"
                                "movq %0,%%rsp\n\t"
                                "addq $0x78,%%rsp\n\t"
                                 PUSHA
                                "movq %%rbx, %%rsp \n\t"
                                :
                                :"r"(tf)
                                :"rbx","cc","memory"
                        );
        task->tf.utf_ds = 0x23;
        task->tf.utf_rsp = stack;
        task->tf.utf_flags = read_eflags() | IF;
        task->tf.utf_cs = 0x1b;
        task->tf.utf_rip = elf_addr;
	task->tf.reg_r15 = 0;
        task->tf.reg_r14 = 0;
        task->tf.reg_r13 = 0;
        task->tf.reg_r12 = 0;
        task->tf.reg_r11 = 0;
        task->tf.reg_r10 = 0;
        task->tf.reg_r9 = 0;
        task->tf.reg_r8 = 0;
        task->tf.reg_rsi = 0;
        task->tf.reg_rdi = 0;
        task->tf.reg_rbp = 0;
        task->tf.reg_rdx = 0;
        task->tf.reg_rcx = 0;
        task->tf.reg_rbx = 0;
        task->tf.reg_rax = 0;
}

void copy_trap_frame(uint64_t stack,struct UTrapframe *tf)
{
	__asm __volatile(       "movq %%rsp,%%rbx\n"
				"movq    %0,%%rsp\n"
				"movq    %1,%%rcx\n"
				"pushq 152(%%rcx)\n"
				"pushq 144(%%rcx)\n"
				"pushq 136(%%rcx)\n"
				"pushq 128(%%rcx)\n"
				"pushq 120(%%rcx)\n"
				"pushq 112(%%rcx)\n"
				"pushq 104(%%rcx)\n"
				"pushq  96(%%rcx)\n"
				"pushq  88(%%rcx)\n"
				"pushq  80(%%rcx)\n"
				"pushq  72(%%rcx)\n"
				"pushq  64(%%rcx)\n"
				"pushq  56(%%rcx)\n"
				"pushq  48(%%rcx)\n"
				"pushq  40(%%rcx)\n"
				"pushq  32(%%rcx)\n"
				"pushq  24(%%rcx)\n"
				"pushq  16(%%rcx)\n"
				"pushq   8(%%rcx)\n"
				"pushq    (%%rcx)\n"
				"movq %%rbx,%%rsp\n"
			:
			:"r"(stack),"r"(tf)
			:"cc","memory","rcx","rbx"
			);

}

void switch_to_user(uint64_t stack)
{
        __asm __volatile(	"movq %0,%%rsp\n"
				"subq $0xA0, %%rsp\n"
				 POPA
				"movw 32(%%rsp),%%gs\n"
				"movw 32(%%rsp),%%es\n"
				"movw 32(%%rsp),%%fs\n"
				"movw 32(%%rsp),%%ds\n"
				"\tiretq \n"
				: 
				: "r" (stack) 
				: "memory"
			);
}

void set_trap_frame(struct task_struct *task)
{
	uint64_t stack = (uint64_t)task->kstack;
        struct UTrapframe *tf = (struct UTrapframe *)&task->tf;
	__asm __volatile(       "movq %%rsp,%%rbx\n"
                                "movq    %0,%%rsp\n"
				"subq $0xA8,%%rsp\n"
                                "movq    %1,%%rax\n"
                                "popq    0(%%rax)\n"
                                "popq    8(%%rax)\n"
                                "popq   16(%%rax)\n"
                                "popq   24(%%rax)\n"
                                "popq   32(%%rax)\n"
                                "popq   40(%%rax)\n"
                                "popq   48(%%rax)\n"
                                "popq   56(%%rax)\n"
                                "popq   64(%%rax)\n"
                                "popq   72(%%rax)\n"
                                "popq   80(%%rax)\n"
                                "popq   88(%%rax)\n"
                                "popq   96(%%rax)\n"
                                "popq  104(%%rax)\n"
                                "popq  112(%%rax)\n"
                                "popq  120(%%rax)\n"
                                "popq  128(%%rax)\n"
                                "popq  136(%%rax)\n"
                                "popq  144(%%rax)\n"
                                "popq  152(%%rax)\n"
                                "movq %%rbx,%%rsp\n"
                        :
                        :"r"(stack),"r"(tf)
                        :"cc","memory","rax","rbx"
                        );

        task->tf.utf_ds = 0x23;
        task->tf.utf_flags |= IF;
        task->tf.utf_cs = 0x1b;
}

void print_trapframe(struct UTrapframe *tf)
{
	kprintf("  r15   0x%x\n", tf->reg_r15);
        kprintf("  r14   0x%x\n", tf->reg_r14);
        kprintf("  r13   0x%x\n", tf->reg_r13);
        kprintf("  r12   0x%x\n", tf->reg_r12);
        kprintf("  r11   0x%x\n", tf->reg_r11);
        kprintf("  r10   0x%x\n", tf->reg_r10);
        kprintf("  r9    0x%x\n", tf->reg_r9);
        kprintf("  r8    0x%x\n", tf->reg_r8);
        kprintf("  rdi   0x%x\n", tf->reg_rdi);
        kprintf("  rsi   0x%x\n", tf->reg_rsi);
        kprintf("  rbp   0x%x\n", tf->reg_rbp);
        kprintf("  rbx   0x%x\n", tf->reg_rbx);
        kprintf("  rdx   0x%x\n", tf->reg_rdx);
        kprintf("  rcx   0x%x\n", tf->reg_rcx);
        kprintf("  rbx   0x%x\n", tf->reg_rbx);
        kprintf("  rax   0x%x\n", tf->reg_rax);
        kprintf("  ds    0x%x\n", tf->utf_ds);
        kprintf("  rsp   0x%x\n", tf->utf_rsp);
        kprintf("  flags 0x%x\n", tf->utf_flags);
        kprintf("  cs    0x%x\n", tf->utf_cs);
        kprintf("  rip   0x%x\n", tf->utf_rip);

}

