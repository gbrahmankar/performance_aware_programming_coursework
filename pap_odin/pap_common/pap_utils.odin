package pap_common

import "core:crypto"

// constants
ONE_KB :: 1 * 1024
ONE_MB :: ONE_KB * 1024
ONE_GB :: ONE_MB * 1024

PI64 :: 3.14159265358979323846264338327950288419716939937510582097494459230781640628

CACHE_LINE_SIZE :: 64
MEMORY_PAGE_SIZE :: 4096

// utils
get_random_u64 :: proc() -> (u64) {
    random_eight_u8s: [8]u8
    crypto.rand_bytes(random_eight_u8s[:])
    return (cast(^u64)&random_eight_u8s[0])^
}