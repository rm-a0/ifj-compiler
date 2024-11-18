// Incorrect expression evaluation with fn call
const ifj = @import("ifj24.zig");

pub fn sum(x : i32, y :i32) i32 {
    return x + y;
}
pub fn main() f64 {
    return sum(1, 2) + 1.5;
}