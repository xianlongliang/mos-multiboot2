all:
	mkdir -p build
	cd build && cmake .. && make 

dump: 
	objdump -S build/kernel > build/kernel.dump

qemu:
	qemu-system-x86_64 -cdrom build/kernel.iso -serial stdio -m 1024M

bochs: 
	bochs -qf bochsrc
	
clean:
	rm -rf build
