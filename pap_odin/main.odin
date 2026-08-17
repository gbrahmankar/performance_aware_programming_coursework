#+feature using-stmt

package pap_odin

import "core:fmt"

import "pap_common"
import "part_five"

main :: proc() {
    using pap_common
    using part_five

    chapter01_our_very_own_haversine()
}