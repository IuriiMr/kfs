#ifndef UTILS_H
#define UTILS_H

#include "types.h" // For size_t type

/**
 * memset - Fills a block of memory with a specified value.
 * @dest: Pointer to the memory block to fill.
 * @value: Value to set (16-bit for VGA text mode).
 * @count: Number of elements to set.
 */
void memset(void *dest, unsigned short value, size_t count);

/**
 * memcpy - Copies a block of memory from one location to another.
 * @dest: Pointer to the destination memory block.
 * @src: Pointer to the source memory block.
 * @count: Number of elements to copy.
 */
void memcpy(void *dest, const void *src, size_t count);

// Formatted output
void printk(const char *format, ...);


void write_char(char character, int color);
// Function to print a string
void print_string(const char *message, int color);
// Helper function to print a number
void print_number(int num, int base, int color);

// Function to write a byte to a port
void outb(uint16_t port, uint8_t value);
// Function to read a byte from a port
uint8_t inb(uint16_t port);
// Function to move the cursor
void move_cursor(int row, int col);

#endif // UTILS_H
