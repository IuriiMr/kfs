
NASMC = nasm
NASMFLAGS = -f elf32
CC = gcc
CFLAGS = -m32 -ffreestanding -g -O0 
#-O0 -g3 -ffreestanding -fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs -Wfatal-errors
LINKERFLAGS = -m32 -ffreestanding -nostdlib -nodefaultlibs -lgcc

# Target definitions
TARGET_BIN = kernel.bin
TARGET_ELF = kernel.elf
LINKER_SCRIPT = link.ld
OBJECTS = boot.o kernel.o

all: $(TARGET_BIN)

%.o: %.asm
	nasm -f elf32 -o boot.o boot.asm
	#(NASMC) $< -o $@ $(NASMFLAGS)

%.o: %.c
	gcc -m32 -ffreestanding -fno-pic -c kernel.c -o kernel.o

	#gcc -m32 -ffreestanding -c kernel.c -o kernel.o
	#$(CC) -c $< -o $@ -Iinclude $(CFLAGS)

$(TARGET_BIN): $(OBJECTS) $(LINKER_SCRIPT)
	#gcc -T link.ld -o kernel.elf boot.o kernel.o -m32 -ffreestanding -nostdlib -nodefaultlibs -lgcc
	#ld -T link.ld -o kernel.bin boot.o kernel.o --oformat binary

	#ld -T link.ld -o kernel.bin boot.o kernel.o --oformat binary -m elf_i386
	#ld -T link.ld -o kernel.bin boot.o kernel.o --oformat binary -m elf_i386 -static



	ld -m elf_i386 -T link.ld -o kernel.bin boot.o kernel.o
	#objcopy -O binary kernel.elf kernel.bin	
	


#$(CC) -T $(LINKER_SCRIPT) -o $(TARGET_BIN) $(OBJECTS) $(LINKERFLAGS) -Wl,--oformat=binary
	#grub-file --is-x86-multiboot $(TARGET_ELF)
	grub-file --is-x86-multiboot $(TARGET_BIN)


	sudo losetup -Pf boot.img
	sudo mount /dev/loop0p1 mnt
	sudo cp grub.cfg mnt/boot/grub/
	sudo cp kernel.bin mnt/boot/
	#sudo cp kernel.elf mnt/boot/
	sudo umount mnt

	qemu-system-i386 -hda boot.img
	#qemu-system-i386 -kernel kernel.elf


clean:
	rm -f $(OBJECTS) $(TARGET_BIN) $(TARGET_ELF)

