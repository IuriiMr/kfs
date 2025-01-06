#include "types.h"

// String length function
size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// Memory set function
void memset(void *dest, uint16_t value, size_t count) {
    uint16_t *ptr = (uint16_t *)dest;
    for (size_t i = 0; i < count; i++) {
        ptr[i] = value;
    }
}

// Memory copy function
void memcpy(void *dest, const void *src, size_t count) {
    const uint16_t *src_ptr = (const uint16_t *)src;
    uint16_t *dest_ptr = (uint16_t *)dest;
    for (size_t i = 0; i < count; i++) {
        dest_ptr[i] = src_ptr[i];
    }
}

// Function to write a byte to a port
void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

// Function to read a byte from a port
uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Set the VGA cursor position
void set_cursor_position(uint16_t pos) {
    // Send command to the VGA controller to set the cursor position
    outb(0x3D4, 0x0E);  // Command to set high byte of cursor position
    outb(0x3D5, (uint8_t)(pos >> 8));  // Send high byte
    outb(0x3D4, 0x0F);  // Command to set low byte of cursor position
    outb(0x3D5, (uint8_t)(pos & 0xFF));  // Send low byte
}

// Move the cursor to the specified row and column
void move_cursor(int row, int col) {
    uint16_t pos = row * VGA_WIDTH + col;
    set_cursor_position(pos);
}

// Scroll the screen up by one line
void scroll(void) {
    unsigned short *vga_buffer = (unsigned short *)VGA_MEMORY;
    
    // Shift all the lines up
    for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
        vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
    }

    // Clear the last line
    for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
        vga_buffer[i] = ' ' | (VGA_COLOR << 8);
    }

    // Move the cursor to the last line (after scroll)
    move_cursor(VGA_HEIGHT - 1, 0);
}



// // Function to move the cursor
// void move_cursor(int row, int col) {
//     uint16_t position = row * SCREEN_WIDTH + col;

//     // Set high byte
//     outb(CURSOR_COMMAND_PORT, 0x0E);
//     outb(CURSOR_DATA_PORT, (uint8_t)(position >> 8));

//     // Set low byte
//     outb(CURSOR_COMMAND_PORT, 0x0F);
//     outb(CURSOR_DATA_PORT, (uint8_t)(position & 0xFF));
// }


// Function to print a character on the screen
void write_char(char character, int color) {
    uint16_t *video_memory = (uint16_t *)VIDEO_MEMORY;
    static int cursor_x = 0;
    static int cursor_y = 0;

    if (character == '\n') {
        cursor_y++;
        cursor_x = 0;
    } else {
        uint16_t *location = video_memory + (cursor_y * SCREEN_WIDTH + cursor_x);
        *location = (color << 8) | character;
        cursor_x++;
    }

    // Move to the next line if the end of the current line is reached
    if (cursor_x >= SCREEN_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }

    // If the cursor moves past the last line, scroll the screen
    if (cursor_y >= SCREEN_HEIGHT) {
        cursor_y = SCREEN_HEIGHT - 1; // Keep the cursor at the bottom
        
        // Scroll the screen up by one line
        memcpy(video_memory, 
               video_memory + SCREEN_WIDTH, 
               (SCREEN_HEIGHT - 1) * SCREEN_WIDTH * sizeof(uint16_t));

        // Clear the last line
        memset(video_memory + (SCREEN_HEIGHT - 1) * SCREEN_WIDTH, 
               0x0F00, 
               SCREEN_WIDTH * sizeof(uint16_t)); // White background
    }

    // Move the cursor to the new position
    move_cursor(cursor_y, cursor_x);
}

void print_string(const char *message, int color) {
    unsigned short *vga_buffer = (unsigned short *)VGA_MEMORY;
    int i = 0;

    while (message[i] != '\0') {
        // Check if the screen is full
        if (i >= VGA_WIDTH * VGA_HEIGHT) {
            scroll();
            i = (VGA_HEIGHT - 1) * VGA_WIDTH; // Reset to the last line after scrolling
        }
        
        // Place character into VGA buffer with specified color
        vga_buffer[i] = message[i] | (color << 8);
        i++;
    }

    // Update cursor position
    move_cursor(i / VGA_WIDTH, i % VGA_WIDTH);
}

void print_number(int num, int base, int color) {
    char buffer[32];
    int i = 0;

    // Handle negative numbers for base 10
    if (num < 0 && base == 10) {
        write_char('-', color);
        num = -num;
    }

    // Convert number to string
    do {
        int digit = num % base;
        buffer[i++] = (digit < 10) ? ('0' + digit) : ('A' + (digit - 10));
        num /= base;
    } while (num > 0);

    // Print the number in reverse order
    for (int j = i - 1; j >= 0; j--) {
        write_char(buffer[j], color);
    }

    // Update cursor position
    move_cursor(i / VGA_WIDTH, i % VGA_WIDTH);
}


// // Function to print a string
// void print_string(const char *message, int color) {
//     while (*message != '\0') {
//         write_char(*message, color);
//         message++;
//     }
// }

// // Helper function to print a number
// void print_number(int num, int base, int color) {
//     char buffer[32];
//     const char *digits = "0123456789ABCDEF";
//     int i = 30;
//     buffer[31] = '\0';

//     if (num == 0) {
//         buffer[i--] = '0';
//     } else {
//         int is_negative = 0;
//         if (num < 0 && base == 10) {
//             is_negative = 1;
//             num = -num;
//         }

//         while (num > 0) {
//             buffer[i--] = digits[num % base];
//             num /= base;
//         }

//         if (is_negative) {
//             buffer[i--] = '-';
//         }
//     }

//     print_string(&buffer[i + 1], color);
// }

// printk function for formatted output
void printk(const char *format, ...) {
    va_list args;
    va_start(args, format);

    const char *p = format;
    int color = 0x07; // Light grey by default

    while (*p) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 'd': { // Integer
                    int num = va_arg(args, int);
                    print_number(num, 10, color);
                    break;
                }
                case 'x': { // Hexadecimal
                    int num = va_arg(args, int);
                    print_number(num, 16, color);
                    break;
                }
                case 'c': { // Character
                    char c = (char)va_arg(args, int);
                    write_char(c, color);
                    break;
                }
                case 's': { // String
                    const char *str = va_arg(args, const char *);
                    print_string(str, color);
                    break;
                }
                default:
                    write_char('%', color);
                    write_char(*p, color);
                    break;
            }
        } else {
            write_char(*p, color);
        }
        p++;
    }

    va_end(args);
}

