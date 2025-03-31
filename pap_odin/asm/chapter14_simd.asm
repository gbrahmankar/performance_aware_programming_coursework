; > nasm.exe -f win64 -g chapter14_simd.asm -o chapter14_simd.obj
; > lib.exe chapter14_simd.obj /out:chapter14_simd.lib

global load_four_bytes_three_times_per_loop
global load_eight_bytes_three_times_per_loop
global load_sixteen_bytes_three_times_per_loop
global load_thirty_two_bytes_three_times_per_loop

section .text

load_four_bytes_three_times_per_loop :
align 64
	xor rax, rax
.loop:
    mov r8d, [rdx]
    mov r8d, [rdx + 4]
    mov r8d, [rdx + 8]

    add rax, 12
    cmp rax, rcx
    jb .loop
    ret

load_eight_bytes_three_times_per_loop :
align 64
	xor rax, rax
.loop:
    mov r8, [rdx]
    mov r8, [rdx + 8]
    mov r8, [rdx + 16]

    add rax, 24
    cmp rax, rcx
    jb .loop
    ret

load_sixteen_bytes_three_times_per_loop :
align 64
	xor rax, rax
.loop:
    vmovdqu xmm0, [rdx]
    vmovdqu xmm0, [rdx + 16]
    vmovdqu xmm0, [rdx + 32]

    add rax, 48
    cmp rax, rcx
    jb .loop
    ret

load_thirty_two_bytes_three_times_per_loop :
align 64
	xor rax, rax
.loop:
    vmovdqu ymm0, [rdx]
    vmovdqu ymm0, [rdx + 32]
    vmovdqu ymm0, [rdx + 64]

    add rax, 96 
    cmp rax, rcx
    jb .loop
    ret