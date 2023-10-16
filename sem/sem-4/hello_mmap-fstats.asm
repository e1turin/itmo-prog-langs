; hello_mmap.asm
%define O_RDONLY 0 
%define PROT_READ 0x1
%define MAP_PRIVATE 0x2
%define SYS_WRITE 1
%define SYS_OPEN 2
%define SYS_MMAP 9
%define SYS_EXIT 60
%define FD_STDOUT 1
%define SYS_MUNMAP 11
%define SYS_CLOSE 3
%define PAGE_SIZE 4096

section .data
    ; This is the file name. You are free to change it.
    fname: db 'hello.txt', 0
    f_stats: times 200 db 0

section .text
global _start

; use exit system call to shut down correctly
exit:
    mov  rax, SYS_EXIT
    xor  rdi, rdi
    syscall

; These functions are used to print a null terminated string
; rdi holds a string pointer
print_string:
    push rdi
    call string_length
    pop  rsi
    mov  rdx, rax 
    mov  rax, SYS_WRITE
    mov  rdi, FD_STDOUT
    syscall
    ret

string_length:
    xor  rax, rax
.loop:
    cmp  byte [rdi+rax], 0
    je   .end 
    inc  rax
    jmp .loop 
.end:
    ret

; This function is used to print a substring with given length
; rdi holds a string pointer
; rsi holds a substring length
print_substring:
    mov  rdx, rsi 
    mov  rsi, rdi
    mov  rax, SYS_WRITE
    mov  rdi, FD_STDOUT
    syscall
    ret
;===============================================================================
; Принимает код символа и выводит его в stdout
print_char:
    push rdi
    mov rax, SYS_WRITE
    mov rdx, 1
    mov rsi, rsp
    mov rdi, FD_STDOUT
    syscall
    pop rax
    ret 
; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
    test    rdi, rdi
    jge print_uint          
    push rdi
    mov rdi, '-'
    call print_char
    pop rdi
    neg rdi                     ;WARN: print_uint is inlined 
; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
    mov rsi, 10                 ;base
    mov r11, rsp
    dec rsp
    mov byte[rsp], byte 0       ;null-terminator
    mov rax, rdi
    .loop:
        xor rdx, rdx            ;rdx is used in divident rdx:rax
        div rsi
        add rdx, '0'            ;make correct char
        dec rsp             
        mov byte[rsp], dl       ;put char on stack
        test rax, rax   
        jnz .loop
    mov rdi, rsp
    push    r11
    call    print_string
    pop r11
    mov rsp, r11
    ret
;===============================================================================

_start:
    ; Вызовите open и откройте fname в режиме read only.
    mov  rax, SYS_OPEN
    mov  rdi, fname
    mov  rsi, O_RDONLY    ; Open file read only
    mov  rdx, 0 	      ; We are not creating a file
                          ; so this argument has no meaning
    syscall
    ; rax holds the opened file descriptor now
    
    push rax ;push fd

        ;fstats
        mov rdi, rax
        mov rax, 5
        mov rsi, f_stats
        syscall

        mov rdi, [f_stats+48]
        mov rsi, 1
        call print_uint

    
    mov r8, [rsp] ; rax holds opened file descriptor ; it is the fourth argument of mmap
    mov rax, 9 ; mmap number
    mov rdi, 0 ; operating system will choose mapping destination
    mov rsi, [f_stats+48]; file size
    mov rdx, PROT_READ ; new memory region will be marked read only
    mov r10, MAP_PRIVATE ; pages will not be shared
    mov r9, 0 ; offset inside test.txt
    syscall ; now rax will point to mapped location

    push rax ;push file ptr

    mov rdi, rax
    call print_string
    

    ;munmap
    mov rax, SYS_MUNMAP
    pop rdi ;page ptr
    mov rsi, [f_stats+48]
    syscall

    ;close
    mov rax, SYS_CLOSE
    pop rdi ;file descriptor
    syscall

    call exit
