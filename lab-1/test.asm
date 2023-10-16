%define syscall_exit 60
%define syscall_read 0
%define syscall_write 1
%define stdout 1
%define stdin 0
%define EOT_symbol 0x4
%define space_symbol 0x20
%define newline_symbol 0xA
%define tab_symbol 0x9
%define zero_symbol '0'
%define nine_symbol '9'
%define minus_symbol '-'
%define null 0

section .data
str1: db 'hell0', 0
str2: db 'hello', 0


section .text
global _start 
 
; Принимает код возврата и завершает текущий процесс
exit: 
    mov rax, 60
    xor rdi, rdi
    syscall

; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
    xor rax, rax
    .loop:
        cmp byte [rdi+rax], 0
        je .end
        inc rax
        jmp .loop
    .end:
    ret

; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
    call string_length
    mov rdx, rax            ;length of string
    mov rsi, rdi            ;string pointer
    mov rdi, 1              ;file descripter
    mov rax, 1              ;syscall write
    syscall 
    ret

; Переводит строку (выводит символ с кодом 0xA)
print_newline:
    mov rdi, `\n`;WARN: print_char is inlined
; Принимает код символа и выводит его в stdout
print_char:
    push rdi
    mov rax, 1
    mov rdx, 1
    mov rsi, rsp
    mov rdi, 1
    syscall
    pop rax
    ret
    

; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
    test    rdi, rdi
    jge print_uint          ;if gr eq 0
    push rdi
    mov rdi, '-'
    call print_char

    pop rdi
    neg rdi                 ;WARN: print_uint is inlined

; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
    mov rsi, 10             ;base
    mov r11, rsp            ;r11 is caller-saved

    dec rsp
    mov byte[rsp], byte 0
    mov rax, rdi
    
    .loop:
        xor rdx, rdx        ;rdx is used in divident rdx:rax
        div rsi
        add rdx, '0'        ;make correct char
        dec rsp             
        mov byte[rsp], dl   ;put char on stack
        test rax, rax   
        jnz .loop

    mov rdi, rsp
    push    r11
    call    print_string
    pop r11
    mov rsp, r11
    ret

;rdi
;rsi
string_equals:
    mov rdx, -1
    .loop:
        inc rdx
        mov al, [rdi+rdx]
        cmp al, [rsi+rdx]
        jne .false
        test al, al
        jne .loop
    .true:
        mov rax, 1
        ret
    .false:
        mov rax, 0
        ret

_start:
    push 0
    xor rax, rax
    xor rdi, rdi
    mov rsi, rsp
    mov rdx, 1
    syscall
    pop rax

    mov rdi, rax
    call print_uint

    call exit
