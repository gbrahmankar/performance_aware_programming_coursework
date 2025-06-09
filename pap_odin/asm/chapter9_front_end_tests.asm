; > nasm.exe -f win64 -g chapter9_front_end_tests.asm -o chapter9_front_end_tests.obj
; > lib.exe chapter9_front_end_tests.obj /out:chapter9_front_end_tests.lib

global mov_all_bytes
global nop_all_bytes
global cmp_all_bytes
global dec_all_bytes

section .text

mov_all_bytes:
    xor rax, rax
.loop:
    mov [rdx + rax], al
    inc rax
    cmp rax, rcx
    jb .loop
    ret

nop_all_bytes:
    xor rax, rax
.loop:
    db 0x0f, 0x1f, 0x00
    inc rax
    cmp rax, rcx
    jb .loop
    ret

cmp_all_bytes:
    xor rax, rax
.loop:
    inc rax
    cmp rax, rcx
    jb .loop
    ret

dec_all_bytes:
.loop:
    dec rcx
    jnz .loop
    ret 
