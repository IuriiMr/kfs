#include "utils.h"

// Function to handle keyboard input
uint8_t get_scancode() {
    // Wait for a byte to be available from the keyboard data port
    while ((inb(KEYBOARD_STATUS_PORT) & 0x01) == 0) {
        // Busy-wait until the data is available
    }
    return inb(KEYBOARD_DATA_PORT); // Return the scan code
}

// Variables to track the Shift and Caps Lock state
uint8_t left_shift_pressed = 0;
uint8_t right_shift_pressed = 0;
uint8_t caps_lock_on = 0;

// Function to handle the Shift key presses and releases
void handle_shift_key(uint8_t scancode) {
    // Check if the scancode corresponds to the left shift key (0x2A for press, 0xAA for release)
    if (scancode == 0x2A) { // Left shift pressed
        left_shift_pressed = 1;
    } else if (scancode == 0xAA) { // Left shift released
        left_shift_pressed = 0;
    }

    // Check if the scancode corresponds to the right shift key (0x36 for press, 0xB6 for release)
    if (scancode == 0x36) { // Right shift pressed
        right_shift_pressed = 1;
    } else if (scancode == 0xB6) { // Right shift released
        right_shift_pressed = 0;
    }
}

// Function to check if Shift is pressed
uint8_t is_shift_pressed() {
    return left_shift_pressed || right_shift_pressed;
}

// Function to handle the Caps Lock key presses and releases
void handle_caps_lock_key(uint8_t scancode) {
    // Check if the scancode corresponds to the Caps Lock key (0x3A for press, 0xBA for release)
    if (scancode == 0x3A) { // Caps Lock pressed
        caps_lock_on = !caps_lock_on; // Toggle the Caps Lock state
    }
}

// Function to determine if the letter should be uppercase or lowercase
uint8_t handle_case(uint8_t character) {
    // If Caps Lock is on, the case is inverted (uppercase), unless Shift is also pressed
    if (caps_lock_on && !is_shift_pressed()) {
        if (character >= 'a' && character <= 'z') {
            character = character - 'a' + 'A'; // Convert to uppercase
        }
    } else if (is_shift_pressed() && !(caps_lock_on)) {
        if (character >= 'a' && character <= 'z') {
            character = character - 'a' + 'A'; // Convert to uppercase
        }
    } else {
        // If neither Caps Lock nor Shift is pressed, make it lowercase
        if (character >= 'A' && character <= 'Z') {
            character = character - 'A' + 'a'; // Convert to lowercase
        }
    }
    return character;
}

// Function to handle keyboard input and print it
void handle_keyboard_input() {
    
    while (1) {
        unsigned char scancode = get_scancode(); // This is a placeholder for your keyboard input logic

        // Handle the Shift key state
        handle_shift_key(scancode);
        // Handle the Caps Lock key state
        handle_caps_lock_key(scancode);

        // Convert scancode to ASCII
        char character = 0;

        // For now, just check for a few basic keys
        if (scancode == 0x1E) { // 'a' key
            character = 'a';
        } else if (scancode == 0x30) { // 'b' key
            character = 'b';
        } else if (scancode == 0x1C) { // Enter key
            character = '\n';
        }

        // If a valid character is found, print it in light grey color
        if (character != 0) {
            character = handle_case(character); // Apply Caps Lock and Shift logic
            write_char(character, INPUT_COLOR); // Print input in light grey
        }
    }
}