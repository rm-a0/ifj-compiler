// Test for wrong function parameters (declared)
const ifj = @import("ifj24.zig");

pub fn sum(x : i32, y : i32) i32 {
    return x + y;
}

pub fn main () void {
    var x : i32 = 1;
    var y : f64 = 2.5;
    var a = sum(x, y);
}