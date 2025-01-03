#define VIDEO_MEMORY 0xB8000
#define CURSOR_COMMAND_PORT 0x3D4
#define CURSOR_DATA_PORT 0x3D5
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

// Function to write a byte to a port
void outb(unsigned short port, unsigned char value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

void move_cursor(int row, int col) {
    unsigned short position = row * SCREEN_WIDTH + col;

    // Set high byte
    outb(CURSOR_COMMAND_PORT, 0x0E);
    outb(CURSOR_DATA_PORT, (unsigned char)(position >> 8));

    // Set low byte
    outb(CURSOR_COMMAND_PORT, 0x0F);
    outb(CURSOR_DATA_PORT, (unsigned char)(position & 0xFF));
}

void write_char(char character, int color) {
    unsigned short *video_memory = (unsigned short *)VIDEO_MEMORY;
    static int cursor_x = 0;
    static int cursor_y = 0;
    
    if (character == '\n') {
        cursor_y++;
        cursor_x=0;
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

void kernel_main() {
    const char *message = "Hello, World from the Kernel!";
    int color = 0x02; // Green text on black background

    // Write the string to the screen
    while (*message != '\0') {
        write_char(*message, color);
        message++;
    }

    // After the string, print two new lines
    write_char('\n', color);
    write_char('\n', color);

    while (1) {
        __asm__("hlt"); // Halt the CPU
    }
}


