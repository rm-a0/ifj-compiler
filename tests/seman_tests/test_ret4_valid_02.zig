// Test for correct fucntion parameters (declared)
const ifj = @import("ifj24.zig");

pub fn sum(x : f64, y : f64) f64 {
    return x + y;
}

pub fn main() void {
    const x : ?f64 = 1.5;
    const y : ?f64 = 2.5;
    const z = sum(x, y);
    ifj.write(z);
}