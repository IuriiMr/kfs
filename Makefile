
NASMC = nasm
CC = gcc
LINKER = ld

# Flags
NASMFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -fno-pic -std=gnu99 -fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs -Wall -Wextra
LINKERFLAGS = -m elf_i386 

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



# Variables
IMG = boot.img
IMG_SIZE_MB = 20
MOUNT_POINT = mnt


# Target: Create and partition the image
$(IMG): 
	@echo "Creating a $(IMG_SIZE_MB)MB image..."
	dd if=/dev/zero of=$(IMG) bs=1M count=$(IMG_SIZE_MB)
	@echo "Partitioning the image with fdisk..."
	printf "o\nn\np\n1\n\n\nt\n83\na\nw\n" | fdisk $(IMG)
	@echo "Image created and partitioned successfully."

# Target: Attach loop device
.PHONY: attach
attach: $(IMG)
	@echo "Attaching loop device..." 
	sudo losetup -Pf --show $(IMG)

# Target: Create filesystem
.PHONY: format
format: attach
	@echo "Formatting partition as ext2..."
	sudo mkfs.ext2 -b 1024 /dev/loop0p1
	sudo tune2fs -m 0 /dev/loop0p1
	@echo "Filesystem created successfully."

# Target: Mount the partition
.PHONY: mount
mount: format
	@echo "Mounting partition to $(MOUNT_POINT)..."
	@mkdir -p $(MOUNT_POINT)
	@sudo mount /dev/loop0p1 $(MOUNT_POINT)
	@echo "Partition mounted at $(MOUNT_POINT)."

grub: mount
	sudo grub-install --target=i386-pc --boot-directory=mnt/boot \
  --modules="normal part_msdos ext2 multiboot" --no-floppy /dev/loop0

	#sudo grub-install --target=i386-pc --boot-directory=mnt/boot /dev/loop0


# Copy grub.cfg and kernel.bin to the img file
copy: grub
	sudo cp grub.cfg mnt/boot/grub/
	sudo cp kernel.bin mnt/boot/

run: copy
	@sudo umount $(MOUNT_POINT) || true
	qemu-system-i386 -hda boot.img


# Target: Clean up
.PHONY: clean
clean:
	@echo "Cleaning up..."
	#@sudo umount $(MOUNT_POINT) || true
	sudo losetup -d /dev/loop0 || true
	@sudo rm -f $(IMG)
	@rm -rf $(MOUNT_POINT)
	@echo "Clean-up complete."

# # Combine tasks: Create, format, and mount
# .PHONY: all
# all: mount
# 	@echo "Image creation, partitioning, formatting, and mounting completed successfully."



# Create .iso image
iso: $(TARGET_BIN)
	@mkdir -p iso/boot/grub
	@cp kernel.bin iso/boot/kernel.bin
	@cp grub.cfg iso/boot/grub/grub.cfg
	@grub-mkrescue -o kfs.iso iso
	@qemu-system-i386 -cdrom kfs.iso


# clean:
# 	rm -f $(OBJECTS) $(TARGET_BIN)
# 	rm -rf iso
# 	rm kfs.iso

