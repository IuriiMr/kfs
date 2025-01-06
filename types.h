#ifndef TYPES_H
#define TYPES_H

// Unsigned integer types
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

// Signed integer types
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

// Size type
typedef unsigned int size_t;

// NULL definition
#define NULL ((void*)0)

// Variadic argument types
typedef char *va_list;

// Define macros for variadic arguments
#define VA_SIZE(type) ((sizeof(type) + sizeof(uint32_t) - 1) & ~(sizeof(uint32_t) - 1))

#define va_start(ap, last) (ap = (va_list)(&last + 1))
#define va_arg(ap, type) (*(type *)((ap += VA_SIZE(type)) - VA_SIZE(type)))
#define va_end(ap) (ap = NULL)

#define VIDEO_MEMORY 0xB8000
#define CURSOR_COMMAND_PORT 0x3D4
#define CURSOR_DATA_PORT 0x3D5
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_CMD_PORT 0x64

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY (char *)0xB8000
#define VGA_COLOR 0x07 // White on black text color


#endif // TYPES_H
