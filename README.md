MOS
======

a x64 os kernel. mos version2 using multiboot2.

implementations :

- [x] stack trace debug
- [x] keyboard (from os-elephant)
- [x] slab
- [x] kmalloc (based on lrmalloc<sup>1</sup>)
- [x] buddy system
- [x] mutex
- [x] spinlock
- [x] semaphore
- [x] condition variable
- [x] kernel userland task 
- [x] pagefault handler
- [x] task context switch (kernel and userland)
- [x] round robin scheduler ???
- [x] syscall and sysret
- [x] ide driver (not commit yet)
- [x] some stl c++ containers
- [x] Shell
- [x] APIC
- [x] IO-APIC
- [x] SMP

todos:

- [ ] VFS
- [ ] SFS Filesystem

## Building

make sure nasm, g++ is installed.
then run make 
```bash
$ make
```

## Running MOS
both qemu and bochs are supported.

### QEMU
vscode configuration file is ready for use. 

just run the script
```bash
$ make qemu
```
and run kernel-debug in vscode.

### Bochs
make sure bochs is compiled properly.
check out the bochsrc file

```bash
$ make bochs
```

## dump kernel elf
you can easily dump the kernel binary file using:
```bash
$ make dump
```

[1] Leite, Ricardo and Ricardo Rocha. “LRMalloc: A Modern and Competitive Lock-Free Dynamic Memory Allocator.” VECPAR (2018).