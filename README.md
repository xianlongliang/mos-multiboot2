MOS
======

a x64 os kernel. mos version2 using multiboot2.

implementations :

- [x] stack trace debug
- [x] printk
- [x] GDT
- [x] IDT
- [x] TSS
- [x] keyboard (from os-elephant)
- [x] slab
- [x] kmalloc (based on slab)
- [x] c++ new and delete (via kmalloc)
- [x] buddy system
- [x] mutex
- [x] spinlock
- [x] semaphore
- [x] condition variable
- [x] task
- [x] task sleep and wake
- [x] userland task
- [x] pagefault handler
- [x] task context switch (kernel and userland)
- [x] round robin scheduler ???
- [x] syscall and sysret
- [x] ide driver
- [x] some stl c++ containers
- [x] Shell
- [x] APIC
todos:

- [ ] VFS
- [ ] SFS Filesystem
- [ ] IO-APIC
- [ ] SMP

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

