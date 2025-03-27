; > nasm.exe -f win64 -g nop_pressure.asm -o nop_pressure.obj
; > lib.exe nop_pressure.obj /out:nop_pressure.lib

global nop_one_three_bytes
global nop_three_one_byte 
global nop_nine_one_byte 

section .text

nop_one_three_bytes:
    xor rax, rax
.loop:
    db 0x0f, 0x1f, 0x00
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