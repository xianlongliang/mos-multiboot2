section .text
bits 64
global ret_syscall
ret_syscall:
    mov [rsp + 0x80], rax
    pop	r15				 
	pop	r14				 	
	pop	r13				 	
	pop	r12				 	
	pop	r11				 	
	pop	r10				 	
	pop	r9				 	
	pop	r8				 	
	pop	rbx				 	
	pop	rcx				 	
	pop	rdx				 	
	pop	rsi				 	
	pop	rdi				 	
	pop	rbp				 	
	pop	rax				 	
	mov	ds, ax		 
	pop	rax				 
	mov	es, ax		 
	pop	rax				 
	add	rsp, 0x38
    pushf
    pop r11
    and r11, 0x3C7FD7
    or r11, 0x2
	
    o64 sysret
	
section .text
bits 64
extern do_exit
global kernel_thread_func
kernel_thread_func:
    pop	r15				 
	pop	r14				 	
	pop	r13				 	
	pop	r12				 	
	pop	r11				 	
	pop	r10				 	
	pop	r9				 	
	pop	r8
	pop	rbx				 	
	pop	rcx				 	
	pop	rdx				 	
	pop	rsi				 	
	pop	rdi	
	pop	rbp				 	
	pop	rax				 	
	mov	ds, ax		 
	pop	rax				 
	mov	es, ax		 
	pop	rax		
	add rsp, 0x38
	mov rdi, rdx
	call rbx
	mov rdi, rax
	call do_exit
