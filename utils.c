#include "types.h"

// // Memory set function
// void memset(void *dest, uint8_t value, size_t len) {
//     uint8_t *ptr = (uint8_t*)dest;
//     for (size_t i = 0; i < len; i++) {
//         ptr[i] = value;
//     }
// }

// // Memory copy function
// void memcpy(void *dest, const void *src, size_t len) {
//     const uint8_t *src_ptr = (const uint8_t*)src;
//     uint8_t *dest_ptr = (uint8_t*)dest;
//     for (size_t i = 0; i < len; i++) {
//         dest_ptr[i] = src_ptr[i];
//     }
// }

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
