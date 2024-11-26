// Error. Cannot compare 'i32' variable 'a' and 'f64' variable 'b' without compile-time known value.
const ifj = @import("ifj24.zig");

pub fn main() void {
    var a: i32 = ifj.readi32();
    var b: f64 = 10.0;
    if (a == b) {
        a = 3;
    }
    a = 1;
    b = 1.0;
}
