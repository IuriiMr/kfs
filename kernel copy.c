void kernel_main() {
    const char *message = "Hello, World from the Kernel!";
    char *video_memory = (char *)0xB8000; // VGA text mode buffer
    for (int i = 0; message[i] != '\0'; i++) {
        video_memory[i * 2] = message[i];
        video_memory[i * 2 + 1] = 0x07; // White text on black background
    }
    while (1) {
        __asm__("hlt");
    }
}

