// Cannot add 'i32' variable 'a' and 'f64' literal '20.5' without explicit conversion.
const ifj = @import("ifj24.zig");

pub fn main() void {
    var a: i32 = 10;
    var result: f64 = a + 20.5;
    a = 1;
    result = 1.0;
}
