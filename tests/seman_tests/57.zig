// No Error. Implicit conversion

const ifj = @import("ifj24.zig");

pub fn main() void {
    var divisor: f64 = 2.0;
    var result: f64 = 10 / divisor;
    divisor = 1.0;
    result = 1.0;
}
