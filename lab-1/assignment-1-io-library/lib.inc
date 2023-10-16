%define syscall_exit    60
%define syscall_read    0
%define syscall_write   1
%define stdout          1
%define stdin           0
%define space_symbol    ' '
%define newline_symbol  `\n`
%define tab_symbol      `\t`
%define minus_symbol    '-'

section .text
 
 
; Принимает код возврата и завершает текущий процесс
exit: 
    mov rax, syscall_exit
    syscall


; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
    xor rax, rax
    .loop:
        cmp byte[rdi+rax], 0
        je  .end
        inc rax
        jmp .loop
    .end:
    ret


; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
    push rdi
    call string_length
    pop rsi
    mov rdx, rax
    mov rdi, stdout
    mov rax, syscall_write
    syscall
    ret


; Переводит строку (выводит символ с кодом 0xA)
print_newline:
    mov rdi, newline_symbol     ;WARN: print_char is inlined


; Принимает код символа и выводит его в stdout
print_char:
    push rdi
    mov rax, syscall_write
    mov rdx, 1
    mov rsi, rsp
    mov rdi, stdout
    syscall
    pop rax
    ret
    

; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
    test    rdi, rdi
    jge print_uint          
    push rdi
    mov rdi, minus_symbol
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


; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
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


; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
    push 0
    xor rax, rax                ;syscall_read
    xor rdi, rdi                ;stdin
    mov rsi, rsp
    mov rdx, 1
    syscall
    pop rax
    ret 


; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор
read_word:
    mov r8, rsi 
    mov r9, rdi 
    xor r10, r10 
    xor rdi, rdi                ;stdin 
    mov rdx, 1 
    .skip:
        xor rax, rax            ;syscall_read
        mov rsi, r9 
        syscall
        test rax, rax
        je  .term_zero 
        cmp byte[r9], space_symbol
        je  .skip 
        cmp byte[r9], tab_symbol
        je  .skip
        cmp byte[r9], newline_symbol
        je  .skip
        inc r10 
    .read:
        xor rax, rax 
        lea rsi, [r9+r10] 
        syscall
        mov cl, byte[r9+r10]
        test    rax, rax
        jz  .term_zero 
        cmp cl, space_symbol 
        je  .term_zero 
        cmp cl, tab_symbol
        je  .term_zero 
        cmp cl, newline_symbol 
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


; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
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
parse_int:
    xor  rax, rax
    cmp  byte[rdi], minus_symbol
    jne  parse_uint
    inc  rdi
    call    parse_uint
    neg  rax
    inc  rdx
    ret 


; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
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
