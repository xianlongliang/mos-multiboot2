#pragma once
#include <std/stdint.h>

struct Regs
{
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;

	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rbp;
	uint64_t rax;

	uint64_t cs;
	uint64_t rip;
	uint64_t rflags;
	uint64_t rsp;
};

inline void save_all_general_registers()
{
	asm volatile(
		"pushq	%%r15	\n\t"
		"pushq	%%r14	\n\t"
		"pushq	%%r13	\n\t"
		"pushq	%%r12	\n\t"
		"pushq	%%r11	\n\t"
		"pushq	%%r10	\n\t"
		"pushq	%%r9	\n\t"
		"pushq	%%r8	\n\t"

		"pushq	%%rbx	\n\t"
		"pushq	%%rcx	\n\t"
		"pushq	%%rdx	\n\t"
		"pushq	%%rsi	\n\t"
		"pushq	%%rdi	\n\t"
		"pushq	%%rbp	\n\t"
		"pushq	%%rax	\n\t");
}

inline void restore_all_general_registers()
{
	asm volatile(
		"popq	%%rax	\n\t"
		"popq	%%rbp	\n\t"
		"popq	%%rdi	\n\t"
		"popq	%%rsi	\n\t"
		"popq	%%rdx	\n\t"
		"popq	%%rcx	\n\t"
		"popq	%%rbx	\n\t"

		"popq	%%r8	\n\t"
		"popq	%%r9	\n\t"
		"popq	%%r10	\n\t"
		"popq	%%r11	\n\t"
		"popq	%%r12	\n\t"
		"popq	%%r13	\n\t"
		"popq	%%r14	\n\t"
		"popq	%%r15	\n\t"
	);
}