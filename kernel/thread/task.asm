section .text
bits 64
global ret_syscall
ret_syscall:
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
			 
	add	rsp, 0x10

	pushf
	pop r11
	pop rsp

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

	add rsp, 0x10
	popf
	pop rsp
	call rbx

	call do_exit
