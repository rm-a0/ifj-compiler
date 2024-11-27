// Undefined variable
const ifj = @import("ifj24.zig");

pub fn main() void {
    var x = null; // Cannot infer type from `null`
    return x;
}