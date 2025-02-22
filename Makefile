
NASMC = nasm
CC = gcc
LINKER = ld

# Flags
NASMFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs -Wall -Wextra
LINKERFLAGS = -m elf_i386

# Target definitions
TARGET_BIN = kernel.bin
LINKER_SCRIPT = link.ld
ASM = boot.asm
SOURCES = kernel.c utils.c keyboard.c
OBJECTS = $(SOURCES:.c=.o) $(ASM:.asm=.o)

# Variables for the disk image
IMG = boot.img
IMG_SIZE_MB = 9
MOUNT_POINT = mnt
LOOPFILE = loopdev.txt

.PHONY: all
all: run

%.o: %.asm
	$(NASMC) $< -o $@ $(NASMFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(TARGET_BIN): $(OBJECTS)
	$(LINKER) $(LINKERFLAGS) -T $(LINKER_SCRIPT) -o $@ $(OBJECTS)

# Check binary as multiboot
	grub-file --is-x86-multiboot $(TARGET_BIN)

# Target: Create and partition the image
$(IMG): $(TARGET_BIN)
	@echo "Creating a $(IMG_SIZE_MB)MB image..."
	@dd if=/dev/zero of=$(IMG) bs=1M count=$(IMG_SIZE_MB)
	@echo "Partitioning the image with fdisk..."
	@printf "o\nn\np\n1\n\n\nt\n83\na\nw\n" | fdisk $(IMG)
	@echo "Image created and partitioned successfully."

# Target: Attach loop device
.PHONY: attach
attach: $(IMG)
	@echo "Attaching loop device..."
	@LOOPDEV=`sudo losetup -Pf --show $(IMG)` && \
	echo $$LOOPDEV > $(LOOPFILE) && \
	echo "Loop device attached: $$LOOPDEV"

# Target: Create filesystem
.PHONY: format
format: attach
	@echo "Formatting partition as ext2..."
	@LOOPDEV=`cat $(LOOPFILE)`; \
	sudo mkfs.ext2 -b 1024 $$LOOPDEV"p1" && \
	sudo tune2fs -m 0 $$LOOPDEV"p1" && \
	echo "Filesystem created successfully."

# Target: Mount the partition
.PHONY: mount
mount: format
	@echo "Mounting partition to $(MOUNT_POINT)..."
	@mkdir -p $(MOUNT_POINT)
	@LOOPDEV=`cat $(LOOPFILE)`; \
	sudo mount $$LOOPDEV"p1" $(MOUNT_POINT) && \
	echo "Partition mounted at $(MOUNT_POINT)."

# Target: Install grub in minimum config and remove fonts and themes to fit in 10M image
.PHONY: grub
grub: mount
	@echo "Installing GRUB to $(IMG)..."
	@sudo grub-install --target=i386-pc --boot-directory=$(MOUNT_POINT)/boot \
		--install-modules="normal linux ext2 part_msdos biosdisk multiboot configfile" \
		--locales="" --no-floppy `cat $(LOOPFILE)`
	@sudo rm -rf $(MOUNT_POINT)/boot/grub/themes
	@sudo rm -f $(MOUNT_POINT)/boot/grub/fonts/*.pf2

# Copy grub.cfg and kernel.bin to the img file
.PHONY: copy
copy: grub
	@echo "Copying files to $(IMG)..."
	@sudo cp grub.cfg $(MOUNT_POINT)/boot/grub/
	@sudo cp kernel.bin $(MOUNT_POINT)/boot/

# Run KVM with the disk image
.PHONY: run
run: copy
	@echo "Running KVM..."
	@sudo umount $(MOUNT_POINT) || true
	@qemu-system-i386 -hda $(IMG) 2>/dev/null

# Target: Clean up
.PHONY: clean
clean:
	@echo "Cleaning up..."
	@sudo losetup -d `cat $(LOOPFILE)` || true
	@rm -rf $(MOUNT_POINT)
	@rm -f $(OBJECTS) $(TARGET_BIN) $(IMG) $(LOOPFILE)
	@echo "Clean-up complete."

# Create .iso image (simple test, more than 10M image)
.PHONY: iso
iso: $(TARGET_BIN)
	@mkdir -p iso/boot/grub
	@cp kernel.bin iso/boot/kernel.bin
	@cp grub.cfg iso/boot/grub/grub.cfg
	@grub-mkrescue -o kfs.iso iso
	@qemu-system-i386 -cdrom kfs.iso

# Clean for .iso version
.PHONY: iso-clean
iso-clean:
	rm -f $(OBJECTS) $(TARGET_BIN)
	rm -rf iso
	rm kfs.iso
