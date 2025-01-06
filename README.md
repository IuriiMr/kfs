# Bare Metal Kernel Project

This project demonstrates a simple bare metal kernel built for the x86 architecture. It includes functionality to display messages on the screen, move the cursor, and handle keyboard input. The kernel is designed to run directly on hardware or in a virtual machine (e.g., QEMU).

## File Descriptions

### 1. `Makefile`
The `Makefile` automates the compilation and linking process of the kernel. Key targets include:
- `kernel.bin`: The compiled and linked kernel binary.
- `boot.img`: A bootable disk image that includes the kernel.
- `run`: A target to boot the kernel in QEMU.

### 2. `link.ld`
The linker script specifies the memory layout of the kernel, ensuring correct placement of the text, data, and bss sections.

### 3. `kernel.c`
This is the main kernel source file written in C. It includes:
- Basic VGA text output using the video memory at `0xB8000`.
- Functions to move the cursor programmatically.
- Keyboard input handling to display typed characters and react to shift and caps lock states.

Key features:
- Displays a welcome message in green.
- Handles basic keyboard input, including shift and caps lock functionality for uppercase letters and symbols.
- Scrolls the screen when the cursor reaches the bottom.

### 4. `boot.asm`
This assembly file is the bootloader, which:
- Complies with the Multiboot specification.
- Sets up the stack and jumps to the kernel's entry point (`kernel_main`).

### 5. `grub.cfg`
This configuration file is used by GRUB to boot the kernel. It specifies the timeout, default entry, and the path to the kernel binary.

## **Helper Files**

### **1. `types.h`**
Defines custom data types for better portability and readability.

- **Key Features:**
  - Explicit integer sizes like `uint8_t`, `int32_t`.
  - Boolean type `bool` with `true` and `false` values.
  - Definition of `size_t` for memory-related operations.

- **Custom `va_list` Implementation**

   Since this is a bare-metal kernel, we cannot rely on the standard library. Instead, we provide a custom implementation of the `va_list` functionality, which is essential for variadic functions like `printk`.

   - Components of the Custom `va_list`:

   1. **`va_list`**:  
   A pointer to the start of the variable arguments in memory.  

   2. **`va_start`**:  
      A macro that initializes the `va_list` pointer to point to the first argument after the last fixed parameter.  

   3. **`va_arg`**:  
      A macro that retrieves the next argument from the `va_list` based on the specified type. It advances the pointer to the next argument in memory.  

   4. **`va_end`**:  
      A macro to clean up the `va_list` by resetting the pointer to `NULL`.  

   5. **`VA_SIZE(type)`**:  
      A helper macro that ensures proper alignment of the arguments in memory (aligned to `uint32_t` boundaries).  

- **Read More:**
  - [Fixed-Width Integer Types in C](https://en.wikipedia.org/wiki/C_data_types#Fixed-width_integer_types)

---

### **2. `utils.c`**
Provides utility functions for memory and string manipulation.

- **Functions:**
  - `memset`: Sets a block of memory to a specific value.
  - `memcpy`: Copies data from one memory location to another.
  - `strlen`: Calculates the length of a null-terminated string.
- **Formatted Output**
  - `printk`: A lightweight `printf`-like function for debugging and displaying formatted messages.

- **Using `printk`**

   - The `printk` function supports the following format specifiers:
      - `%d`: Print an integer in decimal.
      - `%x`: Print an integer in hexadecimal.
      - `%c`: Print a single character.
      - `%s`: Print a null-terminated string.

   Example:
   ```c
   printk("Booting kernel... Version: %d.%d\n", 1, 0);
   printk("Memory location: 0x%x\n", 0xDEADBEEF);
   ```
- **Read More:**
  - [OSDev: Memory Manipulation](https://wiki.osdev.org/Memory_Manipulation)

---


## Building and Running

### Prerequisites
Ensure the following tools are installed:
- GCC with multilib support (for 32-bit compilation).
- `ld` (linker).
- QEMU (for testing).

### Steps
1. Build the kernel, boot image and run itin QEMU:
   ```sh
   make
   ```
2. Clean all files and boot image:
   ```sh
   make clean
   ```

## How It Works
1. **Bootloader**: The `boot.asm` file initializes the system and jumps to the `kernel_main` function in `kernel.c`.
2. **Kernel Execution**:
   - Displays a welcome message using VGA text mode (add colors).
   - Handles keyboard input and updates the screen dynamically.
   - Moves the cursor and scrolls the screen as needed.
3. **Input Handling**:
   - Detects and processes scancodes from the keyboard.
   - Supports shift and caps lock for uppercase letters and symbols.
   - Updates the screen in real-time with typed characters.

## Key Points to Learn More
1. **Multiboot Specification**:
   - [GNU GRUB Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)
2. **VGA Text Mode**:
   - [OSDev Wiki: Text Mode](https://wiki.osdev.org/Text_Mode)
3. **Keyboard Input**:
   - [OSDev Wiki: PS/2 Keyboard](https://wiki.osdev.org/PS/2_Keyboard)
4. **Bare Metal Programming**:
   - [OSDev Wiki: Bare Bones](https://wiki.osdev.org/Bare_Bones)

## Enhancements
- Add more advanced keyboard functionality (e.g., support for function keys).
- Implement kernel types.

