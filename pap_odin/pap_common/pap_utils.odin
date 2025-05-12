package pap_common

import "core:crypto"

get_random_number :: proc() -> (u64) {
    random_eight_u8s: [8]u8
    crypto.rand_bytes(random_eight_u8s[:])
    return (cast(^u64)&random_eight_u8s[0])^
}