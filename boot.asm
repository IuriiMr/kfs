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

section .bss
align 16
resb 4096
stack_top:

