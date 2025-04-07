; > nasm.exe -f win64 -g chapter15_cache_bw_tests.asm -o chapter15_cache_bw_tests.obj
; > lib.exe chapter15_cache_bw_tests.obj /out:chapter15_cache_bw_tests.lib

global cache_size_bw_test 

section .text

cache_size_bw_test :
    xor r9, r9
    xor rax, rax
align 64
.loop:
    vmovdqu ymm0, [rax]
    vmovdqu ymm0, [rax + 16]
    vmovdqu ymm0, [rax + 32]

    vmovdqu ymm0, [rax + 48]
    vmovdqu ymm0, [rax + 64]
    vmovdqu ymm0, [rax + 80]
    
    vmovdqu ymm0, [rax + 96]
    vmovdqu xmm0, [rax + 104]
    vmovdqu ymm0, [rax + 112]

    add r9, 0x100
    and r9, r8
    
    mov rax, rdx
    add rax, r9

    sub rcx, 0x100
    jnz .loop
    ret