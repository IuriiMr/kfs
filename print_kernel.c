#include "utils.h"

// Define the memory printing functions (adapted from your provided code)
void ft_putchar(char c, int size) {
    int i = 0;
    while (i < size) {
        printk("%c", c);
        i++;
    }
}

void ft_xdigit(unsigned long long nb, int check) {
    char *xdigit = "0123456789abcdef";

    if (nb < 16 && check == 1)
        ft_putchar('0', 1);
    if (nb >= 16) {
        ft_xdigit(nb / 16, 0);
        ft_xdigit(nb % 16, 0);
    } else {
        ft_putchar(xdigit[nb], 1);
    }
}

void ft_print_addr(uintptr_t addr) {
    uintptr_t tmp = addr;
    int i = 1;
    while (i++ < 15) {
        if (tmp < 16)
            ft_putchar('0', 1);
        tmp /= 16;
    }
    ft_xdigit(addr, 0);
}

void ft_print_data(unsigned char *c, int size) {
    int i = -1;
    while (i++ < 16) {
        if (i % 2 == 0)
            ft_putchar(' ', 1);
        if (i < size) {
            ft_xdigit((unsigned long long)*(c + i), 1);
        } else if (i != 16) {
            ft_putchar(' ', 2);
        }
    }
    i = -1;

    // Print ASCII representation of the data
    while (i++ < size - 1) {
        if (*(c + i) <= 31 || *(c + i) >= 127)
            ft_putchar('.', 1);
        else
            ft_putchar(*(c + i), 1);
    }
}

void *ft_print_memory(void *addr, unsigned int size) {
    unsigned int i = 0;
    unsigned int columns;

    while (i * 16 < size) {
        if (i < size / 16)
            columns = 16;
        else
            columns = size % 16;

        // Print address at the start of each line
        ft_print_addr((unsigned long long)addr + (i * 16));
        ft_putchar(':', 1);

        // Print the memory data in columns
        ft_print_data(addr + (i * 16), columns);
        ft_putchar('\n', 1);
        i++;
    }
    return addr;
}

// Inline function to get the current stack pointer (ESP)
uint32_t get_stack_pointer() {
    uint32_t stack_ptr;
    __asm__ volatile ("mov %%esp, %0" : "=r" (stack_ptr));  // Get the current stack pointer (ESP)
    return stack_ptr;
}

// Function to print the kernel stack or any memory region
void print_kernel() {
    uint32_t stack_ptr = get_stack_pointer();
    printk("Printing kernel stack starting from address: 0x%x\n", stack_ptr);
    // Print memory from the current stack pointer
    ft_print_memory((void*)stack_ptr, 256);  // Print 256 bytes starting from stack pointer
}
