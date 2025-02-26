#include "utils.h"
#include "keyboard.h"
#include "gdt.h"
#include "print_kernel.h"

// Function to print out the greeting with 42
void greeting() {
    const char *welcome_message = 
    "   ___  _____  ______          _     \n"
    "  /   |/ __  \\ | ___ \\        (_)    \n"
    " / /| |`' / /' | |_/ /_ _ _ __ _ ___ \n"
    "/ /_| |  / /   |  __/ _` | '__| / __|\n"
    "\\___  |./ /___ | | | (_| | |  | \\__ \\\n"
    "    |_/\\_____/ \\_|  \\__,_|_|  |_|___/ \n"
    "                                      \n";
    
    // Print the welcome message in green color
    while (*welcome_message != '\0') {
        write_char(*welcome_message, GREETING_COLOR);
        welcome_message++;
    }
}

void kernel_main() {



//        printk("Printing kernel stack:\n");
//        print_kernel();


    // Print out a greeting in color with 42
    greeting();

    printk("Initializing GDT...\n");
    gdt_install();
    printk("GDT successfully loaded!\n");

    // Example of printing an d integer
    int kernel_version_major = 1;
    int kernel_version_minor = 0;
    printk("Kernel Version: %d.%d\n", kernel_version_major, kernel_version_minor);

    // Example of printing a x memory address
    void *kernel_base_address = (void *)0x100000;
    printk("Kernel Base Address: 0x%x\n", (uint32_t)kernel_base_address);

    // Print two new lines after the message
    write_char('\n', 0);
    write_char('\n', 0);

    // Handle keyboard entries and print them
    handle_keyboard_input();
}

