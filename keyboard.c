#include "utils.h"
#include "shell.h"
#include "print_kernel.h"

// Scancode to ASCII mapping table
const char scancode_map[128] = {
    0,   27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', // 0x00-0x0E
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',    // 0x0F-0x1C
    0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',         // 0x1D-0x28
    0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,           // 0x29-0x35
    '*', 0,   ' ', 0,    // 0x36-0x39 (Space and others)
};

const char scancode_map_shift[128] = {
    0,   27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', // 0x00-0x0E
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',    // 0x0F-0x1C
    0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',          // 0x1D-0x28
    0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,            // 0x29-0x35
    '*', 0,   ' ', 0,    // 0x36-0x39 (Space and others)
};

const uint8_t scancode_non_handling_map[128] = {
    0x01, // Esc
//    0x0E, // Backspace
    0x0F, // Tab
    0x1D, // Left Control
    0x38, // Left Alt
    0x3B, // F1
    0x44, // F2
    0x57, // F3
    0x5E, // F4
    0x5F, // F5
    0x60, // F6
    0x61, // F7
    0x62, // F8
    0x63, // F9
    0x64, // F10
    0x65, // F11
    0x66, // F12
    0x67, // Print Screen
    0x68, // Scroll Lock
    0x69, // Pause/Break
    0x6A, // Insert
    0x6B, // Home
    0x6C, // Page Up
    0x6D, // Delete
    0x6E, // End
    0x6F, // Page Down
    0x70, // Right Arrow
    0x71, // Left Arrow
    0x72, // Down Arrow
    0x73, // Up Arrow
    0x74, // Num Lock
    0x75, // NumPad /
    0x76, // NumPad *
    0x77, // NumPad -
    0x78, // NumPad +
    0x79, // NumPad Enter
    0x7A, // NumPad 1
    0x7B, // NumPad 2
    0x7C, // NumPad 3
    0x7D, // NumPad 4
    0x7E, // NumPad 5
    0x7F, // NumPad 6
    0x80, // NumPad 7
    0x81, // NumPad 8
    0x82, // NumPad 9
    0x83, // NumPad 0
    0x84, // NumPad .
    0x85, // Left Windows
    0x86, // Right Windows
    0x87, // Application key
    0x88, // Power
    0x89, // Sleep
    0x8A, // Wake
    0x8B, // Zoom
    0x8E, // Left Control
    0x8F, // Right Control
    0x90, // Left Alt
    0x91, // Right Alt
    0x92, // Caps Lock
    0x93, // Num Lock
    0x94, // Scroll Lock
    0x95, // Print Screen
    0x96, // Break
    0x99, // Tab
    0x9A, // Backspace
    0x9B, // Escape
    0x9C, // Left Arrow
    0x9D, // Right Arrow
    0x9E, // Up Arrow
    0x9F, // Down Arrow
    0xA0, // Left Control
    0xA1, // Right Control
    0xA2, // Left Alt
    0xA3, // Right Alt
    0xA4, // Left Shift
    0xA5, // Right Shift
    0xA6, // NumLock
    0xA8, // ScrollLock
    0xA9, // Home
    0xAA, // End
    0xAB, // Page Up
    0xAC, // Page Down
    0xAD, // Insert
    0xAE, // Delete
    0xAF, // F1
    0xB0, // F2
    0xB1, // F3
    0xB2, // F4
    0xB3, // F5
    0xB4, // F6
    0xB5, // F7
    0xB6, // F8
    0xB7, // F9
    0xB8, // F10
    0xB9, // F11
    0xBA, // F12
    0xBB, // PrintScreen
    0xBC, // Pause
    0xBD, // Application Key
    0xBE, // Left Windows Key
    0xBF, // Right Windows Key
    0xC0  // Menu Key
};

char command_buffer[MAX_COMMAND_LENGTH];  // Buffer to store input command
uint8_t command_index = 0;  // Tracks current position in buffer

// Variables to track the Shift and Caps Lock state
uint8_t left_shift_pressed = 0;
uint8_t right_shift_pressed = 0;
uint8_t caps_lock_on = 0;

// Function to handle keyboard input
uint8_t get_scancode() {
    // Wait for a byte to be available from the keyboard data port
    while ((inb(KEYBOARD_STATUS_PORT) & 0x01) == 0) {
        // Busy-wait until the data is available
    }
    return inb(KEYBOARD_DATA_PORT); // Return the scan code
}

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

// Function to check if the scancode is in the non-printable map
uint8_t is_non_printable(uint8_t scancode) {
    size_t map_size = sizeof(scancode_non_handling_map) / sizeof(scancode_non_handling_map[0]);
    for (size_t i = 0; i < map_size; i++) {
        if (scancode_non_handling_map[i] == scancode) {
            return 1; // Found in the list, non-printable
        }
    }
    return 0; // Not found, likely printable
}

void execute_command(char *command) {
    // Check which command was entered
    if (strcmp(command, "clear") == 0) {
        clear_screen();  // Function to clear the screen
    } else if (strcmp(command, "halt") == 0) {
        halt();
    } else if (strcmp(command, "reboot") == 0) {
        reboot();
    } else if (strcmp(command, "shutdown") == 0) {
        shutdown();
    } else if (strcmp(command, "stack") == 0) {
        print_kernel();
    } else if (strcmp(command, "help") == 0) {
        print_help();
    } else if (strcmp(command, "clear") == 0) {
        clear_screen();  // Clear screen function
        move_cursor(0, 0);
    } else {
        print_string("\nUnknown command: ", ERROR_COLOR);
        print_string("\n", ERROR_COLOR);
    }
}

// Function to print the shell prompt
void print_prompt() {
    printk(SHELL_PROMPT, INPUT_COLOR);
}

// Modify the handle_keyboard_input function
void handle_keyboard_input() {
    print_prompt();  // Show the prompt at the start

    while (1) {
        unsigned char scancode = get_scancode();
        handle_shift_key(scancode);
        handle_caps_lock_key(scancode);

        if (is_non_printable(scancode)) {
            continue;
        }

        if (scancode < 128) {
            char character = is_shift_pressed() ? scancode_map_shift[scancode] : scancode_map[scancode];

            // Handle backspace
            if (character == '\b' && command_index > 0) {
                command_index--;
                cursor_x--;
                write_char(' ', INPUT_COLOR);  // Overwrite character with space
                cursor_x--;
                move_cursor(cursor_y, cursor_x);
                continue;
            }

            // Handle Enter key (execute command)
            if (character == '\n') {
                command_buffer[command_index] = '\0';  // Null-terminate the command
                execute_command(command_buffer);  // Execute the command
                command_index = 0;  // Reset buffer
                print_prompt();  // Show new prompt
                continue;
            }

            // Store character in buffer and print it
            if (command_index < 255) {
                command_buffer[command_index++] = character;
                write_char(character, INPUT_COLOR);
            }
        }
    }
}
