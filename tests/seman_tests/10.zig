// Test for correct fucntion parameters (undeclared)
const ifj = @import("ifj24.zig");

pub fn sum(x : f64, y : f64) f64 {
    return x + y;
}

pub fn main() void {
    var z = sum(1.5, 4.5);
    ifj.write(z);
    z = 2.0;
}
    