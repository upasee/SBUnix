# Interrupt wrapper

.global t_timer
.global t_stack
.global t_align
.global t_seg
.global t_syscall
.global t_divide
.global t_kbd
.global t_gpf
.global t_pf

.macro pusha
        pushq %rax
        pushq %rbx
        pushq %rcx
        pushq %rdx
        pushq %rbp
        pushq %rdi
        pushq %rsi
        pushq %r8
        pushq %r9
        pushq %r10
        pushq %r11
        pushq %r12
        pushq %r13
        pushq %r14
        pushq %r15
.endm

.macro popa
        popq %r15
        popq %r14
        popq %r13
        popq %r12
        popq %r11
        popq %r10
        popq %r9
        popq %r8
        popq %rsi
        popq %rdi
        popq %rbp
        popq %rdx
        popq %rcx
        popq %rbx
        popq %rax
.endm

t_timer :
	pusha
	call interrupt_handler
	popa
	iretq

t_syscall :
        pusha
        call syscall_handler
        retq

t_kbd :
	pusha
        call kbd_handler
	popa
        iretq

t_pf :
	pusha
        call pf_handler
	addq $0x8,%rsp
	popa
        iretq

t_gpf :
	pusha
        call temp_handler
	popa
        iretq

t_align :
	pusha
        call temp_handler
	popa
        iretq

t_stack :
	pusha
        call temp_handler
	popa
        iretq

t_seg :
	pusha
        call temp_handler
	popa
        iretq


t_divide :
	pusha
        call interrupt_handler2
	popa
        iretq

