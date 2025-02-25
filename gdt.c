#include "gdt.h"

// Define the GDT
struct gdt_entry gdt[6];
struct gdt_ptr gdtp;

extern void gdt_flush(uint32_t);

void set_gdt_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[index].base_low    = (base & 0xFFFF);
    gdt[index].base_middle = (base >> 16) & 0xFF;
    gdt[index].base_high   = (base >> 24) & 0xFF;
    gdt[index].limit_low   = (limit & 0xFFFF);
    gdt[index].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[index].access      = access;
}

void gdt_install() {
    set_gdt_entry(0, 0, 0, 0, 0);                     // Null segment
    set_gdt_entry(1, 0, 0xFFFFF, 0x9A, 0xCF);         // Kernel Code
    set_gdt_entry(2, 0, 0xFFFFF, 0x92, 0xCF);         // Kernel Data
    set_gdt_entry(3, 0, 0xFFFFF, 0xFA, 0xCF);         // User Code
    set_gdt_entry(4, 0, 0xFFFFF, 0xF2, 0xCF);         // User Data
    set_gdt_entry(5, 0, 0xFFFFF, 0x96, 0xCF);         // Kernel Stack

    gdtp.limit = (sizeof(struct gdt_entry) * 6) - 1;
    gdtp.base = (uint32_t)&gdt;

    gdt_flush((uint32_t)&gdtp); // Load GDT
}
