// Undefined variable set to null
const ifj = @import("ifj24.zig");

pub fn main() ?i32 {
    var x = null; // Cannot infer type from `null`
    return x;
}