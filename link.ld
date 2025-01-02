OUTPUT_FORMAT("binary")
ENTRY(_start)

SECTIONS {
    . = 0x100000;  /* The starting address for the kernel */

    .text : {
        *(.multiboot)  /* Include the Multiboot header */
        *(.text)       /* Include the text section (kernel code) */
    }

    .rodata : {
        *(.rodata)     /* Read-only data section */
    }

    .data : {
        *(.data)       /* Data section */
    }

    .bss : {
        *(.bss)         /* Uninitialized data (bss) */
    }

    /* Memory configuration for stack, etc. */
    . = 0x200000;
}

