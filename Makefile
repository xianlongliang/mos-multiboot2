all:
	ld -r -b binary -o u_vga16.o u_vga16.sfn
	mkdir -p build
	cd build && cmake .. && make 

dump: 
	objdump -S build/kernel > build/kernel.dump
	# objdump -S build/user > build/user.dump

qemu:
	qemu-system-x86_64 -smp 4 --bios OVMF.fd -net none -monitor stdio -drive file=build/kernel.iso,index=0,media=disk,format=raw -m 8192M -s -S \
	-drive file=hd80.img,if=none,id=disk \
	-device ich9-ahci,id=ahci \
	-device ide-hd,drive=disk,bus=ahci.0 \

qemu-gdb:
	nohup qemu-system-x86_64 -cdrom build/kernel.iso -serial stdio -m 64M -s -S & 
	gdb ./build/kernel --x ./gdbstart

bochs: 
	bochs -qf bochsrc
	
clean:
	rm -rf build
