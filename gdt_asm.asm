.global gdt_flush
gdt_flush:
    lgdt [eax]        ; Load GDT from memory
    mov ax, 0x10      ; Kernel data selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush   ; Jump to kernel code selector
.flush:
    ret
