; > nasm.exe -f win64 -g chapter18_unaligned_load_penalties.asm -o chapter18_unaligned_load_penalties.obj
; > lib.exe chapter18_unaligned_load_penalties.obj /out:chapter18_unaligned_load_penalties.lib

global load_penalty_test

section .text

load_penalty_test:
align 64
.outer_loop:
    mov r9, r8
    mov rax, rdx
    .inner_loop:
        lea rbx, [rax]
        vmovdqu ymm0, [rbx]
        lea rbx, [rax + 0x20]
        vmovdqu ymm0, [rbx]
        lea rbx, [rax + 0x20 * 2]
        vmovdqu ymm0, [rbx]
        lea rbx, [rax + 0x20 * 3]
        vmovdqu ymm0, [rbx]
        lea rbx, [rax + 0x20 * 4]
        vmovdqu ymm0, [rbx]
        lea rbx, [rax + 0x20 * 5]
        vmovdqu ymm0, [rbx]
        lea rbx, [rax + 0x20 * 6]
        vmovdqu ymm0, [rbx]
        lea rbx, [rax + 0x20 * 7]
        vmovdqu xmm0, [rbx]
        lea rbx, [rax + 0x20 * 7 + 0x10]
        vmovdqu xmm0, [rbx]

        add rax, 0x100

        dec r9
        jnz .inner_loop

    dec rcx
    jnz .outer_loop
    ret