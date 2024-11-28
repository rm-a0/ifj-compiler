// Incorrect expression evaluation with fn call
const ifj = @import("ifj24.zig");

pub fn sum(x : i32, y :i32) i32 {
    return x + y;
}

pub fn not_main2() f64 {
    return sum(1, 2) + 1.5;
}

pub fn main() void {
    const z = not_main2();
    ifj.write(z);
}