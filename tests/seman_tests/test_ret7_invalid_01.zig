// Incorrect expression evaluation with before given data types
const ifj = @import("ifj24.zig");

const x : i32 = 25;
const y : f64 = 1.25;

pub fn main() i32 {
    const z : i32 = x + y;
    return z;
}