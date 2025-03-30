; > nasm.exe -f win64 -g code_alignment.asm -o code_alignment.obj
; > lib.exe code_alignment.obj /out:code_alignment.lib

global aligned_at_64_bytes
global offset_by_1_nop
global offset_by_15_nops
global offset_by_31_nops
global offset_by_63_nops

section .text

aligned_at_64_bytes :
    xor rax, rax
align 64
.loop:
    inc rax
    cmp rax, rcx
    jb .loop
    ret

offset_by_1_nop :
    xor rax, rax
align 64
nop
.loop:
    inc rax
    cmp rax, rcx
    jb .loop
    ret

offset_by_15_nops :
    xor rax, rax
align 64
%rep 15
nop
%endrep
.loop:
    inc rax
    cmp rax, rcx
    jb .loop
    ret

offset_by_31_nops :
    xor rax, rax
align 64
%rep 31
nop
%endrep
.loop:
    inc rax
    cmp rax, rcx
    jb .loop
    ret
 
offset_by_63_nops :
    xor rax, rax
align 64
%rep 63
nop
%endrep
.loop:
    inc rax
    cmp rax, rcx
    jb .loop
    ret 