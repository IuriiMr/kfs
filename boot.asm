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

    ; Jump to the kernel main function
    extern kernel_main
    call kernel_main

    hlt                     ; Halt the CPU
global gdt_flush
gdt_flush:
    lgdt [gdt_ptr]        ; Load the GDT pointer into the GDTR register
    mov ax, 0x10          ; Load the kernel code segment (selector)
    mov ds, ax            ; Load data segments
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax            ; Load stack segment
    ret

; Define the Global Descriptor Table (GDT)
section .gdt
global gdt_start
gdt_start:
    ; Null descriptor
    dq 0x0000000000000000  ; Null descriptor
    ; Kernel code segment (base = 0, limit = 4GB, 32-bit, execute/read)
    dq 0x00CF9A000000FFFF  ; Kernel code segment (0x9A = 10011010b)
    ; Kernel data segment (base = 0, limit = 4GB, 32-bit, read/write)
    dq 0x00CF92000000FFFF  ; Kernel data segment (0x92 = 10010010b)
    ; User code segment (base = 0, limit = 4GB, 32-bit, execute/read)
    dq 0x00CFFA000000FFFF  ; User code segment (0xFA = 11111010b)
    ; User data segment (base = 0, limit = 4GB, 32-bit, read/write)
    dq 0x00CFF2000000FFFF  ; User data segment (0xF2 = 11110010b)
    ; Unused or reserved (user stack can be updated later)
    dq 0x0000000000000000  ; User stack segment (can be updated later)

section .data
; GDT pointer: The pointer to the GDT (limit + base)
gdt_ptr:
    dw 0x2F  ; Limit of GDT (size of the GDT entries)
    dd gdt_start  ; Base address of the GDT

section .bss
align 16
resb 4096
stack_top:

