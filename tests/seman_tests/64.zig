// Error. Cannot compare 'i32' variable 'a' and 'f64' variable 'b' without explicit conversion.
const ifj = @import("ifj24.zig");

pub fn main() void {
    var a: i32 = 10;
    var b: f64 = 10.0;
    if (a == b) {
        a = 2;
    }
    a = 1;
    b = 1.0;
}
