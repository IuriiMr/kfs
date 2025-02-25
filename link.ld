OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS {
    . = 0x800;  /* Set the memory location for GDT */
    .gdt ALIGN(16) : {
        *(.gdt)  /* Place all `.gdt` section data here */
    }

    . = 0x100000;  /* The starting address for the kernel */

    .text BLOCK(4K) : ALIGN(4K) {
        *(.multiboot)  /* Include the Multiboot header */
        *(.text)       /* Include the text section (kernel code) */
    }

    .data : {
        *(.data)       /* Data section */
    }

    .bss : {
        *(.bss)         /* Uninitialized data (bss) */
    }
}

