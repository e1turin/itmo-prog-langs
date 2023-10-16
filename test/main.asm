%define SYSCALL_EXIT    60
%define SYSCALL_READ    0
%define SYSCALL_WRITE   1
%define FD_STDIN        0
%define FD_STDOUT       1
%define FD_STDERR       2
%define SYMBOL_SPACE    ' '
%define SYMBOL_LF       `\n`
%define SYMBOL_TAB      `\t`
%define SYMBOL_MINUS    '-'

%define buf_size 250

;section .rodata
section .bss
input:      resb    250
input_size: equ     $-input


section .text
 
; Принимает код возврата и завершает текущий процесс
global exit
exit: 
    mov rax, SYSCALL_EXIT
    syscall


; Принимает указатель на нуль-терминированную строку, возвращает её длину
global string_length
string_length:
    xor rax, rax
    .loop:
        cmp byte[rdi+rax], 0
        je  .end
        inc rax
        jmp .loop
    .end:
    ret


; Принимает указатель на нуль-терминированную строку, выводит её в FD_STDOUT
global print_string
print_string:
    push rdi
    call string_length
    pop rsi
    mov rdx, rax
    mov rdi, FD_STDOUT
    mov rax, SYSCALL_WRITE
    syscall
    ret



; @param    %rdi    File descriptor to read from
; @param    %rsi    Buffer beginnig address
; @param    %rdx    Buffer size
; @return   %rax    On success buffer address else 0
; @return   %rdx    On success string length else nothing
global read_string_fd
read_string_fd:
    mov r8, rdx 
    mov r9, rsi 
    xor r10, r10 
    mov rdx, 1 
    .read:
        xor rax, rax 
        lea rsi, [r9+r10] 
        syscall
        mov cl, byte[r9+r10]
        test    rax, rax
        jz  .term_zero 
        cmp cl, SYMBOL_LF 
        je  .term_zero 
        cmp r8, r10 
        jl  .overflow 
        inc r10 
        jmp .read 
    .term_zero:
        mov byte[r9+r10], 0
        mov rdx, r10 
        mov rax, r9
        ret
    .overflow:
        xor rax, rax
        ret



global read_word
read_word:
    mov r8, rsi 
    mov r9, rdi 
    xor r10, r10 
    xor rdi, rdi                ;FD_STDIN 
    mov rdx, 1 
    .skip:
        xor rax, rax            ;SYSCALL_READ
        mov rsi, r9 
        syscall
        test rax, rax
        je  .term_zero 
        cmp byte[r9], SYMBOL_SPACE
        je  .skip 
        cmp byte[r9], SYMBOL_TAB
        je  .skip
        cmp byte[r9], SYMBOL_LF
        je  .skip
        inc r10 
    .read:
        xor rax, rax 
        lea rsi, [r9+r10] 
        syscall
        mov cl, byte[r9+r10]
        test    rax, rax
        jz  .term_zero 
        cmp cl, SYMBOL_SPACE
        je  .term_zero 
        cmp cl, SYMBOL_TAB
        je  .term_zero 
        cmp cl, SYMBOL_LF 
        je  .term_zero 
        cmp r8, r10 
        jl  .overflow 
        inc r10 
        jmp .read 
    .term_zero:
        mov byte[r9+r10], 0
        mov rdx, r10 
        mov rax, r9
        ret
    .overflow:
        xor rax, rax
        ret



global _start
_start:
    mov rdi, FD_STDIN
    mov rsi, input
    mov rdx, input_size
    call read_string_fd
    ;mov rdi, input
    ;mov rsi, input_size
    ;call read_word

    mov rdi, input
    call print_string

    xor rdi, rdi
    call exit
    

    


