// Incorrect number of parameters (1 param more)
const ifj = @import("ifj24.zig");

pub fn sum(x : f64, y : f64) f64 {
    return x + y;
}

pub fn main() void {
    var z = sum(1.01, 2.09, 1.05);
}