; > nasm.exe -f win64 -g branch_prediction_pressure.asm -o branch_prediction_pressure.obj
; > lib.exe branch_prediction_pressure.obj /out:branch_prediction_pressure.lib

global try_byte_data_based_branching

section .text

try_byte_data_based_branching :
    xor rax, rax

; execute this if the byte is 1
.loop0 :
    mov bl, [rdx + rax] ; load the byte in

    cmp bl, 0
    je .loop1 ; penalize here

    inc rax
    cmp rax, rcx
    jb .loop0

; we are done, just jump out !
    jmp .loop2

; execute this if the byte is 0, 3 one_byte nops is the penalty
.loop1 :
    nop
    nop
    nop
    
    inc rax
    cmp rax, rcx
    jb .loop0

.loop2 :
    ret


































































































































