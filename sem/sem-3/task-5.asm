;task-5.asm
%include "lib.inc"

section .text

global _start
_start:
    mov rdi, 0x12345
    call print_hex
    call exit

