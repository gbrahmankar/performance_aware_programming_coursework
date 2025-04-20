; > nasm.exe -f win64 -g chapter19_cache_set_indexing.asm -o chapter19_cache_set_indexing.obj
; > lib.exe chapter19_cache_set_indexing.obj /out:chapter19_cache_set_indexing.lib

global cache_set_index_test

section .text

cache_set_index_test:
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
        lea rbx, [rax + 0x20 * 8]
        vmovdqu xmm0, [rbx]

        add rax, 0x100

        dec r9
        jnz .inner_loop

    dec rcx
    jnz .outer_loop
    ret