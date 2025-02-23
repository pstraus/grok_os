// Type definitions
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

// Video memory pointer
volatile char* video = (volatile char*)0xB8000;

// Declaration for timer_interrupt
extern void timer_interrupt();

// Structure for IDT entry
struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_hi;
} __attribute__((packed));

// Structure for IDT pointer
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// IDT array
struct idt_entry idt[256];
struct idt_ptr idt_p;

// I/O port functions
void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Print string to video memory
void print_string(const char* str) {
    while (*str) {
        *video++ = *str++;
        *video++ = 0x07;  // Light grey on black
    }
}

// Set IDT entry
void set_idt_entry(int index, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[index].base_lo = base & 0xFFFF;
    idt[index].base_hi = (base >> 16) & 0xFFFF;
    idt[index].sel = sel;
    idt[index].zero = 0;
    idt[index].flags = flags;
}

// Remap PIC
void pic_remap() {
    outb(0x20, 0x11);  // Start master PIC initialization
    outb(0xA0, 0x11);  // Start slave PIC initialization
    outb(0x21, 0x20);  // Master PIC vector offset (32)
    outb(0xA1, 0x28);  // Slave PIC vector offset (40)
    outb(0x21, 0x04);  // Tell master about slave at IRQ2
    outb(0xA1, 0x02);  // Tell slave its cascade identity
    outb(0x21, 0x01);  // 8086 mode for master
    outb(0xA1, 0x01);  // 8086 mode for slave
    outb(0x21, 0x00);  // Enable all IRQs on master
    outb(0xA1, 0x00);  // Enable all IRQs on slave
}

// Timer interrupt handler
void timer_handler() {
    static int tick = 0;
    tick++;
    if (tick % 18 == 0) {  // Approx. 1 second (18.2 Hz timer)
        print_string(".");
    }
    outb(0x20, 0x20);  // Send End Of Interrupt (EOI) to master PIC
}

// Kernel entry point
void kmain() {
    // Set up IDT
    for (int i = 0; i < 256; i++) {
        set_idt_entry(i, 0, 0x08, 0x8E);  // Null handlers
    }
    set_idt_entry(32, (uint32_t)&timer_interrupt, 0x08, 0x8E);  // Timer IRQ0

    idt_p.limit = sizeof(idt) - 1;
    idt_p.base = (uint32_t)&idt;
    load_idt(&idt_p);

    // Remap PIC
    pic_remap();

    // Enable interrupts
    asm volatile("sti");

    // Print initial message
    print_string("Hello, World!");

    // Infinite loop
    while (1) {}
}