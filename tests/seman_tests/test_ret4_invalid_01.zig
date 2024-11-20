// Test for wrong function parameters (undeclared) + a is unused :)
const ifj = @import("ifj24.zig");

pub fn sum(x : i32, y : i32) i32 {
    return x + y;
}

pub fn main () void {
    var a = sum(1, 2.5);
}
    