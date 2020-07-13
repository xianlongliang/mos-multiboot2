all:
	mkdir -p build
	cd build && cmake .. && make 

dump: 
	objdump -S build/kernel > build/kernel.dump

qemu:
	nohup qemu-system-x86_64 -cdrom build/kernel.iso -serial stdio -m 1024M -s -S > /dev/null 2>&1 &

bochs: 
	bochs -qf bochsrc
	
clean:
	rm -rf build
