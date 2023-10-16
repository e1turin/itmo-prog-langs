;task-5-exit.asm

section .text
global exit

exit:
   mov  rax, 60
   xor  rdi, rdi
   syscall
