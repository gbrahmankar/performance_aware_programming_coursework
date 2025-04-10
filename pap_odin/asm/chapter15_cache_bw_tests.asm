; > nasm.exe -f win64 -g chapter15_cache_bw_tests.asm -o chapter15_cache_bw_tests.obj
; > lib.exe chapter15_cache_bw_tests.obj /out:chapter15_cache_bw_tests.lib

global cache_size_bw_test 

section .text

cache_size_bw_test :
    xor r9, r9
    mov rax, rdx
align 64
.loop:
    vmovdqu ymm0, [rax]
    vmovdqu ymm0, [rax + 32]
    vmovdqu ymm0, [rax + 64]

    vmovdqu ymm0, [rax + 96]
    vmovdqu ymm0, [rax + 128]
    vmovdqu ymm0, [rax + 160]
    
    vmovdqu ymm0, [rax + 192]
    vmovdqu xmm0, [rax + 224]
    vmovdqu xmm0, [rax + 240]

    add r9, 0x100
    and r9, r8
    
    mov rax, rdx
    add rax, r9

    sub rcx, 0x100
    jnz .loop
    ret