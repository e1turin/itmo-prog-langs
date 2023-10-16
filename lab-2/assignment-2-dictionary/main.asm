%include "lib.inc"
%include "dict.inc"
%include "defines.inc"

%define MAX_INPUT_SIZE      255 ;bytes
%define PTR_SIZE            8   ;bytes

section .rodata

msg_err_input_out_of_range: db "ERROR: Input string is too long.", `\n`, 0
msg_err_key_not_found:      db "ERROR: Entry with such key is not found.", `\n`, 0
msg_info_intro:             db "Enter key (end with Enter or Ctrl+D): ", 0
msg_info_dict_value:        db "Value with entered key form the dict: ", 0


section .bss

input_buf: resb MAX_INPUT_SIZE


section .data

%include "words.inc"


section .text

global _start
_start:
    mov rdi, msg_info_intro
    call print_string
    
    ._read_key:
        mov rdi, FD_STDOUT
        mov rsi, input_buf
        mov rdx, MAX_INPUT_SIZE
        call read_string_fd
        test rax, rax
        je .input_out_of_range

    ._find_value_with_key:
        mov rdi, input_buf
        mov rsi, COLON_BEGIN
        call find_word
        test rax, rax
        je .key_is_not_found

    ._print_value_info_msg:
        push rax
        mov rdi, msg_info_dict_value
        call print_string

    ._print_the_value:
        mov rdi, `'`        ;left quotation mark
        call print_char

        pop rdi
        add rdi, PTR_SIZE   ;skip ptr to next entry
        push rdi
        call string_length
        pop rdi
        add rdi, rax
        inc rdi

        call print_string   ;print string of value
        mov rdi, `'`        ;right quotation mark
        call print_char
        call print_newline

    mov rdi, SUCCESS_CODE
    jmp exit

    .input_out_of_range:
        mov rdi, msg_err_input_out_of_range
        jmp .print_out_msg

    .key_is_not_found:
        mov rdi, msg_err_key_not_found

    .print_out_msg:
        mov rdx, FD_STDERR
        call print_string_fd

        mov rdi, ERROR_CODE
        jmp exit


