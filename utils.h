#ifndef UTILS_H
#define UTILS_H

#include "types.h"


/**
 * strlen - String length function
 * @str: The string to mesure.
 */
size_t strlen(const char *str);

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

/**
 * printk - Prints formatted output to the screen.
 * @format: A string containing the format specification.
 * @...: Variable arguments depending on the format string.
 */
void printk(const char *format, ...);

/**
 * write_char - Writes a single character to the screen.
 * @character: The character to print.
 * @color: The color attribute for the character.
 */
void write_char(char character, int color);

/**
 * print_string - Prints a string to the screen with a specific color.
 * @message: The string to print.
 * @color: The color attribute for the text.
 */
void print_string(const char *message, int color);

/**
 * print_number - Prints a number in a given base with a specific color.
 * @num: The number to print.
 * @base: The base to print the number in (e.g., 10 for decimal, 16 for hexadecimal).
 * @color: The color attribute for the number.
 */
void print_number(int num, int base, int color);

/**
 * outb - Writes a byte to an I/O port.
 * @port: The I/O port address.
 * @value: The byte value to write to the port.
 */
void outb(uint16_t port, uint8_t value);


void outw(uint16_t port, uint16_t data);


/**
 * inb - Reads a byte from an I/O port.
 * @port: The I/O port address.
 * 
 * Return: The byte value read from the port.
 */
uint8_t inb(uint16_t port);

/**
 * scroll - Scrolls the screen up by one line when the screen is full.
 */
void scroll(void);

/**
 * move_cursor - Moves the text cursor to a specific location on the screen.
 * @row: The row to move the cursor to.
 * @col: The column to move the cursor to.
 */
void move_cursor(int row, int col);

/**
 * set_cursor_position - Sets the position of the cursor.
 * @pos: The position in the video memory (0-based).
 */
void set_cursor_position(uint16_t pos);

int strcmp(const char *s1, const char *s2);

// Global cursor position variables (Declared as extern)
extern int cursor_x;
extern int cursor_y;


#endif // UTILS_H

