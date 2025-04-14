; > nasm.exe -f win64 -g chapter16_non_pow_of_two.asm -o chapter16_non_pow_of_two.obj
; > lib.exe chapter16_non_pow_of_two.obj /out:chapter16_non_pow_of_two.lib

global  non_pow_of_two_cache_bw_test 

section .text

non_pow_of_two_cache_bw_test:
align 64
.outer_loop:
    mov r9, r8
    mov rax, rdx
    .inner_loop:
        ; issue 256_bytes worth of load
        ; eta : 3_cycles
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