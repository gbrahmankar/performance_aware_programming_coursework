; > nasm.exe -f win64 -g chapter10_branch_prediction.asm -o chapter10_branch_prediction.obj
; > lib.exe chapter10_branch_prediction.obj /out:chapter10_branch_prediction.lib

global try_byte_data_based_branching

section .text

try_byte_data_based_branching :
    xor rax, rax
.loop:
    mov bl, byte [rdx + rax]
    inc rax
    test bl, 1
    jnz .skip
    nop
.skip:
    cmp rax, rcx
    jb .loop
    ret
    


































































































































