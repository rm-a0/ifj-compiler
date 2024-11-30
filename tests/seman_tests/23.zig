// Incorrect expression evaluation with before given data types
const ifj = @import("ifj24.zig");

pub fn not_main() i32 {
    const x : i32 = 25;
    const y : f64 = 1.25;
    const z : i32 = x + y;
    return z;
}

pub fn main() void {
    const z = not_main();
}