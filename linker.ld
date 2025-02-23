ENTRY(_start)

SECTIONS {
    . = 1M;               /* Load kernel at 1MB */
    .multiboot : { *(.multiboot) }
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}