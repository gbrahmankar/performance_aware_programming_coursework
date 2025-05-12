; > nasm.exe -f win64 -g chapter21_prefetch.asm -o chapter21_prefetch.obj
; > lib.exe chapter21_prefetch.obj /out:chapter21_prefetch.lib

global traverse_links_without_prefetch
global traverse_links_with_prefetch

section .text

; rcx : outer loop count
; rdx : src ptr
; r8 : inner loop count

traverse_links_without_prefetch:
align 64
    mov rax, rdx
.outer_loop:
    vmovdqa ymm0, [rax]
    vmovdqa ymm1, [rax + 0x20]
    
    mov rax, [rax]
    mov r10, r8

.inner_loop:
    vpxor ymm0, ymm1
    vpaddd ymm0, ymm1
    dec r10
    jnz .inner_loop
    
    dec rcx
    jnz .outer_loop
    ret 

traverse_links_with_prefetch:
align 64
    mov rax, rdx
.outer_loop:
    vmovdqa ymm0, [rax]
    vmovdqa ymm1, [rax + 0x20]
    
    mov rax, [rax]
    mov r10, r8

    prefetcht0 [rax] 

.inner_loop:
    vpxor ymm0, ymm1
    vpaddd ymm0, ymm1
    dec r10
    jnz .inner_loop
    
    dec rcx
    jnz .outer_loop
    ret 