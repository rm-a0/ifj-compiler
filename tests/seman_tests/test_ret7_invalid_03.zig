// Incorrect expression evaluation with string
const ifj = @import("ifj24.zig");

pub fn main() i32 {
    const x = "ahoj" + 3;
    return x;
}