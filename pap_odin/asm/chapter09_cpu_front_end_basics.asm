; > nasm.exe -f win64 -g chapter09_cpu_front_end_basics.asm -o chapter09_cpu_front_end_basics.obj
; > lib.exe chapter09_cpu_front_end_basics.obj /out:chapter09_cpu_front_end_basics.lib

global nop_one_three_bytes
global nop_three_one_byte 
global nop_nine_one_byte 

section .text

nop_one_three_bytes:
    xor rax, rax
.loop:
    db 0x0f, 0x1f, 0x00
    mov bl, [rdx + rax]
    inc rax
    cmp rax, rcx
    jb .loop
    ret

nop_three_one_byte:
    xor rax, rax
.loop:
    nop
    nop
    nop
    inc rax
    cmp rax, rcx
    jb .loop
    ret

nop_nine_one_byte:
    xor rax, rax
.loop:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    inc rax
    cmp rax, rcx
    jb .loop
    ret