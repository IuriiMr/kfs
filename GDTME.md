
## **1. What is the Global Descriptor Table (GDT)?**
The **GDT** is a table that defines memory segments in **protected mode**. It tells the CPU how to access memory and defines:
- **Base address** (where the segment starts in memory)
- **Limit** (maximum addressable memory inside the segment)
- **Access rights** (read/write/execute permissions)
- **Privilege level** (kernel mode or user mode)

In real mode, segmentation is used differently, but protected mode relies on **flat memory** with GDT managing access and permissions.

---

## **2. How is the GDT Created and Filled in the Kernel?**
The kernel defines the **GDT at memory location `0x800`** in the `.gdt` section using assembly and the linker script.

### **a. GDT Entries (Defined in `boot.asm`):**
The GDT has **six entries**, each **8 bytes** long (total size: `6 * 8 = 48` bytes or `0x30` in hexadecimal). The GDT entries include:

1. **Null Descriptor** *(Always present)*
2. **Kernel Code Segment** *(Ring 0, executable)*
3. **Kernel Data Segment** *(Ring 0, read/write)*
4. **User Code Segment** *(Ring 3, executable)*
5. **User Data Segment** *(Ring 3, read/write)*
6. **User Stack Segment** *(Ring 3, for user processes)*

Each entry is **64-bit (8 bytes)** and stored in **little-endian format**.

#### **GDT Format (64-bit entry structure)**
Each entry in the GDT is structured as follows:

```
Bits:      | 31........16 | 15......0  | 7.....4 | 3..0 |
--------------------------------------------------------
Part:      | Base (HIGH)  | Limit HIGH | Flags   | ATTR |
--------------------------------------------------------
Bits:      | 31........24 | 23......16 | 15......0 |
--------------------------------------------------
Part:      | Base (MID)   | ATTR       | Base LOW |
--------------------------------------------------
```

#### **b. How GDT Entries are Set**
Each descriptor is stored as a `dq` (8-byte entry) in `boot.asm`:

```assembly
gdt_start:
    dq 0x0000000000000000  ; Null descriptor

    ; Kernel Code Segment (Base=0, Limit=4GB, Executable, Privilege=0)
    dq 0x00CF9A000000FFFF

    ; Kernel Data Segment (Base=0, Limit=4GB, Read/Write, Privilege=0)
    dq 0x00CF92000000FFFF

    ; User Code Segment (Base=0, Limit=4GB, Executable, Privilege=3)
    dq 0x00CFFA000000FFFF

    ; User Data Segment (Base=0, Limit=4GB, Read/Write, Privilege=3)
    dq 0x00CFF2000000FFFF

    ; User Stack Segment (Base=0, Limit=4GB, Read/Write, Privilege=3)
    dq 0x00CFF4000000FFFF
```

### **c. GDT Pointer (gdt_ptr)**
The CPU needs to know where the GDT is in memory. The **GDT pointer** is stored in memory at `gdt_ptr`:

```assembly
gdt_ptr:
    dw 0x2F         ; GDT size (6 entries * 8 bytes each - 1 = 47 bytes = 0x2F)
    dd 0x00000800   ; Base address of the GDT (0x800)
```
This tells the CPU:
- The GDT is **47 bytes** long.
- The GDT is stored at **address `0x800`**.

---

## **3. How is the GDT Loaded into the CPU?**
To make the CPU use the new GDT, the **LGDT (Load GDT) instruction** is used in `boot.asm`:

```assembly
lgdt [gdt_ptr]  ; Load the new GDT from gdt_ptr
```

Once the GDT is loaded, the segment registers **(CS, DS, SS, etc.)** need to be updated.

```assembly
mov ax, 0x10    ; Load Kernel Data Segment (Selector 0x10)
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax
```

The `CS` (Code Segment) register is updated via a **far jump**:
```assembly
jmp 0x08:.reload_cs
.reload_cs:
```
This tells the CPU:
- **`0x08`** is the **Kernel Code Segment Selector**.
- It reloads CS to execute code in the new memory model.

---

## **4. How the Stack Works**
### **a. Stack Initialization**
In the kernel, the stack is set up like this:

```assembly
mov esp, stack_top  ; Set stack pointer to stack top
```

- **ESP (Stack Pointer)**: Points to the **top of the stack**.
- The stack grows **downward** in memory.
- `stack_top` is reserved in the `.bss` section with 4096 bytes:

```assembly
section .bss
align 16
resb 4096
stack_top:
```

This means the stack starts **at the top** and moves downward as data is pushed.

### **b. Using the Stack**
Pushing and popping values:

```assembly
push eax  ; Save register
pop ebx   ; Restore into another register
```

---

## **5. How the RAM Works**
### **Memory Layout**
Your **linker script (`link.ld`)** organizes memory like this:

1. **GDT Section** (`0x800`)
2. **Kernel Code** (`0x100000` onwards)
3. **Read-Only Data**
4. **Data Section**
5. **BSS Section** (Uninitialized memory)
6. **Stack** (Top of RAM, growing downward)

### **How GRUB Loads Kernel into RAM**
- **GRUB reads an ELF binary** and loads it at **0x100000**.
- It **initializes registers** and **executes `_start`** in `boot.asm`.
- The **CPU jumps into protected mode**, loads the **GDT**, and sets up segments.

---

## **6. Linking GRUB, and Kernel**
### **a. Bootloader (GRUB) Role**
- GRUB is a **Multiboot-compliant bootloader**.
- It loads a kernel from disk into **RAM at `0x100000`**.
- It sets up a minimal environment and jumps to `_start`.

### **b. How GRUB Finds Kernel**
- The kernel has a **Multiboot header**:

```assembly
section .multiboot
align 4
dd 0x1BADB002            ; Magic number
dd 0x0                   ; Flags
dd -(0x1BADB002 + 0x0)   ; Checksum
```
- GRUB recognizes this and loads the kernel correctly.

### **c. Transition from GRUB to Kernel**
1. GRUB loads the kernel into RAM.
2. It jumps to `_start` in `boot.asm`.
3. `_start` sets up **stack**, **GDT**, and **protected mode**.
4. Kernel execution begins at `kernel_main`.

