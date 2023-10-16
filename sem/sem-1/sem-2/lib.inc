section .text
 
; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
  mov rax, 0
    .loop:
      xor rax, rax
    .count:
      cmp byte [rdi+rax], 0
      je .end
      inc rax
      jmp .count
    .end:
      ret

; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
    xor  rax, rax
    call string_length
    mov  rdx, rax
    mov  rsi, rdi
    mov  rax, 1
    mov  rdi, 1
    syscall
    ret

; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
    xor rax, rax
    xor rsi, rsi
    xor r9, r9
    mov r8, 10
.loop:
    mov sil, [rdi+r9]
    cmp sil, 0
    je .end
    cmp sil, 0x30
    jb .end
    cmp sil, 0x39
    ja .end
    mul r8
    sub sil, 0x30
    add rax, rsi
    inc r9
    jmp .loop
.end:
    mov rdx, r9
    ret

