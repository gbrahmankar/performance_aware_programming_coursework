; > nasm.exe -f win64 -g branch_prediction_pressure.asm -o branch_prediction_pressure.obj
; > lib.exe branch_prediction_pressure.obj /out:branch_prediction_pressure.lib

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
    


































































































































