section .multiboot
align 4
dd 0x1BADB002            ; Magic number
dd 0x0                   ; Flags (no video mode or additional requirements)
dd -(0x1BADB002 + 0x0)   ; Checksum = -(magic + flags)

section .text

global _start
_start:
    ; Set up stack
    cli                     ; Clear interrupts
    xor eax, eax            ; Clear registers
    mov esp, stack_top      ; Set up stack pointer

    ; Load GDT from address 0x800
    lgdt [gdt_ptr]

    ; Reload segment registers
    mov ax, 0x10            ; Kernel Data Segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far jump to reload CS (Kernel Code Segment)
    jmp 0x08:.reload_cs
.reload_cs:

    ; Jump to C kernel
    extern kernel_main
    call kernel_main

    hlt                     ; Halt CPU

; Define GDT at 0x00000800
section .gdt
align 16
global gdt_start
gdt_start:
    dq 0x0000000000000000  ; Null descriptor
    dq 0x00CF9A000000FFFF  ; Kernel code segment (0x9A = 10011010b)
    dq 0x00CF92000000FFFF  ; Kernel data segment (0x92 = 10010010b)
    dq 0x00CFFA000000FFFF  ; User code segment (0xFA = 11111010b)
    dq 0x00CFF2000000FFFF  ; User data segment (0xF2 = 11110010b)
    dq 0x0000000000000000  ; User stack segment (can be updated later)

section .rodata

; GDT pointer: The pointer to the GDT (limit + base)
section .data
gdt_ptr:
    dw 0x2F         ; GDT size (5 entries * 8 bytes each - 1)
    dd 0x00000800   ; Base address of the GDT

section .bss
align 16
resb 4096
stack_top:

