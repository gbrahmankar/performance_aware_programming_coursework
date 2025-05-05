; > nasm.exe -f win64 -g chapter20_non_temporal_stores.asm -o chapter20_non_temporal_stores.obj
; > lib.exe chapter20_non_temporal_stores.obj /out:chapter20_non_temporal_stores.lib

global raw_movs_nt
global streaming_bytes 

section .text

; rcx : outer_loop_count : how many times do we want to copy the src_buffer to the dst_buffer
; rdx : src_data_ptr
; r8 : inner_loop_count : how many times do we want to copy a 256-byte block from the src_buffer to the dst_buffer
; r9 : dst_data_ptr

raw_movs_nt:
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

streaming_bytes:
align 64
.outer_loop:
	mov rax, rdx
	mov r10, r8
.inner_loop:
    lea rbx, [rax]
    vmovdqa ymm0, [rbx]
    lea rbx, [r9]
    vmovntdq [rbx], ymm0 

    lea rbx, [rax + 0x20 * 1]
    vmovdqa ymm0, [rbx]
    lea rbx, [r9 + 0x20 * 1]
    vmovntdq [rbx], ymm0 

    lea rbx, [rax + 0x20 * 2]
    vmovdqa ymm0, [rbx]
    lea rbx, [r9 + 0x20 * 2]
    vmovntdq [rbx], ymm0

    lea rbx, [rax + 0x20 * 3]
    vmovdqa ymm0, [rbx]
    lea rbx, [r9 + 0x20 * 3]
    vmovntdq [rbx], ymm0 

    lea rbx, [rax + 0x20 * 4]
    vmovdqa ymm0, [rbx]
    lea rbx, [r9 + 0x20 * 4]
    vmovntdq [rbx], ymm0 

    lea rbx, [rax + 0x20 * 5]
    vmovdqa ymm0, [rbx]
    lea rbx, [r9 + 0x20 * 5]
    vmovntdq [rbx], ymm0 

    lea rbx, [rax + 0x20 * 6]
    vmovdqa ymm0, [rbx]
    lea rbx, [r9 + 0x20 * 6]
    vmovntdq [rbx], ymm0 

    lea rbx, [rax + 0x20 * 7]
    vmovdqa xmm0, [rbx]
    lea rbx, [r9 + 0x20 * 7]
    vmovntdq [rbx], xmm0 

    lea rbx, [rax + 0x20 * 7 + 0x10]
    vmovdqa xmm0, [rbx]
    lea rbx, [r9 + 0x20 * 7 + 0x10]
    vmovntdq [rbx], xmm0 

    add rax, 0x100
    add r9, 0x100

    dec r10
    jnz .inner_loop

    dec rcx
    jnz .outer_loop
    ret 