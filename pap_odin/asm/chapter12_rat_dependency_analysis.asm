; regular homework : rat slot assignment and serial depecdency analysis

; mov rax, 1 ; rax -> s0 = reg(rax)
; mov rbx, 2 ; rbx -> s1 = reg(rbx)
; mov rcx, 3 ; rcx -> s2 = reg(rcx)
; mov rdx, 4 ; rdx -> s3 = reg(rdx)

; add rax, rbx ; rax -> s4 = s0 + s1
; add rcx, rdx ; rcx -> s5 = s2 + s3
; add rax, rcx ; rax -> s6 = s4 + s5

; mov rcx, rbx ; rcx -> s5 = reg(rbx) which is "referred" s1, so basically, rcx -> s1

; inc rax ; rax -> s7 = s6 + 1
; dec rcx ; rcx -> s8 = s5 - 1

; sub rax, rbx ; rax -> s9 = s7 - s1
; sub rcx, rdx ; rcx -> s10 = s8 - s3
; sub rax, rcx ; rax -> s11 = s9 - s10

; -----------------------------------------------------------------------------------

; > nasm.exe -f win64 -g chapter12_rat_dependency_analysis.asm -o chapter12_rat_dependency_analysis.obj
; > lib.exe chapter12_rat_dependency_analysis.obj /out:chapter12_rat_dependency_analysis.lib

global rat_add 
global rat_mov_add

section .text

rat_add :
align 64
    mov rax, 1000000000
.loop:
    add rcx, 1
    add rcx, 1
    dec rax
    jnz .loop
    ret

rat_mov_add :
align 64
    mov rax, 1000000000
.loop:
    mov rcx, rax
    add rcx, 1
    mov rcx, rax
    add rcx, 1
    dec rax
    jnz .loop
    ret