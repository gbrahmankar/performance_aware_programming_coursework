#+build windows
package part_three

foreign import cache_bw_tests "../asm/chapter15_cache_bw_tests.lib"

@(default_calling_convention="c")
foreign cache_bw_tests {
}