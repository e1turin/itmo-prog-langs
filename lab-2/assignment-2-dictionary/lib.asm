%include "defines.inc"

section .text
 
; @param    %rdi    Exit code
global exit
exit: 
    mov rax, SYSCALL_EXIT
    syscall


; @param    %rdi    Ptr to 0-term string
; @return   %rax    Length of the string
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
; @param    %rdi    Ptr to 0-term string
global print_string
print_string:
    push rdi
    call string_length
    pop rsi
    mov rdx, rax
    mov rdi, FD_STDOUT

; @param    %rdi    FD where to write
; @param    %rsi    Ptr to 0-term string
; @param    %rdx    Length of substring from the beginning of the string
global print_substring_fd
print_substring:
    mov rax, SYSCALL_WRITE
    syscall
    ret


; Prints string to FD
; @param    %rdi    Ptr to string
; @param    %rdx    File descriptor
; @return   %rax    Amount of written bytes or -1 on fail
global print_string_fd
print_string_fd:
    push rdi
    push rdx
    call string_length
    pop rdi
    pop rsi
    mov rdx, rax
    mov rax, SYSCALL_WRITE
    syscall
    ret


; Переводит строку (выводит символ с кодом 0xA)
global print_newline
print_newline:
    mov rdi, SYMBOL_LF     

; Принимает код символа и выводит его в FD_STDOUT
global print_char
print_char:
    push rdi
    mov rax, SYSCALL_WRITE
    mov rdx, 1
    mov rsi, rsp
    mov rdi, FD_STDOUT
    syscall
    pop rax
    ret
    

; Выводит знаковое 8-байтовое число в десятичном формате 
global print_int
print_int:
    test    rdi, rdi
    jge print_uint          
    push rdi
    mov rdi, SYMBOL_MINUS
    call print_char
    pop rdi
    neg rdi                     

; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
global print_uint
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


; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
global string_equals
string_equals:
    xor rcx, rcx
    .loop:
        mov al, [rdi+rcx]       ;is it faster than incrementing %rdi and %rsi?
        cmp al, [rsi+rcx]
        jne .false
        inc rcx
        test al, al
        jne .loop
    .true:
        mov rax, 1
        ret
    .false:
        xor rax, rax
        ret


; Читает один символ из FD_STDIN и возвращает его. Возвращает 0 если достигнут конец потока
global read_char
read_char:
    push 0
    xor rax, rax                ;SYSCALL_READ
    xor rdi, rdi                ;FD_STDIN
    mov rsi, rsp
    mov rdx, 1
    syscall
    pop rax
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
        jz  .term 
        cmp cl, SYMBOL_LF 
        je  .term 
        cmp r8, r10 
        jl  .overflow 
        inc r10 
        jmp .read 
    .term:
        mov byte[r9+r10], 0
        mov rdx, r10 
        mov rax, r9
        ret
    .overflow:
        xor rax, rax
        ret
   


; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из FD_STDIN, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор
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
        je  .term 
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
        jz  .term 
        cmp cl, SYMBOL_SPACE
        je  .term 
        cmp cl, SYMBOL_TAB
        je  .term 
        cmp cl, SYMBOL_LF 
        je  .term 
        cmp r8, r10 
        jl  .overflow 
        inc r10 
        jmp .read 
    .term:
        mov byte[r9+r10], 0
        mov rdx, r10 
        mov rax, r9
        ret
    .overflow:
        xor rax, rax
        ret


; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
global parse_uint
parse_uint:
    xor r8, r8
    xor rax, rax
    xor rdx, rdx
    .loop:
        cmp byte[rdi+rdx], '0'
        jl  .end
        cmp byte[rdi+rdx], '9'
        jg  .end
        mov r8b, [rdi+rdx]
        sub r8b, '0'
        imul    rax, 10
        add rax, r8
        inc rdx
        jmp .loop
    .end:
        ret


; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
global parse_int
parse_int:
    xor  rax, rax
    cmp  byte[rdi], SYMBOL_MINUS
    jne  parse_uint
    inc  rdi
    call    parse_uint
    neg  rax
    inc  rdx
    ret 


; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
global string_copy
string_copy:
    push    rdi
    push    rsi
    push    rdx
    call    string_length
    pop rdx
    pop rsi
    pop rdi
    xor rcx, rcx
    cmp rax, rdx
    jge .end
    .loop:
        mov al, [rdi+rcx]
        mov [rsi+rcx], al
        inc rcx
        test    al, al
        je  .end
        jmp .loop
    .end:
        mov rax, rcx
        ret
