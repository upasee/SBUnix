# Interrupt wrapper

.global t_timer
.global t_stack
.global t_align
.global t_seg
.global t_divide
.global t_kdb
.global t_gpf
.global t_pf
t_timer :
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

	call interrupt_handler

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

	iretq

t_kdb :
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

        call interrupt_handler1

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

        iretq

t_pf :
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

        call temp_handler

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

        //iretq

t_gpf :
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

        call temp_handler

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

        //iretq

t_align :
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

        call temp_handler

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

//        iretq

t_stack :
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

        call temp_handler

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

       // iretq

t_seg :
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

        call temp_handler

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

        iretq


t_divide :
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

        call interrupt_handler2

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

        iretq

