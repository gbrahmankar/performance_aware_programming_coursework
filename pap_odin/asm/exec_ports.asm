; > nasm.exe -f win64 -g exec_ports.asm -o exec_ports.obj
; > lib.exe exec_ports.obj /out:exec_ports.lib

global one_load_per_loop 
global two_loads_per_loop 
global three_loads_per_loop 
global four_loads_per_loop
global five_loads_per_loop 

section .text

one_load_per_loop :
align 64
.loop:
    mov rax, [rdx]
    sub rcx, 1
    jnle .loop
    ret

two_loads_per_loop :
align 64
.loop:
    mov rax, [rdx]
    mov rax, [rdx]
    sub rcx, 2
    jnle .loop
    ret

three_loads_per_loop :
align 64
.loop:
    mov rax, [rdx]
    mov rax, [rdx]
    mov rax, [rdx]
    sub rcx, 3
    jnle .loop
    ret

four_loads_per_loop :
align 64
.loop:
    mov rax, [rdx]
    mov rax, [rdx]
    mov rax, [rdx]
    mov rax, [rdx]
    sub rcx, 4
    jnle .loop
    ret

five_loads_per_loop :
align 64
.loop:
    mov rax, [rdx]
    mov rax, [rdx]
    mov rax, [rdx]
    mov rax, [rdx]
    mov rax, [rdx]
    sub rcx, 5
    jnle .loop
    ret