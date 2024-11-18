// Incorrect expression evaluation with before given data types
const ifj = @import("ifj24.zig");

var x : i32 = 25;
var y : f64 = 1.25;

pub fn main() i32 {
    var z : i32 = x + y;
    return z;
}