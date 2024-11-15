// Test for correct fucntion parameters (declared)
const ifj = @import("ifj24.zig");

pub fn sum(x : f64, y : f64) f64 {
    return x + y;
}

pub fn main() void {
    var x : ?f64 = 1.5;
    var y : ?f64 = 2.5;
    var z = sum(x, y);
}