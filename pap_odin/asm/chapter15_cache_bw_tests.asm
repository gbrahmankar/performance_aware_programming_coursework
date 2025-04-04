; > nasm.exe -f win64 -g chapter15_cache_bw_tests.asm -o chapter15_cache_bw_tests.obj
; > lib.exe chapter15_cache_bw_tests.obj /out:chapter15_cache_bw_tests.lib

global aligned_at_64_bytes

section .text

aligned_at_64_bytes :
    xor rax, rax
align 64
.loop:
    inc rax
    cmp rax, rcx
    jb .loop
    ret