// Incorrect expression evaluation in function return
const ifj = @import("ifj24.zig");

pub fn main() i32 {
    return 2 + 1.35;
}