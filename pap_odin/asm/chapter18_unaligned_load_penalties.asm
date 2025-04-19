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
        vmovdqu ymm0, [rax]
        vmovdqu ymm0, [rax + 32]
        vmovdqu ymm0, [rax + 64]
        vmovdqu ymm0, [rax + 96]
        vmovdqu ymm0, [rax + 128]
        vmovdqu ymm0, [rax + 160]
        vmovdqu ymm0, [rax + 192]
        vmovdqu xmm0, [rax + 224]
        vmovdqu xmm0, [rax + 240]

        add rax, 0x100

        dec r9
        jnz .inner_loop

    dec rcx
    jnz .outer_loop
    ret