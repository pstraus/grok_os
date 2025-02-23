.section .multiboot
.align 4
.long 0x1BADB002          # Multiboot magic number
.long 0x00000000          # Flags (0 for simplicity)
.long -(0x1BADB002)       # Checksum: -(magic + flags)

.section .text
.global _start
_start:
    mov $stack_top, %esp  # Set stack pointer
    call kmain            # Call C kernel entry point
    cli                   # Disable interrupts
    hlt                   # Halt CPU

.global load_idt
load_idt:
    mov 4(%esp), %eax     # Load IDT pointer address from stack
    lidt (%eax)           # Load IDT
    ret

.global timer_interrupt
timer_interrupt:
    pusha                 # Save all registers
    call timer_handler    # Call C handler
    popa                  # Restore registers
    iret                  # Return from interrupt

.section .bss
.align 16
stack_bottom:
.skip 16384              # Reserve 16KB for stack
stack_top: