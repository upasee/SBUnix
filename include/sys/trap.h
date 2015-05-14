#ifndef _TRAP_H
#define _TRAP_H

#include <sys/defs.h>
#include <stdlib.h>
#include <sys/elf.h>
#include <sys/tarfs.h>
#include <sys/process.h>

#define PUSHA \
        "\tpushq %%rax\n" \
        "\tpushq %%rbx\n" \
        "\tpushq %%rcx\n" \
        "\tpushq %%rdx\n" \
        "\tpushq %%rbp\n" \
        "\tpushq %%rdi\n" \
        "\tpushq %%rsi\n" \
        "\tpushq %%r8 \n" \
        "\tpushq %%r9 \n" \
        "\tpushq %%r10\n" \
        "\tpushq %%r11\n" \
        "\tpushq %%r12\n" \
        "\tpushq %%r13\n" \
        "\tpushq %%r14\n" \
        "\tpushq %%r15\n"

#define POPA \
        "\tpopq %%r15\n" \
        "\tpopq %%r14\n" \
        "\tpopq %%r13\n" \
        "\tpopq %%r12\n" \
        "\tpopq %%r11\n" \
        "\tpopq %%r10\n" \
        "\tpopq %%r9 \n" \
        "\tpopq %%r8 \n" \
        "\tpopq %%rsi\n" \
        "\tpopq %%rdi\n" \
        "\tpopq %%rbp\n" \
        "\tpopq %%rdx\n" \
        "\tpopq %%rcx\n" \
        "\tpopq %%rbx\n" \
        "\tpopq %%rax\n"

/* DO NOT TOUCH! Like EVER! */
/*
struct UTrapframe {
	uint64_t reg_r15;
        uint64_t reg_r14;
        uint64_t reg_r13;
        uint64_t reg_r12;
        uint64_t reg_r11;
        uint64_t reg_r10;
        uint64_t reg_r9;
        uint64_t reg_r8;
        uint64_t reg_rsi;
        uint64_t reg_rdi;
        uint64_t reg_rbp;
        uint64_t reg_rdx;
        uint64_t reg_rcx;
        uint64_t reg_rbx;
        uint64_t reg_rax;
	uint64_t utf_rip;
        uint64_t utf_cs;
        uint64_t utf_flags;
        uint64_t utf_rsp;
        uint64_t utf_ds;
} __attribute__((packed));
*/
/*
struct FTrapFrame {
	struct UTrapFrame tf;
	uint64_t *fault_addr;
}
*/
void trap_frame_init(struct task_struct *task, uint64_t stack, uint64_t elf_addr);
void copy_trap_frame(uint64_t stack,struct UTrapframe *tf);
void switch_to_user(uint64_t stack);
void set_trap_frame(struct task_struct *task);
void copy_trapframe_parentToChild(struct task_struct *parent, struct task_struct *child);
void print_trapframe(struct UTrapframe *tf);
#endif
