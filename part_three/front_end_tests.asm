global _movAllBytesAsm
global _nopAllBytesAsm
global _cmpAllBytesAsm
global _decAllBytesAsm

section .text

_movAllBytesAsm:
    xor rax, rax
.loop:
    mov [rdx + rax], al
    inc rax
    cmp rax, rcx
    jb .loop
    ret

_nopAllBytesAsm:
    xor rax, rax
.loop:
    db 0x0f, 0x1f, 0x00
    inc rax
    cmp rax, rcx
    jb .loop
    ret

_cmpAllBytesAsm:
    xor rax, rax
.loop:
    inc rax
    cmp rax, rcx
    jb .loop
    ret

_decAllBytesAsm:
.loop:
    dec rcx
    jnz .loop
    ret
