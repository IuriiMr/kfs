#define VIDEO_MEMORY 0xB8000
#define CURSOR_COMMAND_PORT 0x3D4
#define CURSOR_DATA_PORT 0x3D5
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_CMD_PORT 0x64

// Function to write a byte to a port
void outb(unsigned short port, unsigned char value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

// Function to read a byte from a port
unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Function to move the cursor
void move_cursor(int row, int col) {
    unsigned short position = row * SCREEN_WIDTH + col;

    // Set high byte
    outb(CURSOR_COMMAND_PORT, 0x0E);
    outb(CURSOR_DATA_PORT, (unsigned char)(position >> 8));

    // Set low byte
    outb(CURSOR_COMMAND_PORT, 0x0F);
    outb(CURSOR_DATA_PORT, (unsigned char)(position & 0xFF));
}

// Function to handle keyboard input
unsigned char get_scancode() {
    // Wait for a byte to be available from the keyboard data port
    while ((inb(KEYBOARD_STATUS_PORT) & 0x01) == 0) {
        // Busy-wait until the data is available
    }
    return inb(KEYBOARD_DATA_PORT); // Return the scan code
}

// Function to print a character on the screen
void write_char(char character, int color) {
    unsigned short *video_memory = (unsigned short *)VIDEO_MEMORY;
    static int cursor_x = 0;
    static int cursor_y = 0;

    if (character == '\n') {
        cursor_y++;
        cursor_x = 0;
    } else {
        unsigned short *location = video_memory + (cursor_y * SCREEN_WIDTH + cursor_x);
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
        // Scroll screen up by one line
        for (int i = 0; i < SCREEN_HEIGHT - 1; i++) {
            for (int j = 0; j < SCREEN_WIDTH; j++) {
                video_memory[i * SCREEN_WIDTH + j] = video_memory[(i + 1) * SCREEN_WIDTH + j];
            }
        }
        // Clear the last line
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            video_memory[(SCREEN_HEIGHT - 1) * SCREEN_WIDTH + j] = 0x0F00; // White background
        }
    }

    // Move the cursor to the new position
    move_cursor(cursor_y, cursor_x);
}

// Function to print a string
void print_string(const char *message, int color) {
    while (*message != '\0') {
        write_char(*message, color);
        message++;
    }
}


void kernel_main() {
    const char *welcome_message = 
    "   ___  _____  ______          _     \n"
    "  /   |/ __  \\ | ___ \\        (_)    \n"
    " / /| |`' / /' | |_/ /_ _ _ __ _ ___ \n"
    "/ /_| |  / /   |  __/ _` | '__| / __|\n"
    "\\___  |./ /___ | | | (_| | |  | \\__ \\\n"
    "    |_/\\_____/ \\_|  \\__,_|_|  |_|___/ \n"
    "                                      \n";
    
    int greeting_color = 0x02; // Green text on black background
    int input_color = 0x07; // Light grey text on black background

    // Print the welcome message in green color
    while (*welcome_message != '\0') {
        write_char(*welcome_message, greeting_color);
        welcome_message++;
    }

    // Print two new lines after the message
    write_char('\n', greeting_color);
    write_char('\n', greeting_color);

    // Handle keyboard input in a loop
    while (1) {
        unsigned char scancode = get_scancode(); // This is a placeholder for your keyboard input logic

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
            write_char(character, input_color); // Print input in light grey
        }
    }
}



