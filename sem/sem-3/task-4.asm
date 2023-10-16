;task-4.asm

%include "lib.inc"

global _start
_start:
    mov rdi, 0x1234567890
    call print_hex
    call exit
