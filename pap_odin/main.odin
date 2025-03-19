package pap_odin

import "core:fmt"
import "core:time"

import "pap_common"

main :: proc() {
    using pap_common 

    cpu_freq, has_tsc := get_tsc_frequency()

    all_bytes := make([]byte, 4096 * 1024)
    pf0 := read_page_faults()
    for &a_byte in all_bytes[:] {
       a_byte = cast(byte)1 
    }
    pf1 := read_page_faults()
    fmt.println(pf1-pf0)
}