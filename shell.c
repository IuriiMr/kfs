#include "utils.h"

void print_help() {
    printk("Available commands:\n");
    printk("clear  - Clears the screen\n");
    printk("halt   - Halts the CPU\n");
    printk("reboot - Reboots the system\n");
    printk("shutdown - Shutdown the system\n");
    printk("stack - Prints kernel stack dump\n");
    printk("help   - Prints this help message\n");
}

void clear_screen() {
    uint16_t *vga_buffer = (uint16_t *)0xB8000;
    for (int i = 0; i < 80 * 25; i++) {
        vga_buffer[i] = (0x0F << 8) | ' ';  // Light gray on black, space character
    }

    // Reset cursor position in memory
    cursor_x = 0;
    cursor_y = 0;
    move_cursor(0, 0);  // Reset cursor to top-left
}

void halt() {
    printk("\nHalting the system...\n");
    __asm__ volatile ("hlt");  // Halt the system
}

void reboot() {
    printk("\nRebooting system...\n");
    // Trigger BIOS reboot interrupt
    __asm__ volatile ("int $0x19");
}


void shutdown() {
    printk("\nShutting down system...\n");
    outw(0x604, 0x2000);
}