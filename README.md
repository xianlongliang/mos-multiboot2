MOS
======

a x64 os kernel. mos version2 using multiboot2.

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

