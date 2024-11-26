// Error. Cannot multiply 'i32' variable 'a' and 'f64' variable 'b' without explicit conversion.
const ifj = @import("ifj24.zig");

pub fn main() void {
    var a: i32 = 4;
    var b: f64 = 2.5;
    var result: f64 = a * b;
    a = 1;
    b = 1;
    result = 1;
}