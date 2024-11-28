// No Error. Implicit conversion
const ifj = @import("ifj24.zig");

pub fn main() void {
    var x: f64 = 15.75;
    var result: f64 = 10 + x;
    x = 1.0;
    result = 1.0;
}
