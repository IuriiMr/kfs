    GDT Definition (.gdt Section):
        gdt_start defines the Global Descriptor Table entries.
        Each entry in the GDT is 8 bytes. The 6 entries defined are:
            Null Descriptor: Used as a placeholder for the 0 index.
            Kernel Code Segment: Code segment for the kernel (executive, read).
            Kernel Data Segment: Data segment for the kernel (read/write).
            User Code Segment: Code segment for user mode (execute/read).
            User Data Segment: Data segment for user mode (read/write).
            User Stack (can be updated later): A placeholder for the user stack descriptor (this can be defined or updated later).

    GDT Pointer (gdt_ptr):
        The GDT pointer is defined to point to the GDT, which consists of the limit and base address of the GDT.
        The limit is 0x2F because we have 6 GDT entries (6 entries * 8 bytes = 48 bytes, 0x2F).
        The base is set to the address of gdt_start.

    GDT Flush (gdt_flush function):
        The GDT Flush function uses the lgdt instruction to load the GDT.
        After loading the GDT, it loads the segment selectors (ds, es, fs, gs, and ss) to use the new descriptors.
        The gdt_flush function is called to set the Global Descriptor Table into the CPU registers.

    Stack Setup and Calling Kernel:
        The stack is initialized, and the kernel_main function is called.
        This ensures that after the bootloader finishes execution, the kernel can run with the new GDT in place.

How It Works:

    When the bootloader runs, it first sets up the stack and the GDT.
    The gdt_flush function is called to load the new GDT and switch to the new memory model using the appropriate segment selectors.
    The kernel can then start running, using the newly set up GDT for the kernel and user modes.

This code will allow the kernel to run with proper segmentation using a GDT. You can further customize the GDT entries as required.



When the requirement says, "You must declare your GDT to the BIOS", it refers to the process of informing the BIOS about the Global Descriptor Table (GDT) and allowing it to use the GDT structure. In x86 systems, the GDT holds important segment descriptors that the CPU uses to manage memory access rights, code segments, and data segments. In other words, it allows the CPU to properly manage the different segments that your kernel will use.
Here's how this works:

    Understanding the BIOS' Role:
        The BIOS (Basic Input/Output System) is the initial system firmware that is responsible for basic hardware initialization (keyboard, screen, etc.), and the transfer of control to a bootloader or an operating system. Once the bootloader starts the kernel, it may rely on the BIOS or the bootloader to set up some important system structures like the GDT.

    Declaring GDT to BIOS:
        The BIOS itself doesn't "declare" the GDT. What actually happens is that your kernel needs to set up the GDT and inform the CPU about it so that it can start using the segments defined in the GDT.
        The BIOS allows the bootloader or kernel to set up the GDT using a special CPU instruction lgdt (Load Global Descriptor Table). This tells the CPU to load the GDT pointer, which points to the actual GDT structure that your kernel has set up.
        So when you "declare" the GDT to the BIOS, you're preparing the GDT and telling the CPU to use it for segmenting memory.

How did I implement this?

    Creating the GDT:
        In the code, I defined the GDT with segment descriptors for different segments that the kernel will use: kernel code, kernel data, user code, user data, and user stack.

section .gdt
global gdt_start
gdt_start:
dq 0x0000000000000000  ; Null descriptor (required as a placeholder for the 0th entry)
dq 0x00CF9A000000FFFF  ; Kernel code segment
dq 0x00CF92000000FFFF  ; Kernel data segment
dq 0x00CFFA000000FFFF  ; User code segment
dq 0x00CFF2000000FFFF  ; User data segment
dq 0x0000000000000000  ; User stack (empty placeholder)

These descriptors define the segment's base, limit, and access rights. The access rights in these descriptors are defined in the hexadecimal values and control permissions for the segment.

Loading the GDT:

    In the boot assembly code, I used the lgdt instruction to load the pointer to the GDT into the CPU's GDTR register.

global gdt_flush
gdt_flush:
lgdt [gdt_ptr]        ; Load the GDT pointer into the GDTR register
mov ax, 0x10          ; Load the kernel code segment (selector)
mov ds, ax            ; Set the data segments
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax            ; Set the stack segment
ret

Here:

    lgdt is the CPU instruction that loads the GDT pointer. This tells the CPU where to find the GDT.
    The gdt_ptr points to the GDT structure that we have set up. The gdt_flush function essentially updates the CPU's internal registers to use the segments defined in the GDT.
    We also load the kernel's code and data segments by setting the ds, es, fs, gs, and ss registers.

Making GDT available for BIOS:

    While the BIOS itself doesn't load or "declare" the GDT (this happens after the BIOS hands control over to the bootloader/kernel), your kernel must declare the GDT to the CPU in the early stages of the kernel initialization (just after the bootloader loads the kernel).
    The BIOS/bootloader transition from protected mode to real mode, and then to protected mode again in a typical boot process. This is where the GDT comes into play, and the kernel must load it for proper memory segmentation.

Setting the GDT Pointer:

    I used a special pointer structure (gdt_ptr) to hold the size of the GDT and the address of the GDT itself. This pointer is required for the lgdt instruction.

    gdt_ptr:
        dw 0x001F           ; GDT limit (size of the GDT - 1)
        dd gdt_start        ; Address of the GDT

    The gdt_ptr points to the GDT's starting address (gdt_start) and specifies its size (here 0x001F is the size of the GDT).

In summary:

    The phrase "declare your GDT to the BIOS" is somewhat misleading, as it's not actually the BIOS that uses the GDT, but the CPU. The kernel must set up the GDT early on and load it into the CPU's GDTR register using the lgdt instruction.
    The BIOS, once it hands control over to the kernel, doesn't need to directly interact with the GDT. The GDT will be used by the CPU to handle memory access rights and segmentation, and the kernel must ensure that the GDT is loaded and set up correctly before any protected mode operations are performed.