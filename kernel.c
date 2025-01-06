
#include "utils.h"

// // Function to write a byte to a port
// void outb(uint16_t port, uint8_t value) {
//     __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
// }

// // Function to read a byte from a port
// uint8_t inb(uint16_t port) {
//     uint8_t result;
//     __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
//     return result;
// }

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


// // Function to print a character on the screen
// void write_char(char character, int color) {
//     uint16_t *video_memory = (uint16_t *)VIDEO_MEMORY;
//     static int cursor_x = 0;
//     static int cursor_y = 0;

//     if (character == '\n') {
//         cursor_y++;
//         cursor_x = 0;
//     } else {
//         uint16_t *location = video_memory + (cursor_y * SCREEN_WIDTH + cursor_x);
//         *location = (color << 8) | character;
//         cursor_x++;
//     }

//     // Move to the next line if the end of the current line is reached
//     if (cursor_x >= SCREEN_WIDTH) {
//         cursor_x = 0;
//         cursor_y++;
//     }

//     // If the cursor moves past the last line, scroll the screen
//     if (cursor_y >= SCREEN_HEIGHT) {
//         cursor_y = SCREEN_HEIGHT - 1; // Keep the cursor at the bottom
        
//         // Scroll the screen up by one line
//         memcpy(video_memory, 
//                video_memory + SCREEN_WIDTH, 
//                (SCREEN_HEIGHT - 1) * SCREEN_WIDTH * sizeof(uint16_t));

//         // Clear the last line
//         memset(video_memory + (SCREEN_HEIGHT - 1) * SCREEN_WIDTH, 
//                0x0F00, 
//                SCREEN_WIDTH * sizeof(uint16_t)); // White background
//     }

//     // Move the cursor to the new position
//     move_cursor(cursor_y, cursor_x);
// }


// // Function to print a string
// void print_string(const char *message, int color) {
//     while (*message != '\0') {
//         write_char(*message, color);
//         message++;
//     }
// }

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
    
    int greeting_color = 0x02; // Green text on black background
  

    // Print the welcome message in green color
    while (*welcome_message != '\0') {
        write_char(*welcome_message, greeting_color);
        welcome_message++;
    }
}

// Variables for tracking the current cursor position
int cursor_row = 0;
int cursor_col = 0;

// Function to handle keyboard input and print it
void handle_keyboard_input() {
    int input_color = 0x07; // Light grey text on black background

    while (1) {
        unsigned char scancode = get_scancode(); // This is a placeholder for your keyboard input logic

        // Handle the Shift key state
        handle_shift_key(scancode);
        // Handle the Caps Lock key state
        handle_caps_lock_key(scancode);

        // Handle arrow key presses for cursor movement
        if (scancode == 0x48) { // Up arrow
            if (cursor_row > 0) {
                cursor_row--;
            }
        } else if (scancode == 0x50) { // Down arrow
            if (cursor_row < VGA_HEIGHT - 1) {
                cursor_row++;
            } else {
                // Scroll when reaching the bottom
                scroll();
                cursor_row = VGA_HEIGHT - 1; // Ensure the cursor stays within bounds
            }
        } else if (scancode == 0x4B) { // Left arrow
            if (cursor_col > 0) {
                cursor_col--;
            }
        } else if (scancode == 0x4D) { // Right arrow
            if (cursor_col < VGA_WIDTH - 1) {
                cursor_col++;
            }
        }


        // Convert scancode to ASCII (basic example)
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
            write_char(character, input_color); // Print input in light grey
        }
        
        // Update the cursor position after each key press
        move_cursor(cursor_row, cursor_col);
    }
}


void kernel_main() {
    greeting();

    // Initial welcome message
    printk("Starting Bare-Metal Kernel...\n");

    // Example of printing an integer
    int kernel_version_major = 1;
    int kernel_version_minor = 0;
    printk("Kernel Version: %d.%d\n", kernel_version_major, kernel_version_minor);

    // Example of printing a memory address
    void *kernel_base_address = (void *)0x100000;
    printk("Kernel Base Address: 0x%x\n", (uint32_t)kernel_base_address);

    // Print two new lines after the message
    write_char('\n', 0);
    write_char('\n', 0);

    handle_keyboard_input();
}



