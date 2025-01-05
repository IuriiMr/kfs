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

#endif // UTILS_H
