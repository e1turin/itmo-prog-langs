%include "lib.inc"

%define PTR_SIZE    8   ;bytes

section .text

global find_word
; @param    %rdi    Ptr to 0-term string
; @param    %rsi    Ptr to the beginning of the dict
; @return   %rax    Returns address of the beginning of the entry in the dict, if it is not found returns 0 
find_word:
    .loop:
        test    rsi, rsi
        je      .end    ;not_found
        push    rdi
        push    rsi
        add     rsi, PTR_SIZE
        call    string_equals
        test    rax, rax
        pop     rsi
        pop     rdi
        jne     .end    ;found
        mov     rsi, [rsi]
        jmp     .loop
    .end:
        mov rax, rsi
        ret
    
    
    
