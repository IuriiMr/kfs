#include "print_kernel.h"
#include "utils.h"

void print_kernel() {
    uint32_t *stack_ptr;
    __asm__("movl %%esp, %0" : "=r"(stack_ptr)); // Get current stack pointer

    printk("Kernel Stack Dump:\n");
    for (int i = 0; i < 10; i++) { // Print 10 stack entries
        printk("[%d] 0x%x\n", i, stack_ptr[i]);
    }
}
