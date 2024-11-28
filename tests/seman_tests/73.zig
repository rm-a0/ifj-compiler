// Error. Operator '==' cannot be applied to slices.
const ifj = @import("ifj24.zig");

pub fn main() void {
    var arr1: []u8 = "abc";
    var arr2: []u8 = "def";
    if (arr1 == arr2) {
        arr1 = "ab";
    }
    arr1 = "bb";
    arr2 = "cb";
}