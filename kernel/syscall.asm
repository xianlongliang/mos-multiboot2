section .text
bits 64
global syscall_entry
syscall_entry:
    swappgs
	