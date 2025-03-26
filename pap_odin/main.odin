package pap_odin

import "core:fmt"
import "core:mem/virtual"
import "core:time"

import "pap_common"
import "part_three"

main :: proc() {
    using part_three

    nop_one_three_bytes_asm(5)
    nop_three_one_byte_asm(5)
    nop_nine_one_byte_asm(5)
}