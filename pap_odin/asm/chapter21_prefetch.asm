; > nasm.exe -f win64 -g chapter21_prefetch.asm -o chapter21_prefetch.obj
; > lib.exe chapter21_prefetch.obj /out:chapter21_prefetch.lib

global traverse_links_without_prefetch
global traverse_links_with_prefetch

section .text

; rcx : 
; rdx : 
; r8 : 
; r9 : 

traverse_links_without_prefetch:
align 64
.outer_loop:
	mov rax, rdx
	mov r10, r8
.inner_loop:
    lea rbx, [rax]
    vmovdqa ymm0, [rbx]
    lea rbx, [r9]
    vmovdqa [rbx], ymm0

    lea rbx, [rax + 0x20 * 1]
    vmovdqa ymm0, [rbx]
    lea rbx, [r9 + 0x20 * 1]
    vmovdqa [rbx], ymm0

    lea rbx, [rax + 0x20 * 2]
    vmovdqa ymm0, [rbx]
    lea rbx, [r9 + 0x20 * 2]
    vmovdqa [rbx], ymm0

    lea rbx, [rax + 0x20 * 3]
    vmovdqa ymm0, [rbx]
    lea rbx, [r9 + 0x20 * 3]
    vmovdqa [rbx], ymm0

    lea rbx, [rax + 0x20 * 4]
    vmovdqa ymm0, [rbx]
    lea rbx, [r9 + 0x20 * 4]
    vmovdqa [rbx], ymm0

    lea rbx, [rax + 0x20 * 5]
    vmovdqa ymm0, [rbx]
    lea rbx, [r9 + 0x20 * 5]
    vmovdqa [rbx], ymm0

    lea rbx, [rax + 0x20 * 6]
    vmovdqa ymm0, [rbx]
    lea rbx, [r9 + 0x20 * 6]
    vmovdqa [rbx], ymm0

    lea rbx, [rax + 0x20 * 7]
    vmovdqa xmm0, [rbx]
    lea rbx, [r9 + 0x20 * 7]
    vmovdqa [rbx], xmm0

    lea rbx, [rax + 0x20 * 7 + 0x10]
    vmovdqa xmm0, [rbx]
    lea rbx, [r9 + 0x20 * 7 + 0x10]
    vmovdqa [rbx], xmm0

    add rax, 0x100
    add r9, 0x100

    dec r10
    jnz .inner_loop

    dec rcx
    jnz .outer_loop
    ret 

traverse_links_with_prefetch:
align 64
    xor rax, rax

.outer_loop:
	mov rax, rdx
.inner_loop:
	lea rbx, [rax]
    mov r9, [rbx]

    add rax, 0x100

    dec r8
    jnz .inner_loop

    dec rcx
    jnz .outer_loop
    ret 