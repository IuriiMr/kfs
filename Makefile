
NASMC = nasm
CC = gcc
LINKER = ld

# Flags
NASMFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -fno-pic -nostdlib -nodefaultlibs
LINKERFLAGS = -m elf_i386 --oformat binary -static

# Target definitions
TARGET_BIN = kernel.bin
LINKER_SCRIPT = link.ld
OBJECTS = boot.o kernel.o

all: $(TARGET_BIN)

%.o: %.asm
	$(NASMC) $< -o $@ $(NASMFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(TARGET_BIN): $(OBJECTS)
	$(LINKER) $(LINKERFLAGS) -T $(LINKER_SCRIPT) -o $@ $(OBJECTS)

# Check binary as multiboot
	grub-file --is-x86-multiboot $(TARGET_BIN)

# Copy grub.cfg and kernel.bin to the img file
	sudo losetup -Pf boot.img
	sudo mount /dev/loop0p1 mnt
	sudo cp grub.cfg mnt/boot/grub/
	sudo cp kernel.bin mnt/boot/
	sudo umount mnt

	qemu-system-i386 -hda boot.img
	

clean:
	rm -f $(OBJECTS) $(TARGET_BIN)

